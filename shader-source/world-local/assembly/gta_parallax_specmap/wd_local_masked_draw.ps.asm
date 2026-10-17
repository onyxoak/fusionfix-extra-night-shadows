ps_3_0
def c214, 0.01, 1, 0, 0
def c219, 1.83951739e+025, 3.99382587e+024, 4.54357875e+030, 6.19373921e-043
def c0, 3.99600005, 4, 0.125, 0.25
def c1, 0, -1, -0, 0
def c118, 0.0671105608, 0.00583714992, 52.9829178, 0
def c150, 0.992217898, 0.00389105058, 0, 0
dcl_texcoord v0.xyz
dcl_texcoord1 v1.zw
dcl vPos.xy
dcl_2d s0
dcl_2d s10
texld r0, v0.yzzw, s0
mul r0.x, r0.w, c39.x
dp2add r20.x, vPos.xyyy, c118.xyyy, c118.w
frc r20.x, r20.x
mul r20.x, r20.x, c118.z
frc r20.x, r20.x
mad r20.y, r20.x, c150.x, c150.y
add r20, -r20.y, r0.x
texkill r20
abs r24.x, c56.x
cmp r24.x, -r24.x, c214.y, c214.z
add r24.y, -v1.z, c214.x
mul r24, r24.x, r24.y
texkill r24
mov oC0, v1.w
