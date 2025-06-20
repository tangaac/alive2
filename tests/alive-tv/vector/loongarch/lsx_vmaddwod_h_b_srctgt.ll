define <8 x i16> @src() {
 %1 = call <8 x i16> @llvm.loongarch.lsx.vmaddwod.h.b(<8 x i16> <i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7>, <16 x i8> <i8 8, i8 9, i8 10, i8 11, i8 12, i8 13, i8 14, i8 15, i8 16, i8 17, i8 18, i8 19, i8 20, i8 21, i8 22, i8 23>, <16 x i8> <i8 24, i8 25, i8 26, i8 27, i8 28, i8 29, i8 30, i8 31, i8 32, i8 33, i8 34, i8 35, i8 36, i8 37, i8 38, i8 39>)
 ret <8 x i16> %1
}

define <8 x i16> @tgt() {
 ret <8 x i16> <i16 225, i16 298, i16 379, i16 468, i16 565, i16 670, i16 783, i16 904>
}

declare <8 x i16> @llvm.loongarch.lsx.vmaddwod.h.b(<8 x i16>, <16 x i8>, <16 x i8>)

