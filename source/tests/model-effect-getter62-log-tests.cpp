#include "ModelEffectGetterDiagnostic62.hpp"
#include "standalone-support-log.hpp"
#include <cassert>
#include <cstdio>
// The source identity formatter below is extracted from actual Publish; this
// fixture does not maintain a separate approximate format string.
struct Identity {std::uint32_t pool{},storage{},flags{},owner{},slot{},reference{},model{};};
struct Invocation {std::uint32_t nonce{},frame{},ordinal{},effect{},descriptor{},matrix{},owner{},modelInfo{};};
struct Sample {std::uint32_t thread{},site{},key{},promotedKey{};Identity identity{};Invocation invocation{};};
onyxoak::SupportLog* logger{};
void Event(unsigned level,const char* name,const char* fields){logger->Event(level,name,fields);}
void IdentityLine(const Sample& s){const auto& c=s.invocation;char text[500];
#include "getter62-identity-log-extracted.inc"
}
int main(){
 const auto directory=std::filesystem::current_path()/"getter62-log-output";
 assert(!std::filesystem::exists(directory));std::filesystem::create_directory(directory);
 const auto path=directory/"getter.log";onyxoak::SupportLog actual(path,1,64);logger=&actual;
 Sample s{};s.thread=s.site=s.key=s.promotedKey=UINT32_MAX;s.identity={UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX};
 s.invocation={UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX};IdentityLine(s);
 s.identity={};s.invocation.owner=0x12345670;s.invocation.modelInfo=0xabcd1234;IdentityLine(s);
 model_effect_getter62::Snapshot g{};g.effect=g.vtable=g.codeMask=UINT32_MAX;g.slots.fill(UINT32_MAX);g.status=model_effect_getter62::Status::Observed;for(auto& bytes:g.code)bytes.fill(255);
 char text[500];assert(model_effect_getter62::Format(g,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,text));Event(1,"MODEL_EFFECT_GETTER62",text);
 const std::string getterPayload=text;
 assert(model_effect_getter62::FormatOrigin(g,UINT32_MAX,UINT32_MAX,UINT32_MAX,text));Event(1,"MODEL_EFFECT_ORIGIN62",text);
 std::ifstream input(path);const std::string data((std::istreambuf_iterator<char>(input)),{});
 assert(data.find("MODEL_EFFECT_GETTER62 "+getterPayload)!=data.npos);
 assert(data.find("MODEL_EFFECT_ORIGIN62 "+std::string(text))!=data.npos);
 assert(data.find("effect=ffffffff descriptor=ffffffff emitter_matrix=ffffffff caller_owner=ffffffff caller_model_info=ffffffff observation_not_gpu_execution=1")!=data.npos);
 assert(data.find("owner=0 slot=0 reference=0 model=0")!=data.npos);
 assert(data.find("caller_owner=12345670 caller_model_info=abcd1234")!=data.npos);
 assert(data.find("unsafe-field-omitted")==data.npos&&data.find("observed_not_called=1 no_authority=1")!=data.npos);
 std::puts("actual Publish identity extraction and getter formatter survive real support logger at maximal values");
}
