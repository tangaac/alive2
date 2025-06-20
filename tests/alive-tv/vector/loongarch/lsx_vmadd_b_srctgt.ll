define <16 x i8> @src() {
 %1 = call <16 x i8> @llvm.loongarch.lsx.vmadd.b(<16 x i8> <i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8, i8 9, i8 10, i8 11, i8 12, i8 13, i8 14, i8 15>, <16 x i8> <i8 16, i8 17, i8 18, i8 19, i8 20, i8 21, i8 22, i8 23, i8 24, i8 25, i8 26, i8 27, i8 28, i8 29, i8 30, i8 31>, <16 x i8> <i8 32, i8 33, i8 34, i8 35, i8 36, i8 37, i8 38, i8 39, i8 40, i8 41, i8 42, i8 43, i8 44, i8 45, i8 46, i8 47>)
 ret <16 x i8> %1
}

define <16 x i8> @tgt() {
 ret <16 x i8> <i8 512, i8 562, i8 614, i8 668, i8 724, i8 782, i8 842, i8 904, i8 968, i8 1034, i8 1102, i8 1172, i8 1244, i8 1318, i8 1394, i8 1472>
}

declare <16 x i8> @llvm.loongarch.lsx.vmadd.b(<16 x i8>, <16 x i8>, <16 x i8>)

