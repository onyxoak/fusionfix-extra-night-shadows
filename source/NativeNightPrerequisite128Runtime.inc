namespace NativeNightPrerequisite128 {
native_night128::Route route;
void Prepare() noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 native_night128::currentState=native_night128::State::Unknown;route={};
 if(nativeBackend||!ff||!original)return;
 if(uint64_t(native_night128::NativeGetterRva)+native_night128::NativeGetterBytes>original->bytes.size())return;
 route=native_night128::Authenticate(shortlist_helper48::runtime::binding,
  std::span<const uint8_t>(original->bytes.data()+native_night128::NativeGetterRva,native_night128::NativeGetterBytes),Liberty20::Copy);
}
void Refresh() noexcept {
 const ModelEffectObserver::LastErrorScope error;
 const fusionfix::shadows::FloatingPointState fp;
 if(!route.authenticated||reinterpret_cast<uintptr_t>(pe::ModuleAt(ff))!=route.binding.fusion){
  native_night128::currentState=native_night128::State::Unknown;return;
 }
 const auto result=native_night128::Observe(route,Liberty20::Copy,uintptr_t(__readfsdword(0x2c)));
 native_night128::currentState.store(result.state,std::memory_order_release);
}
}
