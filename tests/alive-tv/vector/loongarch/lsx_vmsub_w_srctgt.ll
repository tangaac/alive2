define <4 x i32> @src() {
 %1 = call <4 x i32> @llvm.loongarch.lsx.vmsub.w(<4 x i32> <i32 0, i32 1, i32 2, i32 3>, <4 x i32> <i32 4, i32 5, i32 6, i32 7>, <4 x i32> <i32 8, i32 9, i32 10, i32 11>)
 ret <4 x i32> %1
}

define <4 x i32> @tgt() {
 ret <4 x i32> <i32 -32, i32 -44, i32 -58, i32 -74>
}

declare <4 x i32> @llvm.loongarch.lsx.vmsub.w(<4 x i32>, <4 x i32>, <4 x i32>)

