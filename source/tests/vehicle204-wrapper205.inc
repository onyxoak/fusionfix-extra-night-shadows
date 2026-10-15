bool Vehicle204(uint32_t owner,liberty20::VehicleIdentity& id) noexcept {
 const immediate_region33::Scope operation;
 rage::VehiclePool header{};rage::VehiclePool* pool{};
 if(!Copy(reinterpret_cast<uintptr_t>(CVehicle::pool),&pool,sizeof(pool)))return false;
 if(!Copy(reinterpret_cast<uintptr_t>(pool),&header,sizeof(header))||header.m_nSize<=0||header.m_nSize>4096||header.m_nStorageSize<0x1308||header.m_nStorageSize>0x10000)return false;
 const auto storage=uint32_t(reinterpret_cast<uintptr_t>(header.m_aStorage)),flags=uint32_t(reinterpret_cast<uintptr_t>(header.m_aFlags));
 if(owner<storage||uint64_t(owner-storage)>=uint64_t(header.m_nSize)*header.m_nStorageSize||(owner-storage)%header.m_nStorageSize)return false;
 const auto slot=(owner-storage)/header.m_nStorageSize;uint8_t reference{},again{};int16_t modelIndex{};uint32_t info{},model{},type{};
 const traffic_signal::fresh_batch30::Span first[]={{flags+slot,&reference,1},{owner+0x2e,&modelIndex,2},{owner+0x1304,&type,4}};
 if(!traffic_signal::fresh_batch30::Copy(first,3,TrafficSignal::ReadableOriginal,CE_VEHICLE_BATCH_QUERY36)||(reference&0x80)||modelIndex<0||modelIndex>=31000||type!=0||
    !Copy(game+0xe95cd8+uint32_t(modelIndex)*4,&info,4))return false;
 rage::VehiclePool current{};
 const traffic_signal::fresh_batch30::Span final[]={{info+0x3c,&model,4},{flags+slot,&again,1},{reinterpret_cast<uintptr_t>(pool),&current,sizeof(current)}};
 if(!traffic_signal::fresh_batch30::Copy(final,3,TrafficSignal::ReadableOriginal,CE_VEHICLE_BATCH_QUERY36)||!model||again!=reference||current.m_aStorage!=header.m_aStorage||current.m_aFlags!=header.m_aFlags||current.m_nStorageSize!=header.m_nStorageSize||current.m_nSize!=header.m_nSize)return false;
 id={uint32_t(reinterpret_cast<uintptr_t>(pool)),storage,flags,owner,slot,reference,info,model};return true;
}
