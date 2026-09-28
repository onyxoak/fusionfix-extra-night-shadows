#include "CrashPathContext31.hpp"
#include <cassert>
#include <iostream>
int wmain(int argc,wchar_t** argv) {
    assert(argc==2);assert(crash_path_context::Install(argv[1]));
    uint8_t node[64]{};uint8_t manager[8192]{};uint8_t links[96]{};
    node[8]=37;manager[0x804]=19;links[16]=37;
    uint32_t stack[20]{};stack[0x18/4]=reinterpret_cast<uint32_t>(manager);
    stack[0x30/4]=reinterpret_cast<uint32_t>(links);stack[0x4C/4]=2;
    CONTEXT c{};c.Eip=0x400000+0x4E7E3B;c.Edi=reinterpret_cast<uint32_t>(node);
    c.Esi=1;c.Esp=reinterpret_cast<uint32_t>(stack);
    const CONTEXT original=c;
    crash_path_context::Capture(0x400000,c);
    assert(std::memcmp(&original,&c,sizeof(c))==0);
    const auto& r=crash_path_context::storage;
    assert(r.currentNode.bytes==64&&r.currentNode.data[8]==37);
    assert(r.linkedNode.bytes==0&&r.linkedNode.address==1);
    assert(r.pathManager.bytes==0x1C10&&r.pathManager.data[0x804]==19);
    assert(r.linkData.bytes==64&&r.linkData.data[0]==37);
    assert(GetFileSize(crash_path_context::file,nullptr)==sizeof(r));
    c.Eip=0x400000+0x4E7DC5;c.Edx=2;stack[0x4C/4]=99;
    crash_path_context::Capture(0x400000,c);
    assert(r.linkData.bytes==64&&r.linkData.data[0]==37);
    assert(GetFileSize(crash_path_context::file,nullptr)==2*sizeof(r));
    c.Edx=UINT32_MAX;
    crash_path_context::Capture(0x400000,c);
    assert(r.linkData.bytes==0); // arithmetic overflow never becomes a read
    assert(GetFileSize(crash_path_context::file,nullptr)==3*sizeof(r));
    c.Eip=0;crash_path_context::Capture(0x400000,c);
    assert(GetFileSize(crash_path_context::file,nullptr)==3*sizeof(r));
    crash_path_context::Remove();
    std::cout<<"PASS bounded path-context capture, invalid pointer handling, context unchanged, unrelated faults ignored\n";
}
