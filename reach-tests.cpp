#include "fusionfix-source/source/ShadowReach.hpp"
#include <cassert>
#include <limits>
#include <iostream>
int main(){
using namespace fusionfix::shadows;
assert(ShadowReachFeet(10)==50);
assert(ShadowReachFeet(15)==75);
assert(ShadowReachFeet(30)==150);
assert(ShadowReachFeet(40)==200);
assert(ShadowReachFeet(99)==200);
assert(ShadowReachFeet(-1)==0);
assert(!WithinShadowReach(0,0));
assert(WithinShadowReach(3600,40));
assert(!WithinShadowReach(3800,40));
assert(!WithinShadowReach(3600,15));
assert(!WithinShadowReach(std::numeric_limits<float>::quiet_NaN(),40));
std::cout<<"PASS: 11 shadow reach boundary and compatibility checks\n";
}
