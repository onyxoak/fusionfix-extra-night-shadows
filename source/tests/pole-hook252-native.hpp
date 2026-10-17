// Real midhook regression: preserve the native skinned-path entry after the
// rigid call. A probe at that call would overwrite the alternate entry.
namespace PoleHook252 {
unsigned draws{},probes{};
__declspec(naked) void Draw(){
 __asm{
 inc dword ptr [draws]
 ret
 }
}
void Mid(SafetyHookContext&){++probes;}
void Check(){
 auto* b=static_cast<uint8_t*>(VirtualAlloc(nullptr,256,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(b);memset(b,0x90,256);
 std::array<uintptr_t,3> vt{0,0,uintptr_t(&Draw)};uintptr_t geometry=uintptr_t(vt.data());uintptr_t array=uintptr_t(&geometry);std::array<uintptr_t,2> model{0,uintptr_t(&array)};
 size_t n=0;const auto put=[&](std::initializer_list<uint8_t> bytes){for(auto v:bytes)b[n++]=v;};
 const auto word=[&](uint32_t v){memcpy(b+n,&v,4);n+=4;};
 put({0x55,0x8b,0xec,0x81,0xec,0x80,0,0,0,0xc7,0x44,0x24,0x58});word(uint32_t(uintptr_t(model.data())));
 put({0xc7,0x44,0x24,8,0,0,0,0,0xc7,0x44,0x24,0x78,42,0,0,0,0x83,0x7d,8,0,0x0f,0x85});
 const auto jump=n;word(uint32_t(64+19-(jump+4)));n=64;
 put({0x8b,0x4c,0x24,0x58,0x8b,0x41,4,0x8b,0x4c,0x24,8,0x8b,0x0c,0x88,0x8b,1,0xff,0x50,8,0x8b,0x54,0x24,0x78,0x8b,0xc2,0xc9,0xc3});
 assert(FlushInstructionCache(GetCurrentProcess(),b,256));using Fn=unsigned(__cdecl*)(unsigned);auto fn=reinterpret_cast<Fn>(b);
 assert(fn(0)==42&&draws==1);assert(fn(1)==42&&draws==1);
 auto h=safetyhook::create_mid(b+64,Mid,safetyhook::MidHook::StartDisabled);assert(h&&h.original_bytes().size()==7);assert(h.enable());
 for(unsigned i=0;i<8;++i){const auto d=draws,p=probes;assert(fn(0)==42&&draws==d+1&&probes==p+1);assert(fn(1)==42&&draws==d+1&&probes==p+1);}
 h.reset();assert(VirtualFree(b,0,MEM_RELEASE));
 std::cout<<"PASS252 real x86 midhook: rigid geometry call and alternate skinned entry both preserve continuation/return; no overlapping branch entry\n";
}
}
