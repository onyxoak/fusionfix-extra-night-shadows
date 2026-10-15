#include "WorldCasterMask20.hpp"
#include <cstdio>
#include <initializer_list>
#include <cstdlib>
using namespace world_caster_mask20;
int main(){unsigned cases=0;
 for(unsigned slot=0;slot<10;++slot)for(unsigned kind=0;kind<7;++kind)for(bool active:{false,true})for(bool deferred:{false,true})for(unsigned key:{0u,0x1234u,UINT32_MAX})for(unsigned mask:{NativeDynamicMask,NativeBothMask,0x800280u,0u,0x1800285u}){
  const bool eligible=ShouldIncludeStatic(slot,kind,active,key,mask,deferred);
  const bool expected=slot>=1&&slot<=7&&kind==4&&active&&!deferred&&key==0x1234u&&mask==NativeDynamicMask;
  if(eligible!=expected){std::fprintf(stderr,"FAIL admission scope\n");std::abort();}
  if(eligible&&IncludeStatic(mask)!=NativeBothMask){std::fprintf(stderr,"FAIL native combined mask\n");std::abort();}
  if((IncludeStatic(mask)^mask)&~StaticBuildingBit){std::fprintf(stderr,"FAIL unrelated bit\n");std::abort();}
  ++cases;
 }
 std::printf("PASS native static building view mask: %u cases; exactkind4/active/key/slot/immediate scope, onlystock0x80bit. Native mask propagation and wall occlusion require livevalidation.\n",cases);
}

