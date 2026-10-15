#pragma once
#include <array>
#include <cmath>
#include <cstdint>
// Diagnostic evidence only. Failure never authorizes a behavior change.
namespace model_owner_transform46 {
enum class Status : uint32_t { NotRequested, OwnerUnmatched, AddressOverflow,
 PointerReadFailed, NullPointer, MatrixReadFailed, NonfiniteMatrix,
 PointerChanged, IdentityChanged, Valid };
struct Snapshot { Status status{};uint32_t address{};std::array<float,16> matrix{}; };
template<class Copy,class IdentityCurrent>
Snapshot Read(uint32_t owner,bool authenticated,Copy&& copy,IdentityCurrent&& current) noexcept {
 Snapshot out{};
 if(!authenticated){out.status=Status::OwnerUnmatched;return out;}
 if(owner>UINT32_MAX-0x24){out.status=Status::AddressOverflow;return out;}
 if(!copy(owner+0x20,&out.address,sizeof(out.address))){out.status=Status::PointerReadFailed;return out;}
 if(!out.address){out.status=Status::NullPointer;return out;}
 if(out.address>UINT32_MAX-sizeof(out.matrix)){out.status=Status::AddressOverflow;return out;}
 if(!copy(out.address,out.matrix.data(),sizeof(out.matrix))){out.status=Status::MatrixReadFailed;return out;}
 for(const auto value:out.matrix)if(!std::isfinite(value)){out.status=Status::NonfiniteMatrix;return out;}
 uint32_t again{};
 if(!copy(owner+0x20,&again,sizeof(again))){out.status=Status::PointerReadFailed;return out;}
 if(again!=out.address){out.status=Status::PointerChanged;return out;}
 if(!current()){out.status=Status::IdentityChanged;return out;}
 out.status=Status::Valid;return out;
}
}
