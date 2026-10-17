// Included inside ModelEffectObserver. This callback runs synchronously before
// the native point/spot store, including its sphere/plane visibility tests.
namespace FixtureLight244 {
bool requested=false;
std::atomic<uint32_t> corrected{0},refused{0};
bool StoreRadius(uintptr_t address,uint32_t value) noexcept {
 __try { *reinterpret_cast<uint32_t*>(address)=value;return true; }
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void Correct(SafetyHookContext& regs,uint32_t site,std::array<uint32_t,18>& arguments,
 const Invocation& invocation,Sample& sample) noexcept {
 if(!requested||!addonActive.load(std::memory_order_acquire)||!ModelLightShadow20::requested||
    !ModelLightShadow20::active.load(std::memory_order_acquire))return;
 const auto abi=model_light_submission20::Layout(site);
 const auto radius=std::bit_cast<float>(arguments[10]);
 if(!abi||arguments[1]!=0||(arguments[2]&6u)||arguments[abi.words-1]||!(radius>0.f&&radius<fixture_light244::IndustrialRadius)||
    invocation.modelInfo>UINT32_MAX-0x40||invocation.invalid||invocation.frame!=Frame())return;
 uint32_t hash{};
 if(!Copy(invocation.modelInfo+0x3c,&hash,4)||hash!=fixture_light244::IndustrialFourHead)return;
 std::array<float,3> direction{};float range{};
 if(!Copy(arguments[3],direction.data(),sizeof(direction))||
    !fixture_light244::Range(hash,arguments[1],radius,direction,range))return;
 ValidateSource46(invocation,sample);
 if(invocation.invalid||!sample.ownerMatched||!sample.modelInfoMatched||!sample.descriptorCopied){++refused;return;}
 TrafficSignal::Match fresh{};std::array<uint8_t,0x6c> descriptor{};bool copied=false;
 std::array<uint32_t,18> current{};uint32_t currentHash{};
 if(!TrafficSignal::MatchOwnerWithDescriptor48(invocation.owner,fresh,invocation.descriptor,
     descriptor.data(),descriptor.size(),copied)||!copied||descriptor!=sample.descriptor||
    fresh.identity!=sample.identity||fresh.modelInfo!=invocation.modelInfo||
    ((sample.ownerKind==2||sample.ownerKind==3)&&(fresh.kind!=sample.ownerKind||fresh.ownerPose109!=sample.ownerPose109))||
    !Copy(invocation.modelInfo+0x3c,&currentHash,4)||currentHash!=hash||
    !Copy(regs.esp,current.data(),abi.words*4)||current!=arguments||invocation.frame!=Frame()){
  ++refused;return;
 }
 // The authenticated model store owns this live outgoing stack. No owner,
 // descriptor, world coordinate, cached bank packet or light key is changed.
 if(!StoreRadius(uintptr_t(regs.esp)+10*4,std::bit_cast<uint32_t>(range))){++refused;return;}
 arguments[10]=std::bit_cast<uint32_t>(range);++corrected;
}
}
