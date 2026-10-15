#pragma once
#include <array>
#include <algorithm>
#include <string_view>
#include <cstdint>
namespace liberty_settings93 {
enum class Preset:uint8_t {Lowest,Medium,High,Ultra,Extreme,Custom};
enum class Feature:uint8_t {DualHeadlights,TailLights,TrafficSignals,EmergencyLights,WorldLighting,Count};
// WorldLighting controls the COMPLETE model/normalized-projection/glow path.
// Separate model/corona switches are not independent in the current runtime.
// Never show invented independent switches for their overlapping producers.
struct State {
 Preset preset=Preset::Ultra,base=Preset::Ultra;
 std::array<bool,size_t(Feature::Count)> features{true,true,true,true,true};
 unsigned lampReachSteps=40,headlightReachSteps=10;
 bool operator==(const State&) const=default;
};
struct PresetInfo {Preset value;std::wstring_view name,label,help;};
inline constexpr std::array<PresetInfo,5> Presets{{
 {Preset::Lowest,L"Lowest",L"Lowest - Original Fix",L"Night Shadows Fix CE 1.7 feature scope: the original headlight and streetlight fixes."},
 {Preset::Medium,L"Medium",L"Medium - Enhanced Classic",L"Enhanced Shadows CE 1.9 feature scope: original fixes plus traffic signals and emergency lights."},
 {Preset::High,L"High",L"High - Vehicle Enhancement",L"Adds dual headlight and taillight shadows, with world geometry occlusion."},
 {Preset::Ultra,L"Ultra",L"Ultra - Full Lighting",L"The full 2.0 lighting reference, including supported building, fixture and glow sources."},
 {Preset::Extreme,L"Extreme",L"Extreme - Maximum Reach",L"Full lighting with greater supported shadow reach. Most demanding; the engine shadow pool is unchanged."}
}};
struct FeatureInfo {Feature value;std::wstring_view key,label,help;bool restart;};
inline constexpr std::array<FeatureInfo,size_t(Feature::Count)> Features{{
 {Feature::DualHeadlights,L"DualHeadlights",L"Dual headlight shadows",L"Uses both physical headlights on supported vehicles. Original illumination stays enabled.",true},
 {Feature::TailLights,L"TaillightShadows",L"Taillight shadows",L"Adds supported taillight shadows without extinguishing the original taillights.",true},
 {Feature::TrafficSignals,L"TrafficSignalShadows",L"Traffic signal shadows",L"Adds shadows from supported active traffic signals.",true},
 {Feature::EmergencyLights,L"EmergencyLightShadows",L"Emergency light shadows",L"Adds shadows from supported emergency lights; replacement vehicles may differ.",true},
 {Feature::WorldLighting,L"WorldLighting",L"Building and fixture shadows",L"Enables the complete supported world-light enhancement, including building, fixture and glow sources.",true}
}};
constexpr State Defaults(Preset preset) noexcept {
 if(preset>Preset::Extreme)preset=Preset::Ultra;
 State s{};s.preset=s.base=preset;
 s.features[size_t(Feature::TrafficSignals)]=preset>=Preset::Medium;
 s.features[size_t(Feature::EmergencyLights)]=preset>=Preset::Medium;
 s.features[size_t(Feature::DualHeadlights)]=preset>=Preset::High;
 s.features[size_t(Feature::TailLights)]=preset>=Preset::High;
 s.features[size_t(Feature::WorldLighting)]=preset>=Preset::Ultra;
 if(preset==Preset::Extreme){s.lampReachSteps=80;s.headlightReachSteps=30;}
 return s;
}
constexpr bool Enabled(const State& s,Feature f) noexcept {return f<Feature::Count&&s.features[size_t(f)];}
inline void Change(State& s,Feature f,bool enabled) noexcept {
 if(f>=Feature::Count)return;
 s.features[size_t(f)]=enabled;s.preset=Preset::Custom;
}
inline void ChangeReach(State& s,bool street,unsigned steps) noexcept {
 (street?s.lampReachSteps:s.headlightReachSteps)=std::min(steps,100u);s.preset=Preset::Custom;
}
inline void RestorePreset(State& s) noexcept {s=Defaults(s.base);}
inline void ResetDefaults(State& s) noexcept {s=Defaults(Preset::Ultra);}
inline bool RestartRequired(const State& applied,const State& pending) noexcept {
 return applied.features!=pending.features||applied.lampReachSteps!=pending.lampReachSteps||applied.headlightReachSteps!=pending.headlightReachSteps;
}
struct RuntimeRequests {
 bool dual{},tail{},traffic{},emergency{},model{},corona{},projection{},keyedWorld{},worldGeometry{};
 unsigned lampReach{},headlightReach{};
};
constexpr RuntimeRequests Requests(const State& s) noexcept {
 const bool world=Enabled(s,Feature::WorldLighting),vehicle=Enabled(s,Feature::DualHeadlights)||Enabled(s,Feature::TailLights);
 return {Enabled(s,Feature::DualHeadlights),Enabled(s,Feature::TailLights),Enabled(s,Feature::TrafficSignals),
  Enabled(s,Feature::EmergencyLights),world,world,world,vehicle||world,vehicle||world,s.lampReachSteps,s.headlightReachSteps};
}
}
