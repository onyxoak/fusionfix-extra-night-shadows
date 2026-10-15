# Production fixture is regenerated from the actual current caller; baseline
# queued implementation is byte-identical frozen111, with current native matcher.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/tests/queued-scratch59-tests.cpp" NATIVE115_TEST)
string(REPLACE "#include <psapi.h>" [=[#include <psapi.h>
const auto RealVirtualQuery115=VirtualQuery;unsigned virtualQueries115{};
SIZE_T WINAPI CountVirtualQuery115(LPCVOID address,PMEMORY_BASIC_INFORMATION info,SIZE_T size){++virtualQueries115;return RealVirtualQuery115(address,info,size);}
#define VirtualQuery CountVirtualQuery115]=] NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "std::vector<std::vector<uintptr_t>> queryOrder59;" "bool traceQueries115=true,failWs115=false; std::vector<std::vector<uintptr_t>> queryOrder59;" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "auto* q=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b);std::vector<uintptr_t> order;" "if(traceQueries115){auto* q=static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b);std::vector<uintptr_t> order;" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "queryOrder59.push_back(order);const auto ok=" "queryOrder59.push_back(order);}const auto ok=" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "const auto ok=RealQuery49(p,b,n);" "const auto ok=failWs115?FALSE:RealQuery49(p,b,n);" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "bool traceQueries115=true,failWs115=false;" "void (*nativeQueryHook115)(PSAPI_WORKING_SET_EX_INFORMATION*,DWORD)=nullptr; bool traceQueries115=true,failWs115=false;" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "return ok;}" "if(nativeQueryHook115)nativeQueryHook115(static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b),n/sizeof(PSAPI_WORKING_SET_EX_INFORMATION));return ok;}" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "bool traceQueries115=true,failWs115=false;" "void (*nativeBeforeQueryHook115)(PSAPI_WORKING_SET_EX_INFORMATION*,DWORD)=nullptr; bool traceQueries115=true,failWs115=false;" NATIVE115_TEST "${NATIVE115_TEST}")
string(REPLACE "const auto ok=failWs115?" "if(nativeBeforeQueryHook115)nativeBeforeQueryHook115(static_cast<PSAPI_WORKING_SET_EX_INFORMATION*>(b),n/sizeof(PSAPI_WORKING_SET_EX_INFORMATION));const auto ok=failWs115?" NATIVE115_TEST "${NATIVE115_TEST}")
string(FIND "${NATIVE115_TEST}" "namespace TrafficSignal {" NATIVE115_NAMESPACE)
string(SUBSTRING "${NATIVE115_TEST}" 0 ${NATIVE115_NAMESPACE} NATIVE115_PREFIX)
string(SUBSTRING "${NATIVE115_TEST}" ${NATIVE115_NAMESPACE} -1 NATIVE115_SUFFIX)
set(NATIVE115_TEST "${NATIVE115_PREFIX}")
string(APPEND NATIVE115_TEST [=[void* NativeAllocate115(size_t) noexcept;bool NativeRelease115(void*) noexcept;
void (*nativeStageHook115)(unsigned)=nullptr;
#define CE_ALLOCATE_NATIVE_SCRATCH115(size) NativeAllocate115(size)
#define CE_RELEASE_NATIVE_SCRATCH115(pointer) NativeRelease115(pointer)
]=] "${NATIVE115_SUFFIX}")
string(REPLACE "void CertificateStage54(unsigned group) noexcept {" "void CertificateStage54(unsigned group) noexcept {if(nativeStageHook115)nativeStageHook115(group);" NATIVE115_TEST "${NATIVE115_TEST}")
string(FIND "${NATIVE115_TEST}" "int main()" NATIVE115_MAIN)
if(NATIVE115_MAIN LESS 0)
 message(FATAL_ERROR "Native115 production fixture main anchor absent")
endif()
string(SUBSTRING "${NATIVE115_TEST}" 0 ${NATIVE115_MAIN} NATIVE115_TEST)
string(APPEND NATIVE115_TEST [=[namespace Baseline111 {
model_light20::Registry<1024> registry;auto& active=ModelLightShadow20::active;auto& lock=ModelLightShadow20::lock;auto& retired=ModelLightShadow20::retired;
#include "native-owner-baseline111/ModelQueuedWindow49Runtime.inc"
#include "native-owner-baseline111/validate.inc"
}
#include "native-batch115-cases.inc"
int main(int argc,char** argv){const bool measure=argc==1;assert(argc==1||(argc==2&&std::string(argv[1])=="--checks"));
SetEnvironmentVariableA("CE_SKIP_TIMING54","1");ConfigTest59();RunSuite59(false);RunSuite59(true);ExtraTests59();Class5Bulk109();InstanceBulk114();NativeBatch115Cases(measure);return 0;}
]=])
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/native-owner-batch115-generated.cpp" "${NATIVE115_TEST}")
add_executable(native-owner-batch115-tests "${CMAKE_CURRENT_BINARY_DIR}/native-owner-batch115-generated.cpp")
target_include_directories(native-owner-batch115-tests PRIVATE . tests "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options(native-owner-batch115-tests PRIVATE /UNDEBUG /W4 /WX)
target_link_libraries(native-owner-batch115-tests PRIVATE psapi)
add_test(NAME native-owner-batch115 COMMAND native-owner-batch115-tests --checks)
