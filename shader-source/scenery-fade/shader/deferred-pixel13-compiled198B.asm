    ps_3_0
    def c114, -198, -199, 1, 0
    def c219, 1.83951739e+025, 3.99382587e+024, 4.54357875e+030, 2.66246708e-044
    def c0, 0.50999999, 512, 0.25, 256
    def c1, 0.998046875, 7.984375, 63.875, -127.999992
    def c2, 9.99999975e-006, 1, -0.100000001, 1.11111116
    def c3, 1, 0, 0.5, -0.5
    def c4, 3, 4.27199984, 0, 0.75
    def c5, 0.159154937, 0.5, 6.28318548, -3.14159274
    def c6, -1, 1, 255, -128
    def c7, -128, -0, -7.9000001, 2
    def c8, 17.0666656, 1.33333325, 0, 0.125
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
    dcl_2d s4
    dcl_2d s5
    dcl_2d s14
    add r0.zw, c0.x, vPos.xyxy
    mul r0.zw, r0, c84
    texld r2, r0.zwzw, s1
    mul r3.xyz, r2.w, c1
    frc r3.xyz, r3
    mad r3.xy, r3.yzzw, -c8.w, r3
    mad r2.xyz, r2, c0.w, r3
    add r2.xyz, r2, c1.w
    nrm r3.xyz, r2
    texld r2, r0.zwzw, s4
    pow r19.x, c209_abs.z, r2.x
    mul r2.y, r19.x, c209.w
    rcp r1.z, v0.w
    mul r1.z, r1.z, r2.y
    mad r2.xyz, v0, -r1.z, c15
    mad r4.xyz, v0, -r1.z, c2.x
    nrm r5.xyz, r4
    mov r20.x, c77.z
    mul r20.xy, r20.x, c8.xyyy
    mad r4.xyz, r3, r20.x, r2
    add r2.xyz, -r2, c78
    mul r6.xyz, r4.y, c73
    mad r4.xyw, r4.x, c72.xyzz, r6.xyzz
    mad r4.xyz, r4.z, c74, r4.xyww
    add r4.xyz, r4, c75
    add r1.z, r4.z, c76.z
    mov r4.w, -r1_abs.z
    dp3 r1.w, r4.xyww, r4.xyww
    rsq r1.w, r1.w
    rcp r1.w, r1.w
    add r2.w, r1_abs.z, r1.w
    cmp r4.zw, -r1.z, c3.xyxy, c3.xyyx
    rcp r1.z, r2.w
    mul r4.xy, r4, r1.z
    mad r4.xy, r4, c3.zwzw, c3.z
    if_eq -c221_abs.x, c221_abs.x
      dp2add r0.x, vPos, c4, c4.z
      mad r0.x, r0.x, c5.x, c5.y
      frc r0.x, r0.x
      mad r0.x, r0.x, c5.z, c5.w
      sincos r1.xy, r0.x
      mul r0.xy, r1.yxzw, r20.y
      mad_sat r0.xy, r0, c4.w, r4
      mad r0.xy, r0, c77.w, c77
      texldl r6, r0, s14
      dp2add r6.y, r6, r4.zwzw, c3.y
      mul r0.xy, -r1.yxzw, r20.y
      mul r7, r1.xyxy, c6.xyyx
      mad_sat r0.xy, r0, c0.z, r4
      mad r0.xy, r0, c77.w, c77
      texldl r8, r0, s14
      dp2add r6.w, r8, r4.zwzw, c3.y
      mul r0.xy, r7, r20.y
      mad_sat r1.xy, r7.zwzw, r20.y, r4
      mad_sat r0.xy, r0, c3.z, r4
      mad r0.xy, r0, c77.w, c77
      texldl r7, r0, s14
      dp2add r6.z, r7, r4.zwzw, c3.y
      mad r0.xy, r1, c77.w, c77
      texldl r7, r0, s14
      dp2add r6.x, r7, r4.zwzw, c3.y
      mad r1, r1.w, c66.w, r6
      cmp r1, r1, c3.x, c3.y
      dp4 r0.x, r1, c0.z
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
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.zwww, c110.w
      mul r21, c131.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c131.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.zwww, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.x, r24, -c110.x
      mul r21, c132.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c132.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.zwww, c110.w
      mul r21, c133.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c133.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.zwww, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.y, r24, -c110.x
      mul r21, c134.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c134.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.zwww, c110.w
      mul r21, c135.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c135.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.zwww, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.z, r24, -c110.x
      mul r21, c136.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c136.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r24, r21.xyyy, s14
      dp2add r24.x, r24, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.y, r23, r4.zwww, c110.w
      mul r21, c137.xyxy, r22
      add r23.xy, r21.xzxz, r21.ywyw
      mul r21, c137.zwzw, r22
      add r23.zw, r21.xzxz, r21.ywyw
      mad_sat r21, r23, r20.y, r4.xyxy
      mad r21, r21, c77.w, c77.xyxy
      texldl r23, r21.xyyy, s14
      dp2add r24.z, r23, r4.zwww, c110.w
      texldl r23, r21.zwww, s14
      dp2add r24.w, r23, r4.zwww, c110.w
      mad r24, r1.w, c66.w, r24
      cmp r24, r24, c110.y, c110.w
      dp4 r25.w, r24, -c110.x
      dp4 r0.x, r25, -c110.x
    endif
    mov r30.w, c76.x
    add r30.x, c114.x, -r30.w
    add r30.y, r30.w, -c114.y
    cmp r30.z, r30.x, c114.z, c114.w
    cmp r30.y, r30.y, c114.z, c114.w
    mul r30.y, r30.y, r30.z
    lrp r29.x, r30.y, r30.x, c114.z
    lrp r0.x, r29.x, r0.x, c114.z
    dp3 r0.y, r2, r2
    add r1.xyz, r2, c2.x
    nrm r2.xyz, r1
    mov r1.y, c2.y
    mad_sat r0.y, r0.y, -c80.x, r1.y
    mul r0.y, r0.y, r0.y
    mad r0.y, r0.y, r0.y, c2.z
    mul r1.x, r0.y, c2.w
    mul r1.yzw, c83.w, c83.xxyz
    mul r1.xyz, r1.x, r1.yzww
    dp3 r1.w, r2, -c79
    add r1.w, r1.w, -c81.x
    mul_sat r1.w, r1.w, c82.x
    mul r1.xyz, r1, r1.w
    mul r1.xyz, r0.x, r1
    cmp r1.xyz, r0.y, r1, c3.y
    dp3 r0.x, r5, r3
    add r0.x, r0.x, r0.x
    mad r4.xyz, r3, -r0.x, r5
    dp3_sat r0.x, r2, r3
    dp3_sat r0.y, r2, r4
    mul r2.xyz, r1, r0.x
    texld r3, r0.zwzw, s2
    mul r3.yz, r3, r3
    add r0.x, r3.x, r3.x
    mul r1.w, r3.y, c0.y
    pow r2.w, r0.y, r1.w
    mul r1.xyz, r1, r2.w
    mul r1.xyz, r0.x, r1
    texld r4, r0.zwzw, s0
    mad r1.xyz, r4, r2, r1
    mov r1.w, c7.w
    texld r0, r0.zwzw, s5
    add_sat r0.y, r3.z, c3.z
    mad r0.z, r0.x, c6.z, c6.w
    cmp r0.z, r0.z, c7.x, c7.y
    mad r0.x, r0.x, c6.z, r0.z
    add r0.x, r0.x, c7.z
    cmp r0.x, r0.x, c2.y, r0.y
    mul oC0, r1, r0.x
    mul r19.x, v9.w, c209.x
    log r19.x, r19.x
    mul r19.x, r19.x, c209.y
    rcp r19.y, v9.w
    mul r19.y, r19.y, v9.z
    cmp oDepth, -v9_abs.y, r19.y, r19.x

// approximately 288 instruction slots used (45 texture, 243 arithmetic)
