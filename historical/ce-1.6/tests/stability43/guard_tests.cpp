#include "source/ShadowAllocationCE.hpp"
#include <windows.h>
#include <fstream>
#include <vector>
#include <cassert>
#include <iostream>
int main(){
    std::ifstream f("D:/SteamLibrary/steamapps/common/Grand Theft Auto IV/GTAIV/GTAIV.exe",std::ios::binary);
    std::vector<uint8_t> file((std::istreambuf_iterator<char>(f)),{});
    assert(file.size()>4096);
    auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(file.data());
    auto* pe=reinterpret_cast<const IMAGE_NT_HEADERS32*>(file.data()+dos->e_lfanew);
    std::vector<uint8_t> image(pe->OptionalHeader.SizeOfImage);
    std::memcpy(image.data(),file.data(),pe->OptionalHeader.SizeOfHeaders);
    auto* sections=IMAGE_FIRST_SECTION(pe);
    for(unsigned i=0;i<pe->FileHeader.NumberOfSections;++i){
        const auto& s=sections[i];assert(s.PointerToRawData+s.SizeOfRawData<=file.size());
        assert(s.VirtualAddress<image.size());
        const auto count=(std::min<size_t>)(s.SizeOfRawData,static_cast<size_t>(image.size()-s.VirtualAddress));
        std::memcpy(image.data()+s.VirtualAddress,file.data()+s.PointerToRawData,count);
    }
    using namespace fusionfix::shadows::ce::allocation;
    constexpr uintptr_t preferred=0x400000;
    assert(ValidateMappedImage(image.data(),image.size(),preferred));
    constexpr uint8_t site[]{0x85,0xc0,0x74,0x18,0x83,0xf8,0x01};
    assert(std::memcmp(image.data()+CompareResultRva,site,sizeof(site))==0);
    image[CompareResultRva]^=1;assert(!ValidateMappedImage(image.data(),image.size(),preferred));image[CompareResultRva]^=1;
    constexpr uintptr_t relocated=0x10000000;
    const auto dir=pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    for(unsigned off=0;off<dir.Size;){
        auto* block=reinterpret_cast<const IMAGE_BASE_RELOCATION*>(image.data()+dir.VirtualAddress+off);
        if(block->SizeOfBlock<8)break;
        auto* fix=reinterpret_cast<const uint16_t*>(block+1);
        for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i)if((fix[i]>>12)==IMAGE_REL_BASED_HIGHLOW){
            auto* word=reinterpret_cast<uint32_t*>(image.data()+block->VirtualAddress+(fix[i]&0xfff));
            *word+=static_cast<uint32_t>(relocated-preferred);
        }
        off+=block->SizeOfBlock;
    }
    assert(ValidateMappedImage(image.data(),image.size(),relocated));
    image[CompareResultRva+2]^=1;assert(!ValidateMappedImage(image.data(),image.size(),relocated));
    std::cout<<"PASS CE executable, exact comparator-hook boundary, relocation, incompatible-code rejection\n";
}

