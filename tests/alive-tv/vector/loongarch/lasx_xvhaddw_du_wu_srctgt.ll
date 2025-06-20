define <4 x i64> @src() {
 %1 = call <4 x i64> @llvm.loongarch.lasx.xvhaddw.du.wu(<8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, <8 x i32> <i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>)
 ret <4 x i64> %1
}

define <4 x i64> @tgt() {
 ret <4 x i64> <i64 9, i64 13, i64 17, i64 21>
}

declare <4 x i64> @llvm.loongarch.lasx.xvhaddw.du.wu(<8 x i32>, <8 x i32>)

