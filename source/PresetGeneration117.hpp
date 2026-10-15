#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <optional>
namespace preset117 {
// Retain the retired VeryHigh enum slot only for old-schema/migration references; never selectable.
enum class Preset:uint8_t {Low,Medium,High,VeryHigh,Custom};
inline constexpr Preset LastNamedPreset122=Preset::High;
inline constexpr unsigned NamedPresetCount122=unsigned(LastNamedPreset122)+1;
enum class Engine:uint8_t {None,Shared};
enum class Feature:uint8_t {DualHeadlights,TailLights,TrafficSignals,EmergencyLights,WorldLighting,Count};
inline constexpr unsigned Schema=3;
struct State {
 Preset preset=Preset::High,base=Preset::High;
 std::array<bool,size_t(Feature::Count)> features{true,true,true,true,true};
 unsigned lampReachSteps=40,headlightReachSteps=10;
 bool enabled=true;
 bool operator==(const State&) const=default;
};
constexpr const wchar_t* Name(Preset p) noexcept {
 switch(p){case Preset::Low:return L"Low";case Preset::Medium:return L"Medium";case Preset::High:return L"High";case Preset::VeryHigh:return L"VeryHigh";case Preset::Custom:return L"Custom";}return L"High";
}
constexpr bool Implemented(Preset p) noexcept {return p<=LastNamedPreset122;}
constexpr bool Enabled(const State& s,Feature f) noexcept {return s.enabled&&f<Feature::Count&&s.features[size_t(f)];}
inline std::optional<State> Defaults(Preset p) noexcept {
 if(!Implemented(p))return {};
 State s{};s.preset=s.base=p;
 if(p<Preset::High)s.features={false,false,p==Preset::Medium,p==Preset::Medium,false};
 return s;
}
inline Engine Implementation(const State& s) noexcept {return s.enabled&&(Implemented(s.preset)||(s.preset==Preset::Custom&&Implemented(s.base)))?Engine::Shared:Engine::None;}
inline bool Select(State& s,Preset p) noexcept {auto next=Defaults(p);if(!next)return false;next->enabled=s.enabled;s=*next;return true;}
inline void Change(State& s,Feature f,bool value) noexcept {if(f>=Feature::Count)return;s.features[size_t(f)]=value;s.preset=Preset::Custom;}
inline bool ChangeReach(State& s,bool street,unsigned steps) noexcept {if(steps>100)return false;(street?s.lampReachSteps:s.headlightReachSteps)=steps;s.preset=Preset::Custom;return true;}
inline bool RestartRequired(const State& applied,const State& pending) noexcept {
 return applied.enabled!=pending.enabled||applied.features!=pending.features||applied.lampReachSteps!=pending.lampReachSteps||applied.headlightReachSteps!=pending.headlightReachSteps;
}
struct RuntimeRequests {
 bool base{},dual{},tail{},traffic{},emergency{},model{},corona{},projection{},keyedWorld{},worldGeometry{};
 unsigned lampReach{},headlightReach{};
};
constexpr RuntimeRequests Requests(const State& s) noexcept {
 if(!s.enabled)return {};
 const bool world=Enabled(s,Feature::WorldLighting),vehicle=Enabled(s,Feature::DualHeadlights)||Enabled(s,Feature::TailLights);
 return {true,Enabled(s,Feature::DualHeadlights),Enabled(s,Feature::TailLights),Enabled(s,Feature::TrafficSignals),Enabled(s,Feature::EmergencyLights),world,world,world,vehicle||world,vehicle||world,s.lampReachSteps,s.headlightReachSteps};
}
// One process, one shared renderer. A pending menu edit cannot reacquire or
// release this lease; true startup Off runs frontend-only before any service.
class StartupLease {
 std::atomic<bool> acquired_{false};
public:
 bool Acquire() noexcept {bool expected=false;return acquired_.compare_exchange_strong(expected,true,std::memory_order_acq_rel);}
 bool Active() const noexcept {return acquired_.load(std::memory_order_acquire);}
};
template<class Frontend,class Shadow> uint32_t RouteStartup117(const State& s,StartupLease& lease,Frontend frontend,Shadow shadow) {
 if(!s.enabled)return lease.Active()?1u:uint32_t(frontend());
 if(Implementation(s)==Engine::None||!lease.Acquire())return 1u;
 return uint32_t(shadow());
}
}
