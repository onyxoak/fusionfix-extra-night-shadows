#pragma once
#include <cstdint>
namespace CaptureDiagnostic44 {
struct Lease {
 uint32_t lastHeartbeat{};bool seen{};
 bool Update(uint32_t now,bool heartbeat,bool requested) noexcept {
  if(heartbeat){lastHeartbeat=now;seen=true;}
  return requested&&seen&&uint32_t(now-lastHeartbeat)<=3000u;
 }
};
}
