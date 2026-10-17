#pragma once
// Startup-only compatibility by required instruction/argument shapes and bindings.
// No release number, hash allowlist, borrowed runtime owner or per-frame scan.
#include "FusionPatterns218.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <algorithm>
namespace fusion_discovery218 {
struct Range {uint32_t rva;size_t size;};
struct Result {
 uint32_t process{},player{},npc{},admission{},lookup{},render{},caster{},contact{},frame{},frameData{};
 uint32_t width{},height{},extra{},head{},vehicle{},warning{},warningKey{},textLookup{};
 uint32_t extraCallback{},listCallback{},vehicleCallback{},flagsCallback{},listIndex{},listFunction{};
 uint32_t poolCallback{},poolBody{},copyCallback{},allocation{};
 size_t poolSize{},flagsSize{},copySize=0x113,copySlot=0x57,copyInvoke=0x70;
 std::vector<Range> code;
};
inline uint32_t Word(std::span<const uint8_t> data,size_t at){
 if(at>data.size()||data.size()-at<4)throw std::runtime_error("fusionfix_integration_operand_out_of_range");
 uint32_t v{};std::memcpy(&v,data.data()+at,4);return v;
}
template<class Image> bool Section(const Image& image,uint32_t at,size_t size,uint32_t flags){
 if(uint64_t(at)+size>image.bytes.size())return false;
 for(const auto& s:image.sections)if(at>=s.VirtualAddress&&uint64_t(at)+size<=uint64_t(s.VirtualAddress)+s.Misc.VirtualSize&&(s.Characteristics&flags)==flags)return true;
 return false;
}
template<class Image> bool MatchAt(const Image& image,const Pattern& p,uint32_t at){
 if(!Section(image,at,p.size,0x20000000u))return false;
 for(size_t i=0;i<p.size;++i)if(p.mask[i]&&image.bytes[at+i]!=p.bytes[i])return false;
 return true;
}
template<class Image> uint32_t Match(const Image& image,const Pattern& p,bool optional=false){
 const std::span<const uint8_t> data(image.bytes);uint32_t found{};unsigned count=0;
 // Longest fixed instruction run is an anchor; every remaining opcode and
 // unmasked argument is checked before accepting the complete shape.
 size_t start=0,length=0,run=0;
 for(size_t i=0;i<p.size;++i){if(p.mask[i]){++run;if(run>length){start=i+1-run;length=run;}}else run=0;}
 if(!length)throw std::runtime_error("fusionfix_integration_empty_pattern");
 for(const auto& section:image.sections){
  if(!(section.Characteristics&0x20000000u))continue;
  const size_t begin=section.VirtualAddress,end=std::min<size_t>(data.size(),begin+size_t(section.Misc.VirtualSize));
  if(end<begin||end-begin<p.size)continue;
  auto cursor=data.begin()+begin;
  while(cursor<data.begin()+end){
   auto hit=std::search(cursor,data.begin()+end,p.bytes+start,p.bytes+start+length);
   if(hit==data.begin()+end)break;cursor=hit+1;
   const auto anchor=size_t(hit-data.begin());if(anchor<begin+start)continue;
   const size_t at=anchor-start;if(at+p.size>end)continue;
   bool good=true;for(size_t i=0;i<p.size&&good;++i)good=!p.mask[i]||data[at+i]==p.bytes[i];
   for(size_t i=0;i<p.count&&good;++i){const auto ref=p.refs[i];const auto operand=at+ref.offset;
    const auto target=ref.relative?uint32_t(operand+4+Word(data,operand)):uint32_t(Word(data,operand)-uint32_t(image.mappedBase));
    good=Section(image,target,1,ref.relative?0x20000000u:0x40000000u);
   }
   if(good){found=uint32_t(at);++count;}
  }
 }
 if(count>1&&optional)return 0;
 if(count>1)throw std::runtime_error(std::string("fusionfix_integration_")+p.name+"_ambiguous");
 if(!count&&!optional)throw std::runtime_error(std::string("fusionfix_integration_")+p.name+"_code_not_found");
 return found;
}
template<class Image> uint32_t Thunk(const Image& image,uint32_t target,const char* role){
 uint32_t found{};unsigned count=0;const std::span<const uint8_t> data(image.bytes);
 for(const auto& s:image.sections)if(s.Characteristics&0x20000000u){
  const size_t end=std::min<size_t>(data.size(),size_t(s.VirtualAddress)+s.Misc.VirtualSize);
  for(size_t i=s.VirtualAddress;i+5<=end;++i)if(data[i]==0xe9&&(i%16==0||(i&&data[i-1]==0xcc))&&uint32_t(i+5+Word(data,i+1))==target){found=uint32_t(i);++count;}
 }
 if(count!=1)throw std::runtime_error(std::string("fusionfix_integration_")+role+(count?"_thunk_ambiguous":"_thunk_missing"));
 return found;
}
template<class Image> Result Discover(const Image& image){
 Result out;std::vector<std::pair<std::string_view,uint32_t>> roles;
 for(const auto& p:Patterns){const bool optional=std::string_view(p.name)=="process"||std::string_view(p.name)=="frame"||std::string_view(p.name)=="warning";
  const auto at=Match(image,p,optional);roles.emplace_back(p.name,at);if(at&&std::string_view(p.name)!="frame"&&std::string_view(p.name)!="process")out.code.push_back({at,p.size});
  if(std::string_view(p.name)=="pool")out.poolSize=p.size;
  if(std::string_view(p.name)=="flags")out.flagsSize=p.size;
 }
 const auto get=[&](std::string_view name){for(auto [key,value]:roles)if(key==name)return value;return uint32_t(0);};
 const std::span<const uint8_t> data(image.bytes);const auto base=uint32_t(image.mappedBase);
 const auto pointer=[&](uint32_t at){const auto rva=uint32_t(Word(data,at)-base);
  if(!Section(image,rva,4,0x40000000u))throw std::runtime_error("fusionfix_integration_data_binding_outside_module");return rva;};
 out.process=get("process");out.player=get("player");out.npc=get("npc");out.admission=get("admission");out.lookup=get("lookup");out.render=get("render");out.contact=get("contact");
 out.caster=Thunk(image,get("casterBody"),"caster");out.poolBody=get("pool");out.poolCallback=Thunk(image,out.poolBody,"pool");
 out.code.push_back({out.caster,5});out.code.push_back({out.poolCallback,5});
 // Old frame-counter overlay is optional: newer FusionFix preserves that
 // native increment and hooks an earlier load instead. Never invent its RVA.
 // Two supported game editions may have identical frame callbacks in the DLL.
 // Select the actually installed callback from its native hook, not code order.
 const auto dimensions=get("dimensions");out.width=pointer(dimensions+13);out.height=pointer(dimensions+22);
 out.extraCallback=get("extra");out.extra=pointer(out.extraCallback+0x21);out.head=pointer(out.extraCallback+0x26);out.vehicle=pointer(out.extraCallback+0x2c);
 out.listCallback=get("list");out.listIndex=pointer(out.listCallback+0x10);out.vehicleCallback=get("vehicle");out.flagsCallback=get("flags");
 out.copyCallback=get("copy");out.allocation=pointer(out.copyCallback+0x58);
 if(out.head!=out.extra+1||pointer(out.player+5)!=out.head||pointer(out.render+2)!=out.extra||pointer(out.flagsCallback+5)!=out.extra)
  throw std::runtime_error("fusionfix_integration_setting_bindings_disagree");
 for(auto at:{out.extra,out.head,out.vehicle,out.width,out.height,out.listIndex,out.allocation})
  if(!Section(image,at,4,0x80000000u))throw std::runtime_error("fusionfix_integration_setting_storage_not_writable");
 // Bind the five-argument draw entry through actual FF call operands; no
 // assumed adjacency or fixed address of its globals.
 for(const auto& s:image.sections)if(s.Characteristics&0x20000000u){
  const size_t end=std::min<size_t>(data.size(),size_t(s.VirtualAddress)+s.Misc.VirtualSize);
  for(size_t i=s.VirtualAddress;i+12<=end;++i)if(data[i]==0xff&&data[i+1]==0x35&&Word(data,i+2)==base+out.listIndex&&data[i+6]==0xff&&data[i+7]==0x15){
   const auto at=pointer(uint32_t(i+8));if(out.listFunction&&out.listFunction!=at)throw std::runtime_error("fusionfix_integration_draw_binding_ambiguous");out.listFunction=at;out.code.push_back({uint32_t(i),12});
  }
 }
 if(!out.listFunction)throw std::runtime_error("fusionfix_integration_draw_binding_missing");
 out.warning=get("warning");if(out.warning){out.warningKey=pointer(out.warning+1);out.textLookup=uint32_t(out.warning+10+Word(data,out.warning+6));}
 return out;
}
template<class Image,class Read> void BindFrame(const Image& image,Result& out,uint32_t game,uint32_t counter,std::span<const uint8_t> expected,std::span<const uint8_t> mid,Read read){
 constexpr uint32_t site=0x4d0f82;
 const auto native=read(game+site,6);
 if(native.size()==expected.size()&&std::equal(native.begin(),native.end(),expected.begin())){out.frame=out.frameData=0;return;}
 const auto fail=[](){throw std::runtime_error("fusionfix_integration_frame_overlay_unrecognized");};
 if(native.size()!=6||native[0]!=0xe9||native[5]!=0x90){fail();return;}
 const uint32_t relay=game+site+5+Word(native,1);const auto jump=read(relay,5);
 if(jump.size()!=5||jump[0]!=0xe9){fail();return;}
 const uint32_t stubAddress=relay+5+Word(jump,1);const auto stub=read(stubAddress,171);
 if(stub.size()!=171||mid.size()!=171){fail();return;}
 for(size_t i=0;i<171;++i)if(!((i>=2&&i<6)||(i>=89&&i<93)||i>=163)&&stub[i]!=mid[i]){fail();return;}
 if(Word(stub,2)!=stubAddress+167||Word(stub,89)!=stubAddress+163){fail();return;}
 const uint32_t callback=Word(stub,163),base=uint32_t(image.mappedBase),at=callback-base;
 const Pattern* shape=nullptr;for(const auto& p:Patterns)if(std::string_view(p.name)=="frame")shape=&p;
 if(callback<base||!shape||!MatchAt(image,*shape,at)){fail();return;}
 const uint32_t storage=Word(image.bytes,at+4);
 if(storage<base||!Section(image,storage-base,4,0x80000000u)||Word(read(storage,4),0)!=game+counter){fail();return;}
 out.frame=at;out.frameData=storage-base;out.code.push_back({at,shape->size});
}
template<class Image,class Read> void ValidateLive(const Image& image,const Result& result,Read read){
 for(const auto& range:result.code){const auto actual=read(uint32_t(image.mappedBase)+range.rva,range.size);
  if(actual.size()!=range.size||std::memcmp(actual.data(),image.bytes.data()+range.rva,range.size))throw std::runtime_error("fusionfix_integration_required_code_modified");
 }
}
}
