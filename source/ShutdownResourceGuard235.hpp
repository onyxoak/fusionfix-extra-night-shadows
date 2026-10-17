#pragma once
#include <cstddef>
#include <cstdint>
namespace shutdown_resource235 {
// Native shutdown passes a signed 16-bit resource index. -1 denotes no slot.
// The observed release routine used that sentinel as an array index and wrote
// before the pool, corrupting a live physics-material hash bucket.
inline constexpr bool Invalid(std::uint32_t signExtendedIndex) noexcept {
 return static_cast<std::int32_t>(signExtendedIndex)<0;
}
inline constexpr std::size_t BodyBytes=46,HookOffset=4,ResumeOffset=29;
inline constexpr char Pattern[]=
 "0F BF 4F 52 FF 35 ? ? ? ? 51 E8 ? ? ? ? 0F BF 47 52 50 E8 ? ? ? ? 83 C4 0C 81 C6 10 01 00 00 81 FE ? ? ? ? 7C C3 5F 5E C3";
inline constexpr unsigned char Body[BodyBytes]={
 0x0f,0xbf,0x4f,0x52,0xff,0x35,0,0,0,0,0x51,0xe8,0,0,0,0,
 0x0f,0xbf,0x47,0x52,0x50,0xe8,0,0,0,0,0x83,0xc4,0x0c,
 0x81,0xc6,0x10,0x01,0,0,0x81,0xfe,0,0,0,0,0x7c,0xc3,0x5f,0x5e,0xc3};
inline bool Wildcard(std::size_t i) noexcept {
 return (i>=6&&i<10)||(i>=12&&i<16)||(i>=22&&i<26)||(i>=37&&i<41);
}
inline bool Shape(const unsigned char* p,std::size_t n) noexcept {
 if(!p||n<BodyBytes)return false;
 for(std::size_t i=0;i<BodyBytes;++i)if(!Wildcard(i)&&p[i]!=Body[i])return false;
 return true;
}
template<class Context> bool Apply(Context& c,std::uintptr_t resume) noexcept {
 if(!Invalid(static_cast<std::uint32_t>(c.ecx)))return false;
 // Entry is before BOTH argument pushes; resume is after their matching ADD.
 // No stack/register/flags adjustment is needed or permitted on this path.
 c.eip=resume;return true;
}
}
