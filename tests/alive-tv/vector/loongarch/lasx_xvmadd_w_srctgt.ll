define <8 x i32> @src() {
 %1 = call <8 x i32> @llvm.loongarch.lasx.xvmadd.w(<8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, <8 x i32> <i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>, <8 x i32> <i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23>)
 ret <8 x i32> %1
}

define <8 x i32> @tgt() {
 ret <8 x i32> <i32 128, i32 154, i32 182, i32 212, i32 244, i32 278, i32 314, i32 352>
}

declare <8 x i32> @llvm.loongarch.lasx.xvmadd.w(<8 x i32>, <8 x i32>, <8 x i32>)

