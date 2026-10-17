// Test the receiver hull only AFTER the native room/object predicates pass.
// These three sites precede the first argument push; rejection therefore needs
// no stack edit, no invented return value and no partial virtual-render call.
namespace LateDrawCull228 {
std::array<SafetyHookMid,3> hooks;
bool prepared=false;
constexpr std::array<uint32_t,3> Sites{0x6e37ad,0x6e3808,0x6e384e};
constexpr std::array<uint8_t,3> Modes{0,2,1};
void Callback(SafetyHookContext& regs) noexcept {
 if(!ReceiverCull226::mode.load(std::memory_order_acquire)||!ReceiverCull226::binding.valid||!addonActive.load(std::memory_order_acquire))return;
 // Visit authenticates current native getters and complete model/physical bounds.
 // All other entities, changed data and unknown bounds retain their draw.
 if(ReceiverCull226::Visit(regs.esi,1))regs.eip=game+0x6e3864;
}
void Disable() noexcept {
 ReceiverCull226::mode.store(false,std::memory_order_release);
 for(auto& h:hooks)if(h)(void)h.disable();
 prepared=false;
}
void PrepareHooks() noexcept {
 if(!ReceiverCull226::prepared||!ReceiverCull226::experimental)return;
 try {
  constexpr std::array<uint8_t,16> tail{0x8b,0x06,0xff,0x75,0x10,0x8b,0xce,0xff,0x90,0x8c,0,0,0,0x8b,0x55,0x14};
  const auto drawTail=ReadMemory(game+0x6e3857,tail.size());
  Require(drawTail.size()==tail.size()&&!memcmp(drawTail.data(),tail.data(),tail.size()),"late228_native_virtual_draw_changed");
  // The exact native pre-push sequences are not version labels. A changed or
  // foreign-patched site refuses only this optional culling prototype.
  for(unsigned i=0;i<Sites.size();++i){
   const std::array<uint8_t,9> expected{0x6a,0xff,0x68,0xff,0,0,0,0x6a,Modes[i]};
   const auto live=ReadMemory(game+Sites[i],expected.size());
   Require(live.size()==expected.size()&&!memcmp(live.data(),expected.data(),expected.size()),"late228_native_pre_push_changed");
  }
  for(unsigned i=0;i<Sites.size();++i){
   Prepare(hooks[i],game+Sites[i],Callback);
   Require(hooks[i].original_bytes().size()==7,"late228_stolen_span_changed");
  }
  prepared=true;
  Event(1,"LATE_DRAW_CULL228_PREPARED","default_off=1 native_room_and_object_checks_first=1 before_argument_push=1 native_bounds229=1");
 }catch(const std::exception& e){Disable();ReceiverCull226::prepared=false;Event(1,"LATE_DRAW_CULL228_REFUSED",e.what());}
}
void Enable() noexcept {
 if(!prepared)return;
 try {for(auto& h:hooks)Require(bool(h.enable()),"late228_enable_failed");}
 catch(const std::exception& e){Disable();ReceiverCull226::prepared=false;Event(1,"LATE_DRAW_CULL228_REFUSED",e.what());}
}
}
