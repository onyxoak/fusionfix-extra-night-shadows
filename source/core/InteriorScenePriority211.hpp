#pragma once
#include "VisibleReceiverRanking133.hpp"
namespace fusionfix::shadows {
inline bool InteriorQualified211(bool enabled,bool driving,bool sourceSupported,
 int interior,int room,uint32_t flags) noexcept {
 return enabled&&!driving&&sourceSupported&&interior>0&&room>0&&!(flags&0x500u);
}
// Rank-only on-foot interior experiment. Projection is potential scene impact,
// not visibility/occlusion proof. Never supplies eligibility or a shadow map.
inline SceneReceiverConsequence44 InteriorScenePriority211(SceneReceiverConsequence44 current,
 const ShadowView& view,Vec3 source,float radius,float intensity,Vec3 rgb,float strength) noexcept {
 if(!current.valid||!view.valid||!std::isfinite(current.score)||current.score<0||
    !std::isfinite(current.consequence)||current.consequence<0||!std::isfinite(radius)||radius<=0||
    !std::isfinite(strength)||strength<=0||!VisibleReceiverEmission133(intensity,rgb))return current;
 const float visible=ViewSample(view,source,true);
 if(!std::isfinite(visible)||visible<=0)return current;
 const float p[4]{source.x,source.y,source.z,1};float eye[4]{},clip[4]{};
 for(int k=0;k<4;++k)for(int n=0;n<4;++n)eye[k]+=p[n]*view.view[n][k];
 for(int k=0;k<4;++k)for(int n=0;n<4;++n)clip[k]+=eye[n]*view.projection[n][k];
 for(float f:eye)if(!std::isfinite(f))return current;
 for(float f:clip)if(!std::isfinite(f))return current;
 const float projectionScale=std::fmin(std::fabs(view.projection[0][0]),std::fabs(view.projection[1][1]));
 if(clip[3]<=.01f||!std::isfinite(projectionScale)||projectionScale<=0)return current;
 const float apparent=std::clamp(radius*projectionScale/clip[3],0.f,1.f);
 const float brightness=SceneBrightness44(intensity,rgb),contrast=brightness/(brightness+4.f);
 const float extra=std::fmin(1.5f,std::clamp(strength,0.f,4.f)*2.f*contrast*apparent*visible);
 if(!std::isfinite(extra)||extra<=0)return current;
 const float score=current.score/(1.f+extra*current.score);
 if(!std::isfinite(score)||score<0)return current;
 current.score=score;current.consequence+=extra;return current;
}
}
