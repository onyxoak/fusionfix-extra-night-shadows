#pragma once
#include <array>
#include <cstdint>
namespace fusionfix::shadows {
// Current-list identity census before native insertion begins. An ambiguous
// key is still eligible, but cannot authorize retention or a previous ordinal.
class ShadowKeyMultiplicity27 {
    struct Cell {std::uint32_t key{}; unsigned first{};};
    std::array<Cell,2048> cells_{};
    std::array<bool,640> ambiguous_{};
public:
    void Begin() noexcept {cells_={};ambiguous_={};}
    bool Add(std::uint32_t key,unsigned index) noexcept {
        if(index>=ambiguous_.size())return false;
        if(!key)return true;
        unsigned cell=(key*0x9e3779b9u)&2047u;
        for(unsigned probe=0;probe<cells_.size();++probe) {
            auto& e=cells_[cell];
            if(!e.key){e={key,index};return true;}
            if(e.key==key){ambiguous_[e.first]=ambiguous_[index]=true;return true;}
            cell=(cell+1)&2047u;
        }
        return false;
    }
    bool Ambiguous(unsigned index) const noexcept {
        return index>=ambiguous_.size() || ambiguous_[index];
    }
};
}
