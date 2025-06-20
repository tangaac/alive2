define <16 x i16> @src() {
 %1 = call <16 x i16> @llvm.loongarch.lasx.xvhsubw.h.b(<32 x i8> <i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8, i8 9, i8 10, i8 11, i8 12, i8 13, i8 14, i8 15, i8 16, i8 17, i8 18, i8 19, i8 20, i8 21, i8 22, i8 23, i8 24, i8 25, i8 26, i8 27, i8 28, i8 29, i8 30, i8 31>, <32 x i8> <i8 32, i8 33, i8 34, i8 35, i8 36, i8 37, i8 38, i8 39, i8 40, i8 41, i8 42, i8 43, i8 44, i8 45, i8 46, i8 47, i8 48, i8 49, i8 50, i8 51, i8 52, i8 53, i8 54, i8 55, i8 56, i8 57, i8 58, i8 59, i8 60, i8 61, i8 62, i8 63>)
 ret <16 x i16> %1
}

define <16 x i16> @tgt() {
 ret <16 x i16> <i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31, i16 -31>
}

declare <16 x i16> @llvm.loongarch.lasx.xvhsubw.h.b(<32 x i8>, <32 x i8>)

