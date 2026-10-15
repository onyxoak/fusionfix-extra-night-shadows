// Initialization/unpaused Install failure only. No hot callback publication.
startup_conflict129::Mismatch startupMismatch129;
std::mutex startupMismatchMutex129;
std::atomic_flag startupFailureAttempt129=ATOMIC_FLAG_INIT;
void RequireNativeRange129(const std::vector<std::uint8_t>& image,std::uint32_t rva,size_t size,const char* code) {
 const bool bounded=original&&std::uint64_t(rva)+size<=original->bytes.size()&&std::uint64_t(rva)+size<=image.size();
 const bool same=bounded&&!std::memcmp(image.data()+rva,original->bytes.data()+rva,size);
 if(!same&&bounded)try {
  std::lock_guard lock(startupMismatchMutex129);
  if(!startupMismatch129.valid)startupMismatch129=startup_conflict129::FirstMismatch(
    std::span<const std::uint8_t>(original->bytes.data()+rva,size),
    std::span<const std::uint8_t>(image.data()+rva,size),rva);
 }catch(...){/* Evidence failure cannot replace the original guard refusal. */}
 Require(same,code); // Exact original error and refusal retained.
}
void StartupFailure129(const char* error) noexcept {
 if(startupFailureAttempt129.test_and_set(std::memory_order_relaxed))return;
 const auto savedError129=GetLastError();
 struct RestoreError129 {DWORD value;~RestoreError129(){SetLastError(value);}} restoreError129{savedError129};
 const fusionfix::shadows::FloatingPointState fp;
 try {
  startup_conflict129::Mismatch mismatch;
  {std::lock_guard lock(startupMismatchMutex129);mismatch=startupMismatch129;}
  const auto fields=startup_conflict129::Fields(CandidateBuild32,error?error:"startup_exception",mismatch);
  if(log)log->StartupFailure(fields);
  else {
   // Earliest Start failures can precede normal log construction. This is
   // still one failure-only bounded report, never an Off startup service.
   SupportLog failureLog129(pe::ModulePath(self).replace_extension(L".log"),0);
   failureLog129.StartupFailure(fields);
  }
 }catch(...){/* Failure reporting must not escape or change the refusal. */}
}
