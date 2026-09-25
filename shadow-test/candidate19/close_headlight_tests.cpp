#include "../../modified-source/CloseHeadlightRelevance.hpp"
#include "../../modified-source/ShadowLightGeometry.hpp"
#include <cstdio>
#include <limits>
using namespace fusionfix::shadows;
int main() {
    int checks = 0;
    auto check = [&](bool result) { ++checks; if (!result) { std::printf("FAIL %d\n", checks); return false; } return true; };
    const Vec3 origin{}, forward{1,0,0};
    // Feet/center outside a narrow beam while body extent crosses it.
    if (!check(!LightVolumeContains({1,0,0.6f},origin,forward,2,20,0.98f))) return 1;
    if (!check(CloseHeadlightTouchesBody({1,0,0.6f},origin,forward,0.98f,20))) return 1;
    if (!check(CloseHeadlightTouchesBody({1,0,0.6f},origin,{10,0,0},0.98f,20))) return 1;
    if (!check(!CloseHeadlightTouchesBody({-1,0,0},origin,forward,0.98f,20))) return 1;
    if (!check(!CloseHeadlightTouchesBody({1,2,0},origin,forward,0.98f,20))) return 1;
    if (!check(!CloseHeadlightTouchesBody({4.01f,0,0},origin,forward,0.98f,20))) return 1;
    if (!check(CloseHeadlightTouchesBody({4,0,0},origin,forward,0.98f,20))) return 1;
    if (!check(!CloseHeadlightTouchesBody({2,0,0},origin,forward,0.98f,0.1f))) return 1;
    if (!check(!CloseHeadlightTouchesBody({1,0,0},origin,{},0.98f,20))) return 1;
    for (float invalid : {0.0f, -1.0f, 1.01f, std::numeric_limits<float>::quiet_NaN()})
        if (!check(!CloseHeadlightTouchesBody({1,0,0},origin,forward,invalid,20))) return 1;
    if (!check(!CloseHeadlightTouchesBody({std::numeric_limits<float>::infinity(),0,0},origin,forward,0.98f,20))) return 1;
    for (int i=0; i<=40; ++i) {
        const float axial=i/10.0f;
        if (!check(CloseHeadlightTouchesBody({axial,0,0},origin,forward,0.98f,20))) return 1;
        if (!check(CloseHeadlightTouchesBody({0,axial,0},origin,{0,1,0},0.98f,20))) return 1;
    }
    std::printf("PASS %d checks\n",checks);
}

