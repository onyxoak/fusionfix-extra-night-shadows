#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <array>
#include <cassert>
#include <memory>
#include <iostream>
#include "standalone-support-log.hpp"
#include "ModelEffectObserverState.hpp"
#include "TrafficSignalIdentity.hpp"
#include "ModelOwnerTransformDiagnostic46.hpp"
#include "ModelEffectGetterDiagnostic62.hpp"
using namespace model_effect_observer;
std::unique_ptr<onyxoak::SupportLog> activeLog150;
bool requested=true;std::atomic<bool> enabled{true};
std::atomic<uint32_t> sampled{},readRejected{},contextRejected{},depthRejected{},ringFull{},ringContended{},nativeInactiveSkipped40{};
std::atomic<uint32_t> ownedFusedAttempts79{},ownedFusedComplete79{},ownedFusedLegacy79{},ownedFusedKeys79{};
namespace ModelValue68 {std::atomic<uint32_t> deferred{},matched77{},legacyFallback{},associationRejected{},matchRejected77{};std::array<std::atomic<uint32_t>,9> mismatch77{};}
uint32_t fakeTime150=2000;DWORD Clock150(){return fakeTime150;}
long long stamp150=4444;long long LogClock150()noexcept{return stamp150;}
void Event(unsigned level,const char* code,const std::string& fields={})noexcept{if(activeLog150)activeLog150->Event(level,code,fields);}
// Sample definition is extracted before publisher; deferred ring declarations
// are inserted by the generation script at that exact boundary.
#define GetTickCount Clock150
#include "log-publish150-extracted.inc"
#undef GetTickCount
std::string Contents150(const std::filesystem::path& p){std::string result;for(auto suffix:{".2",".1",""}){auto path=p;path+=suffix;if(!std::filesystem::exists(path))continue;std::ifstream f(path);result.append(std::istreambuf_iterator<char>(f),{});}return result;}
void Seed150(){head=16;tail=0;for(unsigned i=0;i<16;++i){auto& s=ring[i];s={};s.invocation.nonce=i;s.invocation.frame=12;s.diagnostic=true;s.ownerMatched=s.modelInfoMatched=s.descriptorCopied=s.matrixCopied=s.packetCopied=true;s.ownerTransform.status=model_owner_transform46::Status::Valid;s.getter62.status=model_effect_getter62::Status::Observed;s.getter62.localOriginCopied=s.getter62.localOriginFinite=true;s.resolveMismatch=7;}}
int main(){const auto root=std::filesystem::current_path()/"log150-output";assert(!std::filesystem::exists(root));std::filesystem::create_directory(root);
 for(unsigned mode:{0u,1u,2u}){auto old=root/("old"+std::to_string(mode)+".log"),next=root/("new"+std::to_string(mode)+".log");
 activeLog150=std::make_unique<onyxoak::SupportLog>(old,mode,64,LogClock150);Seed150();fakeTime150+=2000;PublishOld150();auto before=activeLog150->Counts150();activeLog150.reset();
 activeLog150=std::make_unique<onyxoak::SupportLog>(next,mode,64,LogClock150);Seed150();fakeTime150+=2000;PublishBatch150();auto after=activeLog150->Counts150();activeLog150.reset();assert(Contents150(old)==Contents150(next));if(mode){assert(before.opens>after.opens&&before.flushes>after.flushes&&before.sizeQueries>after.sizeQueries);std::cout<<"actual production Publish mode"<<mode<<" opens "<<before.opens<<" -> "<<after.opens<<", flushes "<<before.flushes<<" -> "<<after.flushes<<", sizequeries "<<before.sizeQueries<<" -> "<<after.sizeQueries<<" exact output equal\n";}else{assert(before.opens==0&&after.opens==0&&!std::filesystem::exists(old)&&!std::filesystem::exists(next));}}
 // Batch remains bounded, redaction and levels match immediate production.
 auto old=root/"bounds-old.log",next=root/"bounds-new.log";{onyxoak::SupportLog a(old,1,64,LogClock150);for(unsigned i=0;i<1000;++i)a.Event(1,"A",std::string(500,'x'));a.Event(1,"PRIVATE","user@example.com");a.Event(2,"SKIP","no");}
 {onyxoak::SupportLog b(next,1,64,LogClock150);{onyxoak::SupportLog::Batch150 batch(&b);for(unsigned i=0;i<1000;++i)batch.Event(1,"A",std::string(500,'x'));batch.Event(1,"PRIVATE","user@example.com");batch.Event(2,"SKIP","no");}assert(Contents150(old)==Contents150(next));}
 for(auto& p:std::filesystem::directory_iterator(root))assert(p.file_size()<=65536);
 // Fatal startup remains immediate and one-shot even Logging0 inside a batch.
 auto fatal=root/"fatal.log";{onyxoak::SupportLog off(fatal);onyxoak::SupportLog::Batch150 batch(&off);batch.Event(1,"IGNORED","x");off.StartupFailure("fixed_error=1");assert(Contents150(fatal).find("STARTUP_FAILURE")!=std::string::npos);off.StartupFailure("fixed_error=2");}assert(Contents150(fatal).find("fixed_error=2")==std::string::npos);
 auto stamps=root/"stamps.log";{onyxoak::SupportLog s(stamps,1,64,LogClock150);onyxoak::SupportLog::Batch150 batch(&s);stamp150=10;batch.Event(1,"FIRST","x");stamp150=20;batch.Event(1,"SECOND","x");stamp150=30;batch.Flush();assert(Contents150(stamps)=="10 FIRST x\n20 SECOND x\n");}
std::cout<<"Publisher150 byte/rotation/redaction/level/Logging0/startup immediate fixtures PASS\n";
}



