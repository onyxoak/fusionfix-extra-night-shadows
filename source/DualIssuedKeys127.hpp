#pragma once
// Liberty's Shadows: numeric successful-publication ownership, GPL-3.0.
#include <array>
#include <atomic>
#include <cstdint>
#include <limits>

namespace dual_keys127 {
template<unsigned Capacity=16384> class PublishedKeys {
    static_assert(Capacity && Capacity%2==0);
    std::array<std::atomic<uint32_t>,(Capacity+31)/32> words{};
    static uint32_t Index(uint32_t base,uint32_t key) noexcept {
        if(!base || (base&63u) || uint64_t(base)+uint64_t(Capacity)*64>UINT32_MAX ||
           key<base || (key-base)%64 || uint64_t(key)-base>=uint64_t(Capacity)*64)
            return UINT32_MAX;
        return (key-base)/64;
    }
    static uint32_t PairIndex(uint32_t base,const std::array<uint32_t,2>& keys) noexcept {
        const auto first=Index(base,keys[0]),second=Index(base,keys[1]);
        if(first==UINT32_MAX || (first&1u) || second==UINT32_MAX || second!=first+1)
            return UINT32_MAX;
        return first;
    }
public:
    bool Contains(uint32_t base,uint32_t key) const noexcept {
        const auto index=Index(base,key);
        return index!=UINT32_MAX &&
            (words[index/32].load(std::memory_order_acquire)&(1u<<(index%32)))!=0;
    }
    bool ContainsPair(uint32_t base,const std::array<uint32_t,2>& keys) const noexcept {
        const auto index=PairIndex(base,keys);
        return index!=UINT32_MAX &&
            (words[index/32].load(std::memory_order_acquire)&(3u<<(index%32)))==(3u<<(index%32));
    }
    // Registry pairs start at even indices. Both keys publish in ONE atomic
    // operation before the first native call; reentry cannot observe half a pair.
    // Caller authenticates collision-free issuance. No pointer or lifetime proof
    // is derived from these monotone ownership bits; Resolve remains mandatory.
    bool PublishPair(uint32_t base,const std::array<uint32_t,2>& keys) noexcept {
        const auto index=PairIndex(base,keys);
        if(index==UINT32_MAX)return false;
        words[index/32].fetch_or(3u<<(index%32),std::memory_order_release);
        return true;
    }
};
}
