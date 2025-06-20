define <4 x i32> @src() {
 %1 = call <4 x i32> @llvm.loongarch.lsx.vaddwod.w.hu.h(<8 x i16> <i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7>, <8 x i16> <i16 8, i16 9, i16 10, i16 11, i16 12, i16 13, i16 14, i16 15>)
 ret <4 x i32> %1
}

define <4 x i32> @tgt() {
 ret <4 x i32> <i32 10, i32 14, i32 18, i32 22>
}

declare <4 x i32> @llvm.loongarch.lsx.vaddwod.w.hu.h(<8 x i16>, <8 x i16>)

