define <2 x i64> @src() {
 %1 = call <2 x i64> @llvm.loongarch.lsx.vmaddwod.d.w(<2 x i64> <i64 0, i64 1>, <4 x i32> <i32 2, i32 3, i32 4, i32 5>, <4 x i32> <i32 6, i32 7, i32 8, i32 9>)
 ret <2 x i64> %1
}

define <2 x i64> @tgt() {
 ret <2 x i64> <i64 21, i64 46>
}

declare <2 x i64> @llvm.loongarch.lsx.vmaddwod.d.w(<2 x i64>, <4 x i32>, <4 x i32>)

