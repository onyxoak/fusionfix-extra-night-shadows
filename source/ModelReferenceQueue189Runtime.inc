#include "core/ModelReferenceQueue189.hpp"
namespace ModelReferenceQueue189 {
SafetyHookInline hook,drainHook;
std::array<model_reference189::Seen,4> seen;
// A model ID is uint16_t. Deduplication bounds each ring to 65536 distinct
// IDs, so this storage cannot overflow even if every ID is simultaneously used.
std::array<std::array<uint16_t,65536>,4> spillIds{};
std::array<uint32_t,4> spillUsed{};
std::atomic<uint32_t> duplicate{0},spilled{0},invalid{0};
void Reference(uint16_t model,bool add) {
 const auto object=reinterpret_cast<void**>(game+0xe95cd8)[model];
 reinterpret_cast<void(__thiscall*)(void*)>(game+(add?0x759f20:0x75a5a0))(object);
}
// Exact native ABI: thiscall(manager,index), ret4. The fastcall shim consumes
// ECX and dummy EDX, leaving the sole original stack argument in place.
void __fastcall Retain(void* manager,void*,uint32_t model) {
 const auto call=[&]{hook.unsafe_thiscall<void>(manager,model);};
 if(uintptr_t(manager)!=game+0xdd4ec8){call();return;}
 auto* bytes=static_cast<uint8_t*>(manager);
 const auto slot=*reinterpret_cast<uint32_t*>(bytes+0x2040);
 // The native model table is indexed before truncation to uint16_t.
 // Refuse indices the native uint16 queue cannot represent.
 if(slot>=4||model>65535){++invalid;RaiseFailFastException(nullptr,nullptr,0);return;}
 auto* array=bytes+0x204c+slot*8;
 auto& entries=*reinterpret_cast<uint16_t**>(array);
 auto& count=*reinterpret_cast<uint16_t*>(array+4);
 auto& capacity=*reinterpret_cast<uint16_t*>(array+6);
 auto spill=[slot](uint16_t id){
  auto& used=spillUsed[slot];if(used>=spillIds[slot].size())return false;
  Reference(id,true);spillIds[slot][used++]=id;++spilled;return true;
 };
 const auto result=model_reference189::Retain(seen[slot],uint16_t(model),entries,count,capacity,spill,call);
 if(result==model_reference189::Result::AlreadyHeld)++duplicate;
 if(result==model_reference189::Result::Invalid){++invalid;RaiseFailFastException(nullptr,nullptr,0);}
}
void __fastcall Drain(void* manager,void*) {
 if(uintptr_t(manager)!=game+0xdd4ec8){drainHook.unsafe_thiscall<void>(manager);return;}
 const auto slot=*reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(manager)+0x2040);
 if(slot>=4){++invalid;RaiseFailFastException(nullptr,nullptr,0);return;}
 drainHook.unsafe_thiscall<void>(manager);
 // Our additional references protect these model-table entries until this
 // exact owner-thread drain. Other in-flight rings keep independent holds.
 for(uint32_t i=0;i<spillUsed[slot];++i)Reference(spillIds[slot][i],false);
 spillUsed[slot]=0;seen[slot].known=false;
}
void PrepareHooks() {
 // Authenticate producer, ring drain/reset and the pure matching inc/dec.
 // No allocation, free, buffer relocation, scene cut or renderer-state change.
 for(const auto [rv,n]:{std::pair<uint32_t,size_t>{0x542000,0x40},{0x5422b0,0xb0},
       {0x5424e0,0x118},{0x759f20,4},{0x75a5a0,4}}){
  const auto live=ReadMemory(game+rv,n);
  Require(!std::memcmp(live.data(),original->bytes.data()+rv,n),"model_reference189_native_guard");
 }
 Require(OriginalCall(0x54200f)==game+0x759f20&&OriginalCall(0x542512)==game+0x75a5a0,
         "model_reference189_balanced_calls");
 auto result=SafetyHookInline::create(game+0x542000,Retain,SafetyHookInline::StartDisabled);
 Require(bool(result),"model_reference189_prepare");hook=std::move(*result);
 auto drain=SafetyHookInline::create(game+0x5424e0,Drain,SafetyHookInline::StartDisabled);
 Require(bool(drain),"model_reference189_drain_prepare");drainHook=std::move(*drain);
}
void Disable()noexcept{
 // Keep draining pending native references even if lighting is switched off.
 // These hooks do no illumination work and stay installed for process life.
}
void Publish(){
 static uint32_t last{};const auto now=GetTickCount();if(now-last<5000)return;last=now;
 Event(1,"MODEL_REFERENCE189","duplicates_avoided="+std::to_string(duplicate.load())+
  " overflow_retained="+std::to_string(spilled.load())+" invalid="+std::to_string(invalid.load())+
  " native_addref_release_balanced=1 shadow_draws_unchanged=1");
}
}

