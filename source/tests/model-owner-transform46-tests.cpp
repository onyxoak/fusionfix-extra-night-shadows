#include "ModelOwnerTransformDiagnostic46.hpp"
#include <cassert>
#include <cstring>
#include <limits>
using namespace model_owner_transform46;
struct Fixture {
 uint32_t pointer=0x2000,reads{},failAt{},changedAt{};bool identity=true;
 std::array<float,16> matrix{0,1,0,0,-1,0,0,0,0,0,1,0,10,20,30,1};
 bool Copy(uint32_t address,void* bytes,size_t size) noexcept {
  ++reads;if(reads==failAt)return false;
  if(address==0x1020&&size==4){const auto value=reads==changedAt?pointer+64:pointer;std::memcpy(bytes,&value,4);return true;}
  if(address==pointer&&size==sizeof(matrix)){std::memcpy(bytes,matrix.data(),size);return true;}
  return false;
 }
 Snapshot Read(bool authenticated=true,uint32_t owner=0x1000) noexcept {
  return model_owner_transform46::Read(owner,authenticated,[&](uint32_t p,void* d,size_t n) noexcept{return Copy(p,d,n);},[&]() noexcept{return identity;});
 }
};
int main(){
 Fixture f;const auto valid=f.Read();assert(valid.status==Status::Valid&&valid.address==f.pointer&&valid.matrix==f.matrix&&f.reads==3);
 // A known 90-degree owner pose maps local panel front -Y to world +X.
 // Translation differs from an effect matrix: retain the complete owner basis.
 const std::array<float,3> front{0,-1,0},panel{0,-.478f,.1f};
 std::array<float,3> worldNormal{},worldPoint{};
 for(unsigned j=0;j<3;++j){worldPoint[j]=valid.matrix[12+j];for(unsigned i=0;i<3;++i){worldNormal[j]+=front[i]*valid.matrix[i*4+j];worldPoint[j]+=panel[i]*valid.matrix[i*4+j];}}
 assert(worldNormal[0]==1&&worldNormal[1]==0&&worldNormal[2]==0);
 assert(std::fabs(worldPoint[0]-10.478f)<.0001f&&worldPoint[1]==20&&std::fabs(worldPoint[2]-30.1f)<.0001f);
 Fixture unmatched;assert(unmatched.Read(false).status==Status::OwnerUnmatched&&unmatched.reads==0);
 Fixture overflow;assert(overflow.Read(true,UINT32_MAX-0x20).status==Status::AddressOverflow&&overflow.reads==0);
 Fixture nullPointer;nullPointer.pointer=0;assert(nullPointer.Read().status==Status::NullPointer);
 for(uint32_t failure=1;failure<=3;++failure){Fixture broken;broken.failAt=failure;const auto s=broken.Read();assert(s.status==(failure==2?Status::MatrixReadFailed:Status::PointerReadFailed));}
 Fixture nonfinite;nonfinite.matrix[0]=std::numeric_limits<float>::quiet_NaN();assert(nonfinite.Read().status==Status::NonfiniteMatrix);
 Fixture changed;changed.changedAt=3;assert(changed.Read().status==Status::PointerChanged);
 Fixture expired;expired.identity=false;assert(expired.Read().status==Status::IdentityChanged);
 Fixture preserved;preserved.matrix[0]=2;assert(preserved.Read().status==Status::Valid); // Diagnostics retain scaled transforms rather than silently normalize.
}
