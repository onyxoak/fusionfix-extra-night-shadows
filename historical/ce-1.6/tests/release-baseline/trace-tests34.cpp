#include "ShadowTrace34.hpp"
#include <cassert>
#include <memory>
#include <thread>
#include <iostream>
int main(){using namespace ShadowTrace34;
 auto r=std::make_unique<Recorder>();auto out=std::make_unique<Event[]>(Recorder::Capacity);
 Event e{1,1,10,42,0,1,4,-1,1,2,3,4};assert(r->Push(e));e.tick=11;assert(r->Push(e));
 assert(r->Drain(out.get(),Recorder::Capacity)==1);assert(out[0].key==42);
 for(unsigned i=0;i<Recorder::Capacity;++i){e.frame=i+2;assert(r->Push(e));}
 e.frame+=1;assert(!r->Push(e));assert(r->dropped==1);
 assert(r->Drain(out.get(),Recorder::Capacity)==Recorder::Capacity);
 auto producer=[&](unsigned key){for(unsigned i=0;i<10000;++i){Event x{4,i,i,key,1,-1,0,1,0,0,0,0};r->Push(x);}};
 unsigned before=r->dropped;std::thread a(producer,11),b(producer,22);a.join();b.join();
 auto count=r->Drain(out.get(),Recorder::Capacity);assert(count+r->dropped-before==20000);
 for(unsigned i=0;i<count;++i)assert((out[i].key==11||out[i].key==22)&&out[i].stage==4&&out[i].b==-1);
 assert(Start("D:/GTA-IV-Collection-Lab/diagnostic-candidate34/test-trace.bin",2,123));
 Track(77);assert(Tracked(77)&&!Tracked(78));Emit({5,4,100,77,1,4,1,0,0,0,0,0});Flush();
 assert(std::filesystem::file_size(outputPath)==32+48);
 written=MaximumBytes;Emit({5,5,101,77,1,4,1,0,0,0,0,0});Flush();assert(!enabled);
 std::cout<<"PASS trace deduplication, bounded overflow, concurrent integrity, file framing, tracking and size cap\n";
}
