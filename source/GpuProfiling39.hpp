#pragma once
#include <d3d9.h>
#include <span>
#include <cstring>
#include "GpuQueryRing39.hpp"
namespace gpu39 {
static_assert(sizeof(void*)==4,"Exact CE x86 graphics ABI");
struct NativeBackend {
 using Device=IDirect3DDevice9*;using Query=IDirect3DQuery9*;
 bool Supports(Device d,int type){return SUCCEEDED(d->CreateQuery(D3DQUERYTYPE(type),nullptr));}
 bool Create(Device d,int type,Query& q){return SUCCEEDED(d->CreateQuery(D3DQUERYTYPE(type),&q))&&q;}
 void Release(Query& q){if(q){q->Release();q=nullptr;}}
 bool Issue(Query q,unsigned flags){return SUCCEEDED(q->Issue(flags));}
 int Get(Query q,void* out,unsigned size,unsigned flags){return int(q->GetData(out,size,flags));}
};
// ScriptHook's wrapper forwards reference counts through this+4. That does
// not establish lifetime of the wrapper storage itself during replacement.
inline bool NonOwningReferenceForwarder(std::span<const uint8_t> code,uint8_t methodOffset){
 constexpr uint8_t prefix[]{0x8b,0x44,0x24,0x04,0x8b,0x40,0x04,0x8b,0x08,0x89,0x44,0x24,0x04,0x8b,0x51};
 return code.size()>=18&&std::memcmp(code.data(),prefix,sizeof(prefix))==0&&code[15]==methodOffset&&code[16]==0xff&&code[17]==0xe2;
}
inline bool Forwarder(std::span<const uint8_t> code,uint32_t methodOffset){
 constexpr uint8_t prefix[]{0x8b,0x44,0x24,0x04,0x8b,0x80,0xac,0x11,0x00,0x00,0x8b,0x08,0x89,0x44,0x24,0x04};
 if(code.size()<19||std::memcmp(code.data(),prefix,sizeof(prefix)))return false;
 if(methodOffset<128)return code[16]==0xff&&code[17]==0x61&&code[18]==methodOffset;
 uint32_t got=0;if(code.size()<22||code[16]!=0xff||code[17]!=0xa1)return false;std::memcpy(&got,code.data()+18,4);return got==methodOffset;
}
}
