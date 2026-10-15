// Included after VerifyOfficial. Initialization only; neither callback is replaced.
namespace VolumetricCompat199 {
bool authenticated=false;
volumetric199::Chain copyChain;
void AuthenticateAndNormalize(std::vector<uint8_t>& image){
 const auto module=GetModuleHandleW(L"VolumetricLights.asi");if(!module)return;
 RecordVolumetric210(startup_guidance210::Volume::Checking);
 const auto base=uint32_t(reinterpret_cast<uintptr_t>(module));
 const auto raw=pe::Read(pe::ModulePath(module));
 const auto hash210=pe::Hash(raw);RecordVolumetric210(startup_guidance210::Volume::Checking,hash210);
 Require(hash210==volumetric199::Hash,"volumetric199_unsupported_binary");
 const pe::Image expected(raw,base);
 for(const auto& section:expected.sections)if(section.Characteristics&IMAGE_SCN_MEM_EXECUTE){
  const auto live=ReadMemory(base+section.VirtualAddress,section.Misc.VirtualSize);
  Require(!memcmp(live.data(),expected.bytes.data()+section.VirtualAddress,live.size()),"volumetric199_code_modified");
 }
 const auto* callback=expected.bytes.data()+volumetric199::Callback;
 Require(callback[0x6f]==0xa1&&U32(callback+0x70)==base+volumetric199::Allocation,"volumetric199_copy_slot");
 constexpr uint8_t invoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf0};
 Require(!memcmp(callback+0x81,invoke,sizeof(invoke))&&callback[0xb7]==0xc2&&callback[0xb8]==4&&callback[0xb9]==0,"volumetric199_copy_abi");
 volumetric199::Node volume{base+volumetric199::Callback,base+volumetric199::Allocation,{callback,volumetric199::CallbackSize}};
 std::vector<uint8_t> fusionBytes;volumetric199::Node fusion;
 if(!nativeBackend){
  Require(layout==&Layout511&&shortlist_helper48::runtime::binding.exact511Authenticated,"volumetric199_unsupported_fusionfix");
  const auto ffRaw=pe::Read(pe::ModulePath(reinterpret_cast<HMODULE>(ff)));
  Require(pe::Hash(ffRaw)==Layout511.hash,"volumetric199_fusionfix_identity_changed");
  const pe::Image ffExpected(ffRaw,ff);
  fusionBytes.assign(ffExpected.bytes.begin()+native_copy70::CallbackRva,ffExpected.bytes.begin()+native_copy70::CallbackRva+native_copy70::CallbackSize);
  Require(fusionBytes[0x57]==0xa1&&U32(fusionBytes.data()+0x58)==ff+native_copy70::AllocationRva,"volumetric199_fusionfix_copy_slot");
  constexpr uint8_t ffInvoke[]{0xff,0x75,0x08,0x8b,0xd6,0x8b,0xcf,0xff,0xd0,0x8b,0xf8};
  Require(!memcmp(fusionBytes.data()+0x70,ffInvoke,sizeof(ffInvoke)),"volumetric199_fusionfix_copy_abi");
  fusion={uint32_t(ff)+native_copy70::CallbackRva,uint32_t(ff)+native_copy70::AllocationRva,fusionBytes};
 }
 copyChain=volumetric199::VerifyChain(uint32_t(game),
  {original->bytes.data()+native_copy70::CopyRva,native_copy70::CopySize},volume,nativeBackend?nullptr:&fusion,
  [](uint32_t address,size_t size){return ReadMemory(address,size);});
 volumetric199::NormalizeOperands(image,original->bytes,base,[](uint32_t address,size_t size){return ReadMemory(address,size);});
 HMODULE pinned{};
 Require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(module),&pinned)&&pinned==module,"volumetric199_pin_failed");
 if(!nativeBackend)Require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(ff),&pinned)&&reinterpret_cast<uintptr_t>(pinned)==ff,"volumetric199_fusionfix_pin_failed");
 authenticated=true;RecordVolumetric210(startup_guidance210::Volume::Accepted);
 Event(1,"VOLUMETRIC199_AUTHENTICATED","compatible=1 release=2.00 producer_operands=7 copier_layers="+std::to_string(nativeBackend?1:2)+
  " outer="+std::string(copyChain.outer==volume.callback?"VolumetricLights":"FusionFix")+" callbacks_preserved_once=1 current_packet_witness_exact=1");
}
}
