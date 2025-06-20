

def fmt_v(op, simd):
    ty_size = simd/len(op)
    lane_count = simd/ty_size
    val = ", ".join(map(lambda x: "i%d %d" % (ty_size, x), op))
    return "<" + val + ">"


def teropw(name, fn, suffix, ret_el_size, simd):

    simd_str = "lsx.v" if simd == 128 else "lasx.xv"
    for (s, es) in zip(suffix, ret_el_size):
        filename = ("%s%s.%s.srctgt" % (simd_str, name, s)).replace(".", "_") + ".ll"
        cnt = int(simd / es)
        op0, op0_ty = range(cnt), "<%d x i%d>" % (cnt, int(simd/cnt))
        op1, op1_ty = range(cnt, 3*cnt), "<%d x i%d>" % (2 * cnt, int(simd/cnt/2))
        op2, op2_ty = range(3*cnt, 5*cnt), "<%d x i%d>" % (2 * cnt, int(simd/cnt/2)) 

        filepath = "tests/alive-tv/vector/loongarch/%s" % filename

        with open(filepath, "w+") as f:
            f.write("define %s @src() {\n" %(op0_ty))
            f.write(" %%1 = call %s @llvm.loongarch.%s%s.%s(%s %s, %s %s, %s %s)\n" %(op0_ty, simd_str, name, s, op0_ty, fmt_v(op0, simd), op1_ty, fmt_v(op1, simd), op2_ty, fmt_v(op2, simd)))
            f.write(" ret %s %%1\n" % (op0_ty))
            f.write("}\n")
            f.write("\n")

            ret_ty = op0_ty
            ret = fn(op0, op1, op2)
            f.write("define %s @tgt() {\n" % ret_ty)
            f.write(" ret %s %s\n" % (ret_ty, fmt_v(ret, simd)))
            f.write("}\n")
            f.write("\n")

            f.write("declare %s @llvm.loongarch.%s%s.%s(%s, %s, %s)\n" % (ret_ty, simd_str, name, s, op0_ty, op1_ty, op2_ty))
            f.write("\n")


def terop(name, fn, suffix, ret_el_size, simd):

    simd_str = "lsx.v" if simd == 128 else "lasx.xv"
    for (s, es) in zip(suffix, ret_el_size):
        filename = ("%s%s.%s.srctgt" % (simd_str, name, s)).replace(".", "_") + ".ll"
        cnt = int(simd / es)
        op0, op0_ty = range(cnt), "<%d x i%d>" % (cnt, int(simd/cnt))
        op1, op1_ty = range(cnt, 2*cnt), "<%d x i%d>" % (cnt, int(simd/cnt))
        op2, op2_ty = range(2*cnt, 3*cnt), "<%d x i%d>" % (cnt, int(simd/cnt)) 

        filepath = "tests/alive-tv/vector/loongarch/%s" % filename

        with open(filepath, "w+") as f:
            f.write("define %s @src() {\n" %(op0_ty))
            f.write(" %%1 = call %s @llvm.loongarch.%s%s.%s(%s %s, %s %s, %s %s)\n" %(op0_ty, simd_str, name, s, op0_ty, fmt_v(op0, simd), op1_ty, fmt_v(op1, simd), op2_ty, fmt_v(op2, simd)))
            f.write(" ret %s %%1\n" % (op0_ty))
            f.write("}\n")
            f.write("\n")

            ret_ty = op0_ty
            ret = fn(op0, op1, op2)
            f.write("define %s @tgt() {\n" % ret_ty)
            f.write(" ret %s %s\n" % (ret_ty, fmt_v(ret, simd)))
            f.write("}\n")
            f.write("\n")

            f.write("declare %s @llvm.loongarch.%s%s.%s(%s, %s, %s)\n" % (ret_ty, simd_str, name, s, op0_ty, op1_ty, op2_ty))
            f.write("\n")

def vmaddw_ev_od(simd):
    suffix = ["h.bu.b", "w.hu.h", "d.wu.w"]
    ret_el_size = [16, 32, 64]


    suffix = ["h.b", "w.h", "d.w"]
    fn = lambda x,y,z: [x[i] + y[2*i] * z[2*i] for i in range(len(x))]
    teropw("maddwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y,z: [x[i] + y[2*i+1] * z[2*i+1] for i in range(len(x))]
    teropw("maddwod", fn, suffix, ret_el_size, simd)

    suffix = ["h.bu", "w.hu", "d.wu"]
    fn = lambda x,y,z: [x[i] + y[2*i] * z[2*i] for i in range(len(x))]
    teropw("maddwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y,z: [x[i] + y[2*i+1] * z[2*i+1] for i in range(len(x))]
    teropw("maddwod", fn, suffix, ret_el_size, simd)

    suffix = ["h.bu.b", "w.hu.h", "d.wu.w"]
    fn = lambda x,y,z: [x[i] + y[2*i] * z[2*i] for i in range(len(x))]
    teropw("maddwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y,z: [x[i] + y[2*i+1] * z[2*i+1] for i in range(len(x))]
    teropw("maddwod", fn, suffix, ret_el_size, simd)


def vmadd_sub(simd):

    suffix = ["b", "h", "w", "d"]
    ret_el_size = [8, 16, 32, 64]

    fn = lambda x,y,z: [x[i] + y[i] * z[i] for i in range(len(x))]
    terop("madd", fn, suffix, ret_el_size, simd)


    fn = lambda x,y,z: [x[i] - y[i] * z[i] for i in range(len(x))]
    terop("msub", fn, suffix, ret_el_size, simd)


def binopw(name, fn, suffix, ret_el_size, simd):
    simd_str = "lsx.v" if simd == 128 else "lasx.xv"
    for (s, es) in zip(suffix, ret_el_size):
        filename = ("%s%s.%s.srctgt" % (simd_str, name, s)).replace(".", "_") + ".ll"
        cnt = int(simd / es)
        op0, op0_ty = range(cnt), "<%d x i%d>" % (cnt, int(simd/cnt))
        op1, op1_ty = range(cnt, 2*cnt), "<%d x i%d>" % (cnt, int(simd/cnt))
        ret_ty = "<%d x i%d>" % (int(cnt/2), int(simd/cnt*2))

        filepath = "tests/alive-tv/vector/loongarch/%s" % filename

        with open(filepath, "w+") as f:
            f.write("define %s @src() {\n" %(ret_ty))
            f.write(" %%1 = call %s @llvm.loongarch.%s%s.%s(%s %s, %s %s)\n" %(ret_ty, simd_str, name, s, op0_ty, fmt_v(op0, simd), op1_ty, fmt_v(op1, simd)))
            f.write(" ret %s %%1\n" % (ret_ty))
            f.write("}\n")
            f.write("\n")

            ret = fn(op0, op1)
            f.write("define %s @tgt() {\n" % ret_ty)
            f.write(" ret %s %s\n" % (ret_ty, fmt_v(ret, simd)))
            f.write("}\n")
            f.write("\n")

            f.write("declare %s @llvm.loongarch.%s%s.%s(%s, %s)\n" % (ret_ty, simd_str, name, s, op0_ty, op1_ty))
            f.write("\n")


def binop(name, fn, suffix, ret_el_size, simd):
    pass

def haddw_subw(simd):
    suffix = ["h.b", "w.h", "d.w"]
    ret_el_size = [8, 16, 32]

    fn = lambda x,y: [x[2 * i + 1] + y[2*i] for i in range(int(len(x)/2))]
    binopw("haddw", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2 * i + 1] - y[2*i] for i in range(int(len(x)/2))]
    binopw("hsubw", fn, suffix, ret_el_size, simd)

    suffix = ["hu.bu", "wu.hu", "du.wu"]
    fn = lambda x,y: [x[2 * i + 1] + y[2*i] for i in range(int(len(x)/2))]
    binopw("haddw", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2 * i + 1] - y[2*i] for i in range(int(len(x)/2))]
    binopw("hsubw", fn, suffix, ret_el_size, simd)


def addw_subw_ev_od(simd):
    suffix = ["h.b", "w.h", "d.w"]
    ret_el_size = [8, 16, 32]

    fn = lambda x,y: [x[2*i] + y[2*i] for i in range(int(len(x)/2))]
    binopw("addwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i+1] + y[2*i+1] for i in range(int(len(x)/2))]
    binopw("addwod", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i] - y[2*i] for i in range(int(len(x)/2))]
    binopw("subwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i+1] - y[2*i+1] for i in range(int(len(x)/2))]
    binopw("subwod", fn, suffix, ret_el_size, simd)


    suffix = ["h.bu", "w.hu", "d.wu"]
    fn = lambda x,y: [x[2*i] + y[2*i] for i in range(int(len(x)/2))]
    binopw("addwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i+1] + y[2*i+1] for i in range(int(len(x)/2))]
    binopw("addwod", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i] - y[2*i] for i in range(int(len(x)/2))]
    binopw("subwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i+1] - y[2*i+1] for i in range(int(len(x)/2))]
    binopw("subwod", fn, suffix, ret_el_size, simd)


    suffix = ["h.bu.b", "w.hu.h", "d.wu.w"]
    fn = lambda x,y: [x[2*i] + y[2*i] for i in range(int(len(x)/2))]
    binopw("addwev", fn, suffix, ret_el_size, simd)
    fn = lambda x,y: [x[2*i+1] + y[2*i+1] for i in range(int(len(x)/2))]
    binopw("addwod", fn, suffix, ret_el_size, simd)

if __name__ == '__main__':
    # vmaddw_ev_od(128)
    # vmaddw_ev_od(256)

    # vmadd_sub(128)
    # vmadd_sub(256)

    # haddw_subw(128)
    # haddw_subw(256)

    addw_subw_ev_od(128)
    addw_subw_ev_od(256)