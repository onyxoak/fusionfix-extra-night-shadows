    ps_3_0
    def c114, -198, -199, 1, 0
    def c219, 1.83951739e+025, 3.99382587e+024, 4.54357875e+030, 2.80259693e-044
    def c0, 0.50999999, 2, -0.999989986, 9.99999975e-006
    def c1, 1, -0.333333343, 1.5, 0.100000001
    def c2, 1, 0, 0.5, -0.5
    def c3, 3, 4.27199984, 0, 0.75
    def c4, 0.159154937, 0.5, 6.28318548, -3.14159274
    def c5, -1, 1, 0.25, 0
    def c8, 17.0666656, 1.33333325, 0, 0
    def c110, -0.25, 1, -1, 0
    def c111, 0.159154937, 0.5, 6.28318548, -3.14159274
    def c112, 3, 4.27199984, 0, 0
    def c113, 0.75, -0.5, 0.5, 0
    def c118, 0.0671105608, 0.00583714992, 52.9829178, 0
    def c130, 0.189936459, 0.0270871148, -0.21261242, 0.23391293
    def c131, 0.0477178134, -0.366684079, 0.297730982, 0.398259878
    def c132, -0.509063423, -0.0652868152, 0.507855177, -0.287597597
    def c133, -0.152306169, 0.6426121, -0.302401692, -0.580507278
    def c134, 0.697801948, 0.277117342, -0.699096322, 0.321096063
    def c135, 0.356514245, -0.706641495, 0.266889989, 0.836019099
    def c136, -0.751586139, -0.416098773, 0.910293758, -0.170145273
    def c137, -0.534347117, 0.805859327, -0.113327011, -0.949002564
    dcl_texcoord v0
    dcl vPos.xy
    dcl_texcoord9 v9
    dcl_2d s0
    dcl_2d s1
    dcl_2d s2
    dcl_2d s14
    add r0.xy, c0.x, vPos
    mul r0.xy, r0, c84.zwzw
    texld r1, r0, s2
    pow r19.x, c209_abs.z, r1.x
    mul r1.y, r19.x, c209.w
    rcp r0.z, v0.w
    mul r0.z, r0.z, r1.y
    mad r1.xyz, v0, -r0.z, c15
    texld r2, r0, s1
    texld r0, r0, s0
    mad r2.xyz, r2, c0.y, c0.z
    nrm r3.xyz, r2
    mov r20.x, c77.z
    mul r20.xy, r20.x, c8.xyyy
    mad r2.xyz, r3, r20.x, r1
    add r1.xyz, -r1, c78
    mul r4.xyz, r2.y, c73
    mad r2.xyw, r2.x, c72.xyzz, r4.xyzz
    mad r2.xyz, r2.z, c74, r2.xyww
    add r2.xyz, r2, c75
    add r0.w, r2.z, c76.z
    mov r2.w, -r0_abs.w
    dp3 r1.w, r2.xyww, r2.xyww
    rsq r1.w, r1.w
    rcp r1.w, r1.w
    add r2.z, r0_abs.w, r1.w
    cmp r4.xy, -r0.w, c2, c2.yxzw
    rcp r0.w, r2.z
    mul r2.xy, r2, r0.w
    mad r2.xy, r2, c2.zwzw, c2.z
    if_eq -c221_abs.x, c221_abs.x
      dp2add r0.w, vPos, c3, c3.z
      mad r0.w, r0.w, c4.x, c4.y
      frc r0.w, r0.w
      mad r0.w, r0.w, c4.z, c4.w
      sincos r5.xy, r0.w
      mul r2.zw, r5.xyyx, r20.y
      mad_sat r2.zw, r2, c3.w, r2.xyxy
      mad r2.zw, r2, c77.w, c77.xyxy
      texldl r6, r2.zwzw, s14
      dp2add r6.y, r6, r4, c2.y
      mul r2.zw, -r5.xyyx, r20.y
      mul r5, r5.xyxy, c5.xyyx
      mad_sat r2.zw, r2, c5.z, r2.xyxy
      mad r2.zw, r2, c77.w, c77.xyxy
      texldl r7, r2.zwzw, s14
      dp2add r6.w, r7, r4, c2.y
      mul r2.zw, r5.xyxy, r20.y
      mad_sat r4.zw, r5, r20.y, r2.xyxy
      mad_sat r2.xy, r2.zwzw, c2.z, r2
      mad r2.xy, r2, c77.w, c77
      texldl r2, r2, s14
      dp2add r6.z, r2, r4, c2.y
      mad r2.xy, r4.zwzw, c77.w, c77
      texldl r2, r2, s14
      dp2add r6.x, r2, r4, c2.y
      mad r2, r1.w, c66.w, r6
      cmp r2, r2, c2.x, c2.y
      dp4 r0.w, r2, c5.z
    else
      dp2add r21.x, vPos.xyyy, c118.xyyy, c118.w
      frc r21.x, r21.x
      mul r21.x, r21.x, c118.z
      frc r21.x, r21.x
      mul r21.x, r21.x, c111.z
      sincos r22.xy, r21.x
      mul r22, r22.xyyx, c110.yzyy
      mul r21, c130.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c130.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.xyyy, c110.w
      mul r21, c131.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c131.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.xyyy, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.x, r24, -c110.x
      mul r21, c132.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c132.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.xyyy, c110.w
      mul r21, c133.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c133.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.xyyy, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.y, r24, -c110.x
      mul r21, c134.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c134.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.xyyy, c110.w
      mul r21, c135.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c135.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.xyyy, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.z, r24, -c110.x
      mul r21, c136.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c136.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.xyyy, c110.w
      mul r21, c137.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c137.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r2.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.xyyy, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.xyyy, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.w, r24, -c110.x
      dp4 r0.w, r25, -c110.x
    endif
    mov r30.w, c76.x
    add r30.x, c114.x, -r30.w
    add r30.y, r30.w, -c114.y
    cmp r30.z, r30.x, c114.z, c114.w
    cmp r30.y, r30.y, c114.z, c114.w
    mul r30.y, r30.y, r30.z
    lrp r29.x, r30.y, r30.x, c114.z
    lrp r0.w, r29.x, r0.w, c114.z
    add r2.xyz, r1, c0.w
    dp3 r1.x, r1, r1
    mov r4.x, c1.x
    mad_sat r1.x, r1.x, -c80.x, r4.x
    mad r1.x, r1.x, r1.x, c1.y
    nrm r4.xyz, r2
    dp3 r1.y, r4, -c79
    dp3 r1.z, r3, r4
    add r1.z, r1.z, c1.y
    mul_sat r1.z, r1.z, c1.z
    add r1.y, r1.y, -c81.x
    mul_sat r1.y, r1.y, c82.x
    mul r1.w, r1.x, c1.z
    mul r1.y, r1.y, r1.w
    mul r1.y, r1.z, r1.y
    mul r0.w, r0.w, r1.y
    mul r1.yzw, c83.w, c83.xxyz
    mul r1.yzw, r0.w, r1
    mul r0.xyz, r0, r1.yzww
    cmp oC0.xyz, r1.x, r0, c2.y
    mov oC0.w, c1.x
    mul r19.x, v9.w, c209.x
    log r19.x, r19.x
    mul r19.x, r19.x, c209.y
    rcp r19.y, v9.w
    mul r19.y, r19.y, v9.z
    cmp oDepth, -v9_abs.y, r19.y, r19.x

// approximately 260 instruction slots used (43 texture, 217 arithmetic)
