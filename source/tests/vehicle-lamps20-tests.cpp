#include "../VehicleLampIdentity20.hpp"
#include <cassert>
#include <limits>
#include <cstdio>
int main(){
 using namespace liberty20;
 std::array<float,16> m{1,0,0,0,0,1,0,0,0,0,1,0,10,20,30,1};
 std::array<Vec3,2> positions{};
 assert(LampPositions(m,{-1,2,0},{1,2,0},positions));
 assert(positions[0].x==9&&positions[1].x==11&&positions[0].y==22&&positions[1].z==30);
 m={0,1,0,0,-1,0,0,0,0,0,1,0,10,20,30,1};
 assert(LampPositions(m,{-1,2,0},{1,2,0},positions));
 assert(positions[0].x==8&&positions[0].y==19&&positions[1].y==21);
 assert(!LampPositions(m,{0,0,0},{0,0,0},positions));
 assert(!LampPositions(m,{0,0,0},{100,0,0},positions));
 m[0]=std::numeric_limits<float>::quiet_NaN();assert(!LampPositions(m,{-1,0,0},{1,0,0},positions));
 VehicleLampRegistry<4,8> r;assert(!r.Configure(3));assert(r.Configure(0x20000));
 VehicleIdentity id{0x1000,0x2000,0x3000,0x4000,0,1,0x5000,100},copy{};
 auto first=r.Observe(id,100);assert(first&&first->keys[0]!=first->keys[1]);
 auto keys=first->keys;assert(r.Resolve(keys[0],102,copy)&&copy==id);assert(!r.Resolve(keys[0],103,copy));
 assert(r.Observe(id,103)->keys==keys);
 r.Free(0x9999,0x4000,0);assert(r.Resolve(keys[0],103,copy));
 r.Free(0x1000,0x4000,1);assert(r.Resolve(keys[0],103,copy));
 r.Free(0x1000,0x4000,0);assert(!r.Resolve(keys[0],103,copy));
 id.reference++;auto second=r.Observe(id,104);assert(second&&second->keys!=keys);auto newer=second->keys;
 id.modelInfo++;assert(r.Observe(id,105)->keys!=newer);assert(!r.Resolve(newer[0],105,copy));
 auto before=r.Observe(id,105)->keys;auto rewound=r.Observe(id,50);assert(rewound&&rewound->keys!=before);
 r.Retire(0);assert(!r.Observe(id,51)); // Exhaustion never recycles an issued identity.
 assert(!r.Resolve(0x20001,50,copy));assert(!r.Resolve(0xffffffff,50,copy));
 std::puts("vehicle lamp geometry, generation, resource replacement, free, timer rewind and exhaustion checks passed");
}
