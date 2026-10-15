#define NOMINMAX
#include "standalone-pe.hpp"
#include "FusionHooks213.hpp"
#include "TrafficPoolHookGuard.hpp"
#include "BackendVersions213.hpp"
#include "CopyVersions213.hpp"
#include "VolumetricCompat199.hpp"
#include <cassert>
#include <map>
#include <iostream>
using namespace onyxoak;
static void Put(std::vector<uint8_t>& b,size_t p,uint32_t n){std::memcpy(b.data()+p,&n,4);}
static std::vector<uint8_t> Jump(uint32_t p,uint32_t to){std::vector<uint8_t>b(5);b[0]=0xe9;Put(b,1,to-p-5);return b;}
int wmain(int argc,wchar_t** argv){
 assert(argc==4);
 const char* versions[]={"5.1.1","5.1.0","5.0.1"};
 unsigned mutations=0;
 for(int n=0;n<3;++n)for(uint32_t base:{0x10000000u,0x60000000u}){
  pe::Image image(pe::Read(argv[n+1]),base);
  const auto* l=fusion_hooks213::Find(versions[n]);assert(l);
  auto read=[&](uint32_t at,size_t count){assert(at>=base&&uint64_t(at-base)+count<=image.bytes.size());return std::vector<uint8_t>(image.bytes.begin()+at-base,image.bytes.begin()+at-base+count);};
  assert(fusion_hooks213::Validate(*l,base,read));
  for(const auto* code:{&l->pool,&l->flags})for(size_t i=0;i<code->size;++i){image.bytes[code->rva+i]^=1;assert(!fusion_hooks213::Validate(*l,base,read));image.bytes[code->rva+i]^=1;++mutations;}
  for(size_t i=0;i<5;++i){image.bytes[l->poolCallback+i]^=1;assert(!fusion_hooks213::Validate(*l,base,read));image.bytes[l->poolCallback+i]^=1;++mutations;}
  const auto* backend=backend_versions213::Find(versions[n]);assert(backend);
  auto backendRead=[&](uint32_t a,void* out,size_t size){const auto b=read(a,size);memcpy(out,b.data(),size);return true;};
  assert(backend_versions213::CallbackBodies(*backend,base,backendRead));
  for(auto [rva,size]:{std::pair<uint32_t,size_t>{backend->extraCallback,52},{backend->listCallback,22}})
   for(size_t i=0;i<size;++i){image.bytes[rva+i]^=1;assert(!backend_versions213::CallbackBodies(*backend,base,backendRead));image.bytes[rva+i]^=1;++mutations;}
  const auto* copy=copy_versions213::Find(versions[n]);assert(copy);
  auto cb=read(base+copy->callback,copy->size);
  assert(cb[copy->slot]==0xa1&&native_copy70::Word(cb.data()+copy->slot+1)==base+copy->allocation);
  const uint8_t invoke[]={0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};
  assert(!memcmp(cb.data()+copy->invoke,invoke,sizeof(invoke))&&cb[copy->size-3]==0xc2&&cb[copy->size-2]==4&&cb.back()==0);
  // Full known callback bytes, relocation, register forwarding, original slot,
  // native continuation and both supported two-layer load orders are checked.
  constexpr uint32_t cg=0x440000,ct=0x210000,vt=0x220000,vbase=0x71000000;
  std::vector<uint8_t> native(native_copy70::CopySize,0xcc);
  const uint8_t displaced[]={0x8b,0x54,0x24,0x04,0xf3,0x0f,0x10,0x42,0x04};std::copy_n(displaced,9,native.begin());
  std::map<uint32_t,std::vector<uint8_t>> cm;
  auto putLayer=[&](uint32_t tr,uint32_t callback,uint32_t allocation,bool first,uint32_t preceding){
   const unsigned size=first?19:15;std::vector<uint8_t> b(size);
   if(first)std::copy_n(native.begin(),9,b.begin());else{auto j=Jump(tr,preceding);std::copy(j.begin(),j.end(),b.begin());}
   auto j=Jump(tr+size-10,cg+native_copy70::CopyRva+(first?9:5));std::copy(j.begin(),j.end(),b.begin()+size-10);
   j=Jump(tr+size-5,callback);std::copy(j.begin(),j.end(),b.begin()+size-5);cm[tr]=b;
   std::vector<uint8_t>a(8);Put(a,0,tr);Put(a,4,size);cm[allocation]=a;
  };
  auto cr=[&](uint32_t a,size_t size){auto it=cm.upper_bound(a);if(it!=cm.begin()){--it;if(uint64_t(a)-it->first+size<=it->second.size())return std::vector<uint8_t>(it->second.begin()+a-it->first,it->second.begin()+a-it->first+size);}throw std::runtime_error("missing");};
  cm[base+copy->callback]=cb;putLayer(ct,base+copy->callback,base+copy->allocation,true,0);
  auto copyLive=native;auto cj=Jump(cg+native_copy70::CopyRva,ct+14);std::copy(cj.begin(),cj.end(),copyLive.begin());std::fill(copyLive.begin()+5,copyLive.begin()+9,0x90);cm[cg+native_copy70::CopyRva]=copyLive;
  auto checkCopy=[&]{return native_copy70::Verify(cg,base,true,native,cb,cr,copy->callback,copy->allocation,copy->size,copy->slot,copy->invoke);};
  assert(checkCopy().fusionFixOverlay);
  for(size_t i=0;i<cb.size();++i){cm[base+copy->callback][i]^=1;bool failed=false;try{checkCopy();}catch(...){failed=true;}assert(failed);cm[base+copy->callback][i]^=1;++mutations;}
  const std::vector<uint8_t> vcb(16,0xc3);cm[vbase+0x1000]=vcb;
  for(bool outer:{false,true}){
   putLayer(ct,base+copy->callback,base+copy->allocation,outer,vt+14);
   putLayer(vt,vbase+0x1000,vbase+0x2000,!outer,ct+14);
   cj=Jump(cg+native_copy70::CopyRva,outer?vt+10:ct+10);std::copy(cj.begin(),cj.end(),copyLive.begin());cm[cg+native_copy70::CopyRva]=copyLive;
   volumetric199::Node v{vbase+0x1000,vbase+0x2000,vcb},f{base+copy->callback,base+copy->allocation,cb};
   auto check=[&]{return volumetric199::VerifyChain(cg,native,v,&f,cr);};
   auto proof=check();assert(proof.fusionFix&&proof.volumetric&&proof.outer==(outer?v.callback:f.callback));
   for(const auto& [address,bytes]:cm)for(size_t i=0;i<bytes.size();++i){cm[address][i]^=1;bool failed=false;try{check();}catch(...){failed=true;}assert(failed);cm[address][i]^=1;++mutations;}
  }
  constexpr uint32_t game=0x400000,relay=0x300000,stub=0x301000,tramp=0x302000;
  std::map<uint32_t,std::vector<uint8_t>> memory;
  std::vector<uint8_t> original(0x66,0xcc);const uint8_t arithmetic[]={0x8b,0xc7,0x0f,0xaf,0xc2};std::copy(std::begin(arithmetic),std::end(arithmetic),original.begin()+10);
  auto live=original;auto j=Jump(game+traffic_signal::PoolHookRva,relay);std::copy(j.begin(),j.end(),live.begin()+10);memory[game+traffic_signal::PoolCtorRva]=live;memory[relay]=Jump(relay,stub);
  auto code=std::vector<uint8_t>(MidStubTemplate.begin(),MidStubTemplate.end());Put(code,2,stub+167);Put(code,89,stub+163);Put(code,163,base+l->poolCallback);Put(code,167,tramp);memory[stub]=code;
  std::vector<uint8_t> cont(10,0x90);j=Jump(tramp+5,game+traffic_signal::PoolHookRva+5);std::copy(j.begin(),j.end(),cont.begin()+5);memory[tramp]=cont;
  auto chain=[&](uint32_t at,size_t count){if(at>=base&&uint64_t(at-base)+count<=image.bytes.size())return read(at,count);assert(memory.count(at)&&memory[at].size()>=count);return std::vector<uint8_t>(memory[at].begin(),memory[at].begin()+count);};
  uint32_t observed=0;const auto proof=traffic_signal::VerifyPoolHook(game,base,true,original,chain,l->poolCallback,l->poolBody,&observed);assert(proof.stub==stub&&observed==base+l->poolCallback);
  if(n){bool refused=false;try{traffic_signal::VerifyPoolHook(game,base,true,original,chain);}catch(const std::exception& e){refused=std::string(e.what())=="traffic_pool_callback_owner_changed";}assert(refused);}
  memory[stub][163]^=1;bool refused=false;try{traffic_signal::VerifyPoolHook(game,base,true,original,chain,l->poolCallback,l->poolBody);}catch(const std::exception& e){refused=std::string(e.what())=="traffic_pool_callback_owner_changed";}assert(refused);
 }
 assert(!fusion_hooks213::Find("unknown"));
 std::cout<<"PASS official501/510/511 callback bytes at two bases; "<<mutations<<" byte mutations rejected; old-version false refusal reproduced and corrected; unknown owners refused\n";
}
