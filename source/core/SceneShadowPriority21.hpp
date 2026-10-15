#pragma once
#include "ShadowViewPriority.hpp"
#include <array>
#include <cstdint>
namespace fusionfix::shadows {
// Exact existing photometric term shared by current policy and projection
// alias representative selection. This ranks maps; it never changes radiance.
inline float SceneBrightness44(float intensity,Vec3 rgb) noexcept {
 return std::clamp(intensity,0.f,1000000.f)*std::clamp(
    .6f*(.2126f*rgb.x+.7152f*rgb.y+.0722f*rgb.z)+.4f*std::max({rgb.x,rgb.y,rgb.z}),0.f,4.f);
}
inline float SceneReceiverCoverage21(Vec3 receiver,Vec3 source,Vec3 direction,int type,float outerCos) noexcept {
 if(type==0)return 1.f;
 if(type!=2||!std::isfinite(outerCos)||outerCos<=0||outerCos>1)return 0.f;
 const float x=receiver.x-source.x,y=receiver.y-source.y,z=receiver.z-source.z;
 const float n2=direction.x*direction.x+direction.y*direction.y+direction.z*direction.z;
 const float d2=x*x+y*y+z*z;
 if(!std::isfinite(n2)||n2<.0001f||!std::isfinite(d2))return 0.f;
 const float axial=(x*direction.x+y*direction.y+z*direction.z)/std::sqrt(n2);
 const float radial=std::sqrt(std::max(0.f,d2-axial*axial));
 const float boundary=radial*outerCos-axial*std::sqrt(std::max(0.f,1.f-outerCos*outerCos));
 if(!std::isfinite(boundary)||!std::isfinite(axial))return 0.f;
 // Smooth three-metre receiver extent; native cone stores cosine, not degrees.
 return SmoothUnit((3.f-boundary)/6.f)*SmoothUnit((axial+3.f)/6.f);
}
// Estimate the consequence of removing a CURRENT light's shadow. This is a
// receiver/photometric heuristic, not ray tracing, depth visibility, or native
// falloff replacement. It never changes emission, eligibility, maps or owners.
inline float SceneShadowImportance21(float playerDistanceSquared,float roadDistanceSquared,
 float radius,float intensity,Vec3 rgb,float viewWeight,bool volumeVisible,float strength,
 float playerCoverage=1.f,float roadCoverage=1.f) noexcept {
 if(!volumeVisible||!std::isfinite(strength)||strength<=0||!std::isfinite(radius)||radius<=0||
    !std::isfinite(playerDistanceSquared)||playerDistanceSquared<0||
    !std::isfinite(roadDistanceSquared)||roadDistanceSquared<0)return 1.f;
 const float luminance=.2126f*rgb.x+.7152f*rgb.y+.0722f*rgb.z;
 const float peak=std::max({rgb.x,rgb.y,rgb.z});
 if(!std::isfinite(luminance)||!std::isfinite(peak)||!std::isfinite(intensity)||intensity<=0)return 1.f;
 // Saturated blue/red fixtures must not lose all priority to luminance alone.
 const float brightness=std::min(1000000.f,intensity)*std::clamp(.6f*luminance+.4f*peak,0.f,4.f);
 const float contrast=brightness/(brightness+4.f);
 auto influence=[&](float distanceSquared){
  const float distance=std::sqrt(distanceSquared);
  const float receiver=SmoothUnit((radius+3.f-distance)/std::max(3.f,radius));
  const float local=1.f-SmoothUnit((distance-8.f)/32.f);
  return receiver*local;
 };
 const float receivers=std::max(influence(playerDistanceSquared)*playerCoverage,
                              .85f*influence(roadDistanceSquared)*roadCoverage);
 // Conservative volume visibility protects offscreen fixtures whose influence
 // is visible even when sparse projected samples miss their receiver patch.
 const float visible=.25f+.75f*std::clamp((viewWeight-1.f)*.5f,0.f,1.f);
 return 1.f+std::clamp(strength,0.f,4.f)*contrast*receivers*visible;
}
// Receiver-consequence ranking44: authored radius and cone shape determine
// receiver support. This estimate ranks maps only; it is not native attenuation,
// visibility, emission, or permission to suppress a source/caster.
struct SceneReceiverConsequence44 {
 bool valid=false;
 float subjectSupport=0.f,roadSupport=0.f,consequence=0.f,score=0.f;
};
inline float SceneRadiusSupport44(float distanceSquared,float radius,float extent) noexcept {
 if(!std::isfinite(distanceSquared)||distanceSquared<0||!std::isfinite(radius)||radius<=0||
    !std::isfinite(extent)||extent<0)return 0.f;
 const float nearest=std::max(0.f,std::sqrt(distanceSquared)-extent);
 return SmoothUnit(1.f-nearest/radius);
}
inline SceneReceiverConsequence44 SceneConsequence44(float subjectDistanceSquared,float roadDistanceSquared,
 float radius,float intensity,Vec3 rgb,float viewWeight,bool volumeVisible,bool driving,
 float subjectCoverage,float roadCoverage,float strength) noexcept {
 SceneReceiverConsequence44 r{};
 if(!std::isfinite(subjectDistanceSquared)||subjectDistanceSquared<0||
    !std::isfinite(roadDistanceSquared)||roadDistanceSquared<0||!std::isfinite(radius)||radius<=0||
    !std::isfinite(intensity)||intensity<=0||!std::isfinite(rgb.x)||!std::isfinite(rgb.y)||!std::isfinite(rgb.z)||
    !std::isfinite(viewWeight)||!std::isfinite(subjectCoverage)||!std::isfinite(roadCoverage)||
    !std::isfinite(strength)||strength<=0)return r;
 const float brightness=SceneBrightness44(intensity,rgb);
 const float contrast=brightness/(brightness+4.f);
 r.subjectSupport=SceneRadiusSupport44(subjectDistanceSquared,radius,driving?3.f:1.5f)*std::clamp(subjectCoverage,0.f,1.f);
 r.roadSupport=driving?SceneRadiusSupport44(roadDistanceSquared,radius,3.f)*std::clamp(roadCoverage,0.f,1.f):0.f;
 // Camera origin projection never multiplies central support. An offscreen
 // rear fixture still owns its visible shadow on the central receiver.
 const float visible=volumeVisible?(.25f+.75f*std::clamp((viewWeight-1.f)*.5f,0.f,1.f)):0.f;
 r.consequence=contrast*std::clamp(strength,0.f,4.f)*(4.f*r.subjectSupport+1.25f*r.roadSupport);
 // Smaller influence on peripheral scenery can use spare slots. Its rank is
 // capped below supported central effects and uses radius-normalized distance;
 // outside-radius subject/road support stays exactly zero.
 const float normalized=subjectDistanceSquared/(radius*radius+9.f);
 const float background=.025f*visible/(1.f+normalized);
 r.score=1.f/(.01f+r.consequence+background)+.001f*std::min(normalized,1000000.f);
 r.valid=std::isfinite(r.score)&&std::isfinite(r.consequence);
 return r;
}
class SceneShadowHistory21 {
 struct Entry {uint32_t key{},frame{},time{};uint64_t generation{};float importance=1.f;};
 std::array<Entry,2048> entries_{};
 uintptr_t session_{};uint32_t frame_{},time_{};
public:
 void Begin(uintptr_t session,uint32_t frame,uint32_t time) noexcept {
  if(session!=session_||frame<frame_||uint32_t(time-time_)>2000u)entries_={};
  session_=session;frame_=frame;time_=time;
 }
 float Current(uint32_t key,uint64_t generation,float importance,float releaseMs) noexcept {
  if(!key||!std::isfinite(importance)||importance<1.f||!std::isfinite(releaseMs)||releaseMs<=0)return importance;
  uint32_t hash=(key*0x9e3779b9u)^uint32_t(generation)^uint32_t(generation>>32);
  hash^=hash>>16;hash*=0x7feb352du;hash^=hash>>15;
  Entry* vacant=nullptr;
  // Bounded lookup; a full collision chain uses the fresh score directly.
  // Never let one key's history authorize or influence another source.
  for(unsigned n=0;n<4;++n){auto& e=entries_[(hash+n)&2047u];
   if(e.key==key&&e.generation==generation&&uint32_t(frame_-e.frame)<=16u&&uint32_t(time_-e.time)<=2000u){
    if(importance>=e.importance)e.importance=importance; // Immediate acquisition.
    else {const float alpha=std::clamp(float(uint32_t(time_-e.time))/releaseMs,0.f,1.f);
     e.importance+=(importance-e.importance)*alpha;}
    e.frame=frame_;e.time=time_;return e.importance;
   }
   if(!vacant&&(!e.key||uint32_t(frame_-e.frame)>16u||uint32_t(time_-e.time)>2000u))vacant=&e;
  }
  if(vacant)*vacant={key,frame_,time_,generation,importance};
  return importance;
 }
};
}
