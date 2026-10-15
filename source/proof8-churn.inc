// Fixed-size, render-thread-owned counters; copied to a mailbox every five seconds.
struct ChurnKey {uint32_t key{},flags{},own{},selected{},entered{},left{},lostPresent{},lostRelevant{};};
struct ChurnWindow {uint32_t start{},end{},phase{},frames{},overflow{};std::array<ChurnKey,128> keys{};};
ChurnWindow churnWindow, churnPublished;
std::atomic<unsigned> churnState{0};
std::atomic<uint32_t> churnDropped{0};
uint32_t LabPhase() noexcept {return unsigned(proofRequested.load())+2*policePolicy.load()+4*unsigned(BoundSirenOn())+8*unsigned(proofRetired.load());}
void CountChurn(const PolicePacket& p,const PolicePacket& previous) noexcept {
 using namespace PlayerShadowAllocation;
 const auto phase=LabPhase();
 if(churnWindow.start&&(phase!=churnWindow.phase||p.tick-churnWindow.start>=5000)){
  churnWindow.end=p.tick;unsigned free=0;
  if(churnState.compare_exchange_strong(free,1,std::memory_order_acquire)){churnPublished=churnWindow;churnState.store(2,std::memory_order_release);}else ++churnDropped;
  churnWindow={};
 }
 if(!churnWindow.start){churnWindow.start=p.tick;churnWindow.phase=phase;}
 ++churnWindow.frames;
 auto record=[&](uint32_t key)->ChurnKey*{for(auto& k:churnWindow.keys)if(k.key==key||!k.key){k.key=key;return &k;}++churnWindow.overflow;return nullptr;};
 for(const auto& s:p.slots)if(s.key){auto* k=record(s.key);if(!k)continue;k->flags=s.flags;k->own=s.own;++k->selected;
  bool found=false;for(const auto& old:previous.slots)if(old.key==s.key)found=true;if(!found)++k->entered;
 }
 const auto* lights=CurrentLights();const auto count=CurrentCount();
 for(const auto& old:previous.slots)if(old.key){bool found=false;for(const auto& s:p.slots)if(s.key==old.key)found=true;if(found)continue;
  auto* k=record(old.key);if(!k)continue;k->flags=old.flags;k->own=old.own;++k->left;
  for(uint32_t i=0;i<count;++i)if(static_cast<uint32_t>(lights[i].mCastShadows)==old.key){++k->lostPresent;if(state.nativeCandidates[i].observed&&labRelevant[i])++k->lostRelevant;break;}
 }
}
void PublishChurn(){
 unsigned ready=2;if(!churnState.compare_exchange_strong(ready,3,std::memory_order_acquire))return;
 const auto w=churnPublished;churnState.store(0,std::memory_order_release);char s[500];
 sprintf_s(s,"start=%u end=%u phase=%u frames=%u overflow=%u mailboxDropped=%u",w.start,w.end,w.phase,w.frames,w.overflow,churnDropped.load());Event(2,"CHURN_WINDOW",s);
 for(const auto& k:w.keys)if(k.key){sprintf_s(s,"start=%u phase=%u key=%u flags=%u own=%u selected=%u entered=%u left=%u lost_present=%u lost_relevant=%u",w.start,w.phase,k.key,k.flags,k.own,k.selected,k.entered,k.left,k.lostPresent,k.lostRelevant);Event(2,"CHURN_KEY",s);}
}
