#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace fusionfix::shadows {
// Ranking only. No retained light pointers, maps, eligibility or illumination.
class ShadowPrioritySmoothing20 {
    struct Entry {std::uint32_t key{},frame{},time{};float value{};};
    std::array<Entry,512> entries_{};
    std::uintptr_t session_{};
public:
    void Begin(std::uintptr_t session) noexcept {
        if(session!=session_){entries_={};session_=session;}
    }
    float Observe(std::uint32_t key,float raw,std::uint32_t frame,std::uint32_t now) noexcept {
        if(!key||!std::isfinite(raw)||raw<0)return raw;
        auto& e=entries_[((key>>6)*2654435761u)%entries_.size()];
        if(e.key!=key&&e.key&&e.frame==frame)return raw; // Collision cannot borrow history.
        if(e.key!=key||frame<e.frame||now-e.time>250u||frame-e.frame>16u||
           raw>std::fmax(16.f,e.value*4.f)||e.value>std::fmax(16.f,raw*4.f)) {
            e={key,frame,now,raw};return raw;
        }
        if(frame==e.frame)return e.value;
        const float alpha=float(now-e.time)/(120.f+float(now-e.time));
        e.value+=(raw-e.value)*alpha;e.frame=frame;e.time=now;return e.value;
    }
};
}
