
def binop(name, simd, suffix, el_size, fret, fop0, fop1):
    for (s, es) in zip(suffix, el_size):
        ret = fret(simd, es)
        op0 = fop0(simd, es)
        op1 = fop1(simd, es)
        prefix = "loongarch_lsx_v"
        if simd== 256:
            prefix = "loongarch_lasx_xv"
        print("PROCESS({}{}_{}, {}, {}, {})".format(prefix, name, s, ret, op0, op1))


def vector_shift(simd):
    suffix = ["b", "h", "w", "d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    binop("srl", simd, suffix, el_size, f, f, f)
    binop("sll", simd, suffix, el_size, f, f, f)
    binop("sra", simd, suffix, el_size, f, f, f)
    # binop("rotr", simd, suffix, el_size, f, f, f)

    def fop1(simd, es):
        return "1, 32"
    binop("srli", simd, suffix, el_size, f, f, fop1)
    binop("slli", simd, suffix, el_size, f, f, fop1)
    binop("srai", simd, suffix, el_size, f, f, fop1)
    # binop("rotri", simd, suffix, el_size, f, f, fop1)


def hadd_hsub(simd):
    suffix = ["h_b", "w_h", "d_w", "q_d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    def fret(simd, es):
        return "{}, {}".format(int(simd/es/2),es*2)

    
    # binop("haddw", simd, suffix, el_size, fret, f, f)
    # binop("hsubw", simd, suffix, el_size, fret, f, f)

    suffix = ["hu_bu", "wu_hu", "du_wu", "qu_du"]
    # binop("haddw", simd, suffix, el_size, fret, f, f)
    # binop("hsubw", simd, suffix, el_size, fret, f, f)



def add_sub_ev_od(simd):
    suffix = ["h_b", "w_h", "d_w", "q_d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    def fret(simd, es):
        return "{}, {}".format(int(simd/es/2),es*2)
    
    # binop("addwev", simd, suffix, el_size, fret, f, f)
    # binop("subwev", simd, suffix, el_size, fret, f, f)

    suffix = ["h_bu", "w_hu", "d_wu", "q_du"]
    # binop("addwev", simd, suffix, el_size, fret, f, f)
    # binop("subwev", simd, suffix, el_size, fret, f, f)


    suffix = ["h_bu_b", "w_hu_h", "d_wu_w", "q_du_d"]
    binop("addwev", simd, suffix, el_size, fret, f, f)

def mulh(simd):
    suffix = ["b", "h", "w", "d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    binop("mul", simd, suffix, el_size, f, f, f)

def mulw_ev_od(simd):
    suffix = ["h_b", "w_h", "d_w", "q_d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    def fret(simd, es):
        return "{}, {}".format(int(simd/es/2),es*2)
    # binop("mulwev", simd, suffix, el_size, fret, f, f)


    suffix = ["h_bu_b", "w_hu_h", "d_wu_w", "q_du_d"]
    binop("mulwev", simd, suffix, el_size, fret, f, f)

def madd_sub(simd):
    suffix = ["b", "h", "w", "d"]
    el_size = [8, 16, 32, 64]
    def f(simd, es):
        return "{}, {}".format(int(simd/es),es)
    binop("madd", simd, suffix, el_size, f, f, f)

    


if __name__  == "__main__":
    # vector_shift(128)
    # vector_shift(256)

    # hadd_hsub(128)
    # hadd_hsub(256)

    # add_sub_ev_od(128)
    # add_sub_ev_od(256)

    # mulh(128)
    # mulh(256)

    # mulw_ev_od(128)
    # mulw_ev_od(256)

    # madd_sub(128)
    # madd_sub(256)


