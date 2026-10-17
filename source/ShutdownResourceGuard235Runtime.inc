#include "ShutdownResourceGuard235.hpp"
namespace ShutdownResource235 {
SafetyHookMid hook;
uintptr_t resume{};
bool prepared{};
std::atomic<uint32_t> rejected{0};
void Guard(SafetyHookContext& regs) noexcept {
 if(shutdown_resource235::Apply(regs,resume)){
  if(rejected.fetch_add(1,std::memory_order_relaxed)==0)
   Event(1,"SHUTDOWN_RESOURCE235_INVALID_INDEX","signed_invalid_slot=1 release_calls_skipped=2 fatal_handler_unchanged=1 lighting_unchanged=1");
 }
}
void Disable() noexcept {if(hook)(void)hook.disable();prepared=false;}
void PrepareHooks() noexcept {
 try {
  if(!original)throw std::runtime_error("native_image_unavailable");
  const auto rva=original->Find(shutdown_resource235::Pattern);
  const auto live=ReadMemory(game+rva,shutdown_resource235::BodyBytes);
  // Pool-limit mods can relocate the loop-end/data operands. Authenticate the
  // current instructions and release targets, not those mutable addresses.
  Require(shutdown_resource235::Shape(live.data(),live.size()),"shutdown235_live_call_sequence_changed");
  const auto first=Branch(game+rva+11,0xe8),second=Branch(game+rva+21,0xe8);
  Require(pe::ModuleAt(first)==GetModuleHandleW(nullptr)&&pe::ModuleAt(second)==GetModuleHandleW(nullptr),"shutdown235_release_owner_changed");
  // Authenticate the actual native invalid-index write path, independent of
  // FusionFix identity or addresses. Do not attach to an unknown ABI/handler.
  const auto write=ReadMemory(second,39);
  constexpr uint8_t prefix[]={0x56,0x8b,0x41,0x04,0x8b,0x74,0x24,0x08,
   0xf6,0x04,0x06,0x80,0x75,0x2f,0x8b,0x41,0x0c,0x0f,0xaf,0xc6,
   0x03,0x01,0x74,0x25,0xff,0x48,0x04,0x83,0x78,0x04,0x00,0x7f,0x1c};
  Require(write[0]==0x8b&&write[1]==0x0d&&
   !memcmp(write.data()+6,prefix,sizeof(prefix)),"shutdown235_release_code_changed");
  resume=game+rva+shutdown_resource235::ResumeOffset;
  Prepare(hook,game+rva+shutdown_resource235::HookOffset,Guard);prepared=true;
  Event(1,"SHUTDOWN_RESOURCE235_PREPARED","live_native_call_sequence=1 version_allowlist=0 mod_allowlist=0 negative_index_only=1 valid_releases_unchanged=1");
 }catch(const std::exception& e){Disable();Event(1,"SHUTDOWN_RESOURCE235_UNAVAILABLE",e.what());}
}
void Enable() noexcept {
 if(prepared&&hook){
  if(hook.enable())Event(1,"SHUTDOWN_RESOURCE235_ACTIVE","invalid_pool_release_guard=1 fatal_handler_unchanged=1");
  else {Disable();Event(1,"SHUTDOWN_RESOURCE235_UNAVAILABLE","hook_enable_failed");}
 }
}
}
