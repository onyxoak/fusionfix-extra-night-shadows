#pragma once
// Pure validation/planning only. This file never writes executable memory.
// Captured bytes are admissible only after a version adapter independently
// establishes both the post-initialization barrier and each hook's ownership.
#include <algorithm>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
namespace onyxoak {
struct PreparedHookRange {std::uintptr_t address{};std::size_t bytes{};};
inline bool DisjointPreparedHooks(std::span<const PreparedHookRange> ranges) noexcept {
 for(size_t i=0;i<ranges.size();++i) {
  const auto a=ranges[i];if(!a.address || a.bytes<5 || a.address>UINTPTR_MAX-a.bytes)return false;
  for(size_t j=0;j<i;++j) {
   const auto b=ranges[j];
   if(a.address<b.address+b.bytes && b.address<a.address+a.bytes)return false;
  }
 }
 return true;
}
enum class PatchKind { HeadlightCall, Admission, Lookup, RenderScope, CasterBranch, ContactShadow, FrameCounter };
struct OwnedPatch {
 PatchKind kind{};
 uint32_t rva{};
 std::vector<uint8_t> expectedInstalled;
 std::vector<uint8_t> originalRelocated;
 bool ownershipVerified{};
};
struct GuardImage {
 std::vector<uint8_t> bytes;
 std::string error;
 explicit operator bool() const noexcept{return error.empty()&&!bytes.empty();}
};
inline GuardImage NormalizeForOriginalGuards(std::span<const uint8_t> live,
 std::span<const OwnedPatch> patches,bool knownBinary,bool unpausedPostInitBoundary) {
 GuardImage out;
 if(!knownBinary){out.error="unsupported FusionFix identity";return out;}
 if(!unpausedPostInitBoundary){out.error="FusionFix readiness unproven";return out;}
 for(size_t i=0;i<patches.size();++i){const auto& p=patches[i];
  if(!p.ownershipVerified){out.error="foreign or unverified detour";return out;}
  if(p.expectedInstalled.empty()||p.expectedInstalled.size()!=p.originalRelocated.size()||
     p.rva>live.size()||p.expectedInstalled.size()>live.size()-p.rva){out.error="invalid patch range";return out;}
  if(!std::equal(p.expectedInstalled.begin(),p.expectedInstalled.end(),live.begin()+p.rva)){out.error="hook changed since ownership audit";return out;}
  for(size_t j=0;j<i;++j){const auto& q=patches[j];
   if(uint64_t(p.rva)<uint64_t(q.rva)+q.expectedInstalled.size()&&uint64_t(q.rva)<uint64_t(p.rva)+p.expectedInstalled.size()){
    out.error="overlapping ownership ranges";return out;
   }
  }
 }
 out.bytes.assign(live.begin(),live.end());
 for(const auto& p:patches)std::copy(p.originalRelocated.begin(),p.originalRelocated.end(),out.bytes.begin()+p.rva);
 return out;
}
// No activation until every prepared hook has enabled. Failed hooks leave
// the feature gate closed; every enabled hook must then be disabled by the
// runtime adapter while retaining its trampoline ownership until shutdown.
class ActivationGate {
 bool active_{};
 bool failed_{};
 size_t required_{},enabled_{};
public:
 explicit ActivationGate(size_t required):required_(required){}
 bool HookEnabled(bool success) noexcept {
  if(failed_||!success||enabled_>=required_){active_=false;failed_=true;return false;}
  ++enabled_;return true;
 }
 bool Commit() noexcept {return active_=!failed_&&required_!=0&&enabled_==required_;}
 void Abort() noexcept {active_=false;enabled_=0;failed_=false;}
 bool Active() const noexcept {return active_;}
};
}
