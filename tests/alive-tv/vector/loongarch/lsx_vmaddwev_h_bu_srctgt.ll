define <8 x i16> @src() {
 %1 = call <8 x i16> @llvm.loongarch.lsx.vmaddwev.h.bu(<8 x i16> <i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7>, <16 x i8> <i8 8, i8 9, i8 10, i8 11, i8 12, i8 13, i8 14, i8 15, i8 16, i8 17, i8 18, i8 19, i8 20, i8 21, i8 22, i8 23>, <16 x i8> <i8 24, i8 25, i8 26, i8 27, i8 28, i8 29, i8 30, i8 31, i8 32, i8 33, i8 34, i8 35, i8 36, i8 37, i8 38, i8 39>)
 ret <8 x i16> %1
}

define <8 x i16> @tgt() {
 ret <8 x i16> <i16 192, i16 261, i16 338, i16 423, i16 516, i16 617, i16 726, i16 843>
}

declare <8 x i16> @llvm.loongarch.lsx.vmaddwev.h.bu(<8 x i16>, <16 x i8>, <16 x i8>)

