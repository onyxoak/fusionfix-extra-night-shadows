#define NOMINMAX
#include <cassert>
#include <iostream>
#include <random>
#include "core/ReceiverCasterCull226.hpp"
using namespace receiver_cull226;
int main(){
 std::mt19937 random(241);std::uniform_real_distribution<double> unit(0,1),coordinate(-500,500);
 unsigned additionalPlanes=0,rejections=0;
 for(unsigned n=0;n<100000;++n){
  fusionfix::shadows::ShadowView view{};view.valid=true;
  for(unsigned i=0;i<4;++i)view.view[i][i]=1;
  const double cx=coordinate(random),cy=coordinate(random),cz=coordinate(random);
  view.view[3][0]=float(-cx);view.view[3][1]=float(-cy);view.view[3][2]=float(-cz);
  view.projection[0][0]=1;view.projection[1][1]=1;
  view.projection[2][2]=100.f/99.f;view.projection[2][3]=1;view.projection[3][2]=-100.f/99.f;
  const V light{coordinate(random),coordinate(random),coordinate(random)};
  auto loose=Prepare(view,light),tight=loose;Tighten241(tight,view,light);assert(tight.count>=loose.count&&tight.count<=18);
  additionalPlanes+=tight.count-loose.count;
  const double depth=1.+98.*unit(random);
  const V receiver{cx+(2*unit(random)-1)*depth*.999,cy+(2*unit(random)-1)*depth*.999,cz+depth};
  const double amount=unit(random);
  const V point{light.x+(receiver.x-light.x)*amount,light.y+(receiver.y-light.y)*amount,light.z+(receiver.z-light.z)*amount};
  assert(!OutsideRotatedOrigin(tight,{0,0,0},{0,0,0},point));
  // Every independently generated convex receiver/light segment, including
  // two-metre offsets, must remain inside each added supporting plane.
  for(unsigned i=loose.count;i<tight.count;++i){const auto& p=tight.planes[i];const double distance=p[0]*point.x+p[1]*point.y+p[2]*point.z+p[3];assert(distance>=1.99);}
  const V candidate{coordinate(random),coordinate(random),coordinate(random)};
  if(!OutsideRotatedOrigin(loose,{0,0,0},{0,0,0},candidate)&&OutsideRotatedOrigin(tight,{0,0,0},{0,0,0},candidate))++rejections;
 }
 assert(additionalPlanes>100000&&rejections>100);
 fusionfix::shadows::ShadowView singular{};singular.valid=true;Volume kept{};kept.count=1;kept.planes[0]={1,0,0,2};Tighten241(kept,singular,{1,2,3});assert(kept.count==1&&kept.planes[0][3]==2);
 std::cout<<"PASS241 100000 translated-frustum/light segment cases; extra planes="<<additionalPlanes<<" additional point rejections="<<rejections<<"; singular projection retains original volume\n";
}
