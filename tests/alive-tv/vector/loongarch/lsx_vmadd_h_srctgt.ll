define <8 x i16> @src() {
 %1 = call <8 x i16> @llvm.loongarch.lsx.vmadd.h(<8 x i16> <i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7>, <8 x i16> <i16 8, i16 9, i16 10, i16 11, i16 12, i16 13, i16 14, i16 15>, <8 x i16> <i16 16, i16 17, i16 18, i16 19, i16 20, i16 21, i16 22, i16 23>)
 ret <8 x i16> %1
}

define <8 x i16> @tgt() {
 ret <8 x i16> <i16 128, i16 154, i16 182, i16 212, i16 244, i16 278, i16 314, i16 352>
}

declare <8 x i16> @llvm.loongarch.lsx.vmadd.h(<8 x i16>, <8 x i16>, <8 x i16>)

