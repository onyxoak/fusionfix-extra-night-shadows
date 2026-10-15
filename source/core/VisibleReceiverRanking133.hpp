#pragma once
#include "SceneShadowPriority21.hpp"
namespace fusionfix::shadows {
inline bool VisibleReceiverEmission133(float intensity,Vec3 rgb) noexcept {
 if(!std::isfinite(intensity)||intensity<=0||!std::isfinite(rgb.x)||!std::isfinite(rgb.y)||!std::isfinite(rgb.z))return false;
 const float brightness=SceneBrightness44(intensity,rgb);return std::isfinite(brightness)&&brightness>0;
}
// Ranking proxy only. A sphere/view overlap is potential receiver benefit,
// never visibility, physical fixture identity, source eligibility or emission.
inline float VisibleReceiverBenefit133(bool enabled,bool lamp,bool viewValid,bool volumeVisible,
 float distanceSquared,float radius,float reach,float viewWeight,uint32_t type,float outerCos) noexcept {
 if(!enabled||!lamp||!viewValid||!volumeVisible)return 0.f;
 if((type!=0&&type!=2)||!std::isfinite(distanceSquared)||distanceSquared<0||
    !std::isfinite(radius)||radius<=0||!std::isfinite(reach)||reach<=0||
    !std::isfinite(viewWeight)||viewWeight<1.f||viewWeight>3.f||
    (type==2&&(!std::isfinite(outerCos)||outerCos<=0||outerCos>1)))return 0.f;
 const float nearest=std::max(0.f,std::sqrt(distanceSquared)-radius);
 const float shoulder=SmoothUnit((reach-nearest)/reach);
 // Keep the conservative volume-overlap floor; sparse projected samples
 // cannot prove that a source behind the camera has no visible receiver.
 const float visibility=.25f+.75f*std::clamp((viewWeight-1.f)*.5f,0.f,1.f);
 return shoulder*visibility;
}
inline bool VisibleReceiverRelevant133(bool original,float benefit) noexcept {
 return original||(std::isfinite(benefit)&&benefit>0.f);
}
inline float VisibleReceiverNativeDistance133(float original,float benefit) noexcept {
 if(!std::isfinite(original)||original<0||!std::isfinite(benefit)||benefit<=0)return original;
 return original/(1.f+.15f*std::clamp(benefit,0.f,1.f));
}
// Rank-only static-scenery preference. Called only after current source-family
// qualification. Sparse misses keep a nonzero peripheral preference; central
// receiver support and the inherited volume floor remain exact.
inline float SceneryReceiverFactor163(const ShadowView& view,Vec3 source,Vec3 direction,
 float radius,uint32_t type) noexcept {
 if(!view.valid)return 1.f;
 // No valid clip-space homogeneous coordinate means missing camera data,
 // not evidence against a fixture. Real perspective/orthographic cameras have
 // at least one nonzero W coefficient. Keep the inherited rank on invalid data.
 bool homogeneous=false;
 for(unsigned i=0;i<4;++i){if(!std::isfinite(view.projection[i][3]))return 1.f;homogeneous=homogeneous||view.projection[i][3]!=0.f;}
 if(!homogeneous)return 1.f;
 const float weight=ShadowViewWeight(view,source,direction,radius,type==2,true);
 return std::clamp((weight-1.f)*.5f,.5f,1.f);
}
inline SceneReceiverConsequence44 VisibleReceiverScore133(SceneReceiverConsequence44 current,
 float benefit,float distanceSquared,float radius,float intensity,Vec3 rgb,float viewWeight,bool volumeVisible,float sceneryFactor=1.f) noexcept {
 if(!current.valid||!std::isfinite(benefit)||benefit<=0)return current;
 const float brightness=SceneBrightness44(intensity,rgb);
 const float contrast=brightness/(brightness+4.f);
 const float normalized=distanceSquared/(radius*radius+9.f);
 const float visible=volumeVisible?(.25f+.75f*std::clamp((viewWeight-1.f)*.5f,0.f,1.f)):0.f;
 const float forward=std::isfinite(sceneryFactor)?std::clamp(sceneryFactor,.5f,1.f):1.f;
 const float background=.025f*visible*forward/(1.f+normalized);
 // Central subject/road terms remain exact. Bounded peripheral preference
 // changes one current score only, not radiance, importance or any map lease.
 const float extra=.075f*std::clamp(benefit,0.f,1.f)*contrast*forward;
 const float score=1.f/(.01f+current.consequence+background+extra)+.001f*std::min(normalized,1000000.f);
 if(std::isfinite(score)&&score>=0)current.score=score;
 return current;
}
}
