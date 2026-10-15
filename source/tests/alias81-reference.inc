template<unsigned Max=640> struct AliasPass {
 std::array<uint32_t,Max> keys{};std::array<Projection,Max> values{};
 std::array<bool,Max> alias{};std::array<unsigned,Max> representative{};std::array<float,Max> importance{};uint32_t count{};
 void Begin() noexcept {count=0;alias.fill(false);}
 bool Add(unsigned index,uint32_t key,const Packet& packet,float weight=0.f,bool snowMetadata70=false) noexcept {
  if(index>=Max||!key)return false;const auto value=Values(packet,snowMetadata70);
  for(unsigned i=0;i<count;++i)if(keys[i]==key){
   if(values[i]!=value)return false;
   if(weight>importance[i]){alias[representative[i]]=true;representative[i]=index;importance[i]=weight;alias[index]=false;}
   else alias[index]=true;return true;
  }
  if(count==Max)return false;keys[count]=key;values[count]=value;representative[count]=index;importance[count]=weight;++count;return true;
 }
 bool IsAlias(unsigned index) const noexcept {return index<Max&&alias[index];}
};
