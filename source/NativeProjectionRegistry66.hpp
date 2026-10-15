#pragma once
// Exact normalized projection values, not native emitter/owner identity.
// Does not borrow resources, emit lights, reuse issued keys, or touch native memory.
#include <array>
#include <cstdint>
#include <cstring>
#include <bit>
#include <initializer_list>
#include <atomic>
namespace native_projection66 {
using Packet=std::array<uint32_t,32>;
struct ConstructorContext {
 uintptr_t caller{},returnSlot{},destination{},direction{},tangent{},position{},color{};
 uint32_t type{},flags{},key{},interior{},room{};
};
enum class InsertionDecision:uint8_t { Ordinary,Skipped,Accepted,Rejected };
template<class Witness> inline InsertionDecision GateInsertion(bool owned,bool previewAvailable,
 bool wouldWrite,Witness witness) noexcept {
 if(!owned)return InsertionDecision::Ordinary;
 if(previewAvailable&&!wouldWrite)return InsertionDecision::Skipped;
 return witness()==1?InsertionDecision::Accepted:InsertionDecision::Rejected;
}
// Existing native depth/map domain ONLY: copied non-photometric projection
// values form76-byte immutable identity. Room/masks/textures remain.
// Authenticated CE depth consumers ignore the four vector W lanes. Exclude
// them ONLY from map identity; original packets and full112 witness retain them.
// Evidence: projection-engine66-review/W-IDENTITY74-APPROVAL.md.
// RGB/intensity are still preserved independently and witnessed per packet.
// Volume intensity at DWORD28 is not published to authenticated CE depth/map
// records, commands or current ranking. Its animated illumination remains
// untouched and freshly authenticated by the full112-byte packet witness.
// This narrowly removes observed stationary volume churn; geometry still
// creates immutable keys and no key or native shadow map is recycled.
inline constexpr std::array<unsigned,19> Fields{0,1,2,4,5,6,8,9,10,17,18,19,20,21,22,23,26,27,29};
inline constexpr std::array<unsigned,28> WitnessFields{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,26,27,28,29};
using Projection=std::array<uint32_t,Fields.size()>;
using PacketWitness=std::array<uint32_t,WitnessFields.size()>;
// Allocating a token range does not own a foreign native key in that range.
// Successful collision-checked issuance is published before the native packet.
// Bits only change0→1; burned/unused tokens remain foreign forever.
template<unsigned Capacity=4096> struct IssuedKeys {
 std::array<std::atomic<uint32_t>,(Capacity+31)/32> words{};
 bool Bounds(uint32_t base,uint32_t key) const noexcept {
  return key>=base&&uint64_t(key)-base<uint64_t(Capacity)*64&&(key-base)%64==0;
 }
 bool Publish(uint32_t base,uint32_t key) noexcept {
  if(!Bounds(base,key))return false;const auto index=(key-base)/64;
  words[index/32].fetch_or(1u<<(index%32),std::memory_order_release);return true;
 }
 bool Contains(uint32_t base,uint32_t key) const noexcept {
  if(!Bounds(base,key))return false;const auto index=(key-base)/64;
  return (words[index/32].load(std::memory_order_acquire)&(1u<<(index%32)))!=0;
 }
};
// Original normalized ctor and native assignment each execute exactly once.
// The ctor's actual EAX result is independent of destination and retained.
template<class Original,class Adapt,class Assign>
inline uint32_t ConstructOwned(void* destination,Original original,Adapt adapt,Assign assign) noexcept {
 alignas(16) Packet packet{};const auto result=original(packet.data());
 adapt(packet);assign(destination,packet.data());return result;
}
inline Projection Values(const Packet& p,bool snowMetadata70=false) noexcept {
 Projection out{};for(unsigned i=0;i<Fields.size();++i)out[i]=p[Fields[i]];
 // Authenticated FF511 after-copy snow callback changes only volumetric
 // metadata in this scoped spot domain. Native depth/caster consumers do not
 // receive these three values. Illumination packet and full witness stay exact.
 if(snowMetadata70&&p[17]==2&&!(p[18]&0x390u)&&
    p[21]>=std::bit_cast<uint32_t>(8.f)&&p[21]<=std::bit_cast<uint32_t>(20.f)){
  for(unsigned i=0;i<Fields.size();++i){
   if(Fields[i]==18)out[i]&=~8u;
   if(Fields[i]==28||Fields[i]==29)out[i]=0;
  }
 }
 return out;
}
inline PacketWitness Witness(const Packet& p) noexcept {
 PacketWitness out{};for(unsigned i=0;i<WitnessFields.size();++i)out[i]=p[WitnessFields[i]];return out;
}
inline bool Finite(uint32_t bits) noexcept {return (bits&0x7f800000u)!=0x7f800000u;}
inline bool Eligible(const Packet& p) noexcept {
 if((p[17]!=0&&p[17]!=2)||(p[18]&6u)||p[24]||!Finite(p[21])||
    (p[21]&0x80000000u)||(p[21]&0x7fffffffu)==0)return false;
 // Match legacy finite XYZ/photometry eligibility without treating unused
 // native W components as semantic floats. Exact W bits remain in the witness.
 for(unsigned i:{0u,1u,2u,4u,5u,6u,8u,9u,10u,12u,13u,14u,16u,22u,23u})
  if(!Finite(p[i]))return false;
 return !(p[16]&0x80000000u);
}
inline uint32_t Hash(const Projection& p) noexcept {
 uint32_t h=2166136261u;for(auto x:p){h^=x;h*=16777619u;h^=h>>13;}return h;
}
template<unsigned Capacity=4096> struct Registry {
 static_assert(Capacity&&((Capacity&(Capacity-1))==0));
 struct Entry {Projection projection{};uint32_t key{},lastFrame{};bool issued{};};
 std::array<Entry,Capacity> entries{};
 std::array<uint32_t,Capacity*2> cells{};
 uint32_t used{},keyBase{};
 bool snowMetadata70=false;
 void Init(uint32_t base) noexcept {keyBase=base;}
 bool Contains(uint32_t key) const noexcept {
  return key>=keyBase&&uint64_t(key)-keyBase<uint64_t(used)*64&&(key-keyBase)%64==0;
 }
 const Entry* Lookup(uint32_t key) const noexcept {
  if(!Contains(key))return nullptr;const auto& e=entries[(key-keyBase)/64];return e.issued?&e:nullptr;
 }
 template<class Collision> uint32_t Observe(const Packet& packet,uint32_t frame,Collision collision) noexcept {
  if(!keyBase||!Eligible(packet))return 0;
  const auto projection=Values(packet,snowMetadata70);auto cell=Hash(projection)&(cells.size()-1);
  for(unsigned n=0;n<cells.size();++n){auto& slot=cells[cell];
   if(slot){auto& e=entries[slot-1];if(e.projection==projection){e.lastFrame=frame;return e.issued?e.key:0;}}
   else {
    if(used==Capacity||uint64_t(keyBase)+uint64_t(used)*64>UINT32_MAX)return 0;
    const auto index=used++;auto& e=entries[index];e={projection,keyBase+index*64,frame,false};slot=index+1;
    // A collision burns the token permanently; never reassign it later.
    if(!collision(e.key))e.issued=true;return e.issued?e.key:0;
   }
   cell=(cell+1)&(cells.size()-1);
  }return 0;
 }
 bool Validate(uint32_t key,const Packet& packet) const noexcept {
  // Current accepted native packet is emission authority. The immutable token
  // describes projection only; it never authorizes replay or a retained source.
  const auto* e=Lookup(key);if(!e||packet[24]!=key||(packet[18]&6u)!=4u)return false;
  auto normalized=packet;normalized[18]&=~4u;normalized[24]=0;
  return Eligible(normalized)&&e->projection==Values(normalized,snowMetadata70);
 }
};
// TLS sidecar bound to one allocator pass; never modifies native packet storage.
// All own-domain packets start invalid. Only a complete current packet witness
// makes one valid; contention/fault/reentrancy cannot retain old authorization.
enum class Decision:uint8_t { Ordinary,Invalid,Valid,Alias };
template<unsigned Max=640> struct Sidecar {
 std::array<Decision,Max> decisions{};std::array<Packet,Max> packets{};
 uintptr_t bank{},anchor{};uint32_t count{},frame{};bool begun{};
 void Begin(uintptr_t source,uint32_t size,uint32_t epoch) noexcept {
  decisions.fill(Decision::Ordinary);bank=source;count=size;frame=epoch;anchor=0;begun=size<=Max&&source!=0;
 }
 bool Envelope(uintptr_t source,uint32_t size,uint32_t epoch) const noexcept {
  return begun&&source==bank&&size==count&&epoch==frame;
 }
 void Own(unsigned index,const Packet& p) noexcept {if(begun&&index<count){packets[index]=p;decisions[index]=Decision::Invalid;}}
 void Accept(unsigned index,bool alias) noexcept {if(begun&&index<count)decisions[index]=alias?Decision::Alias:Decision::Valid;}
 Decision Current(unsigned index,const Packet& p,uintptr_t source,uint32_t size,uint32_t epoch,uintptr_t stack) noexcept {
  if(!Envelope(source,size,epoch)||index>=count)return Decision::Invalid;
  if(stack){if(!anchor)anchor=stack;else if(anchor!=stack)return Decision::Invalid;}
  const auto d=decisions[index];
  if((d!=Decision::Valid&&d!=Decision::Alias)||p[24]!=packets[index][24]||Witness(p)!=Witness(packets[index]))return Decision::Invalid;
  auto normalized=p;normalized[18]&=~4u;normalized[24]=0;
  return Eligible(normalized)?d:Decision::Invalid;
 }
};
// Independent illumination packets are never merged. An exact same-projection
// alias may only bypass duplicate shadow insertion, after current packet auth.
template<unsigned Max=640> struct AliasPass {
 std::array<uint32_t,Max> keys{};std::array<Projection,Max> values{};
 std::array<bool,Max> alias{};std::array<unsigned,Max> representative{};std::array<float,Max> importance{};uint32_t count{};
 static_assert(Max>0&&Max<=32768);
 static constexpr unsigned Cells=[](){unsigned n=1;while(n<Max*2)n*=2;return n;}();
 // TLS current-pass index only. Keys still identify immutable, separately
 // authenticated projections. A hash collision never merges two keys.
 std::array<uint16_t,Cells> cells{};bool indexed{};
 static unsigned KeyCell(uint32_t key) noexcept {
  key^=key>>16;key*=0x7feb352du;key^=key>>15;
  return key&(Cells-1);
 }
 void Begin() noexcept {count=0;alias.fill(false);indexed=false;}
 bool Add(unsigned index,uint32_t key,const Packet& packet,float weight=0.f,bool snowMetadata70=false) noexcept {
  if(index>=Max||!key)return false;const auto value=Values(packet,snowMetadata70);
  unsigned found=Max,cell=0;
  // Keep tiny/duplicate-heavy passes on the original scan. The table is built
  // exactly once after32 distinct keys, including all earlier representatives.
  if(count<32){for(unsigned i=0;i<count;++i)if(keys[i]==key){found=i;break;}}
  else {
   if(!indexed){cells.fill(0);
    for(unsigned i=0;i<count;++i){auto target=KeyCell(keys[i]);
     while(cells[target])target=(target+1)&(Cells-1);
     cells[target]=uint16_t(i+1);
    }
    indexed=true;
   }
   cell=KeyCell(key);
   for(unsigned probes=0;probes<Cells;++probes,cell=(cell+1)&(Cells-1)){
    const auto slot=cells[cell];if(!slot)break;
    const unsigned i=slot-1;if(keys[i]==key){found=i;break;}
   }
  }
  if(found!=Max){const unsigned i=found;
   if(values[i]!=value)return false;
   if(weight>importance[i]){alias[representative[i]]=true;representative[i]=index;importance[i]=weight;alias[index]=false;}
   else alias[index]=true;return true;
  }
  if(count==Max)return false;
  if(indexed){if(cells[cell])return false;cells[cell]=uint16_t(count+1);}
  keys[count]=key;values[count]=value;representative[count]=index;importance[count]=weight;++count;return true;
 }
 bool IsAlias(unsigned index) const noexcept {return index<Max&&alias[index];}
};
}
