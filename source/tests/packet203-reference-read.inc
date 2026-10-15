bool CopyPacket(const rage::CLightSource& light,Packet& p,bool fresh=true,unsigned* queryCount133=nullptr) noexcept {
 if(queryCount133)*queryCount133=0;
 p={};if(fresh){++winnerPreflights;const auto permission=native_projection66::Preflight(reinterpret_cast<uintptr_t>(&light),0x78);
  permissionQueries.fetch_add(permission.queries,std::memory_order_relaxed);
  if(queryCount133)*queryCount133=permission.queries;
  if(!permission.allowed){++winnerDenied;return false;}
 }
 const auto copied=Liberty20::CopyLocal(reinterpret_cast<uintptr_t>(&light),p.data(),0x78);
 if(!copied)++packetFaults;return copied;
}
bool OwnKey(uint32_t key) noexcept {
 return issuedKeys.Contains(registry,key);
}
