// Included in ModelEffectObserver after the production legacy finish helper.
// This owns only one synchronous native call's copied numeric context. It does
// not authorize owner lifetime, borrow permission snapshots, or publish keys.
#include "ModelNativeAssociation77.hpp"
namespace ModelValue68 {
struct Binding {
 uintptr_t caller{},returnSlot{},destination{},position{},color{};
 uint32_t type{},flags{},key{};
};
struct Pending {
 SafetyHookContext regs{};Invocation invocation{};
 std::array<uint32_t,18> arguments{};
 uint32_t site{};size_t depth{};bool present{},consumed{};
};
thread_local std::array<Pending,MaxDepth> pending;
// Integration wires this only to an authenticated, active constructor adapter
// with its requested native model category and allocator/night gates enabled.
bool (*canReplace)(uint32_t,uint32_t,uint32_t) noexcept=nullptr;
std::atomic<uint32_t> deferred{0},legacyFallback{0},associationRejected{0};
std::atomic<uint32_t> matched77{0},matchRejected77{0};
std::array<std::atomic<uint32_t>,9> mismatch77{};
void ClearDepth(size_t depth) noexcept {if(depth&&depth<=pending.size())pending[depth-1]={};}
uintptr_t ReturnSlot(uintptr_t argumentBase,uint32_t site) noexcept {
 const auto abi=model_light_submission20::Layout(site);
 return abi?model_native_association77::ReturnSlot(argumentBase,abi.words==16):0;
}
bool SameInvocation(const Invocation& a,const Invocation& b) noexcept {
 return a.frame==b.frame&&a.nonce==b.nonce&&a.preStack==b.preStack&&
  a.owner==b.owner&&a.effect==b.effect&&a.descriptor==b.descriptor&&
  a.matrix==b.matrix&&a.ordinal==b.ordinal&&a.modelInfo==b.modelInfo&&
  a.proposedKey==b.proposedKey&&a.sampled&&b.sampled&&a.resolved&&b.resolved&&
  a.submitted&&b.submitted&&!a.invalid&&!b.invalid;
}
bool TryDefer(const SafetyHookContext& regs,uint32_t site,
 const std::array<uint32_t,18>& args,const Invocation& current,const Sample& sample) noexcept {
 const auto abi=model_light_submission20::Layout(site);const auto depth=local.state.Depth();
 if(!abi||!depth||depth>pending.size()||sample.diagnostic||!ModelLightShadow20::requested||
    !ModelLightShadow20::active.load(std::memory_order_acquire)||!current.nonce||
    !SameInvocation(current,current)||!ReturnSlot(regs.esp,site)||!canReplace||
    !canReplace(args[1],args[2],args[abi.keyOffset/4]))return false;
 pending[depth-1]={regs,current,args,site,depth,true,false};++deferred;return true;
}
bool Matches(const Binding& binding) noexcept {
 const auto depth=local.state.Depth();const auto* current=local.state.Current();
 if(!depth||depth>pending.size()||!current)return false;
 const auto& p=pending[depth-1];const auto abi=model_light_submission20::Layout(p.site);
 if(!p.present||p.consumed||p.depth!=depth||!abi)return false;
 const auto slot=ReturnSlot(p.regs.esp,p.site);
 const std::array<bool,9> checks{current->frame==Frame()&&SameInvocation(p.invocation,*current),
  binding.caller==game+0x6bd194,binding.returnSlot==slot,binding.destination==slot+0xa8,
  binding.position==p.arguments[5],binding.color==p.arguments[6],binding.type==p.arguments[1],
  model_native_association77::SameFlags(p.arguments[2],binding.flags,abi.words==16),
  binding.key==p.arguments[abi.keyOffset/4]};
 bool valid=true;for(unsigned i=0;i<checks.size();++i)if(!checks[i]){++mismatch77[i];valid=false;}
 if(valid)++matched77;else ++matchRejected77;return valid;
}
uint32_t Fallback(std::array<uint32_t,32>& owned,uint32_t frame,const Binding& binding) noexcept {
 const LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 if(frame!=Frame()||!Matches(binding)){++associationRejected;return 0;}
 const auto depth=local.state.Depth();auto& p=pending[depth-1];
 p.consumed=true;++legacyFallback;
 const traffic_signal::fresh_page::Operation44 fallbackReads68;
 // No second Submission(): the pre-native callback already authenticated and
 // marked this exact source. Repeating it would poison a valid invocation.
 const auto abi=model_light_submission20::Layout(p.site);
 const PointEvidence evidence{frame,p.regs.ebp+8,DispatchReturn,p.arguments[0],
  uint32_t(p.regs.edi),p.site,p.arguments[1],p.arguments[2],p.arguments[abi.keyOffset/4]};
 return FinishPoint68(p.regs,p.site,p.arguments,evidence,local.state.Current(),local.samples[depth-1],&owned);
}
void Completed(const Binding& binding) noexcept {
 const auto depth=local.state.Depth();if(!depth||depth>pending.size())return;
 const auto& p=pending[depth-1];
 if(p.present&&binding.returnSlot==ReturnSlot(p.regs.esp,p.site)&&
    binding.destination==binding.returnSlot+0xa8)ClearDepth(depth);
}
}
