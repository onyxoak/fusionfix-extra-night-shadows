bool MatchOwner204(uintptr_t owner,Match& value) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::OwnerMatch,128);
 const immediate_region33::Scope operation;
 return MatchOwnerContained32(owner,value);
}
bool MatchOwnerWithDescriptor48204(uintptr_t owner,Match& value,
 uintptr_t descriptor,void* output,size_t size,bool& copied) noexcept {
 const shadow_cpu29::Scope cpu(shadow_cpu29::Phase::OwnerMatch,128);
 const immediate_region33::Scope operation;
 copied=false;
 const traffic_signal::fresh_batch30::Span extra{descriptor,output,size};
 bool descriptorFault49=false;
 const bool matched=MatchOwnerWithDescriptorContained48(owner,value,extra,copied,descriptorFault49);
 // Preserve the old independent descriptor read even when owner validation
 // faults before reaching the initial batch (unsupported diagnostics/coronas).
 // A Stage fault may consume PAGE_GUARD; never convert it into approval by
 // retrying that descriptor. Ordinary permission/early-owner faults retain fallback.
 if(!copied&&!descriptorFault49)copied=OwnerCopy35(&extra,1);
 return matched;
}
