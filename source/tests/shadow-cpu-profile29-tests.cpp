#include "core/ShadowCpuProfile29.hpp"
#include <cassert>
#include <thread>
int main(){
 using namespace shadow_cpu29;
 Configure(false);
 {Scope disabled(Phase::Selection,1);}
 assert(counters[0].calls.load()==0&&counters[0].samples.load()==0);
 Configure(true);assert(enabled&&frequency);
 for(unsigned i=0;i<128;++i){
  SetLastError(0x12345678);
  {Scope sample(Phase::Selection,64);assert(GetLastError()==0x12345678);}
  assert(GetLastError()==0x12345678);
 }
 assert(counters[0].calls.load()==128&&counters[0].samples.load()==2);
 auto work=[](){for(unsigned i=0;i<1024;++i){Scope sample(Phase::Sources,16);}};
 std::thread a(work),b(work);a.join();b.join();
 assert(counters[1].calls.load()==2048&&counters[1].samples.load()==128);
 assert(counters[1].maximum.load()<=counters[1].ticks.load());
 Configure(false);{Scope disabled(Phase::Sources,1);}
 assert(counters[1].calls.load()==2048);
}
