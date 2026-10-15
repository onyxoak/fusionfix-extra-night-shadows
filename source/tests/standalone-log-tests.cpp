#include "standalone-support-log.hpp"
#include "ScratchCounterPayload59.hpp"
#include <cassert>
#include <iostream>
int main(){
 const auto root=std::filesystem::current_path()/"support-log-test-output";
 if(std::filesystem::exists(root)){std::cerr<<"Test directory already exists; choose a fresh workspace\n";return 1;}
 std::filesystem::create_directory(root);
 const auto p=root/"mod.log";
 {onyxoak::SupportLog off(p);off.Event(1,"START","build=alpha");assert(!std::filesystem::exists(p));}
 {onyxoak::SupportLog basic(p,1,64);basic.Event(2,"DETAIL","not-written");assert(!std::filesystem::exists(p));
  basic.Event(1,"START","build=alpha game=1.2.0.59 ff=5.1.0");
  basic.Event(1,"UNSAFE","C:\\Users\\Someone\\save.dat");
 }
 {std::ifstream f(p);std::string text((std::istreambuf_iterator<char>(f)),{});assert(text.find("Someone")==text.npos);assert(text.find("unsafe-field-omitted")!=text.npos);assert(text.find("not-written")==text.npos);}
 // The ACTUAL production formatter must survive the real512-byte sanitizer.
 const auto scratchFields59=scratch_report59::Payload(true,UINT32_MAX,UINT32_MAX,UINT32_MAX);
 assert(scratchFields59.size()<512);
 {onyxoak::SupportLog scratch(p,1,64);scratch.Event(1,"MODEL_SCRATCH59_COUNTS",scratchFields59);}
 {std::ifstream f(p);std::string text((std::istreambuf_iterator<char>(f)),{});
  assert(text.find("MODEL_SCRATCH59_COUNTS "+scratchFields59)!=text.npos);
  for(const auto* field:{"requested=1","attempted=4294967295","completed=4294967295","allocation_fallback=4294967295"})assert(text.find(field)!=text.npos);
 }
 {onyxoak::SupportLog detail(p,2,64);for(unsigned i=0;i<1000;++i)detail.Event(2,"COUNTERS",std::string(500,'x'));}
 size_t count=0,total=0;for(const auto& f:std::filesystem::directory_iterator(root)){assert(f.file_size()<=65536);total+=f.file_size();++count;}
 assert(count<=3&&total<=3*65536);
 std::cout<<"PASS logging OFF by default; detail gated; path redaction; <=3 files; strict 64KiB test rotation\n";
}
