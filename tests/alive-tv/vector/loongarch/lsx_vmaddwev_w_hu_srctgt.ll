define <4 x i32> @src() {
 %1 = call <4 x i32> @llvm.loongarch.lsx.vmaddwev.w.hu(<4 x i32> <i32 0, i32 1, i32 2, i32 3>, <8 x i16> <i16 4, i16 5, i16 6, i16 7, i16 8, i16 9, i16 10, i16 11>, <8 x i16> <i16 12, i16 13, i16 14, i16 15, i16 16, i16 17, i16 18, i16 19>)
 ret <4 x i32> %1
}

define <4 x i32> @tgt() {
 ret <4 x i32> <i32 48, i32 85, i32 130, i32 183>
}

declare <4 x i32> @llvm.loongarch.lsx.vmaddwev.w.hu(<4 x i32>, <8 x i16>, <8 x i16>)

