// Called only when a cached known layout does not identify this actual module.
// Discovery uses code/argument/data relationships, not its release label/hash.
void DiscoverFusion218(const std::vector<uint8_t>& raw,const std::string& hash,uint32_t process){
 detectedImage218=std::make_unique<pe::Image>(raw,ff);
 auto capabilities=fusion_discovery218::Discover(*detectedImage218);
 const uint32_t processRva=uint32_t(process-ff);
 Require(process>=ff&&fusion_discovery218::Section(*detectedImage218,processRva,16,0x20000000u),"fusionfix_integration_game_loop_owner_mismatch");
 capabilities.process=processRva;capabilities.code.push_back({processRva,16});
 fusion_discovery218::BindFrame(*detectedImage218,capabilities,uint32_t(game),fusionfix::shadows::ce::FrameCounterRva,
  std::span<const uint8_t>(original->bytes.data()+0x4d0f82,6),MidStubTemplate,[](uint32_t at,size_t n){return ReadMemory(at,n);});

 fusion_discovery218::ValidateLive(*detectedImage218,capabilities,[](uint32_t at,size_t n){return ReadMemory(at,n);});
 const auto& c=capabilities;
 detectedHash218=hash;
 detectedLayout218={"code-detected",detectedHash218.c_str(),c.process,c.player,c.npc,c.admission,c.lookup,c.render,c.caster,c.contact,c.frame,c.frameData,
  c.width,c.height,c.extra,c.head,c.vehicle,c.warning,c.warningKey,c.textLookup};
 detectedBackend218={"code-detected",c.extraCallback,c.admission,c.extra,c.head,c.vehicle,c.listCallback,c.vehicleCallback,c.flagsCallback,c.listIndex,c.listFunction};
 detectedCopy218={"code-detected",c.copyCallback,c.allocation,c.copySize,c.copySlot,c.copyInvoke};
 // These bytes are already relocated to the current module base. The generic
 // callback validator therefore needs no additional relocation entries.
 detectedHooks218={"code-detected",c.poolCallback,c.poolBody,c.flagsCallback,
  {c.poolBody,detectedImage218->bytes.data()+c.poolBody,c.poolSize,nullptr,0},
  {c.flagsCallback,detectedImage218->bytes.data()+c.flagsCallback,c.flagsSize,nullptr,0}};
 Require(fusion_hooks213::Validate(detectedHooks218,uint32_t(ff),[](uint32_t at,size_t n){return ReadMemory(at,n);}),"fusionfix_integration_pool_or_flags_callback_changed");
 Require(backend_versions213::CallbackBodies(detectedBackend218,uint32_t(ff),[](uintptr_t at,void* out,size_t n){try{const auto bytes=ReadMemory(at,n);std::memcpy(out,bytes.data(),n);return true;}catch(...){return false;}}),"fusionfix_integration_setting_callback_shape_changed");
 layout=&detectedLayout218;fusionHooks213=&detectedHooks218;
 fusionCallbacks213Authenticated=true;fusionModule213Authenticated=true;structuralFusion218=true;
 // Exact-511 optional profiling/copy helpers retain their own narrower ABI;
 // no release label is forged to enable them on a structurally discovered DLL.
 shortlist_helper48::runtime::binding={game,ff,original->bytes.size(),detectedImage218->bytes.size(),true};
 RecordStartupExtra213("fusionfix_detection=required_code_and_bindings hash_allowlist=0 fusionfix_sha256="+hash);
 Event(1,"FUSIONFIX_CAPABILITIES218","detection=code_and_bindings version_allowlist=0 process=found lighting=found settings=found pool=found copier=found frame_overlay="+std::to_string(c.frame!=0)+" warning_hook="+std::to_string(c.warning!=0)+" startup_only=1");
}
