#include "core/NearbyPoliceShadows.hpp"
#include "core/NearbyPoliceGuard.hpp"
#include "standalone-pe.hpp"
#include <cassert>
#include <iostream>
using namespace lab_siren;
int main(int argc,char** argv){
 auto clear=[](unsigned){return true;};auto valid=[](NearbyPolice::Owner&){return true;};
 auto id=[](unsigned n,unsigned ref=1){return Identity{0x100000+n*0x2000,0x900000,n,ref};};
 for(auto name:PoliceModels)assert(PoliceModel(ModelHash(name)));
 for(auto name:{"polmav","predator","patriot1","patriot2","taxi"})assert(!PoliceModel(ModelHash(name)));
 for(auto site:PoliceSites)assert(PoliceSite(site));assert(!PoliceSite(0x640d0b));
 for(auto site:{0x640d0au,0x64126eu,0x6418f0u,0x6425abu})assert(PoliceOwnerAtSite(site,0x101000,0x202000,0x303000)==0x101000);
 assert(PoliceOwnerAtSite(0x64161d,UINT32_MAX,0x202000,0x303000)==0x202000);
 assert(PoliceOwnerAtSite(0x642053,UINT32_MAX,0x202000,0x303000)==0x303000);
 assert(!PoliceOwnerAtSite(0x64161e,0x101000,0x202000,0x303000));
 for(auto name:PoliceModels)for(auto site:PoliceSites){NearbyPolice p;p.Begin(100,1000,valid);auto* o=p.Touch(id(0),ModelHash(name),site,1,true,true,clear);assert(o);assert(!p.Promote(*o,0));p.Begin(101,1010,valid);assert(p.Promote(*o,123));assert(!p.Promote(*o,456));assert(o->inputBits==123);assert(!p.Touch(id(0),ModelHash(name),PoliceSites[(site==PoliceSites[0])?1:0],1,true,true,clear));}
 NearbyPolice p;p.Begin(1,100,valid);
 for(unsigned i=0;i<8;++i)assert(p.Touch(id(i),ModelHash("police"),PoliceSites[0],float(i+1)*10,i==7,true,clear));
 assert(!p.Touch(id(8),ModelHash("police"),PoliceSites[0],1,false,true,clear));
 p.Begin(2,110,valid);unsigned promoted=0;for(auto& o:p.owners)promoted+=p.Promote(o,0);assert(promoted==2);assert(p.owners[7].rank==0);assert(p.owners[0].rank==1);
 // Ranking is frozen for a frame: new observations and duplicate Begin cannot
 // spend a third promotion, even when player ownership or distance changes.
 p.Touch(id(6),ModelHash("police"),PoliceSites[0],0,true,true,clear);p.Begin(2,111,valid);promoted=0;for(auto& o:p.owners)promoted+=p.Promote(o,0);assert(promoted==0);
 p.Begin(3,120,valid);promoted=0;for(auto& o:p.owners)promoted+=p.Promote(o,0);assert(promoted==2);
 NearbyPolice rearm;rearm.Begin(1,100,valid);auto* a=rearm.Touch(id(0),ModelHash("police"),PoliceSites[0],1,true,true,clear);const auto token=a->token,generation=a->generation;
 rearm.Begin(2,110,valid);assert(rearm.Promote(*a,0));a->eligible=false;rearm.Begin(3,120,valid);assert(!rearm.Promote(*a,0));
 assert(rearm.Touch(id(0),ModelHash("police"),PoliceSites[0],1,true,true,clear)==a);rearm.Begin(4,130,valid);assert(rearm.Promote(*a,0));assert(a->token==token&&a->generation==generation);
 // A despawn/recycled pool slot retires the lease. Re-entry gets a fresh
 // lifetime even when a seven-bit native reference eventually wraps.
 rearm.Begin(5,140,[](NearbyPolice::Owner&){return false;});assert(!a->live);
 a=rearm.Touch(id(0),ModelHash("police"),PoliceSites[0],1,true,true,clear);assert(a&&a->token!=token&&a->generation!=generation);
 const auto oldGeneration=a->generation;auto* b=rearm.Touch(id(0,2),ModelHash("police"),PoliceSites[0],1,true,true,clear);assert(b&&b->generation!=oldGeneration);
 NearbyPolice quarantine;quarantine.Begin(1,100,valid);auto onlyFirst=[](unsigned i){return i==0;};a=quarantine.Touch(id(0),ModelHash("police"),PoliceSites[0],1,false,true,onlyFirst);assert(a);
 quarantine.Begin(2,110,[](NearbyPolice::Owner&){return false;});assert(!quarantine.Touch(id(1),ModelHash("police"),PoliceSites[0],1,false,true,onlyFirst));
 quarantine.Begin(26,360,valid);assert(!quarantine.Touch(id(1),ModelHash("police"),PoliceSites[0],1,false,true,[](unsigned){return false;}));
 a=quarantine.Touch(id(1),ModelHash("police"),PoliceSites[0],1,false,true,onlyFirst);assert(a&&a->token==0&&a->generation==2);
 // A full quarantined/referenced bank rejects new owners rather than aliasing.
 NearbyPolice exhaust;for(unsigned n=0;n<64;++n){exhaust.Begin(n+1,100+n,[](NearbyPolice::Owner&){return false;});assert(exhaust.Touch(id(n),ModelHash("police"),PoliceSites[0],1,false,true,clear));}
 exhaust.Begin(65,165,[](NearbyPolice::Owner&){return false;});assert(!exhaust.Touch(id(65),ModelHash("police"),PoliceSites[0],1,false,true,clear));
 NearbyPolice range;range.Begin(1,100,valid);assert(!range.Touch(id(0),ModelHash("police"),PoliceSites[0],1,false,false,clear));a=range.Touch(id(0),ModelHash("police"),PoliceSites[0],1,false,true,clear);assert(a);
 range.Begin(2,110,valid);range.Touch(id(0),ModelHash("police"),PoliceSites[0],9000,false,false,clear);assert(!range.Promote(*a,0));range.Begin(62,1200,valid);assert(!a->live);
 using P=NearbyPolice;
 // Relevant native input and observed candidates are required. Own/special
 // priority is preserved; secondary police yields to visible relevant lamps.
 assert(P::Compare(1,true,false,0,2,true,true,true,true,false,false,false,false,false,true)==0);
 assert(P::Compare(0,true,false,1,2,true,true,true,true,false,false,false,false,false,true)==2);
 assert(P::Compare(2,false,true,2,1,true,true,true,true,false,false,false,false,true,false)==0);
 assert(P::Compare(1,true,false,1,2,true,true,true,true,false,false,false,false,false,false)==0);
 assert(P::Compare(0,true,false,1,2,true,true,true,true,false,true,false,false,false,false)==2);
 assert(P::Compare(1,true,false,0,2,true,true,true,true,false,true,false,false,false,false)==1);
 assert(P::Compare(1,true,false,0,2,true,true,true,true,false,false,false,true,false,false)==1);
 assert(P::Compare(1,true,false,0,2,true,true,false,true,false,false,false,false,false,false)==1);
 assert(P::Compare(1,true,false,0,2,false,true,true,true,false,false,false,false,false,false)==1);
 assert(P::Compare(1,true,true,0,1,true,true,true,true,false,false,false,false,false,false)==0);
 assert(P::Compare(1,true,true,1,0,true,true,true,true,false,false,false,false,false,false)==2);
 if(argc==2){const auto raw=onyxoak::pe::Read(std::filesystem::path(argv[1]));onyxoak::pe::Image mapped(raw,0x400000);assert(PoliceImageGuard(mapped.bytes,mapped.bytes,0x400000));
  for(auto site:PoliceSites)for(unsigned byte=0;byte<5;++byte){auto changed=mapped.bytes;changed[site+byte]^=1;assert(!PoliceImageGuard(changed,mapped.bytes,0x400000));}
  for(auto site:{0x6bcc50u,0x640790u,0x644b36u,0x640620u,0x63eb30u,0x64152au,0x641ee0u}){auto changed=mapped.bytes;changed[site]^=1;assert(!PoliceImageGuard(changed,mapped.bytes,0x400000));}
  assert(!PoliceImageGuard(mapped.bytes,mapped.bytes,0x500000));std::cout<<"PASS exact CE image guard: all six callsites, dispatcher, wrapper, model table, per-site mutations and relocation rejection\n";
 }
 std::cout<<"PASS emergency coverage2: 13 models x 6 sites; callsite-specific EDI/ESI/ECX owner ABI including counter decoys; one emitter/owner; 8 candidates/2 promotions; frozen frame budget; off/on rearm; despawn/recycled/wrapped reference; quarantine plus native-reference exclusion; key exhaustion; range expiry; primary/secondary/own/lamp priorities\n";
 return 0;
}
