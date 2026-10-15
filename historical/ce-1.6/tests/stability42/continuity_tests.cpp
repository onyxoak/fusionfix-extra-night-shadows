#include "source/NativeShadowContinuity42.hpp"
#include "source/ShadowAllocationPass.hpp"
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <cstdlib>
using Policy=fusionfix::shadows::NativeShadowContinuity42;
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::cerr<<__LINE__<<": "<<#x<<"\n";std::exit(1);}}while(0)
struct Light {unsigned key,flags;float distance;bool visible=true,own=false;std::uint64_t generation=1;};
int Native(unsigned a,unsigned b) {if(!b || a&0x400)return 0;if(a&0x100)return b&0x400?2:((b>>8)&1);return b&0x500?2:1;}
std::vector<Light> Select(Policy& p,std::vector<Light> input,bool continuity) {
    std::vector<Light> out;
    for(auto c:input) {
        auto at=out.begin();
        for(;at!=out.end();++at) {
            int result=Native(c.flags,at->flags);
            if(continuity)result=Policy::Compare(result,p.Observe({c.key,c.generation},c.flags,c.visible,c.own),
                                                p.Observe({at->key,at->generation},at->flags,at->visible,at->own));
            if(result==0 || (result==1 && c.distance<at->distance))break;
        }
        out.insert(at,c);if(out.size()>7)out.pop_back();
    }
    return out;
}
void Commit(Policy& p,const std::vector<Light>& lights) {
    std::array<Policy::Identity,7> ids{};
    for(unsigned i=0;i<lights.size();++i)ids[i]={lights[i].key,lights[i].generation};
    CHECK(p.Commit(ids));
}
bool Has(const std::vector<Light>& lights,unsigned key){for(auto l:lights)if(l.key==key)return true;return false;}
int main() {
    // Exact identities/types from CE41 frame3679731; the eighth headlight in
    // frame3679732 used to evict lamp0x1cca3863 despite its valid static cache0.
    std::vector<Light> baseline{{0x1c1857f0,0x164,5},{0x1ccb0380,0x6f,8},
        {0x1ccc2620,0x6f,10},{0x1ccc2303,0x67,12},{0x1cc97060,0x6f,14},
        {0x1ccbc543,0x67,16},{0x1cca3863,0x67,20}};
    Policy p;p.Begin(1,1,16);Commit(p,baseline);
    auto input=baseline;input.push_back({0x1c18b970,0x164,6});
    CHECK(!Has(Select(p,input,false),0x1cca3863));
    CHECK(Has(Select(p,input,true),0x1cca3863));
    // Hold every valid visible choice, irrespective of input ordering, small
    // distance oscillations, category or repeated transient headlights.
    std::mt19937 rng(42);
    for(unsigned f=2;f<6002;++f) {
        p.Begin(1,f,f*16);input=baseline;
        for(auto& l:input)l.distance=1.0f+float(rng()%10000)/100;
        for(unsigned k=0;k<20;++k)input.push_back({100+k,k%2?0x164u:0x67u,0.5f+float(k)});
        std::shuffle(input.begin(),input.end(),rng);
        const auto held=Select(p,input,true);CHECK(held.size()==7);
        for(unsigned i=0;i<7;++i)CHECK(held[i].key==baseline[i].key);
        Commit(p,held);
    }
    // Both a lamp and an admitted headlight keep their claims. A source that
    // leaves view becomes replaceable immediately; missing input never draws.
    input=baseline;input.back().visible=false;input.push_back({0x1c18b970,0x164,6});
    auto released=Select(p,input,true);CHECK(!Has(released,0x1cca3863));CHECK(Has(released,0x1c18b970));
    input=baseline;input.erase(input.begin()+1);input.push_back({0x1c18b970,0x164,6});
    released=Select(p,input,true);CHECK(!Has(released,0x1ccb0380));CHECK(Has(released,0x1c18b970));
    // A reused stationary key does not inherit an unrelated light's claim.
    input=baseline;input.back().generation=2;input.push_back({0x1c18b970,0x164,6});
    CHECK(!Has(Select(p,input,true),0x1cca3863));
    // Entering the player's car can acquire one beam without repeated churn.
    input=baseline;input.push_back({77,0x164,30,true,true});
    released=Select(p,input,true);CHECK(released[0].key==77);CHECK(released.size()==7);Commit(p,released);
    for(unsigned f=6002;f<6202;++f){p.Begin(1,f,f*16);std::shuffle(input.begin(),input.end(),rng);auto next=Select(p,input,true);
        for(unsigned i=0;i<7;++i)CHECK(next[i].key==released[i].key);Commit(p,next);}
    CHECK(Policy::Compare(0,{0x464,7,false,true},{0x67,0,false,true})==0);
    CHECK(Policy::Compare(2,{0x67,0,false,true},{0x464,7,false,true})==2);
    CHECK(Policy::Compare(1,{0x67,0,false,false},{0x67,7,false,true})==1);
    p.Begin(2,6203,99248);CHECK(p.Observe({77,1},0x164,true,true).claim==7); // load/session
    Commit(p,baseline);p.Begin(2,6204,100000);CHECK(p.Observe({baseline[0].key,1},0x164,true,false).claim==7);
    // Atomic final-output validation preserves the native set, INCLUDING beam
    // identity/order, instead of independently reranking it after cache work.
    using namespace fusionfix::shadows::budget;
    ShadowAllocationPass pass;
    std::array<std::array<std::uint8_t,128>,8> records{};
    struct Output{unsigned before=0x12345678;std::array<int,7> indices{0,1,2,3,4,5,6};unsigned after=0x87654321;} output;
    const auto original=output;
    pass.Begin({1,16,1,false},records.data(),8);
    for(unsigned i=0;i<8;++i)pass.Observe({i+1,i,i<6?Kind::Lamp:Kind::OtherBeam,400.0f-i*50,true,true,0},records[i].data());
    CHECK(pass.Commit(records.data(),8,output.indices.data(),true,true));
    CHECK(!std::memcmp(&original,&output,sizeof(output)));
    CHECK(pass.LastSelection().slots[6].key==7); // custom score prefers beam8; must not replace7
    std::cout<<"PASS "<<checks<<" continuity checks: recorded beam/lamp conflict, 6000 shuffled crowded frames,\n"
        <<"whole-view claims, invalid/missing inputs, identity reuse, own beam, resets, special lights, native output canaries\n";
}
