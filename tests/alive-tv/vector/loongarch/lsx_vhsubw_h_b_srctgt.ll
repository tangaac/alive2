define <8 x i16> @src() {
 %1 = call <8 x i16> @llvm.loongarch.lsx.vhsubw.h.b(<16 x i8> <i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8, i8 9, i8 10, i8 11, i8 12, i8 13, i8 14, i8 15>, <16 x i8> <i8 16, i8 17, i8 18, i8 19, i8 20, i8 21, i8 22, i8 23, i8 24, i8 25, i8 26, i8 27, i8 28, i8 29, i8 30, i8 31>)
 ret <8 x i16> %1
}

define <8 x i16> @tgt() {
 ret <8 x i16> <i16 -15, i16 -15, i16 -15, i16 -15, i16 -15, i16 -15, i16 -15, i16 -15>
}

declare <8 x i16> @llvm.loongarch.lsx.vhsubw.h.b(<16 x i8>, <16 x i8>)

