#include "ShortlistHelperRuntime48.hpp"
#include <map>
#include <vector>
#include <cassert>
#include <cstdio>
namespace g=shortlist_helper48;
std::map<uintptr_t,std::vector<uint8_t>> memory;
unsigned reads{};
bool Read(uintptr_t a,void* p,size_t n) noexcept {++reads;for(auto& [base,bytes]:memory)if(a>=base&&a-base<=bytes.size()&&n<=bytes.size()-(a-base)){std::memcpy(p,bytes.data()+a-base,n);return true;}return false;}
template<class T> void Put(uintptr_t a,T value){auto& bytes=memory[a];bytes.resize(sizeof(value));std::memcpy(bytes.data(),&value,sizeof(value));}
template<size_t N,size_t R> void Code(uintptr_t a,const uint8_t(&bytes)[N],const unsigned(&relocs)[R],uintptr_t base,uintptr_t recorded){
 memory[a]={bytes,bytes+N};for(auto offset:relocs){uint32_t v{};std::memcpy(&v,memory[a].data()+offset,4);v=uint32_t(v-recorded+base);std::memcpy(memory[a].data()+offset,&v,4);}
}
void Jump(uintptr_t a,uintptr_t target){memory[a].resize(5);memory[a][0]=0xe9;int32_t d=int32_t(target-a-5);std::memcpy(memory[a].data()+1,&d,4);}
void Setup(const g::Binding& b){
 memory.clear();Code(b.game+g::NativeModeRva,g::NativeModeBytes,g::NativeModeRelocs,b.game,0x440000);Code(b.game+g::NativeSizeRva,g::NativeSizeBytes,g::NativeSizeRelocs,b.game,0x440000);
 Code(b.game+g::NativeInitRva,g::NativeInitBytes,g::NativeInitRelocs,b.game,0x440000);Code(b.game+g::NativeReadRva,g::NativeReadBytes,g::NativeReadRelocs,b.game,0x440000);Code(b.fusion+g::FusionReadRva,g::FusionReadBytes,g::FusionReadRelocs,b.fusion,0x56d00000);
 Put<uint32_t>(b.game+0xc45538,0);Put<uint32_t>(b.game+0x1899330,1);Put<uint64_t>(b.game+0x13ed968,0x1c200000);Put<uint32_t>(b.game+0xd60ea8,1);
 Jump(b.game+0x525e70,0x310000);Jump(0x310000,b.fusion+g::FusionReadRva);
 Put<uint32_t>(b.fusion+0x366150,3);Put<uint32_t>(0x200000+12,0x210000);Put<int32_t>(b.fusion+0x336604,INT32_MIN+4);Put<int32_t>(0x210010,INT32_MIN+8);
 Put<uint32_t>(b.fusion+0x3365fc,uint32_t(b.fusion+0x340000));Put<uint32_t>(b.fusion+0x340000,2);Put<uint8_t>(b.fusion+0x336518,1);Put<uint32_t>(b.fusion+0x2e3980,42);
}
int main(){
 const g::Binding original{0x440000,0x56d00000,0x1a00000,6062080,true};
 for(auto b:{original,g::Binding{0x540000,0x46d00000,0x1a00000,6062080,true}}){Setup(b);assert(g::Check(b,Read,0x200000)==g::Reason::Safe);}
 auto b=original;Setup(b);b.exact511Authenticated=false;assert(g::Check(b,Read,0x200000)==g::Reason::Unconfigured);b=original;
 for(unsigned mode:{1,2,3,4}){Setup(b);Put<uint32_t>(b.game+0xc45538,mode);assert(g::Check(b,Read,0x200000)==g::Reason::Mode);}
 Setup(b);Put<uint32_t>(b.game+0x1899330,0);assert(g::Check(b,Read,0x200000)==g::Reason::NativeInit);
 Setup(b);memory[b.fusion+g::FusionReadRva][0x120]^=1;assert(g::Check(b,Read,0x200000)==g::Reason::Bytes);
 Setup(b);Jump(0x310000,b.fusion+g::FusionReadRva+1);assert(g::Check(b,Read,0x200000)==g::Reason::Thunk);
 Setup(b);Put<uint32_t>(b.fusion+0x366150,4096);assert(g::Check(b,Read,0x200000)==g::Reason::TlsIndex);
 Setup(b);memory.erase(0x20000c);assert(g::Check(b,Read,0x200000)==g::Reason::TlsPointer);
 Setup(b);Put<int32_t>(b.fusion+0x336604,-1);assert(g::Check(b,Read,0x200000)==g::Reason::TlsEpoch);
 Setup(b);Put<int32_t>(0x210010,INT32_MIN+3);assert(g::Check(b,Read,0x200000)==g::Reason::TlsEpoch);
 Setup(b);Put<uint32_t>(b.fusion+0x3365fc,0x300000);Put<uint32_t>(0x300000,2);assert(g::Check(b,Read,0x200000)==g::Reason::QualityPointer);
 Setup(b);memory.erase(b.fusion+0x340000);assert(g::Check(b,Read,0x200000)==g::Reason::ReadFault);
 Setup(b);assert(g::Check(b,Read,0x200000)==g::Reason::Safe);Put<uint32_t>(b.game+0x1899330,0);assert(g::Check(b,Read,0x200000)==g::Reason::NativeInit); // No cached approval between operations.
 uint32_t output=123;assert(!g::runtime::ReadBytes(1,&output,4)&&output==123);assert(!g::runtime::ReadBytes(0x10000,&output,4));
 std::puts("helper48: initialized routes, relocated bytes, mode/init/TLS/ownership/heap/changed-code/read-fault rejection and fresh operation checks passed; runtime not claimed.");
}
