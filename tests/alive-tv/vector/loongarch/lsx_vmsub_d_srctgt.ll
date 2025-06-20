define <2 x i64> @src() {
 %1 = call <2 x i64> @llvm.loongarch.lsx.vmsub.d(<2 x i64> <i64 0, i64 1>, <2 x i64> <i64 2, i64 3>, <2 x i64> <i64 4, i64 5>)
 ret <2 x i64> %1
}

define <2 x i64> @tgt() {
 ret <2 x i64> <i64 -8, i64 -14>
}

declare <2 x i64> @llvm.loongarch.lsx.vmsub.d(<2 x i64>, <2 x i64>, <2 x i64>)

