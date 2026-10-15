#include "../core/LabSirenVacancy.hpp"
#include <cstdio>
#include "../standalone-ownership.hpp"
#include <cstdlib>
using namespace lab_siren;
void check(bool v){if(!v)std::abort();}
int main(){
 for(unsigned fail=0;fail<16;++fail){onyxoak::ActivationGate gate(16);for(unsigned i=0;i<16;++i)gate.HookEnabled(i!=fail);check(!gate.Commit());} onyxoak::ActivationGate complete(16);for(unsigned i=0;i<16;++i)check(complete.HookEnabled(true));check(complete.Commit());
 std::array<VacancyEntry,7> lit{};for(unsigned i=0;i<7;++i)lit[i]={i+1,1,false};auto dark=lit;dark[0]={8,0,false};
 auto arm=[&](Vacancy& v){check(v.Filter(100,1000,true,true,true,true,.02f,1,lit)==-1);};
 Vacancy v;arm(v);check(v.Filter(101,1010,true,true,true,true,.0002f,1,dark)==0);
 check(v.Filter(102,1020,true,true,true,true,.02f,1,dark)==-1);
 check(v.Filter(102,1020,true,true,true,false,0,1,dark)==-1);
 auto priority=dark;priority[0].protectedPriority=true;check(v.Filter(102,1020,true,true,true,true,0,1,priority)==-1);
 auto changed=dark;changed[1].generation=2;check(v.Filter(102,1020,true,true,true,true,0,1,changed)==-1);
 auto hole=dark;hole[2].key=0;check(v.Filter(102,1020,true,true,true,true,0,1,hole)==-1);
 check(v.Filter(113,1050,true,true,true,true,0,1,dark)==-1);
 arm(v);check(v.Filter(101,1121,true,true,true,true,0,1,dark)==-1);
 for(unsigned mode=0;mode<3;++mode){arm(v);check(v.Filter(101,1010,mode!=0,mode!=1,mode!=2,true,0,1,dark)==-1);check(v.Filter(102,1020,true,true,true,true,0,1,dark)==-1);}
 arm(v);check(v.Filter(101,1010,true,true,true,true,NAN,1,dark)==-1);
 std::puts("PASS bounded vacancy: exact six incumbents, dark known input, no bright/unknown suppression, protected priority, generation change, holes, frame/time expiry and off/disabled/invalid release");
}

