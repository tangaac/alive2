define <8 x i32> @src() {
 %1 = call <8 x i32> @llvm.loongarch.lasx.xvaddwod.w.h(<16 x i16> <i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8, i16 9, i16 10, i16 11, i16 12, i16 13, i16 14, i16 15>, <16 x i16> <i16 16, i16 17, i16 18, i16 19, i16 20, i16 21, i16 22, i16 23, i16 24, i16 25, i16 26, i16 27, i16 28, i16 29, i16 30, i16 31>)
 ret <8 x i32> %1
}

define <8 x i32> @tgt() {
 ret <8 x i32> <i32 18, i32 22, i32 26, i32 30, i32 34, i32 38, i32 42, i32 46>
}

declare <8 x i32> @llvm.loongarch.lasx.xvaddwod.w.h(<16 x i16>, <16 x i16>)

