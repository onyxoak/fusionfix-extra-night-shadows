import sys
from pathlib import Path
source=Path(sys.argv[1]);output=Path(sys.argv[2]);baseline=Path(sys.argv[3]).read_text()
current=(source/'ModelLightCatalogRuntime20.inc').read_text()
def body(text,name):
 a=text.index('void Publish() noexcept {');b=a+text[a:].index('{');depth=1;i=b+1
 while depth:
  if text[i]=='{':depth+=1
  elif text[i]=='}':depth-=1
  i+=1
 return text[a:i].replace('void Publish()',f'void {name}()')
a=current.index('struct Sample {');b=current.index('struct Local {');sample=current[a:b]
a=current.index('uint32_t DescriptorWord(');b=current.index('void Publish() noexcept {');helpers=current[a:b]
assert current.count('Batch150 batch150(log.get())')==1
output.write_text(sample+'\nstd::array<Sample,256> ring{};uint32_t head{},tail{};std::atomic_flag ringLock=ATOMIC_FLAG_INIT;\n'+helpers+'\n'+body(baseline,'PublishOld150')+'\n'+body(current,'PublishBatch150').replace('log.get()','activeLog150.get()'))

