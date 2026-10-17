// Startup-only discovery from the native copier's actual installed hook chain.
// No module filename, release version, hash allowlist or whole-module code gate.
namespace VolumetricCompat199 {
bool authenticated=false;
volumetric199::Chain copyChain;
void AuthenticateAndNormalize(std::vector<uint8_t>& image){
 auto branch=[&](uint32_t at){const auto bytes=ReadMemory(at,5);Require(bytes.size()==5&&bytes[0]==0xe9,"shadow_copy233_hook_branch_missing");return uint32_t(at+5+U32(bytes.data()+1));};
 const auto live=ReadMemory(game+native_copy70::CopyRva,9);
 if(live[0]!=0xe9)return;
 uint32_t callback=branch(branch(uint32_t(game)+native_copy70::CopyRva));
 const auto* profile=!nativeBackend?CopyProfile218():nullptr;
 if(profile&&callback==ff+profile->callback){
  const auto slot=ReadMemory(ff+profile->allocation,8);const auto trampoline=U32(slot.data()),size=U32(slot.data()+4);
  if(size==19)return;
  Require(size==15,"shadow_copy233_previous_hook_layout_changed");callback=branch(branch(trampoline));
 }
 const auto module=pe::ModuleAt(callback);Require(module,"shadow_copy233_callback_owner_missing");
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(module));const pe::Image expected(pe::Read(pe::ModulePath(module)),base);
 Require(callback>=base&&uint64_t(callback-base)+volumetric199::CallbackSize<=expected.bytes.size(),"shadow_copy233_callback_out_of_range");
 const auto rva=callback-base;
 Require(fusion_discovery218::Section(expected,rva,volumetric199::CallbackSize,IMAGE_SCN_MEM_EXECUTE),"shadow_copy233_callback_not_executable");
 const auto* code=expected.bytes.data()+rva;
 Require(code[0x6f]==0xa1,"shadow_copy233_delegate_slot_instruction_changed");
 const auto allocation=U32(code+0x70);
 Require(allocation>=base&&fusion_discovery218::Section(expected,allocation-base,8,IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE),"shadow_copy233_delegate_binding_out_of_range");
 constexpr uint8_t invoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf0};
 Require(!memcmp(code+0x81,invoke,sizeof(invoke))&&code[0xb7]==0xc2&&code[0xb8]==4&&code[0xb9]==0,"shadow_copy233_callback_argument_contract_changed");
 volumetric199::Node volume{callback,allocation,{code,volumetric199::CallbackSize}};
 std::vector<uint8_t> fusionBytes;volumetric199::Node fusion;
 if(!nativeBackend){
  Require(profile&&fusionModule213Authenticated,"shadow_copy233_primary_copier_hook_missing");
  const pe::Image ffExpected(pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff))),ff);
  Require(uint64_t(profile->callback)+profile->size<=ffExpected.bytes.size(),"shadow_copy233_primary_callback_out_of_range");
  fusionBytes.assign(ffExpected.bytes.begin()+profile->callback,ffExpected.bytes.begin()+profile->callback+profile->size);
  Require(fusionBytes[profile->slot]==0xa1&&U32(fusionBytes.data()+profile->slot+1)==ff+profile->allocation,"shadow_copy233_primary_delegate_changed");
  constexpr uint8_t ffInvoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};
  Require(!memcmp(fusionBytes.data()+profile->invoke,ffInvoke,sizeof(ffInvoke)),"shadow_copy233_primary_argument_contract_changed");
  fusion={uint32_t(ff)+profile->callback,uint32_t(ff)+profile->allocation,fusionBytes};
 }
 copyChain=volumetric199::VerifyChain(uint32_t(game),{original->bytes.data()+native_copy70::CopyRva,native_copy70::CopySize},volume,nativeBackend?nullptr:&fusion,
  [](uint32_t at,size_t n){return ReadMemory(at,n);});
 volumetric199::NormalizeCurrentOperands233(image,original->bytes,base,expected.bytes.size(),[](uint32_t at,size_t n){return ReadMemory(at,n);});
 HMODULE pinned{};Require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(uintptr_t(callback)),&pinned)&&pinned==module,"shadow_copy233_callback_owner_lifetime_failed");
 authenticated=true;RecordVolumetric210(startup_guidance210::Volume::Accepted);
 Event(1,"SHADOW_COPY_CHAIN233_CONNECTED","live_hooks=1 module_name_allowlist=0 version_allowlist=0 file_hash_allowlist=0 callbacks_preserved_once=1 layers="+std::to_string(nativeBackend?1:2));
}
}
