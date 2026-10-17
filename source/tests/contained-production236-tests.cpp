#define CONTAINED_REPLAY236
#include "model-point-value68-tests.cpp"
int main(){
 memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(memory);
 auto allocate=[](){auto* p=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;};
 vectorPages50=static_cast<uint8_t*>(VirtualAlloc(nullptr,4096*4,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(vectorPages50);
 headers50=allocate();flags50=allocate();model50=allocate();dummyOwner50=allocate();game=uint32_t(uintptr_t(gameImage.data()));
 auto* building=reinterpret_cast<rage::VehiclePool*>(headers50);auto* object=building+1;
 *building={dummyOwner50,flags50,1,0x70,0,0};*object={memory+8192,flags50,1,0x320,0,0};
 auto root=[&](unsigned offset,uint32_t pointer){std::memcpy(gameImage.data()+offset,&pointer,4);};
 root(0xebd0e8,uint32_t(uintptr_t(building)));root(0x1232c60,uint32_t(uintptr_t(object)));root(0xe95cd8+5*4,uint32_t(uintptr_t(model50)));
 uint32_t hash=123;std::memcpy(model50+0x3c,&hash,4);
 unsigned cases{},accepted{},refused{};
 for(unsigned owner=0;owner<5;++owner){class5Case109=owner==1||owner==2;instanceCase114=owner>=3;inlineCase109=owner==2||owner==4;
  for(auto site:{model_effect_observer::WrapperPoint,model_effect_observer::DirectPoint,model_light_submission20::WrapperSpot,model_light_submission20::DirectSpot}){
   for(auto mutation:{Change::None,Change::Descriptor,Change::Generation,Change::Revoke,Change::Guard,Change::Index,Change::Hash,Change::Matrix,Change::Range,Change::Free,Change::Frame}){
    for(bool fused:{false,true}){
     contained_read236::enabled=false;const auto before=Run(true,mutation,false,false,fused,false,false,site,0,true,1);
     contained_read236::enabled=true;const auto after=Run(true,mutation,false,false,fused,false,false,site,0,true,1);
     assert(before.flags==after.flags&&before.key==after.key&&before.invalid==after.invalid);
     if(after.key)++accepted;else ++refused;++cases;
    }
   }
  }
 }
 assert(cases==440&&accepted&&refused&&contained_read236::guardRestoreFailures==0);
 for(auto* p:{headers50,flags50,model50,dummyOwner50,vectorPages50,memory})assert(VirtualFree(p,0,MEM_RELEASE));
 printf("contained236 production replay: %u exact output/invalidity comparisons, %u accepted, %u refused; real Point/Finish/Promote and five native owner layouts, four store ABIs, fresh owner/descriptor/generation/free/frame/protection mutations retained\n",cases,accepted,refused);
}
