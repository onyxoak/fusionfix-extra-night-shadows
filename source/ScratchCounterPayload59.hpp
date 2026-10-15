#pragma once
#include <string>
#include <cstdint>
namespace scratch_report59 {
inline std::string Payload(bool requested,uint32_t attempted,uint32_t completed,uint32_t allocationFallback){
 return "requested="+std::to_string(requested)+" attempted="+std::to_string(attempted)+
  " completed="+std::to_string(completed)+" allocation_fallback="+std::to_string(allocationFallback);
}
}
