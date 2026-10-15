#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <limits>
#include "NativeProjectionRegistry66.hpp"
namespace reference81 {
using native_projection66::Projection;using native_projection66::Packet;
using native_projection66::Values;
#include "alias81-reference.inc"
}
using namespace native_projection66;
uint32_t Next(uint32_t& seed){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
template<unsigned Max> void Equivalent(unsigned passes){
 AliasPass<Max> current;reference81::AliasPass<Max> reference;
 uint32_t seed=7919;
 for(unsigned run=0;run<passes;++run){current.Begin();reference.Begin();
  for(unsigned step=0;step<Max+5;++step){
   const unsigned index=step%Max;
   const auto k=Next(seed);const uint32_t key=run%3==0?k:run%3==1?((k%11)+1)*64:((k%3)+1);
   Packet packet{};for(auto& word:packet)word=Next(seed);
   // Keep some full projection duplicates, changing only photometry and W.
   if(step&&step%3==0){packet={};packet[17]=2;packet[18]=0;packet[21]=std::bit_cast<uint32_t>(12.f);}
   packet[24]=key;float weight=float(k%9);
   if(step%19==0)weight=std::numeric_limits<float>::quiet_NaN();
   const bool snow=run%2!=0;
   assert(current.Add(index,key,packet,weight,snow)==reference.Add(index,key,packet,weight,snow));
   assert(current.count==reference.count&&current.keys==reference.keys&&current.values==reference.values);
   assert(current.alias==reference.alias&&current.representative==reference.representative);
   for(unsigned n=0;n<current.count;++n)assert(std::bit_cast<uint32_t>(current.importance[n])==std::bit_cast<uint32_t>(reference.importance[n]));
  }
  Packet packet{};assert(!current.Add(Max,1,packet));assert(!current.Add(0,0,packet));
 }
}
void Collisions(unsigned bucket){
 AliasPass<640> current;reference81::AliasPass<640> reference;
 std::array<uint32_t,640> keys{};unsigned count=0;
 for(uint32_t key=1;count<640;++key)if(AliasPass<640>::KeyCell(key)==bucket)keys[count++]=key;
 current.Begin();reference.Begin();Packet packet{};
 for(unsigned i=0;i<640;++i){assert(current.Add(i,keys[i],packet)==reference.Add(i,keys[i],packet));}
 for(unsigned i=0;i<640;++i){assert(current.Add(i,keys[i],packet,float(i))==reference.Add(i,keys[i],packet,float(i)));}
 assert(current.alias==reference.alias&&current.count==640);
 current.Begin();reference.Begin();assert(current.Add(0,keys[0],packet)==reference.Add(0,keys[0],packet));assert(current.count==1);
}
template<class Pass> __declspec(noinline) double Measure(Pass& pass,unsigned population,unsigned duplicate,unsigned loops){
 LARGE_INTEGER a{},b{},frequency{};QueryPerformanceFrequency(&frequency);
 Packet packet{};QueryPerformanceCounter(&a);
 uint32_t witness=0;
 for(unsigned rep=0;rep<loops;++rep){pass.Begin();
  for(unsigned i=0;i<population;++i){packet[8]=i%duplicate;const auto key=0x20000000u+(i%duplicate)*64;
   witness+=pass.Add(i,key,packet,float(i));}
  for(unsigned i=0;i<population;++i)witness+=pass.IsAlias(i);
 }
 QueryPerformanceCounter(&b);assert(witness>0);
 return 1000.*double(b.QuadPart-a.QuadPart)/double(frequency.QuadPart)/loops;
}
int main(){
 SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
 Equivalent<1>(200);Equivalent<7>(200);Equivalent<32>(200);Equivalent<640>(50);Collisions(0);Collisions(AliasPass<640>::Cells-1);
 std::puts("Exact81 differential: unique/duplicate keys, conflicts, ties, NaNs, snow normalization, full capacity, resets and worst-case hash collisions passed.");
 AliasPass<640> current;reference81::AliasPass<640> reference;
 for(unsigned population:{8u,32u,128u,500u,640u})for(unsigned duplicate:{8u,population}){
  std::array<double,9> old{},now{};
  for(unsigned r=0;r<9;++r){if(r%2){old[r]=Measure(reference,population,duplicate,300);now[r]=Measure(current,population,duplicate,300);}else{now[r]=Measure(current,population,duplicate,300);old[r]=Measure(reference,population,duplicate,300);}}
  std::sort(old.begin(),old.end());std::sort(now.begin(),now.end());
  std::printf("MEDIAN population%u unique%u reference%.6fms index%.6fms\n",population,duplicate,old[4],now[4]);
 }
 std::puts("Includes pass reset, complete projection construction/comparison and alias consumption; offline component timing, not game FPS.");
}
