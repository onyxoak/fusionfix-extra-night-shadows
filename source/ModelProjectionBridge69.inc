// Synchronous bridge: exact66 constructor context to68 current invocation.
// No casts between callback ABIs, borrowed lifetime leases or cached permissions.
static ModelEffectObserver::ModelValue68::Binding ModelBinding69(
 const native_projection66::ConstructorContext& context) noexcept {
 return {context.caller,context.returnSlot,context.destination,context.position,
  context.color,context.type,context.flags,context.key};
}
#include "ArcadePlacement197Runtime.inc"
static void WireModelProjection69() noexcept {
 if(ArcadePlacement197::requested&&ArcadePlacement197::assetsAuthenticated){
  NativeProjection66::privateProjections197.Init();
  NativeProjection66::placeModel197=ArcadePlacement197::Apply;
  NativeProjection66::finishPlacement197=ArcadePlacement197::Finish;
  NativeProjection66::placementBank197=ArcadePlacement197::Bank;
 }
 ModelEffectObserver::ModelValue68::canReplace=NativeProjection66::CanReplaceModelSubmission;
 NativeProjection66::matchModelConstructor66=[](const native_projection66::ConstructorContext& context) noexcept {
  return ModelEffectObserver::ModelValue68::Matches(ModelBinding69(context));
 };
 NativeProjection66::legacyFallback66=[](native_projection66::Packet& packet,uint32_t frame,
  const native_projection66::ConstructorContext& context) noexcept {
  ++NativeProjection66::modelFallback;
  return ModelEffectObserver::ModelValue68::Fallback(packet,frame,ModelBinding69(context));
 };
 NativeProjection66::constructorCompleted66=[](bool success,
  const native_projection66::ConstructorContext& context) noexcept {
  if(success)++NativeProjection66::modelDeferred;
  ModelEffectObserver::ModelValue68::Completed(ModelBinding69(context));
 };
}
