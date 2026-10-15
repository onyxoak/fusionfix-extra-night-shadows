#pragma once
#include "InteriorScenePriority211.hpp"
#include "ConsumerShadow75.hpp"
namespace indoor212 {
inline float (*strengthProvider)(const consumer_shadow75::Packet&,uint32_t) noexcept=nullptr;
inline float Current(const consumer_shadow75::Packet& p,uint32_t f) noexcept{return strengthProvider?strengthProvider(p,f):-1.f;}
inline constexpr float Feet35=35.f*.3048f,Feet30=30.f*.3048f;
inline bool Packet(const consumer_shadow75::Packet& p) noexcept {
 return p.words[24]&&p.words[24]!=UINT32_MAX&&(p.words[17]==0||p.words[17]==2)&&
  !(p.words[18]&0x500u)&&int32_t(p.words[26])>0&&int32_t(p.words[27])>0;
}
inline float Strength(fusionfix::shadows::Vec3 player,fusionfix::shadows::Vec3 source) noexcept {
 const float x=player.x-source.x,y=player.y-source.y,z=player.z-source.z;
 const float d2=x*x+y*y+z*z;if(!std::isfinite(d2)||d2>=Feet35*Feet35)return -1.f;
 return 1.f-fusionfix::shadows::SmoothUnit((std::sqrt(d2)-Feet30)/(Feet35-Feet30));
}
inline fusionfix::shadows::SceneReceiverConsequence44 Rank(
 fusionfix::shadows::SceneReceiverConsequence44 current,const fusionfix::shadows::ShadowView& view,
 fusionfix::shadows::Vec3 player,fusionfix::shadows::Vec3 source,fusionfix::shadows::Vec3 direction,
 float radius,float intensity,fusionfix::shadows::Vec3 rgb,uint32_t type) noexcept {
 using namespace fusionfix::shadows;
 const float bubble=Strength(player,source);
 if(!current.valid||!view.valid||bubble<0||!std::isfinite(current.score)||current.score<0||
    !VisibleReceiverEmission133(intensity,rgb)||!std::isfinite(radius)||radius<=0||(type!=0&&type!=2))return current;
 const float visible=std::clamp((ShadowViewWeight(view,source,direction,radius,type==2,true)-1.f)*.5f,0.f,1.f);
 const float p[4]{source.x,source.y,source.z,1};float eye[4]{};
 for(int k=0;k<4;++k)for(int n=0;n<4;++n)eye[k]+=p[n]*view.view[n][k];
 for(float f:eye)if(!std::isfinite(f))return current;
 const float scale=std::fmin(std::fabs(view.projection[0][0]),std::fabs(view.projection[1][1]));
 if(!std::isfinite(scale)||scale<=0)return current;
 const float apparent=std::clamp(radius*scale/std::fmax(std::fabs(eye[2]),radius*.25f),0.f,1.f);
 const float brightness=SceneBrightness44(intensity,rgb),contrast=brightness/(brightness+4.f);
 const float consequence=4.f*contrast*(.15f+.85f*visible)*(.25f+.75f*apparent);
 const float roomScore=1.f/(.05f+consequence);
 const float score=current.score+(roomScore-current.score)*bubble;
 if(!std::isfinite(score)||score<0)return current;
 current.score=score;current.consequence=current.consequence+(consequence-current.consequence)*bubble;return current;
}
inline bool Application(const consumer_shadow75::Sample& s,const consumer_shadow75::Packet& p) noexcept {
 return Packet(p)&&s.available&&!s.sizeGate&&s.finite&&s.reproduced&&
  std::isfinite(s.gateRadius)&&s.gateRadius>0&&std::isfinite(s.threshold)&&s.threshold>0&&
  std::isfinite(s.ratio)&&s.ratio>0;
}
template<class Read,class Authorize,class Inside,class Write>
bool Gate(consumer_shadow75::Context& c,const consumer_shadow75::Registers& r,uint32_t frame,
 uint32_t tick,uintptr_t global,Read&& read,Authorize&& authorize,Inside&& inside,Write&& write) noexcept {
 bool dispatched=false;
 if(!consumer_shadow75::Probe(c,consumer_shadow75::Stage::Gate,r,frame,tick,global,read,
    [&](const consumer_shadow75::Sample&)noexcept{dispatched=true;})||dispatched||!Application(c.sample,c.packet))return false;
 const auto packet=c.packet;
 const float initial=inside(packet,frame);
 if(!std::isfinite(initial)||initial<=0||!authorize(packet,frame))return false;
 auto fresh=c;fresh.gateSeen=false;
 if(!consumer_shadow75::Probe(fresh,consumer_shadow75::Stage::Gate,r,frame,tick,global,read,
    [](const consumer_shadow75::Sample&)noexcept{})||std::memcmp(&fresh.packet,&packet,sizeof(packet))||
    !Application(fresh.sample,fresh.packet))return false;
 const float final=inside(fresh.packet,frame);if(!std::isfinite(final)||final<=0)return false;
 return write(r.esp+0xe,uint8_t(1));
}
}
