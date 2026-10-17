#pragma once
// Adapted from MrBulkinAleksey's VehicleBoxShadows (GPL-3.0), commit
// 16318f97a79939c53bb0abda1c6aa247dc6078e0, source/vehicleboxshadows.ixx.
// Original rounded-box fallback, light/car retention and real/box latch by
// MrBulkinAleksey. OnyxOak integration: bounded storage and native witnesses.
#include <array>
#include <bit>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "../PerformanceWorkAvoidance224.hpp"

namespace vehicle_box216 {
struct V {float x{},y{},z{};};
inline float D2(V a,V b) noexcept {const auto x=a.x-b.x,y=a.y-b.y,z=a.z-b.z;return x*x+y*y+z*z;}
inline bool Finite(V a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
struct Car {
    V centre{};float length{},width{},height{},fx{},fy{};
    uint32_t owner{},slot{},generation{};
};
inline bool Same(const Car& a,const Car& b) noexcept {return a.owner==b.owner&&a.slot==b.slot&&a.generation==b.generation;}
struct Light {V position{};float radius{},intensity{};uint32_t key{},flags{},type{},interior{},room{};};
inline uint64_t Identity(const Light& l) noexcept {
    // Numeric light grouping only, never permission, cache identity or owner.
    uint64_t h=14695981039346656037ull;
    auto add=[&](uint32_t v){h^=v;h*=1099511628211ull;};
    for(float f:{l.position.x,l.position.y,l.position.z,l.radius})add(std::bit_cast<uint32_t>(f));
    for(auto v:{l.type,l.interior,l.room,l.flags&0x300u})add(v);
    return h?h:1;
}
struct Settings {
    float rounding=.2f,lightSize=.5f,endRounding=.4f,maxDistance=60.f,minLight=.04f;
    float lengthScale=1.f,widthScale=.95f,heightScale=.9f;
    unsigned perLight=3,perCar=2,fadeInMs=300,fadeOutMs=300,decideMs=500;
};
enum class Latch:uint8_t {Pending,Real,Box};
struct Pair {Car car{};float strength{};uint32_t taken{};Latch latch{};bool chosen{};};
struct Lamp {
    uint64_t identity{};uint32_t key{},tick{},frame{},excludedFrame{};bool excluded{};
    std::array<Pair,3> pairs{};unsigned count{};
};
struct Output {
    // Three centre/half-length and forward/half-width/half-height pairs,
    // followed by the original algorithm's light shape/strength constants.
    std::array<float,28> constants{};std::array<Car,3> cars{};unsigned count{};
};
class State {
    std::array<Lamp,256> lamps_{};
    struct Rank {Car car{};std::array<uint64_t,2> allowed{};std::array<std::pair<uint64_t,float>,8> seen{},before{};unsigned count{},beforeCount{};uint32_t lastFrame{};bool ranked{};};
    std::array<Rank,160> ranks_{};
    // Numeric copied-state index only; fresh native validation is unchanged.
    std::array<uint16_t,4096> rankBySlot224_{};
    // Conservative numeric presence filter. A miss can only avoid an exclusion;
    // a hit still requires the unchanged fresh pool/generation checks.
    std::array<uint32_t,8> owners224_{};
    static uint32_t OwnerHash224(uint32_t owner) noexcept {
        owner^=owner>>16;owner*=0x7feb352du;owner^=owner>>15;return owner;
    }
    void RememberOwner224(uint32_t owner) noexcept {
        const auto h=OwnerHash224(owner),a=h&255u,b=(h>>16)&255u;
        owners224_[a>>5]|=1u<<(a&31);owners224_[b>>5]|=1u<<(b&31);
    }
    void RebuildOwners224() noexcept {
        owners224_={};
        for(const auto& l:lamps_)for(unsigned i=0;i<l.count;++i)
            if(l.pairs[i].latch==Latch::Box&&!(l.pairs[i].strength<=0))RememberOwner224(l.pairs[i].car.owner);
    }
    uint32_t frame_{},tick_{};
    uint32_t excludeKey_{},excludeFrame_{};Lamp* excludeLamp_{};bool excludeCached_{};
    void RankCars(unsigned perCar) noexcept {
        for(auto& r:ranks_)if(r.car.owner){
            if(frame_-r.lastFrame>2){r={};continue;}
            auto both=r.seen;unsigned n=r.count;
            for(unsigned j=0;j<r.beforeCount&&n<both.size();++j){
                bool found=false;for(unsigned i=0;i<n;++i)found|=both[i].first==r.before[j].first;
                if(!found)both[n++]=r.before[j];
            }
            std::sort(both.begin(),both.begin()+n,[](auto a,auto b){return a.second!=b.second?a.second>b.second:a.first<b.first;});
            r.allowed={};for(unsigned i=0;i<std::min({n,perCar,2u});++i)r.allowed[i]=both[i].first;
            r.ranked=n>0;r.before=r.seen;r.beforeCount=r.count;r.count=0;
        }
    }
    bool Allow(const Car& c,uint64_t id,float light,unsigned limit) noexcept {
        Rank* free=nullptr;
        const unsigned cached=c.slot<rankBySlot224_.size()?rankBySlot224_[c.slot]:0;
        const bool direct=work_avoidance224::Enabled()&&cached&&cached<=ranks_.size()&&Same(ranks_[cached-1].car,c);
        const unsigned visits=direct?1u:unsigned(ranks_.size());
        for(unsigned visit=0;visit<visits;++visit){
            const unsigned index=direct?cached-1:visit;auto& r=ranks_[index];
            if(!r.car.owner){if(!free)free=&r;continue;}
            if(!Same(r.car,c))continue;
            if(c.slot<rankBySlot224_.size())rankBySlot224_[c.slot]=uint16_t(index+1);
            r.lastFrame=frame_;const bool held=r.allowed[0]==id||r.allowed[1]==id;
            const float score=light*(held?1.33f:1.f);
            unsigned at=r.count;
            for(unsigned i=0;i<r.count;++i)if(r.seen[i].first==id){at=i;break;}
            const bool adding=at==r.count&&r.count<r.seen.size();
            if(at==r.count){if(r.count<r.seen.size())++r.count;else at=unsigned(std::min_element(r.seen.begin(),r.seen.end(),[](auto a,auto b){return a.second<b.second;})-r.seen.begin());}
            if(adding||r.seen[at].first==id||r.seen[at].second<score)r.seen[at]={id,score};
            return !limit||!r.ranked||held;
        }
        if(free){*free={};free->car=c;free->lastFrame=frame_;free->seen[0]={id,light};free->count=1;if(c.slot<rankBySlot224_.size())rankBySlot224_[c.slot]=uint16_t(free-ranks_.data()+1);}
        return true;
    }
public:
    void Begin(uint32_t frame,uint32_t tick,unsigned perCar) noexcept {
        if(frame<frame_||tick-tick_>1000u){lamps_={};ranks_={};rankBySlot224_={};owners224_={};excludeCached_=false;excludeLamp_=nullptr;}
        if(frame!=frame_){frame_=frame;RankCars(perCar);RebuildOwners224();}
        tick_=tick;
    }
    bool MayContainOwner224(uint32_t owner) const noexcept {
        const auto h=OwnerHash224(owner),a=h&255u,b=(h>>16)&255u;
        return (owners224_[a>>5]&(1u<<(a&31)))&&(owners224_[b>>5]&(1u<<(b&31)));
    }
    Lamp* Find(uint64_t id) noexcept {
        auto index=unsigned(id^(id>>32))&255u;Lamp* free=nullptr;
        for(unsigned i=0;i<4;++i){auto& l=lamps_[(index+i)&255u];
            if(l.identity==id)return &l;
            if(!free&&(!l.identity||tick_-l.tick>1000u))free=&l;
        }
        if(free){*free={};free->identity=id;}
        return free;
    }
    template<class Validate> bool Exclude(uint32_t key,uint32_t entity,uint32_t type,Validate valid) noexcept {
        if(!key||(type!=2&&type!=3))return false;
        // A pass contains many entities. Resolve its key once, rather than
        // scanning the lamp table for every caster in the scene.
        if(!excludeCached_||excludeKey_!=key||excludeFrame_!=frame_){
            excludeKey_=key;excludeFrame_=frame_;excludeCached_=true;excludeLamp_=nullptr;
            for(auto& l:lamps_)if(l.identity&&l.key==key&&frame_-l.frame<=2&&tick_-l.tick<200u){excludeLamp_=&l;break;}
        }
        if(auto* l=excludeLamp_;l&&l->key==key&&frame_-l->frame<=2&&tick_-l->tick<200u){
            for(unsigned i=0;i<l->count;++i){const auto& p=l->pairs[i];
                if(!p.chosen||p.latch!=Latch::Box||p.strength<=0)continue;
                if(work_avoidance224::Enabled()&&type==2&&entity!=p.car.owner)continue;
                if(valid(p.car,entity,type)){l->excludedFrame=frame_;l->excluded=true;return true;}
            }
        }
        return false;
    }
    template<class Validate> Output Select(const Light& light,const Car* cars,unsigned count,V focus,V eye,
                                           bool mapped,const Settings& s,Validate valid) noexcept {
        Output out{};excludeCached_=false;
        if(!Finite(light.position)||!Finite(focus)||!Finite(eye)||!std::isfinite(light.radius)||light.radius<=0||
           !std::isfinite(light.intensity)||light.intensity<=0||(light.flags&0x100u)||
           (light.type!=0&&light.type!=2)||count>160||!cars)return out;
        const float away=std::sqrt(D2(light.position,eye))-light.radius;
        if(away>=s.maxDistance)return out;
        const auto id=Identity(light);auto* state=Find(id);if(!state)return out;
        const bool fresh=!state->tick||tick_-state->tick>1000u;
        const float dt=fresh?0.f:float(tick_-state->tick);
        state->key=light.key;state->frame=frame_;state->tick=tick_;
        std::array<std::pair<float,const Car*>,3> chosen{};unsigned n=0;
        for(unsigned i=0;i<count;++i){const auto& car=cars[i];
            if(!car.owner||!Finite(car.centre)||!std::isfinite(car.length)||!std::isfinite(car.width)||!std::isfinite(car.height)||
               car.length<=.2f||car.width<=.1f||car.height<=.1f)continue;
            bool held=false;for(unsigned k=0;k<state->count;++k)held|=Same(car,state->pairs[k].car)&&state->pairs[k].chosen;
            const float distance=D2(light.position,car.centre),dz=light.position.z-car.centre.z;
            const float reach=(light.radius+car.length+car.width+car.height)*(held?1.15f:1.f);
            if(distance>reach*reach||dz<car.height+(held?.1f:.5f))continue;
            const float falloff=std::max(0.f,1.f-std::sqrt(distance)/light.radius);
            if(falloff*falloff<s.minLight*(held?.5f:1.f)||!Allow(car,id,light.intensity*falloff*falloff,s.perCar))continue;
            const float score=distance*(1.f+D2(car.centre,focus)/400.f)*(held?.25f:1.f);
            const unsigned limit=std::clamp(s.perLight,1u,3u);
            if(n==limit&&score>=chosen[n-1].first)continue;
            unsigned at=n<limit?n++:limit-1;
            while(at&&chosen[at-1].first>score){chosen[at]=chosen[at-1];--at;}
            chosen[at]={score,&car};
        }
        std::array<Pair,3> next{};unsigned nextCount=0;
        const auto step=[&](unsigned ms){return ms?dt/float(ms):1.f;};
        for(unsigned i=0;i<n;++i){Pair p{};p.car=*chosen[i].second;p.chosen=true;p.taken=tick_;
            p.latch=mapped?Latch::Real:Latch::Pending;
            for(unsigned j=0;j<state->count;++j)if(Same(p.car,state->pairs[j].car)){p=state->pairs[j];p.car=*chosen[i].second;p.chosen=true;break;}
            if(p.latch==Latch::Pending)p.latch=mapped?Latch::Real:tick_-p.taken>=s.decideMs?Latch::Box:Latch::Pending;
            if(p.latch==Latch::Real&&!mapped){p.latch=Latch::Box;p.strength=1.f;}
            else if(p.latch==Latch::Real)p.strength=std::max(0.f,p.strength-step(600));
            else if(p.latch==Latch::Box)p.strength=std::min(1.f,p.strength+step(s.fadeInMs));
            next[nextCount++]=p;
        }
        // Retain only a currently validated car; old geometry cannot authorize a source.
        for(unsigned j=0;j<state->count&&nextCount<std::clamp(s.perLight,1u,3u);++j){auto p=state->pairs[j];bool current=false;
            for(unsigned i=0;i<n;++i)current|=Same(p.car,*chosen[i].second);
            if(current)continue;
            if(mapped)continue; // The real map may already contain a departing car.
            bool supplied=false;for(unsigned i=0;i<count;++i)if(Same(p.car,cars[i])){p.car=cars[i];supplied=true;break;}
            if(!supplied)continue; // Never leave an old silhouette behind a moving/despawned car.
            p.strength=std::max(0.f,p.strength-step(s.fadeOutMs));p.chosen=false;
            if(p.strength>0&&valid(p.car,p.car.owner,2))next[nextCount++]=p;
        }
        state->pairs=next;state->count=nextCount;
        for(unsigned i=0;i<nextCount;++i)if(next[i].latch==Latch::Box&&!(next[i].strength<=0))RememberOwner224(next[i].car.owner);
        float rounding=s.rounding;
        for(unsigned i=0;i<nextCount;++i)rounding=std::min(rounding,std::min({next[i].car.length,next[i].car.width,next[i].car.height})-.05f);
        rounding=std::clamp(rounding,0.f,.99f);
        const float distanceFade=std::clamp((s.maxDistance-away)/std::min(20.f,s.maxDistance),0.f,1.f);
        for(unsigned i=0;i<nextCount;++i){auto p=next[i];
            if(p.latch!=Latch::Box||p.strength<=0||!valid(p.car,p.car.owner,2))continue;
            // A real map gains this caster exclusion BEFORE fallback is drawn.
            if(mapped&&(!state->excluded||frame_-state->excludedFrame>1))continue;
            const auto& car=p.car;float* a=out.constants.data()+out.count*8;
            const float dz=light.position.z-car.centre.z,flat=std::sqrt(std::max(0.f,D2(light.position,car.centre)-dz*dz));
            const float t=std::clamp((dz/std::max(.01f,std::sqrt(flat*flat+dz*dz))-.3f)/.6f,0.f,1.f);
            const float lower=1.f-.4f*(1.f-t*t*(3.f-2.f*t));
            const float height=car.height*s.heightScale*lower;
            a[0]=car.centre.x;a[1]=car.centre.y;a[2]=car.centre.z-(car.height-height);
            a[3]=std::max(.05f,car.length*s.lengthScale-rounding)+16.f*std::round(std::clamp(p.strength,0.f,1.f)*15.f);
            a[4]=car.fx;a[5]=car.fy;a[6]=std::max(.05f,car.width*s.widthScale-rounding);a[7]=std::max(.05f,height-rounding);
            out.cars[out.count++]=car;
        }
        out.constants[24]=s.lightSize;out.constants[25]=rounding+2.f*std::round(s.endRounding/.05f);
        out.constants[26]=1.f;out.constants[27]=out.count?distanceFade:0.f;
        return out;
    }
};
}
