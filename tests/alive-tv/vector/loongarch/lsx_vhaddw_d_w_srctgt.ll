define <2 x i64> @src() {
 %1 = call <2 x i64> @llvm.loongarch.lsx.vhaddw.d.w(<4 x i32> <i32 0, i32 1, i32 2, i32 3>, <4 x i32> <i32 4, i32 5, i32 6, i32 7>)
 ret <2 x i64> %1
}

define <2 x i64> @tgt() {
 ret <2 x i64> <i64 5, i64 9>
}

declare <2 x i64> @llvm.loongarch.lsx.vhaddw.d.w(<4 x i32>, <4 x i32>)

