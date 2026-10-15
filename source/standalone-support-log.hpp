#pragma once
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
namespace onyxoak {
// All disk logging is OFF by default. Call only from initialization or the
// bounded diagnostic publisher, never from the hot shadow hooks.
// Fields supplied by callers are build IDs, versions, hashes, setting numbers,
// counters and fixed error codes. Never supply paths, names, saves or tokens.
class SupportLog {
 std::filesystem::path path_;
 unsigned mode_{};
 size_t limit_{256*1024};
 std::mutex mutex_;
 static std::string Clean(std::string_view text) {
  std::string out;out.reserve((std::min<size_t>)(text.size(),512));
  // A path/email/raw multi-line diagnostic must not accidentally become a
  // public support-log field. Callers still must obey the whitelist above.
  if(text.find_first_of("\\/@")!=text.npos)return "[unsafe-field-omitted]";
  for(char c:text){if(out.size()==512)break;out+=c>=32&&c<127?c:' ';}return out;
 }
 void Rotate() {
  std::error_code ec;
  auto one=path_;one+=".1";auto two=path_;two+=".2";
  std::filesystem::remove(two,ec);ec.clear();
  if(std::filesystem::exists(one))std::filesystem::rename(one,two,ec);
  ec.clear();if(std::filesystem::exists(path_))std::filesystem::rename(path_,one,ec);
  if(ec){std::ofstream reset(path_,std::ios::trunc);}
 }
public:
 explicit SupportLog(std::filesystem::path path,unsigned mode=0,size_t maximumKiB=256)
  :path_(std::move(path)),mode_((std::min)(mode,2u)),limit_(std::clamp<size_t>(maximumKiB,64,1024)*1024){}
 bool Enabled() const noexcept {return mode_!=0;}
 void Event(unsigned level,std::string_view code,std::string_view fields={}) noexcept {
  if(!mode_||level>mode_)return;
  try {
   std::lock_guard lock(mutex_);
   const auto stamp=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
   const auto line=std::to_string(stamp)+" "+Clean(code)+" "+Clean(fields)+"\n";
   std::error_code ec;auto size=std::filesystem::file_size(path_,ec);if(ec)size=0;
   if(size+line.size()>limit_)Rotate();
   std::ofstream out(path_,std::ios::app);out<<line;out.flush();
  }catch(...){/* Logging failure never escapes into the game. */}
 }
};
}
