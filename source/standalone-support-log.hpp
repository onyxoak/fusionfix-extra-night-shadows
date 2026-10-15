#pragma once
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
namespace onyxoak {
// Ordinary disk logging is OFF by default; a fatal startup gets one bounded
// failure report. Call only from initialization or the
// bounded diagnostic publisher, never from the hot shadow hooks.
// Fields supplied by callers are build IDs, versions, hashes, setting numbers,
// counters and fixed error codes. Never supply paths, names, saves or tokens.
class SupportLog {
 std::filesystem::path path_;
 unsigned mode_{};
 size_t limit_{256*1024};
 std::mutex mutex_;
 bool startupReported_{};
 using Clock150=long long(*)() noexcept;
 static long long Stamp150() noexcept {return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
 Clock150 clock150_=Stamp150;
 size_t opens150_{},flushes150_{},sizeQueries150_{};
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
 std::string Line150(std::string_view code,std::string_view fields) {return std::to_string(clock150_())+" "+Clean(code)+" "+Clean(fields)+"\n";}
 void Lines150(const std::string* lines,size_t count,bool startup=false) noexcept {
  try {
   std::lock_guard lock(mutex_);
   if(startup){if(startupReported_)return;startupReported_=true;}
   if(!count)return;
   std::error_code ec;++sizeQueries150_;auto size=std::filesystem::file_size(path_,ec);if(ec)size=0;
   std::ofstream out;
   for(size_t index=0;index<count;++index){const auto& line=lines[index];
#ifdef _WIN32
    const auto encodedSize=line.size()+std::count(line.begin(),line.end(),char(10)); // Native text streams translate LF to CRLF.
#else
    const auto encodedSize=line.size();
#endif
    if(size+encodedSize>limit_){if(out.is_open()){out.flush();++flushes150_;out.close();}Rotate();size=0;}
    if(!out.is_open()){out.open(path_,std::ios::app);++opens150_;}
    if(!out)return;out<<line;if(!out)return;size+=encodedSize;
   }
   if(out.is_open()){out.flush();++flushes150_;}
  }catch(...){/* Logging failure never escapes into the game. */}
 }
 void Write(std::string_view code,std::string_view fields,bool startup) noexcept {try{const auto line=Line150(code,fields);Lines150(&line,1,startup);}catch(...){}}
public:
 explicit SupportLog(std::filesystem::path path,unsigned mode=0,size_t maximumKiB=256,Clock150 clock=Stamp150)
  :path_(std::move(path)),mode_((std::min)(mode,2u)),limit_(std::clamp<size_t>(maximumKiB,64,1024)*1024),clock150_(clock?clock:Stamp150){}
 struct IO150 {size_t opens{},flushes{},sizeQueries{};};
 IO150 Counts150() noexcept {std::lock_guard lock(mutex_);return {opens150_,flushes150_,sizeQueries150_};}
 class Batch150 {
  SupportLog* owner_{};std::vector<std::string> lines_;size_t bytes_{};
 public:
  explicit Batch150(SupportLog* owner) noexcept:owner_(owner){}
  ~Batch150() noexcept{Flush();}
  Batch150(const Batch150&)=delete;Batch150& operator=(const Batch150&)=delete;
  void Flush() noexcept {if(owner_&&!lines_.empty())owner_->Lines150(lines_.data(),lines_.size());lines_.clear();bytes_=0;}
  void Event(unsigned level,std::string_view code,std::string_view fields={}) noexcept {
   if(!owner_||!owner_->mode_||level>owner_->mode_)return;
   try{auto line=owner_->Line150(code,fields);if(lines_.size()>=512||bytes_+line.size()>65536)Flush();bytes_+=line.size();lines_.push_back(std::move(line));}
   catch(...){Flush();owner_->Event(level,code,fields);}
  }
 };
 bool Enabled() const noexcept {return mode_!=0;}
 void Event(unsigned level,std::string_view code,std::string_view fields={}) noexcept {
  if(!mode_||level>mode_)return;
  Write(code,fields,false);
 }
 // One startup fatal report even when ordinary logging is OFF. Do not call
 // from gameplay publishers; caller supplies only bounded safe fields.
 void StartupFailure(std::string_view fields) noexcept {Write("STARTUP_FAILURE",fields,true);}
};
}



