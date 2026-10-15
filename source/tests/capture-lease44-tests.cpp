#include "CaptureLease44.hpp"
#include <cassert>
int main(){using CaptureDiagnostic44::Lease;Lease l;
 assert(!l.Update(100,false,false));assert(!l.Update(100,false,true));
 assert(l.Update(100,true,true));assert(l.Update(3100,false,true));assert(!l.Update(3101,false,true));
 assert(!l.Update(3200,true,false));assert(l.Update(3201,false,true));
 Lease wrap;assert(wrap.Update(0xfffffff0u,true,true));assert(wrap.Update(10,false,true));assert(!wrap.Update(4000,false,true));
}
