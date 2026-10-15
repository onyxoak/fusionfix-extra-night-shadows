#include "NativeGraphicsMenu117.hpp"
#include "MenuSnapshot117.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace native_graphics117;
namespace {
constexpr uint32_t ScreenAddress=0x10000,FirstData=0x11000;
struct Memory {
 std::vector<uint8_t> bytes=std::vector<uint8_t>(0x20000);
 unsigned reads{},writes{},appends{},moves{};
 unsigned failWrite{},partialWrite{},foreignWrite{},badReturn{},unknownDescriptor{};
 bool inaccessible{};
 bool Read(uint32_t address,void* out,size_t count) noexcept {
  ++reads;if(inaccessible||address<ScreenAddress||uint64_t(address-ScreenAddress)+count>bytes.size())return false;
  std::memcpy(out,bytes.data()+address-ScreenAddress,count);return true;
 }
 bool Write(uint32_t address,const void* in,size_t count) noexcept {
  ++writes;if(inaccessible||address<ScreenAddress||uint64_t(address-ScreenAddress)+count>bytes.size())return false;
  if(writes==foreignWrite){bytes[FirstData-ScreenAddress+1]=uint8_t('Z');return false;}
  if(writes==failWrite)return false;
  if(writes==partialWrite){std::memcpy(bytes.data()+address-ScreenAddress,in,count/2);return false;}
  std::memcpy(bytes.data()+address-ScreenAddress,in,count);return true;
 }
 template<class T> T Load(uint32_t address) const {T result{};std::memcpy(&result,bytes.data()+address-ScreenAddress,sizeof(T));return result;}
 template<class T> void Store(uint32_t address,const T& value){std::memcpy(bytes.data()+address-ScreenAddress,&value,sizeof(T));}
 uint32_t Append(uint32_t descriptor,int arg) {
  assert(descriptor==ScreenAddress+16&&arg==0);++appends;
  auto a=Load<Array32>(descriptor);
  if(a.count==a.capacity){const auto data=a.data+0x2000;assert(uint64_t(data-ScreenAddress)+size_t(a.capacity+16)*sizeof(Option)<=bytes.size());
   std::memcpy(bytes.data()+data-ScreenAddress,bytes.data()+a.data-ScreenAddress,size_t(a.count)*sizeof(Option));
   std::fill_n(bytes.data()+a.data-ScreenAddress,size_t(a.count)*sizeof(Option),uint8_t(0xdd));a.data=data;a.capacity=uint16_t(a.capacity+16);++moves;
  }
  const auto result=a.data+uint32_t(a.count)*sizeof(Option);++a.count;
  // Native append returns uninitialized storage, represented by a zero row.
  Store(result,Option{});if(appends==unknownDescriptor)++a.count;Store(descriptor,a);
  return appends==badReturn?result+2:result;
 }
};
std::vector<Option> Original(unsigned count=26) {
 assert(count>0);std::vector<Option> rows(count);
 for(unsigned i=0;i+1<count;++i){rows[i].action=1;rows[i].label[0]='X';rows[i].label[1]=char('A'+i%26);rows[i].preference=int16_t(100+i);rows[i].scaler=2;rows[i].display=0;}
 if(count>=19){
  std::memset(rows[14].label,0,16);std::memcpy(rows[14].label,"Antialiasing",12);
  std::memset(rows[16].label,0,16);std::memcpy(rows[16].label,"Volumetric Fog",14);
  rows[15]={};rows[15].display=100;rows[15].padding=0x7b;
  std::memcpy(rows[15].label+1,"stale tail",10);
 }
 rows.back().action=46;return rows;
}
Memory Setup(unsigned count=26,unsigned capacity=32) {
 Memory memory;Screen32 screen{};std::memcpy(screen.header,"MH_GFX",6);screen.options={FirstData,uint16_t(count),uint16_t(capacity)};
 memory.Store(ScreenAddress,screen);const auto rows=Original(count);std::memcpy(memory.bytes.data()+FirstData-ScreenAddress,rows.data(),rows.size()*sizeof(Option));return memory;
}
PublishResult Run(Memory& memory,bool ownership=true,bool ownedGeneration=false) {
 return Publish(memory,ScreenAddress,1,ownership,ownedGeneration,[&](uint32_t descriptor,int arg){return memory.Append(descriptor,arg);});
}
void CheckOriginal(const Memory& memory,unsigned count=26) {
 const auto screen=memory.Load<Screen32>(ScreenAddress);assert(screen.options.count==count);
 const auto original=Original(count);assert(!std::memcmp(memory.bytes.data()+screen.options.data-ScreenAddress,original.data(),original.size()*sizeof(Option)));
}
}

int main() {
 static_assert(sizeof(Option)==22&&sizeof(Screen32)==24&&sizeof(Array32)==8);
 for(size_t i=0;i<ControlCount;++i){const auto row=Row(Control(i));assert(row.action==127&&row.preference==-1&&row.display==100&&row.scaler==1);assert(Stamp(row)==Control(i));assert(Labels[i].size()<60);}
 assert(!Stamp(Row(Control::Count)));
 const auto original=Original();const auto plan=MakePlan(original);assert(plan&&plan->count==26&&plan->first==15&&!plan->already);
 for(size_t n=0;n<original.size();++n)if(n!=15)assert(!std::memcmp(&plan->rows[n],&original[n],sizeof(Option)));
 assert(plan->rows[25].action==46&&Stamp(plan->rows[15])==Control::Preset);
 assert(!MakePlan(std::span(plan->rows).first(plan->count)));assert(MakePlan(std::span(plan->rows).first(plan->count),true)->already);
 // Exact observed native NONE semantics, not arbitrary empty functional rows.
 {auto bad=original;bad[15].action=1;assert(!MakePlan(bad));bad=original;bad[15].preference=-1;assert(!MakePlan(bad));
  bad=original;bad[15].scaler=1;assert(!MakePlan(bad));bad=original;bad[15].display=0;assert(!MakePlan(bad));
  bad=original;bad[15].label[0]='X';assert(!MakePlan(bad));bad=original;bad[14].label[0]='X';assert(!MakePlan(bad));
  bad=original;bad[16].action=127;assert(!MakePlan(bad));
  bad=original;bad[20]=bad[14];bad[21]=bad[15];bad[22]=bad[16];assert(!MakePlan(bad));
 }
 {auto bad=original;bad[15]=Row(Control::Preset);bad[15].padding=1;assert(!MakePlan(bad,true));
  bad=original;bad[3]=Row(Control::Preset);assert(!MakePlan(bad,true));
 }
 {Snapshot snapshot;snapshot.Capture(std::span(plan->rows).first(plan->count));assert(snapshot.active&&snapshot.CanRenew(std::span(plan->rows).first(plan->count)));
  auto changed=plan->rows;changed[plan->count]=changed[plan->count-1];changed[plan->count-1]=original[0];
  assert(snapshot.CanRenew(std::span(changed).first(plan->count+1)));
  changed[0].preference++;assert(!snapshot.CanRenew(std::span(changed).first(plan->count+1)));changed[0].preference--;
  changed[15].padding=1;assert(!snapshot.CanRenew(std::span(changed).first(plan->count+1)));
  assert(!snapshot.CanRenew(original));
 }
 for(unsigned count=1;count<=RowLimit;++count)assert(bool(MakePlan(Original(count)))==(count>=19));
 {auto bad=Original();bad[0].action=46;assert(!MakePlan(bad));bad=Original();std::memset(bad[1].label,'A',16);assert(!MakePlan(bad));}
 for(unsigned capacity:{26u,32u,48u}){auto memory=Setup(26,capacity);const auto result=Run(memory);assert(result.result==Publication::Published&&result.receipt.active);
  assert(!memory.appends&&!memory.moves&&memory.writes==1);const auto after=memory.Load<Screen32>(ScreenAddress);
  assert(after.options.count==26&&after.options.capacity==capacity&&after.options.data==FirstData);
  assert(!std::memcmp(memory.bytes.data()+FirstData-ScreenAddress,plan->rows.data(),26*sizeof(Option)));
  for(unsigned row=0;row<26;++row)assert(bool(result.receipt.Own(memory,ScreenAddress,1,row))==(row==15));
  assert(!result.receipt.Own(memory,ScreenAddress,2,15));assert(!result.receipt.Own(memory,ScreenAddress+24,1,15));
  assert(Run(memory,true,true).result==Publication::AlreadyPublished&&!memory.appends);
  auto changed=memory.Load<Option>(FirstData+15*22);changed.padding=1;memory.Store(FirstData+15*22,changed);
  assert(!result.receipt.Own(memory,ScreenAddress,1,15)&&Run(memory,true,true).result==Publication::Refused);
 }
 {auto memory=Setup();assert(Run(memory,false).result==Publication::Refused);assert(!memory.reads&&!memory.writes&&!memory.appends);}
 {auto memory=Setup();memory.inaccessible=true;assert(Run(memory).result==Publication::Refused&&!memory.appends);}
 {auto memory=Setup(50,50);assert(Run(memory).result==Publication::Published&&!memory.appends&&memory.Load<Screen32>(ScreenAddress).options.count==50);}
 {auto memory=Setup();auto screen=memory.Load<Screen32>(ScreenAddress);screen.header[0]='X';memory.Store(ScreenAddress,screen);assert(Run(memory).result==Publication::Refused&&!memory.appends);}
 {auto memory=Setup();memory.failWrite=1;assert(Run(memory).result==Publication::RolledBack);CheckOriginal(memory);assert(!memory.appends);}
 {auto memory=Setup();memory.partialWrite=1;assert(Run(memory).result==Publication::RolledBack);CheckOriginal(memory);assert(!memory.appends);}
 {auto memory=Setup();memory.foreignWrite=1;assert(Run(memory).result==Publication::OwnershipLost);assert(memory.bytes[FirstData-ScreenAddress+1]==uint8_t('Z'));}
 {auto memory=Setup();const auto result=Run(memory);auto screen=memory.Load<Screen32>(ScreenAddress);++screen.options.capacity;memory.Store(ScreenAddress,screen);assert(!result.receipt.Own(memory,ScreenAddress,1,result.receipt.first));}
 // Reserved names cannot select or silently run an unimplemented preset.
 const auto high=*preset117::Defaults(preset117::Preset::High);
 {Controller q(high);assert(q.QualityValue121()==L"High");
  assert(q.DispatchQuality121(Action::Next)==Change::Changed&&!q.Pending().enabled&&q.Applied()==high);
  assert(q.QualityValue121()==L"Off (restart required)");
  assert(q.Persist([](const preset117::State&s){return !s.enabled;}));
  assert(q.DispatchQuality121(Action::Next)==Change::Changed&&q.Pending()==*preset117::Defaults(preset117::Preset::Low));
  assert(q.DispatchQuality121(Action::Next)==Change::Changed&&q.Pending()==*preset117::Defaults(preset117::Preset::Medium));
  assert(q.DispatchQuality121(Action::Next)==Change::Changed&&q.Pending()==high);
  assert(q.DispatchQuality121(Action::Previous)==Change::Changed&&q.Pending()==*preset117::Defaults(preset117::Preset::Medium));
  assert(!q.Persist([](const preset117::State&){return false;}));assert(q.QualityValue121()==L"Medium (save failed)");
 }
 Controller controller(high);assert(controller.Saved()&&!controller.RestartRequired()&&controller.Value(Control::Status)==L"Applied");
 {Controller c(high);assert(c.Dispatch(Control::Enabled,Action::Activate)==Change::Changed&&!c.Pending().enabled&&c.Applied().enabled&&c.RestartRequired());
  const auto off=c.Pending();for(auto control:{Control::Preset,Control::DualHeadlights,Control::WorldLighting,Control::StreetReach,Control::Restore})assert(c.Dispatch(control,Action::Activate)==Change::None&&c.Pending()==off);
  assert(c.Value(Control::Enabled)==L"Off (restart required)"&&c.Value(Control::Preset)==L"Enable Liberty's Shadows");
  assert(c.Dispatch(Control::Save,Action::Activate)==Change::SaveRequested);
  assert(c.Persist([](const preset117::State& s){return !s.enabled;}));assert(c.Dispatch(Control::Enabled,Action::Activate)==Change::Changed&&c.Pending().enabled&&c.Pending().preset==high.preset);
 }
 assert(controller.Dispatch(Control::Preset,Action::Next)==Change::Changed);assert(controller.Pending().preset==preset117::Preset::Low);
 assert(controller.Applied()==high&&controller.RestartRequired());
 assert(controller.Dispatch(Control::Preset,Action::Next)==Change::Changed);assert(controller.Pending().preset==preset117::Preset::Medium);
 assert(controller.Dispatch(Control::Preset,Action::Next)==Change::Changed);assert(controller.Pending().preset==preset117::Preset::High);
 assert(controller.Dispatch(Control::Preset,Action::Previous)==Change::Changed);assert(controller.Pending().preset==preset117::Preset::Medium);
 for(unsigned i=0;i<5;++i){Controller c(high);const auto control=Control(unsigned(Control::DualHeadlights)+i);assert(c.Dispatch(control,Action::Activate)==Change::Changed);
  assert(c.Pending().preset==preset117::Preset::Custom&&!c.Pending().features[i]&&c.Applied()==high&&c.RestartRequired());
 }
 {Controller c(high);assert(c.Value(Control::StreetReach)==L"200 ft"&&c.Value(Control::HeadlightReach)==L"50 ft");
  assert(c.Dispatch(Control::StreetReach,Action::Next)==Change::Changed);assert(c.Pending().lampReachSteps==41);assert(c.Dispatch(Control::StreetReach,Action::Previous)==Change::Changed);assert(c.Dispatch(Control::StreetReach,Action::Previous)==Change::Changed);assert(c.Pending().lampReachSteps==39&&c.Value(Control::StreetReach)==L"195 ft");
  assert(c.Dispatch(Control::HeadlightReach,Action::Next)==Change::Changed);assert(c.Pending().headlightReachSteps==11&&c.Value(Control::HeadlightReach)==L"55 ft");
  assert(c.Dispatch(Control::Restore,Action::Activate)==Change::Changed);assert(c.Pending()==high&&c.Applied()==high);
 }
 {auto zero=high;zero.lampReachSteps=zero.headlightReachSteps=0;Controller c(zero);assert(c.Value(Control::StreetReach)==L"Original"&&c.Value(Control::HeadlightReach)==L"Original");assert(c.Dispatch(Control::StreetReach,Action::Previous)==Change::None);}
 {Controller c(high);c.Dispatch(Control::WorldLighting,Action::Activate);assert(c.Dispatch(Control::Save,Action::Next)==Change::None);assert(c.Dispatch(Control::Save,Action::Activate)==Change::SaveRequested);
  unsigned saves=0;assert(!c.Persist([&](const preset117::State& s){++saves;assert(s==c.Pending());return false;}));assert(saves==1&&c.SaveFailed()&&!c.Saved()&&c.Applied()==high);
  assert(c.Value(Control::Status)==L"Save failed; changes retained");assert(c.Persist([&](const preset117::State&){++saves;return true;}));assert(saves==2&&c.Saved()&&c.RestartRequired()&&c.Applied()==high);
  assert(c.Value(Control::Status)==L"Saved; restart required");
 }
 {auto invalid=high;invalid.preset=invalid.base=preset117::Preset(255);Controller c(invalid);assert(c.Dispatch(Control::Preset,Action::Activate)==Change::None);assert(c.Pending()==invalid);}
 assert(controller.Dispatch(Control::Status,Action::Activate)==Change::None);assert(controller.Dispatch(Control::Count,Action::Activate)==Change::None);
 {Controller c(high);assert(c.Dispatch(Control::WorldLighting,Action::Activate)==Change::Changed);const auto pending=c.Pending();auto actual=high;actual.features[0]=false;
  c.ObserveApplied119(actual,true,true,false);assert(c.Pending()==pending&&!c.Saved()&&c.Applied()==actual&&c.Value(Control::Status)==L"Enable FusionFix vehicle shadows");
  assert(c.Persist([](const preset117::State&){return true;}));c.ObserveApplied119(high,false,false,false);assert(c.Saved()&&c.Pending()==pending&&c.RestartRequired()&&c.Value(Control::Status)==L"Saved; restart required");
  c.ObserveApplied119(actual,true,false,true);assert(c.Value(Control::Status)==L"FusionFix shadow state unavailable"&&c.Pending()==pending&&c.Saved());
 }
 assert(controller.Dispatch(Control::Preset,Action(255))==Change::None);
 // Actual CE two-stage Process -> Scroll(return0) -> optional Select flow.
 // The supplied owned enum represents a fresh Receipt::Own result; production
 // wrappers still have to provide authenticated menu/current-screen context.
 {Controller c(high);InputRouter input;
  const auto outside=input.Scroll(c,Control::Preset,25,-92,1,ScreenAddress,1);assert(!outside.handled&&c.Pending()==high);
  InputRouter::Scope scope(input,0,ScreenAddress,1,true);
  const auto previous=input.Scroll(c,Control::Preset,25,-92,-1,ScreenAddress,1);assert(previous.handled&&previous.nativeReturn==0&&previous.change==Change::Changed);
  assert(c.Pending().preset==preset117::Preset::Medium&&scope.NeedsRefresh());
  const auto next=input.Scroll(c,Control::Preset,25,-92,1,ScreenAddress,1);assert(next.handled&&next.nativeReturn==0&&c.Pending().preset==preset117::Preset::High);
  const auto activate=input.Scroll(c,Control::WorldLighting,30,30,1,ScreenAddress,1);assert(activate.handled&&activate.change==Change::None&&c.Pending()==high);
  const auto selected=input.Select(c,Control::WorldLighting,0,30,0,ScreenAddress,1);assert(selected.handled&&selected.nativeReturn==0&&selected.change==Change::Changed);
  assert(!preset117::Enabled(c.Pending(),preset117::Feature::WorldLighting)&&c.Applied()==high);
  assert(input.Select(c,Control::WorldLighting,0,30,0,ScreenAddress,1).change==Change::None); // consume once
 }
 {Controller c(high);InputRouter input;InputRouter::Scope scope(input,0,ScreenAddress,1,true);
  for(const auto accepted:{-91,-90,-1}){assert(input.Scroll(c,Control::Preset,25,accepted,1,ScreenAddress,1).handled);assert(input.Select(c,Control::Preset,0,25,0,ScreenAddress,1).change==Change::None);}
  assert(input.Scroll(c,Control::Preset,25,-92,0,ScreenAddress,1).change==Change::None);assert(c.Pending()==high);
  assert(!input.Scroll(c,{},2,-92,1,ScreenAddress,1).handled);assert(!input.Scroll(c,Control::Preset,25,-92,1,ScreenAddress,2).handled);
  input.Scroll(c,Control::Preset,25,25,1,ScreenAddress,1);assert(input.Select(c,Control::Preset,0,25,1,ScreenAddress,1).change==Change::None);
  assert(input.Select(c,Control::Preset,0,25,0,ScreenAddress,1).change==Change::None);
  input.Scroll(c,Control::Preset,25,25,1,ScreenAddress,1);assert(!input.Select(c,Control::Preset,1,25,0,ScreenAddress,1).handled);
  input.Scroll(c,Control::Preset,25,25,1,ScreenAddress,1);assert(input.Select(c,Control::DualHeadlights,0,26,0,ScreenAddress,1).change==Change::None);assert(c.Pending()==high);
 }
 {Controller c(high);InputRouter input;InputRouter::Scope outer(input,0,ScreenAddress,1,true);
  input.Scroll(c,Control::WorldLighting,30,30,1,ScreenAddress,1);
  {InputRouter::Scope inner(input,1,ScreenAddress+24,2,true);
   assert(input.Select(c,Control::WorldLighting,1,30,0,ScreenAddress+24,2).change==Change::None);
   assert(input.Scroll(c,Control::Preset,25,-92,-1,ScreenAddress+24,2).change==Change::Changed);
  }
  assert(outer.NeedsRefresh());assert(input.Select(c,Control::WorldLighting,0,30,0,ScreenAddress,1).change==Change::Changed);
  assert(c.Pending().preset==preset117::Preset::Custom&&c.Applied()==high);
 }
 {Controller c(high);InputRouter input;InputRouter::Scope scope(input,0,ScreenAddress,1,false);
  assert(!input.Scroll(c,Control::Preset,25,-92,1,ScreenAddress,1).handled);assert(c.Pending()==high);
 }
 {Controller c(high);InputRouter input;
  {InputRouter::Scope scope(input,0,ScreenAddress,1,true);input.Scroll(c,Control::Save,34,34,1,ScreenAddress,1);assert(input.Select(c,Control::Save,0,34,0,ScreenAddress,1).change==Change::SaveRequested);}
  {InputRouter::Scope scope(input,0,ScreenAddress,1,true);assert(input.Select(c,Control::Save,0,34,0,ScreenAddress,1).change==Change::None);}
 }
 std::cout<<"native-graphics-menu117 POD/planner/allocator/publication/ownership/controller tests passed; runtime hooks and input/rendering flow uninstalled and unproved\n";
}
