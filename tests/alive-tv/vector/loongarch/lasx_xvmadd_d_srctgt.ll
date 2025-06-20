define <4 x i64> @src() {
 %1 = call <4 x i64> @llvm.loongarch.lasx.xvmadd.d(<4 x i64> <i64 0, i64 1, i64 2, i64 3>, <4 x i64> <i64 4, i64 5, i64 6, i64 7>, <4 x i64> <i64 8, i64 9, i64 10, i64 11>)
 ret <4 x i64> %1
}

define <4 x i64> @tgt() {
 ret <4 x i64> <i64 32, i64 46, i64 62, i64 80>
}

declare <4 x i64> @llvm.loongarch.lasx.xvmadd.d(<4 x i64>, <4 x i64>, <4 x i64>)

