// Audited producer code capabilities, independent of whole-file version/hash.
// Generated once from the preserved authenticated CE image at preferred base.
void VerifyLightingCapabilities(const std::vector<uint8_t>& raw) {
 const pe::Image canonical(raw,0x400000);
 struct Capability {bool needed;uint32_t rva,size;const char* digest;};
 const Capability capabilities[]{
  {Liberty20::dualRequested,0x63f5e0,0x93f,"36911d48f1e1e2054b123dd2be0202f162a8a72c0689aa6a601673a76a39f675"},
  {TailLight20::requested,0x642b10,0x92d,"fdbd193ca62e4e7ee245eed8f45fa644eef53eb3c3d39078f347403175128b84"},
  {Liberty20::dualRequested||TailLight20::requested,0x63de90,0x35b,"5778be20335885b7e02a1d56b97c1682e536bbf323867d05b4109d2519623241"},
  {ModelEffectObserver::requested,0x81db60,0x691,"60b5b57b35450ad5a30279cd7463e7b55988e0c1849bb557b6cb82681f928708"},
  {ModelEffectObserver::requested,0x81e200,0xbe5,"c6dbf6f4e910ada13d5e718bf37c8b2098f955c0c45ab916296ba0e855158610"},
  {WorldGeometry20::requested,0x978410,0xa4,"04bb0f178736c4e84360f56fc71d80544697bf1582d08e170c6ca70a24e03216"},
  {WorldGeometry20::requested,0x6e77f0,0x196,"26a9d0ce7b39dd3c53e058485f2b2471ab5cdc3773293b009edb684130bfbddb"},
  {Liberty20::dualRequested||TailLight20::requested||ModelLightShadow20::requested||CoronaLight20::requested,0x70ba00,0x22,"4bd0a0a1f6b5169de0e79638eb1f36c9ff871f412e887cbe9a9a692a8702b307"},
  {CoronaLight20::requested,0x81f020,0x143,"404319181f69eeb2d243cb9af2b44d44bc1846d17291be9e38c41dce0b7c1447"},
 };
 for(const auto& cap:capabilities)if(cap.needed){
  Require(uint64_t(cap.rva)+cap.size<=canonical.bytes.size(),"lighting_producer_capability_range");
  const std::vector<uint8_t> code(canonical.bytes.begin()+cap.rva,canonical.bytes.begin()+cap.rva+cap.size);
  Require(pe::Hash(code)==cap.digest,"lighting_producer_ABI_capability_changed");
 }
}
