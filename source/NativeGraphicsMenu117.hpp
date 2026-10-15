#pragma once
#include "PresetGeneration117.hpp"
#include "LiveReach122.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>

// CPU planning/backend only. This header installs no hooks and performs no
// native lookup. The caller must authenticate frontend ownership, the current
// screen/generation, append ABI and existing hook chains before publication.
namespace native_graphics117 {
struct Array32 {uint32_t data{};uint16_t count{},capacity{};bool operator==(const Array32&) const=default;};
struct Screen32 {char header[16]{};Array32 options;};
struct Option {uint8_t action{};char label[16]{};uint8_t padding{};int16_t preference{};uint8_t scaler{},display{};};
static_assert(sizeof(Array32)==8&&sizeof(Screen32)==24&&sizeof(Option)==22);
static_assert(offsetof(Option,preference)==18&&offsetof(Option,scaler)==20&&offsetof(Option,display)==21);
constexpr unsigned RowLimit=50;
enum class Control:uint8_t {Enabled,Preset,DualHeadlights,TailLights,TrafficSignals,EmergencyLights,WorldLighting,StreetReach,HeadlightReach,Restore,Save,Status,Count};
constexpr size_t ControlCount=size_t(Control::Count);
// One native spacer replacement; semantic advanced controls remain in the INI.
constexpr size_t PublishedControlCount121=1;
constexpr Control PublishedControl121(size_t) noexcept{return Control::Preset;}
inline constexpr std::array<std::string_view,ControlCount> Keys{
 "OO_LS_ENABLED","OO_LS_PRESET","OO_LS_DUAL","OO_LS_TAIL","OO_LS_TRAFFIC","OO_LS_EMERGENCY","OO_LS_WORLD","OO_LS_STREET","OO_LS_HEADREACH","OO_LS_RESTORE","OO_LS_SAVE","OO_LS_STATUS"};
inline constexpr std::array<std::wstring_view,ControlCount> Labels{
 L"Liberty's Shadows",L"Liberty's Shadows",L"Dual headlight shadows",L"Taillight shadows",L"Traffic signal shadows",L"Emergency light shadows",L"Building and fixture shadows",L"Street shadow reach",L"Headlight shadow reach",L"Restore this preset",L"Save lighting settings",L"Lighting settings status"};
inline Option Row(Control control) noexcept {
 Option out{};if(control>=Control::Count)return out;
 out.action=127;out.preference=-1;out.scaler=1;out.display=100;
 const auto key=Keys[size_t(control)];std::memcpy(out.label,key.data(),key.size());return out;
}
inline std::optional<Control> Stamp(const Option& row) noexcept {
 if(!std::memchr(row.label,0,sizeof(row.label)))return {};
 for(size_t i=0;i<ControlCount;++i){const auto expected=Row(Control(i));if(!std::memcmp(&row,&expected,sizeof(row)))return Control(i);}
 return {};
}
inline bool ReservedLabel(const Option& row) noexcept {
 const auto end=static_cast<const char*>(std::memchr(row.label,0,sizeof(row.label)));
 if(!end)return true;
 const std::string_view key(row.label,size_t(end-row.label));
 return std::find(Keys.begin(),Keys.end(),key)!=Keys.end();
}
inline bool Graphics(const Screen32& screen) noexcept {
 constexpr std::array<char,16> expected{'M','H','_','G','F','X'};
 return !std::memcmp(screen.header,expected.data(),expected.size());
}
inline bool Range(uint32_t address,size_t bytes) noexcept {
 return address>=0x10000&&bytes&&uint64_t(address)+bytes<=uint64_t(UINT32_MAX)+1;
}
inline bool ArrayValid(const Array32& a) noexcept {
 return a.count&&a.count<=RowLimit&&a.count<=a.capacity&&!(a.data&1u)&&Range(a.data,size_t(a.capacity)*sizeof(Option));
}
// FF511 XML and authenticated native tables49/62: NONE blank PREF_NULL,
// scaler0/displayNONE100 between Antialiasing and Volumetric Fog. Ignore only
// unused label tail/padding; retain all original bytes for contained rollback.
inline bool Label123(const Option& row,std::string_view expected) noexcept {
 const auto* end=static_cast<const char*>(std::memchr(row.label,0,16));
 return end&&std::string_view(row.label,size_t(end-row.label))==expected;
}
inline bool Spacer123(const Option& row) noexcept {
 return row.action==0&&row.label[0]==0&&row.preference==0&&row.scaler==0&&row.display==100;
}
inline bool SpacerContext123(std::span<const Option> rows,size_t at) noexcept {
 return at>0&&at+1<rows.size()&&rows[at-1].action==1&&rows[at+1].action==1&&
  Label123(rows[at-1],"Antialiasing")&&Label123(rows[at+1],"Volumetric Fog");
}
struct Plan {std::array<Option,RowLimit> rows{};uint16_t count{},first{};bool already{};};
inline std::optional<Plan> MakePlan(std::span<const Option> original,bool ownedGeneration=false) noexcept {
 if(original.empty()||original.size()>RowLimit||original.back().action!=46)return {};
 size_t first=original.size(),found=0;
 for(size_t i=0;i<original.size();++i){
  if(!std::memchr(original[i].label,0,16))return {};
  if(i+1<original.size()&&original[i].action==46)return {};
  if(ReservedLabel(original[i])){if(!Stamp(original[i]))return {};first=std::min(first,i);++found;}
 }
 Plan plan{};
 if(found){
  if(!ownedGeneration||found!=PublishedControlCount121||first+PublishedControlCount121>=original.size()||!SpacerContext123(original,first))return {};
  for(size_t i=0;i<PublishedControlCount121;++i){const auto expected=Row(PublishedControl121(i));if(std::memcmp(&original[first+i],&expected,sizeof(expected)))return {};}
  std::copy(original.begin(),original.end(),plan.rows.begin());plan.count=uint16_t(original.size());plan.first=uint16_t(first);plan.already=true;return plan;
 }
 size_t replacement=original.size();
 for(size_t i=1;i+1<original.size();++i)if(Spacer123(original[i])&&SpacerContext123(original,i)){
  if(replacement!=original.size())return {};replacement=i;
 }
 if(replacement==original.size())return {}; // Refuse unknown layouts; never add an overflowing row.
 std::copy(original.begin(),original.end(),plan.rows.begin());plan.first=uint16_t(replacement);
 plan.rows[replacement]=Row(Control::Preset);plan.count=uint16_t(original.size());return plan;
}
struct Receipt {
 uint32_t screen{};uint64_t generation{};Array32 array{};uint16_t first{};
 bool active{};
 template<class Memory> std::optional<Control> Own(Memory& memory,uint32_t currentScreen,uint64_t currentGeneration,unsigned row) const noexcept {
  if(!active||screen!=currentScreen||generation!=currentGeneration||row<first||row>=unsigned(first)+PublishedControlCount121)return {};
  Screen32 now{};if(!Range(screen,sizeof(now))||!memory.Read(screen,&now,sizeof(now))||!Graphics(now)||!(now.options==array)||!ArrayValid(now.options))return {};
  const auto control=PublishedControl121(row-first);const auto expected=Row(control);Option actual{};
  const auto address=uint64_t(array.data)+uint64_t(row)*sizeof(Option);
  if(address>UINT32_MAX||!memory.Read(uint32_t(address),&actual,sizeof(actual))||std::memcmp(&actual,&expected,sizeof(actual)))return {};
  return control;
 }
};
enum class Publication:uint8_t {Refused,Published,AlreadyPublished,RolledBack,OwnershipLost};
struct PublishResult {Publication result=Publication::Refused;Receipt receipt{};};

// Memory.Read/Write must perform fresh bounded permission checks and contain
// faults. Append is the authenticated native __thiscall Option allocator,
// called on the frontend owner thread inside an exclusive publication scope.
// Native capacity/data may change; old data pointers are never restored.
// ownedGeneration may only be supplied from an authenticated lifetime ledger.
template<class Memory,class Append>
PublishResult Publish(Memory& memory,uint32_t screenAddress,uint64_t generation,bool ownership, bool ownedGeneration,Append&& append) noexcept {
 PublishResult result{};if(!ownership||!generation||!Range(screenAddress,sizeof(Screen32)))return result;
 Screen32 screen{};if(!memory.Read(screenAddress,&screen,sizeof(screen))||!Graphics(screen)||!ArrayValid(screen.options))return result;
 std::array<Option,RowLimit> original{};
 if(!memory.Read(screen.options.data,original.data(),size_t(screen.options.count)*sizeof(Option)))return result;
 const auto plan=MakePlan(std::span(original).first(screen.options.count),ownedGeneration);if(!plan)return result;
 auto receipt=[&](const Array32& array){return Receipt{screenAddress,generation,array,plan->first,true};};
 if(plan->already){result.result=Publication::AlreadyPublished;result.receipt=receipt(screen.options);return result;}
 const auto before=screen.options;auto current=before;bool changed=false;
 auto readCurrent=[&](Screen32& actual){return memory.Read(screenAddress,&actual,sizeof(actual))&&Graphics(actual)&&actual.options==current&&ArrayValid(actual.options);};
 auto rollback=[&]() {
  Screen32 actual{};if(!readCurrent(actual)){result.result=Publication::OwnershipLost;return;}
  std::array<Option,RowLimit> observed{};
  if(!memory.Read(current.data,observed.data(),size_t(current.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return;}
  // Permit a contained partial final write, but never overwrite unrelated
  // row changes. Every byte must still be from our before/staged/final image.
  const auto* actualBytes=reinterpret_cast<const uint8_t*>(observed.data());
  const auto* plannedBytes=reinterpret_cast<const uint8_t*>(plan->rows.data());
  // Appended tail bytes are deliberately not restored or interpreted: an
  // allocator may leave the last returned element uninitialized on refusal.
  for(unsigned row=0;row<before.count;++row){
   const auto* beforeBytes=reinterpret_cast<const uint8_t*>(&original[row]);
   for(size_t byte=0;byte<sizeof(Option);++byte){const auto offset=size_t(row)*sizeof(Option)+byte;
    if(actualBytes[offset]!=beforeBytes[byte]&&actualBytes[offset]!=plannedBytes[offset]){result.result=Publication::OwnershipLost;return;}
   }
  }
  // The caller's exclusive frontend scope owns these writes. Restore only
  // the original rows/count in the current game-owned allocation.
  if(plan->count==before.count){
   const auto rowAddress=current.data+uint32_t(plan->first)*sizeof(Option);
   if(!memory.Write(rowAddress,&original[plan->first],sizeof(Option))){result.result=Publication::OwnershipLost;return;}
  }else if(!memory.Write(current.data,original.data(),size_t(before.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return;}
  const auto countAddress=screenAddress+uint32_t(offsetof(Screen32,options)+offsetof(Array32,count));
  if(plan->count!=before.count&&!memory.Write(countAddress,&before.count,sizeof(before.count))){result.result=Publication::OwnershipLost;return;}
  Screen32 restored{};std::array<Option,RowLimit> check{};
  auto restoredArray=current;restoredArray.count=before.count;
  if(!memory.Read(screenAddress,&restored,sizeof(restored))||!Graphics(restored)||!(restored.options==restoredArray)||
     !memory.Read(restoredArray.data,check.data(),size_t(before.count)*sizeof(Option))||std::memcmp(check.data(),original.data(),size_t(before.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return;}
  result.result=Publication::RolledBack;
 };
 for(size_t i=before.count;i<plan->count;++i){
  Screen32 check{};std::array<Option,RowLimit> prefix{};
  if(!readCurrent(check)||!memory.Read(current.data,prefix.data(),size_t(current.count)*sizeof(Option))){if(changed)rollback();return result;}
  if(std::memcmp(prefix.data(),original.data(),size_t(before.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
  for(unsigned n=before.count;n<current.count;++n)if(std::memcmp(&prefix[n],&original[before.count-1],sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
  uint32_t returned{};
  try{returned=append(screenAddress+uint32_t(offsetof(Screen32,options)),0);}
  catch(...){result.result=Publication::OwnershipLost;return result;}
  Screen32 after{};
  if(!memory.Read(screenAddress,&after,sizeof(after))||!Graphics(after)||!ArrayValid(after.options)||after.options.count!=uint16_t(current.count+1)||after.options.capacity<current.capacity){result.result=Publication::OwnershipLost;return result;}
  current=after.options;changed=true;
  const auto expected=uint64_t(current.data)+uint64_t(current.count-1)*sizeof(Option);
  if(expected>UINT32_MAX||returned!=uint32_t(expected)){rollback();return result;}
  if(!memory.Read(current.data,prefix.data(),size_t(current.count-1)*sizeof(Option))||std::memcmp(prefix.data(),original.data(),size_t(before.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
  for(unsigned n=before.count;n+1<current.count;++n)if(std::memcmp(&prefix[n],&original[before.count-1],sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
  if(!memory.Write(uint32_t(expected),&original[before.count-1],sizeof(Option))){rollback();return result;}
 }
 Screen32 check{};if(!readCurrent(check)){result.result=Publication::OwnershipLost;return result;}
 std::array<Option,RowLimit> beforeWrite{};
 if(!memory.Read(current.data,beforeWrite.data(),size_t(current.count)*sizeof(Option))||std::memcmp(beforeWrite.data(),original.data(),size_t(before.count)*sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
 const auto writeAddress=current.data+uint32_t(plan->first)*sizeof(Option);
 if(!memory.Write(writeAddress,&plan->rows[plan->first],sizeof(Option))){rollback();return result;}
 std::array<Option,RowLimit> final{};
 if(!readCurrent(check)||!memory.Read(current.data,final.data(),size_t(plan->count)*sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
 if(std::memcmp(final.data(),plan->rows.data(),size_t(plan->count)*sizeof(Option))){result.result=Publication::OwnershipLost;return result;}
 result.result=Publication::Published;result.receipt=receipt(current);return result;
}

enum class Action:uint8_t {Previous,Next,Activate};
enum class Change:uint8_t {None,Changed,SaveRequested};
class Controller {
 preset117::State applied_{},pending_{};
 bool saved_=true,saveFailed_=false,backendLimited_=false,vehicleDepthUnavailable119_=false,backendReadFailed119_=false;
 static std::optional<preset117::Feature> Feature(Control control) noexcept {
  if(control<Control::DualHeadlights||control>Control::WorldLighting)return {};
  return preset117::Feature(unsigned(control)-unsigned(Control::DualHeadlights));
 }
public:
 explicit Controller(preset117::State applied) noexcept:applied_(applied),pending_(applied){}
 Controller(preset117::State applied,preset117::State pending,bool limited) noexcept:applied_(applied),pending_(pending),backendLimited_(limited){}
 void ObserveApplied119(const preset117::State& applied,bool limited,bool vehicleUnavailable,bool unreadable) noexcept {
  applied_=applied;backendLimited_=limited;vehicleDepthUnavailable119_=vehicleUnavailable;backendReadFailed119_=unreadable;
  // External observations never overwrite pending edits or save state.
 }
 Change DispatchQuality121(Action action) noexcept {
  if(action>Action::Activate)return Change::None;
  const auto base=preset117::Implemented(pending_.preset)?pending_.preset:pending_.base;
  if(!preset117::Implemented(base))return Change::None;
  constexpr unsigned count=preset117::NamedPresetCount122+1u; // Off + three implemented tiers.
  const unsigned current=pending_.enabled?unsigned(base)+1u:0u;
  const unsigned selected=action==Action::Previous?(current+count-1u)%count:(current+1u)%count;
  auto next=pending_;
  if(!selected)next.enabled=false;
  else {next=*preset117::Defaults(preset117::Preset(selected-1u));next.enabled=true;}
  if(next==pending_)return Change::None;
  pending_=next;saved_=false;saveFailed_=false;return Change::Changed;
 }
 std::wstring QualityValue121() const {
  std::wstring value=pending_.enabled?preset117::Name(pending_.preset):L"Off";
  if(saveFailed_)return value+L" (save failed)";
  if(backendReadFailed119_)return value+L" (FusionFix state unavailable)";
  if(vehicleDepthUnavailable119_)return value+L" (enable FF vehicle shadows)";
  if(backendLimited_)return value+L" (backend limited)";
  if(!RestartRequired())return value;
  const bool reachOnly=pending_.enabled&&applied_.enabled&&pending_.features==applied_.features;
  return value+(reachOnly&&saved_?L" (applying)":L" (restart required)");
 }
 const preset117::State& Applied() const noexcept{return applied_;}
 const preset117::State& Pending() const noexcept{return pending_;}
 bool RestartRequired() const noexcept{return preset117::RestartRequired(applied_,pending_);}
 bool Saved() const noexcept{return saved_;}
 bool SaveFailed() const noexcept{return saveFailed_;}
 Change Dispatch(Control control,Action action) noexcept {
  if(control>=Control::Count||action>Action::Activate||control==Control::Status)return Change::None;
  if(control==Control::Save)return action==Action::Activate?Change::SaveRequested:Change::None;
  if(!pending_.enabled&&control!=Control::Enabled)return Change::None;
  auto next=pending_;
  if(control==Control::Enabled){next.enabled=!next.enabled;}
  else if(control==Control::Preset){
   const auto base=preset117::Implemented(next.preset)?next.preset:next.base;
   if(!preset117::Implemented(base))return Change::None;
   constexpr unsigned count=preset117::NamedPresetCount122;
   const unsigned value=unsigned(base),selected=action==Action::Previous?(value+count-1u)%count:(value+1u)%count;
   if(!preset117::Select(next,preset117::Preset(selected)))return Change::None;
  }else if(const auto feature=Feature(control)){preset117::Change(next,*feature,!preset117::Enabled(next,*feature));}
  else if(control==Control::StreetReach||control==Control::HeadlightReach){
   const bool street=control==Control::StreetReach;const auto old=street?next.lampReachSteps:next.headlightReachSteps;
   if(old>100)return Change::None;
   const auto value=action==Action::Previous?(old?old-1:0):std::min(old+1,100u);
   if(value==old||!preset117::ChangeReach(next,street,value))return Change::None;
  }else if(control==Control::Restore){
   if(action!=Action::Activate)return Change::None;
   const auto restored=preset117::Defaults(next.base);if(!restored)return Change::None;next=*restored;
  }
  if(next==pending_)return Change::None;
  pending_=next;saved_=false;saveFailed_=false;return Change::Changed;
 }
 // The callback uses PresetIni117's serializer/owned INI path. The active
 // startup engine and every applied lighting request remain unchanged.
 template<class Save> bool Persist(Save&& save) noexcept {
  if(!save(pending_)){saveFailed_=true;saved_=false;return false;}
  saveFailed_=false;saved_=true;preset117::QueueLiveReach122(pending_);return true;
 }
 std::wstring Value(Control control) const {
  if(control>=Control::Count)return {};
  if(control==Control::Enabled){
   if(pending_.enabled!=applied_.enabled)return pending_.enabled?L"On (restart required)":L"Off (restart required)";
   return pending_.enabled?L"On":L"Off";
  }
  if(!pending_.enabled&&control!=Control::Save&&control!=Control::Status)return L"Enable Liberty's Shadows";
  if(control==Control::Preset)return preset117::Name(pending_.preset);
  if(const auto feature=Feature(control))return preset117::Enabled(pending_,*feature)?L"On":L"Off";
  if(control==Control::StreetReach||control==Control::HeadlightReach){
   const auto steps=control==Control::StreetReach?pending_.lampReachSteps:pending_.headlightReachSteps;
   return steps?std::to_wstring(uint64_t(steps)*5u)+L" ft":L"Original";
  }
  if(control==Control::Restore||control==Control::Save)return L"Select";
  if(saveFailed_)return L"Save failed; changes retained";
  if(backendReadFailed119_)return L"FusionFix shadow state unavailable";
  if(vehicleDepthUnavailable119_)return L"Enable FusionFix vehicle shadows";
  if(backendLimited_)return L"Backend limited; restart required";
  if(!saved_)return RestartRequired()?L"Unsaved; restart required":L"Unsaved";
  return RestartRequired()?L"Saved; restart required":L"Applied";
 }
};

struct InputResult {bool handled{};uint8_t nativeReturn{};Change change=Change::None;};
// Thread-private event model for the authenticated CE Process call. Native
// Scroll(owned row) must return zero: a nonzero result reaches native negative
// preference reads/writes. This model never modifies the adjusted byte.
class InputRouter {
 struct Frame {
  int32_t menu=-1;uint32_t screen{};uint64_t generation{};
  unsigned depth{};bool active{},refresh{};
  int32_t pendingRow=-1;std::optional<Control> pendingControl;
 } frame_;
public:
 static constexpr int32_t NoAcceptedEvent=-92;
 class Scope {
  InputRouter& router_;Frame previous_;
 public:
  Scope(InputRouter& router,int32_t menu,uint32_t screen,uint64_t generation,bool authenticated) noexcept:router_(router),previous_(router.frame_){
   router.frame_={};router.frame_.depth=previous_.depth+1;
   router.frame_.menu=menu;router.frame_.screen=screen;router.frame_.generation=generation;
   router.frame_.active=authenticated&&previous_.depth<8&&menu>=0&&Range(screen,sizeof(Screen32))&&generation;
  }
  ~Scope(){const auto refresh=router_.frame_.refresh;router_.frame_=previous_;if(router_.frame_.active)router_.frame_.refresh|=refresh;}
  Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
  bool NeedsRefresh() const noexcept{return router_.frame_.refresh;}
 };
 InputResult Scroll(Controller& controller,std::optional<Control> owned,int32_t row,int32_t accepted,int32_t direction,uint32_t screen,uint64_t generation,bool quality121=false) noexcept {
  if(!owned||*owned>=Control::Count||row<0||!frame_.active||screen!=frame_.screen||generation!=frame_.generation)return {};
  frame_.pendingRow=-1;frame_.pendingControl.reset();
  InputResult result{true,0,Change::None};
  if(*owned==Control::Status)return result;
  if(accepted>=0){frame_.pendingRow=row;frame_.pendingControl=*owned;return result;}
  if(accepted!=NoAcceptedEvent||!direction)return result;
  result.change=quality121?controller.DispatchQuality121(direction<0?Action::Previous:Action::Next):controller.Dispatch(*owned,direction<0?Action::Previous:Action::Next);
  frame_.refresh|=result.change==Change::Changed;return result;
 }
 InputResult Select(Controller& controller,std::optional<Control> owned,int32_t menu,int32_t row,uint8_t adjusted,uint32_t screen,uint64_t generation,bool quality121=false) noexcept {
  if(!owned||*owned>=Control::Count||row<0||!frame_.active||menu!=frame_.menu||screen!=frame_.screen||generation!=frame_.generation)return {};
  InputResult result{true,0,Change::None};
  const bool activate=!adjusted&&frame_.pendingRow==row&&frame_.pendingControl==owned;
  frame_.pendingRow=-1;frame_.pendingControl.reset();
  if(activate)result.change=quality121?controller.DispatchQuality121(Action::Activate):controller.Dispatch(*owned,Action::Activate);
  frame_.refresh|=result.change!=Change::None;return result;
 }
};
}
