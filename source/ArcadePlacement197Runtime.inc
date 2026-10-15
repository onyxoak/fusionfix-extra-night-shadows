// Included after ModelBinding69. Default OFF, no new hook or emission path.
#include "ArcadePlacement197.hpp"
namespace ArcadePlacement197 {
using namespace arcade_placement197;
bool requested=false,assetsAuthenticated=false;
struct Pose {uint32_t pointer{};Matrix matrix{};friend bool operator==(const Pose&,const Pose&)=default;};
struct Proof {
 model_effect_observer::Invocation invocation{};
 traffic_signal::Identity source{},fixture{};
 uint32_t sourceInfo{},fixtureInfo{};
 Pose sourcePose{},fixturePose{};Matrix emitter{};
 std::array<uint8_t,108> descriptor{};std::array<uint32_t,4> effect{};
 unsigned ordinal{};uint32_t frame{};Packet original{},moved{};bool present{},retired{};
};
std::array<Proof,2> leases{};
std::atomic_flag leaseLock=ATOMIC_FLAG_INIT;
thread_local std::array<Proof,ModelEffectObserver::MaxDepth> pending{};
// Discovery addresses are hints only. No permission or owner proof survives a
// callback. A reused slot must pass current pool generation/model/pose checks.
thread_local uint32_t fixtureHint{},scanPool{},scanFrame=UINT32_MAX;
thread_local std::array<uint32_t,4> scanSlots{};
std::atomic<uint32_t> candidates{0},moved{0},refused{0},retired{0},restored{0},restoreRefused{0};
bool Copy(uintptr_t at,void* out,size_t n) noexcept {return ModelEffectObserver::Copy(at,out,n);}
bool Owner(uint32_t address,uint32_t model,traffic_signal::Identity& identity,uint32_t& info,Pose& pose) noexcept {
 TrafficSignal::Match first{},last{};
 if(!TrafficSignal::MatchOwner(address,first)||first.identity.model!=model||address>UINT32_MAX-0x30)return false;
 Pose staged{};uint32_t again{};
 if(!Copy(address+0x20,&staged.pointer,4))return false;
 if(staged.pointer){if(!Copy(staged.pointer,staged.matrix.data(),64))return false;}
 else {
  // Exact known WPL has zero heading. Permit the native inline representation
  // only when its current bits describe this same pose; no guessed transform.
  std::array<float,4> inlinePose{};
  if(!Copy(address+0x10,inlinePose.data(),16)||inlinePose[3]!=0.f)return false;
  staged.matrix[0]=staged.matrix[5]=staged.matrix[10]=staged.matrix[15]=1.f;
  for(unsigned j=0;j<3;++j)staged.matrix[12+j]=inlinePose[j];
 }
 if(!KnownPose(staged.matrix)||!Copy(address+0x20,&again,4)||again!=staged.pointer||
    !TrafficSignal::MatchOwner(address,last)||last.identity!=first.identity||last.modelInfo!=first.modelInfo)return false;
 Matrix final{};
 if(staged.pointer){if(!Copy(staged.pointer,final.data(),64)||final!=staged.matrix)return false;}
 else {std::array<float,4> tail{};if(!Copy(address+0x10,tail.data(),16)||tail[3]!=0.f)return false;
  for(unsigned j=0;j<3;++j)if(Bits(tail[j])!=Bits(staged.matrix[12+j]))return false;}
 identity=first.identity;info=first.modelInfo;pose=staged;return true;
}
bool FindFixture(uint32_t frame,Proof& out) noexcept {
 if(fixtureHint&&Owner(fixtureHint,FixtureModel,out.fixture,out.fixtureInfo,out.fixturePose))return true;
 fixtureHint=0;
 // At most128 slots in one pool per frame, and only when an exact target
 // packet actually emits. Common lights and absent arcade sources do no work.
 if(scanFrame==frame)return false;scanFrame=frame;
 constexpr uint32_t roots[]{0xebd0e8,0x1232c60,0x126d9ec,0x12f7d60};
 constexpr uint32_t strides[]{0x70,0x320,0x80,0x110};
 uint32_t pool{};rage::VehiclePool header{};
 const auto which=scanPool%4;scanPool=(which+1)%4;auto& scanSlot=scanSlots[which];const auto stride=strides[which];
 if(!Copy(game+roots[which],&pool,4)||!Copy(pool,&header,sizeof(header))||
    header.m_nSize<=0||header.m_nSize>65536||header.m_nStorageSize!=int32_t(stride)){
  scanSlot=0;return false;
 }
 const auto storage=reinterpret_cast<uintptr_t>(header.m_aStorage),flags=reinterpret_cast<uintptr_t>(header.m_aFlags);
 if(storage<0x10000||flags<0x10000||uint64_t(storage)+uint64_t(header.m_nSize)*stride>uint64_t(UINT32_MAX)+1||
    uint64_t(flags)+header.m_nSize>uint64_t(UINT32_MAX)+1){scanSlot=0;return false;}
 if(scanSlot>=uint32_t(header.m_nSize))scanSlot=0;
 const unsigned start=scanSlot,n=(std::min)(128u,uint32_t(header.m_nSize)-start);
 scanSlot+=n;if(scanSlot==uint32_t(header.m_nSize))scanSlot=0;
 std::array<uint8_t,128> refs{};std::array<uint8_t,128*0x320> bodies{};
 if(!Copy(flags+start,refs.data(),n)||!Copy(storage+start*stride,bodies.data(),size_t(n)*stride))return false;
 for(unsigned i=0;i<n;++i){
  if(refs[i]&0x80)continue;int16_t index{};std::memcpy(&index,bodies.data()+i*stride+0x2e,2);
  if(index<0||index>=31000)continue;uint32_t info{},hash{};
  if(!Copy(game+0xe95cd8+uint32_t(index)*4,&info,4)||info<0x10000||info>UINT32_MAX-0x40||
     !Copy(info+0x3c,&hash,4)||hash!=FixtureModel)continue;
  const auto address=uint32_t(storage+(start+i)*stride);
  if(Owner(address,FixtureModel,out.fixture,out.fixtureInfo,out.fixturePose)){fixtureHint=address;return true;}
 }
 return false;
}
bool Current(const Proof& p,uint32_t frame) noexcept {
 if(!requested||!assetsAuthenticated||!p.present||p.retired||p.frame!=frame||frame!=ModelEffectObserver::Frame()||p.ordinal>=2)return false;
 Proof now{};
 if(!Owner(p.source.owner,SourceModel,now.source,now.sourceInfo,now.sourcePose)||
    now.source!=p.source||now.sourceInfo!=p.sourceInfo||now.sourcePose!=p.sourcePose||
    !Owner(p.fixture.owner,FixtureModel,now.fixture,now.fixtureInfo,now.fixturePose)||
    now.fixture!=p.fixture||now.fixtureInfo!=p.fixtureInfo||now.fixturePose!=p.fixturePose)return false;
 if(!Copy(p.invocation.descriptor,now.descriptor.data(),now.descriptor.size())||now.descriptor!=p.descriptor||
    !Copy(p.invocation.effect,now.effect.data(),sizeof(now.effect))||now.effect!=p.effect||
    !Copy(p.invocation.matrix,now.emitter.data(),sizeof(now.emitter))||now.emitter!=p.emitter)return false;
 return frame==ModelEffectObserver::Frame()&&Descriptor(now.descriptor)&&Effect(p.ordinal,now.effect);
}
bool Apply(Packet& packet,Packet& original,uint32_t frame,const native_projection66::ConstructorContext& context) noexcept {
 // Integer-only exact numeric gate precedes every additional memory query.
 if(!requested||!assetsAuthenticated)return false;const int ordinal=NumericCandidate(packet);if(ordinal<0)return false;
 ++candidates;
 const auto depth=ModelEffectObserver::local.state.Depth();const auto* invocation=ModelEffectObserver::local.state.Current();
 if(!depth||depth>pending.size()||!invocation||invocation->ordinal!=unsigned(ordinal)||
    !ModelEffectObserver::ModelValue68::Matches(ModelBinding69(context))){++refused;return false;}
 pending[depth-1]={};
 const traffic_signal::fresh_page::Operation44 reads(32);const immediate_region33::Scope regions;
 Proof p{};p.invocation=*invocation;p.ordinal=unsigned(ordinal);p.frame=frame;p.original=packet;
 if(frame!=invocation->frame||!Owner(invocation->owner,SourceModel,p.source,p.sourceInfo,p.sourcePose)||
    p.sourceInfo!=invocation->modelInfo||!p.sourcePose.pointer||invocation->matrix!=p.sourcePose.pointer||
    !Copy(invocation->descriptor,p.descriptor.data(),p.descriptor.size())||
    !Descriptor(p.descriptor)||!Copy(invocation->effect,p.effect.data(),sizeof(p.effect))||!Effect(p.ordinal,p.effect)||
    !FindFixture(frame,p)){++refused;return false;}
 // Authenticate actual native argument origin and emitter transform as well as
 // owner pose. The exact constructor association is rechecked after all reads.
 std::array<float,3> origin{};
 if(!Copy(context.position,origin.data(),sizeof(origin))||!Copy(invocation->matrix,p.emitter.data(),sizeof(p.emitter))||
    !KnownPose(p.emitter)){++refused;return false;}
 for(unsigned j=0;j<3;++j)if(Bits(origin[j])!=packet[8+j]){++refused;return false;}
 p.present=true;
 if(!Current(p,frame)||!ModelEffectObserver::ModelValue68::Matches(ModelBinding69(context))||
    !Move(packet,p.ordinal,p.sourcePose.matrix,p.fixturePose.matrix)){++refused;return false;}
 original=p.original;pending[depth-1]=p;return true;
}
bool Finish(Packet& packet,uint32_t frame,const native_projection66::ConstructorContext&,bool success) noexcept {
 const auto depth=ModelEffectObserver::local.state.Depth();if(!depth||depth>pending.size())return false;
 auto p=pending[depth-1];pending[depth-1]={};
 if(!success||!p.present||p.frame!=frame||frame!=ModelEffectObserver::Frame()||p.ordinal>=2||
    !NativeProjection66::privateProjections197.Owns(packet[24]))return false;
 const auto slot=(packet[24]-NativeProjection66::privateProjections197.registry.keyBase)/64;
 if(slot>=leases.size())return false;
 if(leaseLock.test_and_set(std::memory_order_acquire))return false;
 if(frame!=ModelEffectObserver::Frame()){leaseLock.clear(std::memory_order_release);return false;}
 // Preserve old-key originals for exact restoration. A new immutable key for
 // this ordinal retires its preceding projection, never loses its witness.
 for(auto& entry:leases)if(entry.present&&entry.ordinal==p.ordinal)entry.retired=true;
 p.moved=packet;leases[slot]=p;leaseLock.clear(std::memory_order_release);++moved;return true;
}
// A fresh whole-bank preflight is performed by the caller. Restore only when
// the actual current120-byte bank packet still matches our exact witness.
bool RestoreBank(rage::CLightSource& light,const Packet& expected,const Packet& original) noexcept {
 __try {
  Packet current{};std::memcpy(current.data(),&light,0x78);
  if(!Restore(current,expected,original))return false;
  auto* words=reinterpret_cast<uint32_t*>(&light);
  words[8]=current[8];words[9]=current[9];words[10]=current[10];words[18]=current[18];words[24]=current[24];return true;
 } __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION||GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR||
            GetExceptionCode()==EXCEPTION_GUARD_PAGE?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){return false;}
}
bool Bank(rage::CLightSource& light,Packet& packet,uint32_t frame) noexcept {
 // Every issued private key needs a live lease, including older keys after an
 // ordinal changes projection. Unrelated native/global keys bypass this path.
 if(!NativeProjection66::privateProjections197.Owns(packet[24]))return true;
 Proof p{};bool found=false;
 if(leaseLock.test_and_set(std::memory_order_acquire))return false;
 for(const auto& entry:leases)if(entry.present&&entry.moved[24]==packet[24]){p=entry;found=true;break;}
 leaseLock.clear(std::memory_order_release);
 if(!found)return false;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 reads(32);const immediate_region33::Scope regions;
 if(SamePacket(packet,p.moved)&&Current(p,frame))return true;
 ++retired;
 if(RestoreBank(light,p.moved,p.original)){
  (void)Restore(packet,p.moved,p.original);++restored;
 }else ++restoreRefused;
 return false;
}
void Prepare() {
 if(!requested)return;
 // Startup-only exact local archive/IDE authentication. The table range binds
 // names to offsets, then the source model, actual companion mesh/embedded
 // texture and WPL pose are checked separately without reading a whole IMG.
 try {
  const auto root=pe::ModulePath(reinterpret_cast<HMODULE>(game)).parent_path();
  std::ifstream file(root/L"pc/data/maps/manhat/manhat01.img",std::ios::binary);
  struct Span {uint64_t offset;size_t size;const char* hash;};
  constexpr Span spans[]{
   {0,21107,"f70e0f0c44f997e3451b5a5aafab1ed832b4322c842dd6f6da6e9aaf1d4f2c84"},
   {178292736,1322570,"019a4778d07078431fc140027f33f689b913ba6db6bb7fd01d73cedd223866fa"},
   {303153152,454668,"5449e7c650cb2c6b15c253f21950ed8d8f9ff714c68433a47c8f7470712b5239"},
   {239251456,18740,"ca4542b9643a041ceea66931a845b3f92b36d5f5c234da076e554754c32bed5e"}};
  bool valid=bool(file);for(const auto& s:spans){std::vector<uint8_t> bytes(s.size);file.seekg(s.offset);
   file.read(reinterpret_cast<char*>(bytes.data()),bytes.size());valid=valid&&bool(file)&&pe::Hash(bytes)==s.hash;}
  const auto ide=root/L"pc/data/maps/manhat/manhat01.ide";
  valid=valid&&std::filesystem::file_size(ide)==258022&&pe::Hash(pe::Read(ide))=="6d287f63cf55fd4acb60a119ddd8b33bf62f6f0f1e82cfd03e69cca386b7ec63";
  assetsAuthenticated=valid;
 }catch(...){assetsAuthenticated=false;}
 Event(1,"ARCADE_PLACEMENT197_AUTH","requested=1 authenticated="+std::to_string(assetsAuthenticated)+" model=2e73d323 ordinals=0,1 fixture=90507341 offset_mm=30 point_type_preserved=1");
}
void Publish() {
 if(!requested)return;static uint32_t last{};const auto now=GetTickCount();if(uint32_t(now-last)<5000)return;last=now;
 Event(1,"ARCADE_PLACEMENT197_COUNTS","candidates="+std::to_string(candidates.load())+" moved="+std::to_string(moved.load())+
  " refused="+std::to_string(refused.load())+" retired="+std::to_string(retired.load())+" restored="+std::to_string(restored.load())+
  " restore_refused="+std::to_string(restoreRefused.load())+" two_ordinals_only=1 extra_maps=0");
}
}
