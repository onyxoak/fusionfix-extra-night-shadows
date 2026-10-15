#include "PoleDepthRepair157.hpp"
// Bounded native command/consumer/child receipts. No geometry decision changes.
namespace PoleDrawWitness145 {
SafetyHookMid objectHook,constructorHook,childHook;SafetyHookInline consumerHook;
struct Hint {pole_caster134::Snapshot owner{};PoleCasterDiagnostic134::Binding map{};bool sun{};};
struct Receipt {Hint hint{};uint32_t command{},model{},alphaA{},alphaB{},flags{},data6{},data7{},expectedTag{};std::array<float,16> pose{};bool valid{};};
thread_local Hint hint;thread_local std::array<Receipt,128> records{};thread_local unsigned next{},published{},objects{},sunAttempts{},sunWindowUsed{},children{},constructorAttempts{};thread_local uint32_t clip{},receiptFrame{},sunWindow{};
thread_local Receipt current;
struct ObjectEntry {Hint hint{};uint32_t stack{},mode{};bool valid{};};thread_local ObjectEntry objectEntry;
struct Preserve {DWORD error=GetLastError();fusionfix::shadows::FloatingPointState fp;~Preserve()noexcept{SetLastError(error);}};
bool Window() noexcept {return PoleCasterDiagnostic134::requested&&CaptureDiagnostic44::sampleSources.load(std::memory_order_relaxed)&&ShadowTrace34::enabled.load(std::memory_order_relaxed)&&uint32_t(GetTickCount()-CaptureDiagnostic44::lastStart)<10000u;}
bool Read(uint64_t at,void* out,size_t n) noexcept{return pole_caster134::Address(at,n)&&Liberty20::Copy(uintptr_t(at),out,n);}
uint32_t Frame()noexcept{uint32_t v{};return CShadows::pFrameCounter&&Liberty20::Copy(uintptr_t(CShadows::pFrameCounter),&v,4)?v:UINT32_MAX;}
void ResetClip()noexcept{if(clip!=CaptureDiagnostic44::lastStart){clip=CaptureDiagnostic44::lastStart;records={};hint={};current={};objectEntry={};next=published=objects=sunAttempts=sunWindowUsed=children=constructorAttempts=0;receiptFrame=0;sunWindow=GetTickCount();}}
template<class Map> void HintPole(const pole_caster134::Snapshot& value,const Map& map)noexcept{
 if(!Window())return;ResetClip();hint={value,map};
}
bool CurrentOwner(const Hint& h)noexcept {
 if(!h.map.valid||h.map.clip!=CaptureDiagnostic44::lastStart||h.map.thread!=GetCurrentThreadId()||Frame()!=h.map.frame)return false;
 pole_caster134::Snapshot fresh{},expected=h.owner;if(!pole_caster134::Capture(Read,Frame,uint32_t(game),h.owner.entity,h.map.frame,fresh))return false;
 // Native7103FB sets this temporary render bit,710556 clears it. This is
 // observation identity only; no flag is changed or eligibility bypassed.
 fresh.flags24&=~0x800u;expected.flags24&=~0x800u;if(std::memcmp(&fresh,&expected,sizeof(fresh)))return false;
 if(h.sun)return true;
 std::array<uint8_t,19> map{};uint32_t key{},kind{};
 if(!Read(uint64_t(game)+0xd9f1ed+h.map.slot*0x110,map.data(),19)||map[0]!=1)return false;
 std::memcpy(&key,map.data()+11,4);std::memcpy(&kind,map.data()+15,4);return key==h.map.key&&kind==h.map.kind;
}
void SunCaster(uintptr_t entity,uint32_t type,uint32_t mode,uintptr_t stackFrame,bool excluded)noexcept {
 if(type!=4||mode!=2||excluded||!Window())return;const Preserve preserve;ResetClip();const auto now=GetTickCount();if(uint32_t(now-sunWindow)>=128){sunWindow=now;sunWindowUsed=0;}if(objects>=128||sunAttempts>=128||sunWindowUsed>=8)return;++sunAttempts;++sunWindowUsed;
 const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 const auto thread=GetCurrentThreadId();if(thread!=PlayerShadowAllocation::ownerThread.load(std::memory_order_relaxed))return;
 uint32_t actualMode{};if(!Read(uint64_t(stackFrame)+0x14,&actualMode,4)||actualMode!=2)return;
 // Cheap model hint before a bounded full publication; no global object scan.
 int16_t index{};uint32_t info{},hash{};if(!Read(uint64_t(entity)+0x2e,&index,2)||index<0||index>=31000||!Read(uint64_t(game)+0xe95cd8+uint32_t(index)*4,&info,4)||!Read(uint64_t(info)+0x3c,&hash,4)||!pole_caster134::SupportedPole(hash))return;
 const auto frame=Frame();pole_caster134::Snapshot value{};if(!pole_caster134::Capture(Read,Frame,uint32_t(game),uint32_t(entity),frame,value))return;
 hint={value,{0,0,2,frame,CaptureDiagnostic44::lastStart,thread,true},true};
}
void Emit(unsigned stage,const Receipt& r,uint32_t a,uint32_t b,uint32_t c,uint32_t d)noexcept{
 ShadowTrace34::Emit({stage,r.hint.map.frame,GetTickCount(),r.hint.owner.entity,int32_t(a),int32_t(b),int32_t(c),int32_t(d),0,0,0,0});
}
void Object(SafetyHookContext& regs)noexcept{
 objectEntry={};
 if(!Window())return;const Preserve preserve;ResetClip();if(objects>=128||uint32_t(regs.ecx)!=hint.owner.entity)return;
 const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 std::array<uint32_t,5> args{},again{};if(!Read(regs.esp,args.data(),sizeof(args))||args[0]!=uint32_t(game)+0x6e3864||args[2]!=(hint.sun?2u:0u)||!CurrentOwner(hint)||!Read(regs.esp,again.data(),sizeof(again))||again!=args)return;
 objectEntry={hint,uint32_t(regs.esp),args[2],true};Receipt r{};r.hint=hint;++objects;Emit(194,r,args[1],args[2],args[3],args[4]);
}
void Constructor(SafetyHookContext& regs)noexcept{
 const ObjectEntry entry=objectEntry;objectEntry={};
 if(!Window())return;const Preserve preserve;ResetClip();if(published>=128||constructorAttempts>=128||!entry.valid)return;++constructorAttempts;
 const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 uint32_t parentReturn{},outerReturn{},outerMode{};
 Receipt candidate{};candidate.hint=entry.hint;Emit(189,candidate,uint32_t(regs.esp),entry.stack,uint32_t(regs.esi),entry.mode); // Candidate-only stack provenance, not accepted command.
 if(!entry.valid||uint64_t(regs.esp)+0x60!=entry.stack||!Read(uint64_t(regs.esp)+0x34,&parentReturn,4)||(parentReturn!=uint32_t(game)+0x713087&&parentReturn!=uint32_t(game)+0x71303c)||!Read(entry.stack,&outerReturn,4)||outerReturn!=uint32_t(game)+0x6e3864||!Read(uint64_t(entry.stack)+8,&outerMode,4)||outerMode!=entry.mode)return;
 std::array<uint32_t,8> args{};if(!Read(regs.esp,args.data(),sizeof(args))||args[0]!=uint32_t(game)+0x7104f2||uint32_t(regs.esi)!=entry.hint.owner.entity||args[1]!=entry.hint.owner.modelIndex||args[7]!=entry.mode||!CurrentOwner(entry.hint))return;
 // Constructor fifth argument is the actual matrix copied into command+10.
 Receipt r{};r.hint=entry.hint;r.command=uint32_t(regs.ecx);r.model=args[1];r.alphaA=args[2];r.alphaB=args[3];r.flags=args[4];r.data6=args[6];r.data7=args[7];
 uint32_t oldTag{},counter{},againCounter{};
 if(!Read(uint64_t(r.command)+4,&oldTag,4)||!Read(uint64_t(game)+0xc327a0,&counter,4))return;
 // Exact guarded native4DAE72..85 keeps high18 bits and installs counter low14.
 r.expectedTag=(oldTag&0xffffc000u)|(counter&0x3fffu);
 std::array<uint32_t,8> againArgs{};std::array<float,16> againPose{};
 if(!Read(args[5],r.pose.data(),64)||!Read(regs.esp,againArgs.data(),sizeof(againArgs))||againArgs!=args||!Read(args[5],againPose.data(),64)||againPose!=r.pose||!CurrentOwner(entry.hint)||!Read(uint64_t(game)+0xc327a0,&againCounter,4)||againCounter!=counter)return;
 if(receiptFrame!=r.hint.map.frame){records={};next=0;receiptFrame=r.hint.map.frame;}
 r.valid=true;records[next++%records.size()]=r;++published;
 Emit(190,r,r.command,r.model,args[5],r.hint.map.key);Emit(191,r,r.flags,r.alphaA,r.alphaB,r.data7);
}
bool ConsumerReceipt(uintptr_t object,Receipt& r)noexcept{
 if(!Window())return false;const Preserve preserve;ResetClip();const auto frame=Frame();if(!receiptFrame||receiptFrame!=frame)return false;
 const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 for(unsigned i=next;i>0;--i){const auto& item=records[i-1];if(item.valid&&item.command==object&&item.hint.map.frame==frame){
  std::array<uint8_t,0x60> bytes{},again{};if(!Read(object,bytes.data(),bytes.size())||!Read(object,again.data(),again.size())||bytes!=again||!CurrentOwner(item.hint))return false;
  uint16_t model{};uint32_t tag{},view{},mode{};std::memcpy(&tag,bytes.data()+4,4);std::memcpy(&model,bytes.data()+0x58,2);std::memcpy(&view,bytes.data()+0x50,4);std::memcpy(&mode,bytes.data()+0x54,4);if(model!=item.model||tag!=item.expectedTag||view!=item.data6||mode!=item.data7||bytes[0x5a]!=uint8_t(item.alphaA)||bytes[0x5b]!=uint8_t(item.alphaB)||bytes[0x5c]!=uint8_t(item.flags))return false;
  // Native constructor only copies XYZ of each row; w is deliberately ignored.
  for(unsigned element=0;element<16;++element)if(element%4!=3){float value{};std::memcpy(&value,bytes.data()+0x10+element*4,4);if(std::memcmp(&value,&item.pose[element],4))return false;}
  r=item;Emit(192,r,uint32_t(object),model,bytes[0x5a],bytes[0x5b]);return true;
 }}return false;
}
//155 Observes the command actually executing now. No queued owner/map token
//is reused: key=command address, model identity is authenticated afresh.
struct Actual155 {uint32_t command{},model{},info{},hash{},fragment{},mode{},view{},thread{},frame{};std::array<uint8_t,0x60> bytes{};bool valid{};};
thread_local Actual155 actual155;
thread_local unsigned actualCommands155{},actualChildren155{},actualEntries155{},actualAttempts155{},actualWindowUsed155{};
thread_local uint32_t actualClip155{},actualWindow155{};thread_local bool actualExhausted155{};
bool ActualCommand155(uintptr_t object,Actual155& out)noexcept {
 out={};if(!Window()||object>UINT32_MAX)return false;
 const Preserve preserve;const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 if(actualClip155!=CaptureDiagnostic44::lastStart){actualClip155=CaptureDiagnostic44::lastStart;actualCommands155=actualChildren155=actualEntries155=actualAttempts155=actualWindowUsed155=0;actualWindow155=GetTickCount();actualExhausted155=false;}
 const auto now155=GetTickCount();if(uint32_t(now155-actualWindow155)>=128){actualWindow155=now155;actualWindowUsed155=0;}
 if(actualAttempts155>=4096||actualWindowUsed155>=64){if(!actualExhausted155){actualExhausted155=true;ShadowTrace34::Emit({204,0,now155,0,int32_t(actualAttempts155),int32_t(actualWindowUsed155),0,0,0,0,0,0});}return false;}
 ++actualAttempts155;++actualWindowUsed155;
 if(actualEntries155<16){++actualEntries155;ShadowTrace34::Emit({200,Frame(),GetTickCount(),uint32_t(object),int32_t(GetCurrentThreadId()),0,0,0,0,0,0,0});}
 if(actualCommands155>=128)return false;
 Actual155 r{};r.command=uint32_t(object);r.thread=GetCurrentThreadId();r.frame=Frame();
 if(!Read(object,r.bytes.data(),r.bytes.size()))return false;
 uint16_t index{};std::memcpy(&index,r.bytes.data()+0x58,2);r.model=index;
 std::memcpy(&r.mode,r.bytes.data()+0x54,4);std::memcpy(&r.view,r.bytes.data()+0x50,4);
 if(index>=31000||(r.mode!=0&&r.mode!=2))return false;
 const uint64_t slot=uint64_t(game)+0xe95cd8u+uint32_t(index)*4u;
 if(!Read(slot,&r.info,4)||!Read(uint64_t(r.info)+0x3c,&r.hash,4)||!pole_caster134::SupportedPole(r.hash)||!Read(uint64_t(r.info)+8,&r.fragment,4)||!r.fragment)return false;
 uint32_t info{},hash{},fragment{};std::array<uint8_t,0x60> again{};
 if(!Read(object,again.data(),again.size())||again!=r.bytes||!Read(slot,&info,4)||info!=r.info||!Read(uint64_t(info)+0x3c,&hash,4)||hash!=r.hash||!Read(uint64_t(info)+8,&fragment,4)||fragment!=r.fragment||Frame()!=r.frame)return false;
 r.valid=true;out=r;++actualCommands155;
 ShadowTrace34::Emit({198,r.frame,GetTickCount(),r.command,int32_t(r.model),int32_t(r.hash),int32_t(r.view),int32_t(r.mode),0,0,0,0});
 uint32_t tag{};std::memcpy(&tag,r.bytes.data()+4,4);
 ShadowTrace34::Emit({199,r.frame,GetTickCount(),r.command,int32_t(tag),int32_t(r.bytes[0x5a]),int32_t(r.bytes[0x5b]),int32_t(r.bytes[0x5c]),0,0,0,0});
 return true;
}
void ActualChild155(SafetyHookContext& regs)noexcept {
 if(!actual155.valid||!Window()||actualChildren155>=512)return;
 const Preserve preserve;const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 const auto& r=actual155;std::array<uint8_t,0x60> command{};uint32_t info{},hash{},fragment{},group{},againGroup{};std::array<uint32_t,4> view{},againView{};
 if(r.thread!=GetCurrentThreadId()||Frame()!=r.frame||!Read(r.command,command.data(),command.size())||command!=r.bytes||
  !Read(uint64_t(game)+0xe95cd8u+r.model*4u,&info,4)||info!=r.info||!Read(uint64_t(info)+0x3c,&hash,4)||hash!=r.hash||
  !Read(uint64_t(info)+8,&fragment,4)||fragment!=r.fragment||!Read(regs.eax,view.data(),16)||!Read(uint64_t(regs.ecx)+8,&group,4)||
  !Read(regs.eax,againView.data(),16)||againView!=view||!Read(uint64_t(regs.ecx)+8,&againGroup,4)||againGroup!=group||Frame()!=r.frame)return;
 ++actualChildren155;
 ShadowTrace34::Emit({201,r.frame,GetTickCount(),r.command,int32_t(regs.ecx),int32_t(group),int32_t(view[3]),int32_t(r.mode),0,0,0,0});
 ShadowTrace34::Emit({202,r.frame,GetTickCount(),r.command,int32_t(view[0]),int32_t(view[1]),int32_t(view[2]),int32_t(view[3]),0,0,0,0});
 std::array<float,4> scalars{};if(Read(uint64_t(game)+0xc3f260,scalars.data(),16))ShadowTrace34::Emit({203,r.frame,GetTickCount(),r.command,int32_t(r.model),int32_t(r.mode),0,0,scalars[0],scalars[1],scalars[2],scalars[3]});
}
uint32_t __fastcall Consumer(void* object,void*) {
 const PoleDepthRepair157::Scope depthScope157(object);
 if(!Window())return consumerHook.thiscall<uint32_t>(object);
 // Read scopes finish before the original native consumer; its state changes
 // are preserved. TLS context is numeric and lasts only through this call.
 Actual155 observed{};const bool observedNow=ActualCommand155(uintptr_t(object),observed);
 Receipt receipt{};const bool matched=ConsumerReceipt(uintptr_t(object),receipt);
 const Receipt prior=current;current=matched?receipt:Receipt{};
 const Actual155 prior155=actual155;actual155=observedNow?observed:Actual155{};
 const auto result=consumerHook.thiscall<uint32_t>(object);actual155=prior155;current=prior;return result;
}
void Child(SafetyHookContext& regs)noexcept{
 ActualChild155(regs);
 if(!current.valid||!Window()||children>=512)return;const Preserve preserve;
 const traffic_signal::fresh_page::Operation44 pages;const immediate_region33::Scope regions;
 if(!CurrentOwner(current.hint))return;
 std::array<uint32_t,4> view{};uint32_t shaderGroup{};if(!Read(regs.eax,view.data(),sizeof(view))||!Read(uint64_t(regs.ecx)+8,&shaderGroup,4))return;
 ++children;Emit(193,current,uint32_t(regs.ecx),shaderGroup,view[3],current.command);Emit(196,current,view[0],view[1],view[2],view[3]);
 // Actual child-dispatch scope, after native command's scalar setters. This
 // observes CPU storage used by setters, not a GPU constant-readback claim.
 std::array<float,4> scalars{};if(Read(uint64_t(game)+0xc3f260,scalars.data(),16))ShadowTrace34::Emit({197,current.hint.map.frame,GetTickCount(),current.hint.owner.entity,int32_t(current.command),0,0,0,scalars[0],scalars[1],scalars[2],scalars[3]});
 // Actual impending native child dispatch, not GPU execution/depth proof.
}
void PrepareHooks()noexcept {
 if(!PoleCasterDiagnostic134::requested&&(!PoleDepthRepair157::requested||!PoleDepthRepair157::prepared))return;
 try{
  for(const auto range:{std::pair<uint32_t,size_t>{0x712e80,0x298},{0x4dae60,0xb0},{0x4dd0b0,0xbf},{0x1fd550,0xa8}}){if(!PoleCasterDiagnostic134::requested&&range.first!=0x4dd0b0)continue;const auto live=ReadMemory(game+range.first,range.second);Require(!std::memcmp(live.data(),original->bytes.data()+range.first,range.second),"pole_draw145_native_range");}
  if(PoleCasterDiagnostic134::requested){Prepare(objectHook,game+0x712e80,Object);Prepare(constructorHook,game+0x4dae60,Constructor);Prepare(childHook,game+0x1fd5e9,Child);}
  Prepare(consumerHook,game+0x4dd0b0,Consumer);
  const auto exact=[](const auto& hook,const auto& bytes){return hook.original_bytes().size()==bytes.size()&&!std::memcmp(hook.original_bytes().data(),bytes.data(),bytes.size());};
  if(PoleCasterDiagnostic134::requested){Require(exact(objectHook,std::array<uint8_t,6>{0x83,0xec,0x08,0x56,0x8b,0xf1}),"pole_draw145_object_stolen");
  Require(exact(constructorHook,std::array<uint8_t,5>{0x56,0x8b,0x74,0x24,0x08}),"pole_draw145_constructor_stolen");
  Require(exact(childHook,std::array<uint8_t,6>{0x50,0x8b,0x40,0x0c,0xff,0xd0}),"pole_draw145_child_stolen");}
  Require(exact(consumerHook,std::array<uint8_t,8>{0x51,0x56,0x8b,0xf1,0x0f,0xb7,0x46,0x58}),"pole_draw145_consumer_stolen");
  Event(1,"POLE_DRAW145_PREPARED","diagnostic_only=1 first10s=1 max128_commands=1 no_render_decisions_changed=1");
 }catch(...){objectHook={};constructorHook={};childHook={};consumerHook={};Event(1,"POLE_DRAW145_REFUSED","native_hook_contract_refused");}
}
}
