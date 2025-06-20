define <4 x i64> @src() {
 %1 = call <4 x i64> @llvm.loongarch.lasx.xvmaddwev.d.wu(<4 x i64> <i64 0, i64 1, i64 2, i64 3>, <8 x i32> <i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11>, <8 x i32> <i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19>)
 ret <4 x i64> %1
}

define <4 x i64> @tgt() {
 ret <4 x i64> <i64 48, i64 85, i64 130, i64 183>
}

declare <4 x i64> @llvm.loongarch.lasx.xvmaddwev.d.wu(<4 x i64>, <8 x i32>, <8 x i32>)

