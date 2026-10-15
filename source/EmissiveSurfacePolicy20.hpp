#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>
// Numeric metadata derived from local authored mesh regions, UV texture samples
// and vertex colour. Point falloff and sampled animation colour are approximate;
// no game texture/mesh buffers are distributed in the metadata.
namespace emissive_surface20 {
struct Model { uint32_t hash{},first{},count{},shaderCount{}; std::array<float,12> bounds{}; };
struct Surface {
 std::array<float,3> position{},normal{},rgb{};
 float extent{},area{},authoredMultiplier{};
 uint16_t lodModel{},geometry{},shader{},flags{};
 uint32_t textureHash{},vertexCount{},indexCount{};
};
static_assert(sizeof(Model)==64 && sizeof(Surface)==68);
class Catalog {
 std::vector<Model> models_;std::vector<Surface> surfaces_;
public:
 bool Load(const std::filesystem::path& path) {
  models_.clear();surfaces_.clear();
  std::ifstream in(path,std::ios::binary|std::ios::ate);if(!in)return false;
  const auto length=in.tellg();if(length<32||length>32*1024*1024)return false;
  in.seekg(0);std::array<uint32_t,8> h{};in.read(reinterpret_cast<char*>(h.data()),32);
  if(!in||h[0]!=0x3253454cu||h[1]!=1||!h[2]||h[2]>32768||!h[3]||h[3]>262144||h[4]!=64||h[5]!=68||
     uint64_t(h[2])*64+uint64_t(h[3])*68!=h[6]||uint64_t(length)!=uint64_t(h[6])+32)return false;
  std::vector<uint8_t> bytes(h[6]);in.read(reinterpret_cast<char*>(bytes.data()),bytes.size());if(!in)return false;
  uint32_t digest=2166136261u;for(auto c:bytes)digest=(digest^c)*16777619u;if(digest!=h[7])return false;
  std::vector<Model> models(h[2]);std::vector<Surface> surfaces(h[3]);
  std::memcpy(models.data(),bytes.data(),models.size()*64);
  std::memcpy(surfaces.data(),bytes.data()+models.size()*64,surfaces.size()*68);
  uint32_t previous=0,nextSurface=0;
  for(const auto& m:models){
   if(!m.hash||m.hash<=previous||!m.count||m.first!=nextSurface||uint64_t(m.first)+m.count>surfaces.size()||!m.shaderCount||m.shaderCount>256)return false;
   for(auto f:m.bounds)if(!std::isfinite(f))return false;
   for(unsigned axis=0;axis<3;++axis)if(m.bounds[4+axis]>m.bounds[8+axis])return false;
   previous=m.hash;nextSurface=m.first+m.count;
   for(uint32_t i=m.first;i<m.first+m.count;++i){const auto& s=surfaces[i];
    for(auto f:s.position)if(!std::isfinite(f))return false;
    for(auto f:s.normal)if(!std::isfinite(f))return false;
    for(auto f:s.rgb)if(!std::isfinite(f)||f<0||f>1)return false;
    const auto n=s.normal[0]*s.normal[0]+s.normal[1]*s.normal[1]+s.normal[2]*s.normal[2];
    if(n<.99f||n>1.01f||!std::isfinite(s.extent)||s.extent<0||s.extent>4096||!std::isfinite(s.area)||s.area<=0||
       !std::isfinite(s.authoredMultiplier)||s.authoredMultiplier<=0||s.shader>=m.shaderCount||(s.flags&~3)||!s.textureHash||
       !s.vertexCount||s.vertexCount>65535||!s.indexCount||s.indexCount>1000000||s.indexCount%3)return false;
   }
  }
  if(nextSurface!=surfaces.size())return false;
  models_=std::move(models);surfaces_=std::move(surfaces);return true;
 }
 const Model* Find(uint32_t hash) const noexcept {
  auto i=std::lower_bound(models_.begin(),models_.end(),hash,[](const Model& m,uint32_t h){return m.hash<h;});
  return i!=models_.end()&&i->hash==hash?&*i:nullptr;
 }
 const Surface* Get(uint32_t i) const noexcept {return i<surfaces_.size()?&surfaces_[i]:nullptr;}
 uint32_t Size()const noexcept{return uint32_t(surfaces_.size());}
};
struct Settings {
 bool allowSampledAnimationColour=true;
 uint32_t maximumPerFrame=48,reservedNative=192;
 float reach=64,radiusScale=1.5f,minRadius=2,maxRadius=32,intensityScale=.08f,maxIntensity=8,surfaceBias=.04f;
 void Clamp() noexcept {
  maximumPerFrame=std::min(maximumPerFrame,128u);reservedNative=std::clamp(reservedNative,160u,640u);
  auto finite=[](float v,float fallback,float lo,float hi){return std::isfinite(v)?std::clamp(v,lo,hi):fallback;};
  reach=finite(reach,64,1,512);radiusScale=finite(radiusScale,1.5f,.01f,16);
  minRadius=finite(minRadius,2,.1f,128);maxRadius=finite(maxRadius,32,minRadius,128);
  intensityScale=finite(intensityScale,.08f,.001f,16);maxIntensity=finite(maxIntensity,8,.01f,64);
  surfaceBias=finite(surfaceBias,.04f,0,.25f);
 }
};
// Filled from authenticated current native material state by the integration.
// A missing native night factor suppresses night-only sources. Never substitute
// the model-effect fade scalar for material night/switch/UV-animation authority.
struct Authoring {
 bool verified{},localMeshUnchanged{},uvUnchanged{},nightFactorVerified{},rgbOverrideVerified{};
 float multiplier{},visibility{},nightFactor{};
 std::array<float,3> rgbOverride{};
 uint32_t material{},revision{};
};
struct Packet { std::array<float,4> position{},rgb{};float radius{},intensity{}; };
inline bool Make(const Surface& s,const std::array<float,16>& world,const Authoring& a,float ownerFade,
                 const Settings& config,Packet& out) noexcept {
 if(!a.verified||!a.localMeshUnchanged||!a.material||!std::isfinite(a.multiplier)||a.multiplier<=0||
    !std::isfinite(a.visibility)||a.visibility<=0||!std::isfinite(ownerFade)||ownerFade<=0||ownerFade>1.001f||
    (!a.uvUnchanged&&!a.rgbOverrideVerified&&!config.allowSampledAnimationColour))return false;
 float visibility=std::min(a.visibility,1.f)*std::min(ownerFade,1.f);
 if(s.flags&1){if(!a.nightFactorVerified||!std::isfinite(a.nightFactor)||a.nightFactor<=0)return false;visibility*=std::min(a.nightFactor,1.f);}
 for(auto f:world)if(!std::isfinite(f))return false;
 // GTA entity matrices use row basis vectors and translation at +0x30.
 // Reject non-rigid transforms until native deformation/scaling is supported.
 for(unsigned i=0;i<3;++i){float norm=0;for(unsigned j=0;j<3;++j)norm+=world[i*4+j]*world[i*4+j];if(norm<.98f||norm>1.02f)return false;
  for(unsigned k=0;k<i;++k){float dot=0;for(unsigned j=0;j<3;++j)dot+=world[i*4+j]*world[k*4+j];if(std::fabs(dot)>.01f)return false;}}
 out={};
 for(unsigned j=0;j<3;++j){
  float normal=0;out.position[j]=world[12+j];
  for(unsigned i=0;i<3;++i){out.position[j]+=s.position[i]*world[i*4+j];normal+=s.normal[i]*world[i*4+j];}
  out.position[j]+=normal*config.surfaceBias;
  const auto color=a.rgbOverrideVerified?a.rgbOverride[j]:s.rgb[j];
  if(!std::isfinite(color)||color<0||color>1||!std::isfinite(out.position[j]))return false;
  out.rgb[j]=color;
 }
 out.position[3]=out.rgb[3]=1;
 out.radius=std::clamp(std::max(.1f,s.extent)*config.radiusScale,config.minRadius,config.maxRadius);
 out.intensity=std::min(config.maxIntensity,a.multiplier*visibility*config.intensityScale);
 return std::isfinite(out.intensity)&&out.intensity>0&&std::max({out.rgb[0],out.rgb[1],out.rgb[2]})>.001f;
}
// Native submission capacity is 0x280; reserve headroom for original lights.
inline bool Budget(uint32_t current,uint32_t emitted,const Settings& s) noexcept {
 return s.maximumPerFrame&&emitted<s.maximumPerFrame&&s.reservedNative<640&&current<640-s.reservedNative;
}
}
