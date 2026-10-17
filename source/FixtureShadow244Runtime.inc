// Authored multi-head light proxies can originate inside their own support.
// Exclude only that support from its own light, as for a vehicle's headlights.
// It remains a caster for every other source. Floors/buildings are untouched.
#include "core/PoleCasterSnapshot134.hpp"
namespace FixtureShadow244 {
using Packet=native_projection66::Packet;
struct Record {
 uint32_t key{},frame{},owner{},info{},hash{},descriptor{},kind{};
 uint16_t modelIndex{};
 traffic_signal::Identity identity{};
 std::array<uint8_t,0x6c> bytes{};
 std::array<uint8_t,68> ownerPose{};
 std::array<float,16> pose{};
};
std::array<Record,2048> records{};std::atomic_flag lock=ATOMIC_FLAG_INIT;
thread_local Record selected{};
thread_local uint32_t selectedThread{};
bool requested=false;
std::atomic<uint32_t> recorded{},bound{},excluded{},refused{};
void (*previousObserver)(uint32_t,uint32_t,uint32_t,bool) noexcept=nullptr;
// Native keys are not64-byte aligned. Hash every key bit and retain colliding
// current-frame emitters independently. Never evict another live record.
constexpr unsigned ProbeCount259=32;
unsigned Start259(uint32_t key) noexcept {
 key^=key>>16;key*=0x7feb352du;key^=key>>15;key*=0x846ca68bu;key^=key>>16;
 return key&unsigned(records.size()-1);
}
Record* Find259(uint32_t key,uint32_t frame) noexcept {
 const auto start=Start259(key);
 for(unsigned i=0;i<ProbeCount259;++i){auto& cell=records[(start+i)&(records.size()-1)];
  if(cell.key==key&&cell.frame==frame)return &cell;
 }
 return nullptr;
}
bool Store259(const Record& record) noexcept {
 const auto start=Start259(record.key);Record* stale=nullptr;Record* exact=nullptr;
 for(unsigned i=0;i<ProbeCount259;++i){auto& cell=records[(start+i)&(records.size()-1)];
  if(cell.key==record.key){exact=&cell;break;}
  if(!stale&&(!cell.key||cell.frame!=record.frame))stale=&cell;
 }
 auto* cell=exact?exact:stale;if(!cell){++refused;return false;}
 // Two independent emitters sharing the same key remain ambiguous this frame.
 if(cell->key==record.key&&cell->frame==record.frame&&(!cell->owner||cell->identity!=record.identity))cell->owner=0;
 else *cell=record;
 return true;
}

bool Proxy(uint32_t hash) noexcept {
 // Apply to each supported street/park lamp globally as well as industrial fixtures.
 // Only the emitting fixture is excluded from its own light; other sources retain it.
 if(pole_caster134::SupportedPole(hash))return true;
 static const std::array<uint32_t,23> hashes{0x42945123u,0x386b3cd1u,0x2630985cu,0xc54ee0a4u,0xfc3a6b32u,0x3336ea4fu,0x058b0ef8u,0x4fc1a364u,0x22224826u,0xd8e035dfu,0xc6809120u,0x3c3b7c94u,0x2a84d927u,0x91d8a7d1u,0x329cf087u,0x65d80e52u,0x5a5e775fu,0xbb4c3939u,0x8015c2cdu,0x4526cdd8u,0x6ef9217cu,0xa86f9468u,0xa5b73d9cu};
 for(auto h:hashes)if(h&&h==hash)return true;return false;
}
bool Pose(uint32_t owner,std::array<float,16>& matrix) noexcept {
 uint32_t pointer{},again{};
 if(!ModelEffectObserver::Copy(uintptr_t(owner)+0x20,&pointer,4))return false;
 if(pointer){
  if(!ModelEffectObserver::Copy(pointer,matrix.data(),sizeof(matrix)))return false;
  // RAGE's affine transform uses three XYZ basis vectors and XYZ translation.
  // The fourth lane of each row carries padding/metadata, not a transform.
  // HD/native aliases can differ there while having exactly the same pose.
  for(unsigned i:{0u,1u,2u,4u,5u,6u,8u,9u,10u,12u,13u,14u})if(!std::isfinite(matrix[i]))return false;
  matrix[3]=matrix[7]=matrix[11]=0.f;matrix[15]=1.f;
 }
 else {
  // Compare the native inline translation/heading as raw pose data. Do not
  // invent a world transform or equate it with a matrix-backed owner alias.
  std::array<float,4> pose{},tail{};
  if(!ModelEffectObserver::Copy(uintptr_t(owner)+0x10,pose.data(),sizeof(pose))||
     !ModelEffectObserver::Copy(uintptr_t(owner)+0x10,tail.data(),sizeof(tail))||pose!=tail)return false;
  matrix={};matrix[3]=matrix[7]=2.f;
  for(unsigned i=0;i<4;++i)matrix[12+i]=pose[i];
 }
 if(!ModelEffectObserver::Copy(uintptr_t(owner)+0x20,&again,4)||again!=pointer)return false;
 for(float f:matrix)if(!std::isfinite(f))return false;
 return true;
}
bool Fresh(const Record& record) noexcept {
 if(!requested||!addonActive.load(std::memory_order_acquire)||record.frame!=ModelEffectObserver::Frame())return false;
 TrafficSignal::Match current{};std::array<uint8_t,0x6c> bytes{};bool copied{};
 uint32_t hash{};uint16_t index{};std::array<float,16> pose{};
 return TrafficSignal::MatchOwnerWithDescriptor48(record.owner,current,record.descriptor,bytes.data(),bytes.size(),copied)&&
  copied&&bytes==record.bytes&&current.identity==record.identity&&current.modelInfo==record.info&&current.kind==record.kind&&
  ((record.kind!=2&&record.kind!=3)||current.ownerPose109==record.ownerPose)&&
  ModelEffectObserver::Copy(uintptr_t(record.info)+0x3c,&hash,4)&&hash==record.hash&&
  ModelEffectObserver::Copy(uintptr_t(record.owner)+0x2e,&index,2)&&index==record.modelIndex&&
  Pose(record.owner,pose)&&pose==record.pose&&record.frame==ModelEffectObserver::Frame();
}
void Constructed(const Packet& packet,uint32_t frame,const native_projection66::ConstructorContext& context) noexcept {
 // Recording must observe the same constructor/key path as ordinary play.
 // Populate diagnostics from its actual numeric packet instead of switching
 // sampled sources to a different legacy identity just to collect vectors.
 const auto depth=ModelEffectObserver::local.state.Depth();
 const auto* current=ModelEffectObserver::local.state.Current();
 if(current&&current->frame==frame&&depth&&depth<=ModelEffectObserver::local.samples.size()){
  auto& sample=ModelEffectObserver::local.samples[depth-1];
  if(sample.diagnostic){
   for(unsigned i=0;i<3;++i){
    sample.direction[i]=std::bit_cast<float>(packet[i]);sample.up[i]=std::bit_cast<float>(packet[4+i]);
    sample.position[i]=std::bit_cast<float>(packet[8+i]);sample.color[i]=std::bit_cast<float>(packet[12+i]);
   }
   sample.intensity=std::bit_cast<float>(packet[16]);sample.radius=std::bit_cast<float>(packet[21]);
   sample.innerCone=std::bit_cast<float>(packet[22]);sample.outerCone=std::bit_cast<float>(packet[23]);
   sample.promotedKey=packet[24];sample.packetCopied=true;
  }
 }
 if(!requested||!packet[24]||(packet[17]!=0&&packet[17]!=2)||
    !NativeProjection66::OwnKey(packet[24]))return;
 const auto* invocation=ModelEffectObserver::local.state.Current();if(!invocation||frame!=invocation->frame)return;
 Record record{};record.key=packet[24];record.frame=frame;record.owner=invocation->owner;record.info=invocation->modelInfo;record.descriptor=invocation->descriptor;
 // A contained numeric rejection hint avoids a second owner/stack validation
 // for every unrelated city source. Full fresh authority follows for matches.
 if(!Liberty20::CopyLocal(uintptr_t(record.info)+0x3c,&record.hash,4)||!Proxy(record.hash))return;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
 TrafficSignal::Match match{};bool copied{};
 if(!TrafficSignal::MatchOwnerWithDescriptor48(record.owner,match,record.descriptor,record.bytes.data(),record.bytes.size(),copied)||
    !copied||match.modelInfo!=record.info||!Pose(record.owner,record.pose)||
    !ModelEffectObserver::Copy(uintptr_t(record.owner)+0x2e,&record.modelIndex,2)||record.modelIndex>=31000)return;
 record.identity=match.identity;record.kind=match.kind;record.ownerPose=match.ownerPose109;
 if(!Fresh(record)||!ModelEffectObserver::ModelValue68::Matches(ModelBinding69(context)))return;
 if(lock.test_and_set(std::memory_order_acquire)){++refused;return;}
 const bool stored=Store259(record);
 lock.clear(std::memory_order_release);if(stored)++recorded;
}
// Native streetlamp keys bypass the private projection constructor adapter.
// Native flags7 are valid shadowed model submissions; bit2 is not a veto.
// The existing model producer supplies this key after authenticating its stack,
// effect descriptor and current invocation. No pointer/key convention is guessed.
void Native256(const ModelEffectObserver::Invocation& invocation,uint32_t key,uint32_t type,uint32_t flags) noexcept {
 if(!requested||!addonActive.load(std::memory_order_acquire)||!key||key==UINT32_MAX||
    (type!=0&&type!=2)||!(flags&4u)||invocation.frame!=ModelEffectObserver::Frame()||
    ModelEffectObserver::local.state.Current()!=&invocation)return;
 Record record{};record.key=key;record.frame=invocation.frame;record.owner=invocation.owner;
 record.info=invocation.modelInfo;record.descriptor=invocation.descriptor;
 if(!Liberty20::CopyLocal(uintptr_t(record.info)+0x3c,&record.hash,4)||!Proxy(record.hash))return;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
 TrafficSignal::Match match{};bool copied{};
 if(!TrafficSignal::MatchOwnerWithDescriptor48(record.owner,match,record.descriptor,record.bytes.data(),record.bytes.size(),copied)||
    !copied||match.modelInfo!=record.info||!Pose(record.owner,record.pose)||
    !ModelEffectObserver::Copy(uintptr_t(record.owner)+0x2e,&record.modelIndex,2)||record.modelIndex>=31000)return;
 record.identity=match.identity;record.kind=match.kind;record.ownerPose=match.ownerPose109;
 if(!Fresh(record)||ModelEffectObserver::local.state.Current()!=&invocation)return;
 if(lock.test_and_set(std::memory_order_acquire)){++refused;return;}
 const bool stored=Store259(record);
 lock.clear(std::memory_order_release);if(stored)++recorded;
}
void Observe(uint32_t key,uint32_t slot,uint32_t kind,bool active) noexcept {
 if(previousObserver)previousObserver(key,slot,kind,active);
 selected={};selectedThread=0;
 if(!requested||!active||kind<3||kind>5||slot<1||slot>7||!addonActive.load(std::memory_order_acquire))return;
 Record record{};
 if(lock.test_and_set(std::memory_order_acquire))return;
 if(const auto* cell=Find259(key,ModelEffectObserver::Frame()))record=*cell;
 lock.clear(std::memory_order_release);
 if(record.key!=key||!record.owner||record.frame!=ModelEffectObserver::Frame())return;
 const ModelEffectObserver::LastErrorScope error;const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
 if(Fresh(record)){selected=record;selectedThread=GetCurrentThreadId();++bound;}
}
bool Exclude(uint32_t key,uintptr_t entity,uint32_t type) noexcept {
 if(!requested||key!=selected.key||!selected.owner||selected.frame!=ModelEffectObserver::Frame()||
    selectedThread!=GetCurrentThreadId()||!addonActive.load(std::memory_order_acquire)||
    entity>UINT32_MAX||(type==2||type==3))return false;
 const ModelEffectObserver::LastErrorScope error;
 // Cheap model hint rejects other scene geometry; it grants no lifetime.
 uint16_t model{};
 if(!Liberty20::CopyLocal(entity+0x2e,&model,2)||model!=selected.modelIndex)return false;
 const fusionfix::shadows::FloatingPointState fp;
 const traffic_signal::fresh_page::Operation44 pages(32);const immediate_region33::Scope regions;
 TrafficSignal::Match caster{};std::array<float,16> pose{};
 if(!TrafficSignal::MatchOwner(uint32_t(entity),caster)||caster.modelInfo!=selected.info||
    !Pose(uint32_t(entity),pose)||!Fresh(selected))return false;
 // HD/native-pool aliases of the same fixture can have distinct addresses.
 // Require the same complete transform; another lamp of this type still casts.
 for(unsigned i:{0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,12u,13u,14u,15u})
  if(std::abs(pose[i]-selected.pose[i])>0.001f)return false;
 TrafficSignal::Match again{};
 if(!TrafficSignal::MatchOwner(uint32_t(entity),again)||again.identity!=caster.identity||again.modelInfo!=caster.modelInfo||
    selected.frame!=ModelEffectObserver::Frame())return false;
 ++excluded;return true;
}
bool Combined(uint32_t key,uintptr_t entity,uint32_t type) noexcept {
 return VehicleBoxes216::Exclude(key,entity,type)||Exclude(key,entity,type);
}
void Enable() noexcept {
 NativeProjection66::constructed244.store(Constructed,std::memory_order_release);
 ModelEffectObserver::nativeFixture256=Native256;
 if(!requested)return;
 previousObserver=OwnHeadlightCaster::labRenderObserver;OwnHeadlightCaster::labRenderObserver=Observe;
 OwnHeadlightCaster::boxExclude216=Combined;
 Event(1,"FIXTURE_SHADOW244_ACTIVE","emitting_fixture_only=1 other_sources_preserved=1 surrounding_geometry_preserved=1 extra_maps=0");
}
}
