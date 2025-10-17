.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared v0 at offset 0
    # Declared v1 at offset 4
    # Declared v2 at offset 8
    # Declared v3 at offset 12
    # Declared v4 at offset 16
    # Declared v5 at offset 20
    # Declared v6 at offset 24
    # Declared v7 at offset 28
    # Declared v8 at offset 32
    # Declared v9 at offset 36
    # Declared v10 at offset 40
    # Declared v11 at offset 44
    # Declared v12 at offset 48
    # Declared v13 at offset 52
    # Declared v14 at offset 56
    # Declared v15 at offset 60
    # Declared v16 at offset 64
    # Declared v17 at offset 68
    # Declared v18 at offset 72
    # Declared v19 at offset 76
    # Declared v20 at offset 80
    # Declared v21 at offset 84
    # Declared v22 at offset 88
    # Declared v23 at offset 92
    # Declared v24 at offset 96
    # Declared v25 at offset 100
    # Declared v26 at offset 104
    # Declared v27 at offset 108
    # Declared v28 at offset 112
    # Declared v29 at offset 116
    # Declared v30 at offset 120
    # Declared v31 at offset 124
    # Declared v32 at offset 128
    # Declared v33 at offset 132
    # Declared v34 at offset 136
    # Declared v35 at offset 140
    # Declared v36 at offset 144
    # Declared v37 at offset 148
    # Declared v38 at offset 152
    # Declared v39 at offset 156
    # Declared v40 at offset 160
    # Declared v41 at offset 164
    # Declared v42 at offset 168
    # Declared v43 at offset 172
    # Declared v44 at offset 176
    # Declared v45 at offset 180
    # Declared v46 at offset 184
    # Declared v47 at offset 188
    # Declared v48 at offset 192
    # Declared v49 at offset 196
    # Declared v50 at offset 200
    # Declared v51 at offset 204
    # Declared v52 at offset 208
    # Declared v53 at offset 212
    # Declared v54 at offset 216
    # Declared v55 at offset 220
    # Declared v56 at offset 224
    # Declared v57 at offset 228
    # Declared v58 at offset 232
    # Declared v59 at offset 236
    # Declared v60 at offset 240
    # Declared v61 at offset 244
    # Declared v62 at offset 248
    # Declared v63 at offset 252
    # Declared v64 at offset 256
    # Declared v65 at offset 260
    # Declared v66 at offset 264
    # Declared v67 at offset 268
    # Declared v68 at offset 272
    # Declared v69 at offset 276
    # Declared v70 at offset 280
    # Declared v71 at offset 284
    # Declared v72 at offset 288
    # Declared v73 at offset 292
    # Declared v74 at offset 296
    # Declared v75 at offset 300
    # Declared v76 at offset 304
    # Declared v77 at offset 308
    # Declared v78 at offset 312
    # Declared v79 at offset 316
    # Declared v80 at offset 320
    # Declared v81 at offset 324
    # Declared v82 at offset 328
    # Declared v83 at offset 332
    # Declared v84 at offset 336
    # Declared v85 at offset 340
    # Declared v86 at offset 344
    # Declared v87 at offset 348
    # Declared v88 at offset 352
    # Declared v89 at offset 356
    # Declared v90 at offset 360
    # Declared v91 at offset 364
    # Declared v92 at offset 368
    # Declared v93 at offset 372
    # Declared v94 at offset 376
    # Declared v95 at offset 380
    # Declared v96 at offset 384
    # Declared v97 at offset 388
    # Declared v98 at offset 392
    # Declared v99 at offset 396
    # Declared v100 at offset 400
    # Declared v101 at offset 404
    # Declared v102 at offset 408
    # Declared v103 at offset 412
    # Declared v104 at offset 416
    # Declared v105 at offset 420
    # Declared v106 at offset 424
    # Declared v107 at offset 428
    # Declared v108 at offset 432
    # Declared v109 at offset 436
    # Declared v110 at offset 440
    # Declared v111 at offset 444
    # Declared v112 at offset 448
    # Declared v113 at offset 452
    # Declared v114 at offset 456
    # Declared v115 at offset 460
    # Declared v116 at offset 464
    # Declared v117 at offset 468
    # Declared v118 at offset 472
    # Declared v119 at offset 476
    # Declared v120 at offset 480
    # Declared v121 at offset 484
    # Declared v122 at offset 488
    # Declared v123 at offset 492
    # Declared v124 at offset 496
    # Declared v125 at offset 500
    # Declared v126 at offset 504
    # Declared v127 at offset 508
    # Declared v128 at offset 512
    # Declared v129 at offset 516
    # Declared v130 at offset 520
    # Declared v131 at offset 524
    # Declared v132 at offset 528
    # Declared v133 at offset 532
    # Declared v134 at offset 536
    # Declared v135 at offset 540
    # Declared v136 at offset 544
    # Declared v137 at offset 548
    # Declared v138 at offset 552
    # Declared v139 at offset 556
    # Declared v140 at offset 560
    # Declared v141 at offset 564
    # Declared v142 at offset 568
    # Declared v143 at offset 572
    # Declared v144 at offset 576
    # Declared v145 at offset 580
    # Declared v146 at offset 584
    # Declared v147 at offset 588
    # Declared v148 at offset 592
    # Declared v149 at offset 596
    # Declared v150 at offset 600
    # Declared v151 at offset 604
    # Declared v152 at offset 608
    # Declared v153 at offset 612
    # Declared v154 at offset 616
    # Declared v155 at offset 620
    # Declared v156 at offset 624
    # Declared v157 at offset 628
    # Declared v158 at offset 632
    # Declared v159 at offset 636
    # Declared v160 at offset 640
    # Declared v161 at offset 644
    # Declared v162 at offset 648
    # Declared v163 at offset 652
    # Declared v164 at offset 656
    # Declared v165 at offset 660
    # Declared v166 at offset 664
    # Declared v167 at offset 668
    # Declared v168 at offset 672
    # Declared v169 at offset 676
    # Declared v170 at offset 680
    # Declared v171 at offset 684
    # Declared v172 at offset 688
    # Declared v173 at offset 692
    # Declared v174 at offset 696
    # Declared v175 at offset 700
    # Declared v176 at offset 704
    # Declared v177 at offset 708
    # Declared v178 at offset 712
    # Declared v179 at offset 716
    # Declared v180 at offset 720
    # Declared v181 at offset 724
    # Declared v182 at offset 728
    # Declared v183 at offset 732
    # Declared v184 at offset 736
    # Declared v185 at offset 740
    # Declared v186 at offset 744
    # Declared v187 at offset 748
    # Declared v188 at offset 752
    # Declared v189 at offset 756
    # Declared v190 at offset 760
    # Declared v191 at offset 764
    # Declared v192 at offset 768
    # Declared v193 at offset 772
    # Declared v194 at offset 776
    # Declared v195 at offset 780
    # Declared v196 at offset 784
    # Declared v197 at offset 788
    # Declared v198 at offset 792
    # Declared v199 at offset 796
    # Declared v200 at offset 800
    # Declared v201 at offset 804
    # Declared v202 at offset 808
    # Declared v203 at offset 812
    # Declared v204 at offset 816
    # Declared v205 at offset 820
    # Declared v206 at offset 824
    # Declared v207 at offset 828
    # Declared v208 at offset 832
    # Declared v209 at offset 836
    # Declared v210 at offset 840
    # Declared v211 at offset 844
    # Declared v212 at offset 848
    # Declared v213 at offset 852
    # Declared v214 at offset 856
    # Declared v215 at offset 860
    # Declared v216 at offset 864
    # Declared v217 at offset 868
    # Declared v218 at offset 872
    # Declared v219 at offset 876
    # Declared v220 at offset 880
    # Declared v221 at offset 884
    # Declared v222 at offset 888
    # Declared v223 at offset 892
    # Declared v224 at offset 896
    # Declared v225 at offset 900
    # Declared v226 at offset 904
    # Declared v227 at offset 908
    # Declared v228 at offset 912
    # Declared v229 at offset 916
    # Declared v230 at offset 920
    # Declared v231 at offset 924
    # Declared v232 at offset 928
    # Declared v233 at offset 932
    # Declared v234 at offset 936
    # Declared v235 at offset 940
    # Declared v236 at offset 944
    # Declared v237 at offset 948
    # Declared v238 at offset 952
    # Declared v239 at offset 956
    # Declared v240 at offset 960
    # Declared v241 at offset 964
    # Declared v242 at offset 968
    # Declared v243 at offset 972
    # Declared v244 at offset 976
    # Declared v245 at offset 980
    # Declared v246 at offset 984
    # Declared v247 at offset 988
    # Declared v248 at offset 992
    # Declared v249 at offset 996
    # Declared v250 at offset 1000
    # Declared v251 at offset 1004
    # Declared v252 at offset 1008
    # Declared v253 at offset 1012
    # Declared v254 at offset 1016
    # Declared v255 at offset 1020
    # Declared v256 at offset 1024
    # Declared v257 at offset 1028
    # Declared v258 at offset 1032
    # Declared v259 at offset 1036
    # Declared v260 at offset 1040
    # Declared v261 at offset 1044
    # Declared v262 at offset 1048
    # Declared v263 at offset 1052
    # Declared v264 at offset 1056
    # Declared v265 at offset 1060
    # Declared v266 at offset 1064
    # Declared v267 at offset 1068
    # Declared v268 at offset 1072
    # Declared v269 at offset 1076
    # Declared v270 at offset 1080
    # Declared v271 at offset 1084
    # Declared v272 at offset 1088
    # Declared v273 at offset 1092
    # Declared v274 at offset 1096
    # Declared v275 at offset 1100
    # Declared v276 at offset 1104
    # Declared v277 at offset 1108
    # Declared v278 at offset 1112
    # Declared v279 at offset 1116
    # Declared v280 at offset 1120
    # Declared v281 at offset 1124
    # Declared v282 at offset 1128
    # Declared v283 at offset 1132
    # Declared v284 at offset 1136
    # Declared v285 at offset 1140
    # Declared v286 at offset 1144
    # Declared v287 at offset 1148
    # Declared v288 at offset 1152
    # Declared v289 at offset 1156
    # Declared v290 at offset 1160
    # Declared v291 at offset 1164
    # Declared v292 at offset 1168
    # Declared v293 at offset 1172
    # Declared v294 at offset 1176
    # Declared v295 at offset 1180
    # Declared v296 at offset 1184
    # Declared v297 at offset 1188
    # Declared v298 at offset 1192
    # Declared v299 at offset 1196
    # Declared w0 at offset 1200
    # Declared w1 at offset 1204
    # Declared w2 at offset 1208
    # Declared w3 at offset 1212
    # Declared w4 at offset 1216
    # Declared w5 at offset 1220
    # Declared w6 at offset 1224
    # Declared w7 at offset 1228
    # Declared w8 at offset 1232
    # Declared w9 at offset 1236
    # Declared w10 at offset 1240
    # Declared w11 at offset 1244
    # Declared w12 at offset 1248
    # Declared w13 at offset 1252
    # Declared w14 at offset 1256
    # Declared w15 at offset 1260
    # Declared w16 at offset 1264
    # Declared w17 at offset 1268
    # Declared w18 at offset 1272
    # Declared w19 at offset 1276
    # Declared w20 at offset 1280
    # Declared w21 at offset 1284
    # Declared w22 at offset 1288
    # Declared w23 at offset 1292
    # Declared w24 at offset 1296
    # Declared w25 at offset 1300
    # Declared w26 at offset 1304
    # Declared w27 at offset 1308
    # Declared w28 at offset 1312
    # Declared w29 at offset 1316
    # Declared w30 at offset 1320
    # Declared w31 at offset 1324
    # Declared w32 at offset 1328
    # Declared w33 at offset 1332
    # Declared w34 at offset 1336
    # Declared w35 at offset 1340
    # Declared w36 at offset 1344
    # Declared w37 at offset 1348
    # Declared w38 at offset 1352
    # Declared w39 at offset 1356
    # Declared w40 at offset 1360
    # Declared w41 at offset 1364
    # Declared w42 at offset 1368
    # Declared w43 at offset 1372
    # Declared w44 at offset 1376
    # Declared w45 at offset 1380
    # Declared w46 at offset 1384
    # Declared w47 at offset 1388
    # Declared w48 at offset 1392
    # Declared w49 at offset 1396
    # Declared w50 at offset 1400
    # Declared w51 at offset 1404
    # Declared w52 at offset 1408
    # Declared w53 at offset 1412
    # Declared w54 at offset 1416
    # Declared w55 at offset 1420
    # Declared w56 at offset 1424
    # Declared w57 at offset 1428
    # Declared w58 at offset 1432
    # Declared w59 at offset 1436
    # Declared w60 at offset 1440
    # Declared w61 at offset 1444
    # Declared w62 at offset 1448
    # Declared w63 at offset 1452
    # Declared w64 at offset 1456
    # Declared w65 at offset 1460
    # Declared w66 at offset 1464
    # Declared w67 at offset 1468
    # Declared w68 at offset 1472
    # Declared w69 at offset 1476
    # Declared w70 at offset 1480
    # Declared w71 at offset 1484
    # Declared w72 at offset 1488
    # Declared w73 at offset 1492
    # Declared w74 at offset 1496
    # Declared w75 at offset 1500
    # Declared w76 at offset 1504
    # Declared w77 at offset 1508
    # Declared w78 at offset 1512
    # Declared w79 at offset 1516
    # Declared w80 at offset 1520
    # Declared w81 at offset 1524
    # Declared w82 at offset 1528
    # Declared w83 at offset 1532
    # Declared w84 at offset 1536
    # Declared w85 at offset 1540
    # Declared w86 at offset 1544
    # Declared w87 at offset 1548
    # Declared w88 at offset 1552
    # Declared w89 at offset 1556
    # Declared w90 at offset 1560
    # Declared w91 at offset 1564
    # Declared w92 at offset 1568
    # Declared w93 at offset 1572
    # Declared w94 at offset 1576
    # Declared w95 at offset 1580
    # Declared w96 at offset 1584
    # Declared w97 at offset 1588
    # Declared w98 at offset 1592
    # Declared w99 at offset 1596
    li $t0, 1
    sw $t0, 0($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 2
    sw $t0, 4($sp)
    lw $t0, 4($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 3
    sw $t0, 8($sp)
    lw $t0, 8($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 4
    sw $t0, 12($sp)
    lw $t0, 12($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 5
    sw $t0, 16($sp)
    lw $t0, 16($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 6
    sw $t0, 20($sp)
    lw $t0, 20($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 7
    sw $t0, 24($sp)
    lw $t0, 24($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 8
    sw $t0, 28($sp)
    lw $t0, 28($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 9
    sw $t0, 32($sp)
    lw $t0, 32($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 10
    sw $t0, 36($sp)
    lw $t0, 36($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 11
    sw $t0, 40($sp)
    lw $t0, 40($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 12
    sw $t0, 44($sp)
    lw $t0, 44($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 13
    sw $t0, 48($sp)
    lw $t0, 48($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 14
    sw $t0, 52($sp)
    lw $t0, 52($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 15
    sw $t0, 56($sp)
    lw $t0, 56($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 16
    sw $t0, 60($sp)
    lw $t0, 60($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 17
    sw $t0, 64($sp)
    lw $t0, 64($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 18
    sw $t0, 68($sp)
    lw $t0, 68($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 19
    sw $t0, 72($sp)
    lw $t0, 72($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 20
    sw $t0, 76($sp)
    lw $t0, 76($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 21
    sw $t0, 80($sp)
    lw $t0, 80($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 22
    sw $t0, 84($sp)
    lw $t0, 84($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 23
    sw $t0, 88($sp)
    lw $t0, 88($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 24
    sw $t0, 92($sp)
    lw $t0, 92($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 25
    sw $t0, 96($sp)
    lw $t0, 96($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 26
    sw $t0, 100($sp)
    lw $t0, 100($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 27
    sw $t0, 104($sp)
    lw $t0, 104($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 28
    sw $t0, 108($sp)
    lw $t0, 108($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 29
    sw $t0, 112($sp)
    lw $t0, 112($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 30
    sw $t0, 116($sp)
    lw $t0, 116($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 31
    sw $t0, 120($sp)
    lw $t0, 120($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 32
    sw $t0, 124($sp)
    lw $t0, 124($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 33
    sw $t0, 128($sp)
    lw $t0, 128($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 34
    sw $t0, 132($sp)
    lw $t0, 132($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 35
    sw $t0, 136($sp)
    lw $t0, 136($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 36
    sw $t0, 140($sp)
    lw $t0, 140($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 37
    sw $t0, 144($sp)
    lw $t0, 144($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 38
    sw $t0, 148($sp)
    lw $t0, 148($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 39
    sw $t0, 152($sp)
    lw $t0, 152($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 40
    sw $t0, 156($sp)
    lw $t0, 156($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 41
    sw $t0, 160($sp)
    lw $t0, 160($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 42
    sw $t0, 164($sp)
    lw $t0, 164($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 6
    li $t1, 7
    mult $t0, $t1
    mflo $t0
    sw $t0, 168($sp)
    lw $t0, 168($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 100
    li $t1, 5
    div $t0, $t1
    mflo $t0
    sw $t0, 172($sp)
    lw $t0, 172($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    mult $t0, $t1
    mflo $t0
    sw $t0, 176($sp)
    lw $t0, 176($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 176($sp)
    lw $t1, 20($sp)
    div $t0, $t1
    mflo $t0
    sw $t0, 180($sp)
    lw $t0, 180($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 4($sp)
    li $t1, 10
    mult $t0, $t1
    mflo $t0
    sw $t0, 184($sp)
    lw $t0, 184($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 84
    lw $t1, 28($sp)
    div $t0, $t1
    mflo $t0
    sw $t0, 188($sp)
    lw $t0, 188($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print integer
    move $a0, $t-1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    # Print integer
    move $a0, $t-1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    li $t0, 10
    li $t1, 0
    div $t0, $t1
    mflo $t0
    sw $t0, 192($sp)
    lw $t0, 192($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
