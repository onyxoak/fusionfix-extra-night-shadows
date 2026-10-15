#pragma once
#include "FusionBackendBytes130.hpp"
#include <string_view>
#include <vector>
namespace backend_versions213 {
struct Layout {std::string_view version;uint32_t extraCallback,admission,extra,head,vehicle,listCallback,vehicleCallback,flagsCallback,listIndex,listFunction;};
inline constexpr Layout Layouts[]={
 {"5.1.1",0xd7940,0x748c0,0x332045,0x332046,0x2d6be1,0xa6a30,0xa6a50,0xa6b50,0x33666c,0x3366f4},
 {"5.1.0",0xd7850,0x74820,0x32b045,0x32b046,0x2cfbe1,0xa6960,0xa6980,0xa6a80,0x32f66c,0x32f6f4},
 {"5.0.1",0xca6d0,0x6fda0,0x33ab99,0x33ab9a,0x2dfa39,0xa0410,0xa0430,0xa0450,0x33df54,0x33dfdc}};
inline const Layout* Find(std::string_view v){for(const auto& l:Layouts)if(l.version==v)return &l;return nullptr;}
template<class Read> bool CallbackBodies(const Layout& l,uint32_t ff,Read read){
 auto extra=fusion_backend130::ExtraBody;auto list=fusion_backend130::ListBody;
 for(auto [offset,rva]:{std::pair<size_t,uint32_t>{0x21,l.extra},{0x26,l.head},{0x2c,l.vehicle}}){uint32_t value=ff+rva;std::memcpy(extra.data()+offset,&value,4);}
 uint32_t index=ff+l.listIndex;std::memcpy(list.data()+0x10,&index,4);
 std::array<uint8_t,52> actualExtra{};std::array<uint8_t,22> actualList{};
 return read(ff+l.extraCallback,actualExtra.data(),actualExtra.size())&&actualExtra==extra&&read(ff+l.listCallback,actualList.data(),actualList.size())&&actualList==list;
}
}
