void Caster189(SafetyHookContext& regs) {
 if(!addonActive.load(std::memory_order_acquire))return;
 const fusionfix::shadows::FloatingPointState fp;
 const bool artificial=!*reinterpret_cast<const uint8_t*>(regs.esp+0x0b);
 bool skip=OwnHeadlightCaster::Exclude(regs.esi,regs.eax,artificial);
 if(!bVehicleNightShadows&&regs.eax==3&&(*reinterpret_cast<const uint8_t*>(regs.esi+620)&4)&&artificial)skip=true;
 PoleCasterDiagnostic134::Observe(regs.esi,regs.eax,artificial,skip);
 PoleDrawWitness145::SunCaster(regs.esi,uint32_t(regs.eax),uint32_t(regs.edx),regs.ebp,skip);
 uint32_t eligibility142{};
 if(PoleStaticPartition141::Reclassify(regs.esi,uint32_t(regs.eax),uint32_t(regs.edx),regs.ebp,skip,&eligibility142)){regs.eax=eligibility142;regs.eip=game+0x6e3767;return;}
 regs.eip=game+(skip?0x6e3867:0x6e374f);
}
