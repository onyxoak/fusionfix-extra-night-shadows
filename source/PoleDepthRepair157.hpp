#pragma once
#include <array>
#include <cstdint>
namespace PoleDepthRepair157 {
struct Binding {uint32_t command{},model{},info{},hash{},fragment{},frame{},thread{},localGroup{};std::array<uint8_t,0x60> bytes{};bool valid{};};
extern SafetyHookMid dispatchHook;extern bool prepared;extern bool requested;
class Scope {Binding previous;public:explicit Scope(void*)noexcept;~Scope()noexcept;Scope(const Scope&)=delete;Scope&operator=(const Scope&)=delete;};
void PrepareHooks()noexcept;
}
