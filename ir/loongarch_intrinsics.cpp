#include "ir/loongarch_intrinsics.h"
#include "ir/state_value.h"

using namespace smt;
using namespace std;

// the shape of a vector is stored as <# of lanes, element bits>
static constexpr std::pair<uint8_t, uint8_t> binop_shape_op0[] = {
#define PROCESS(NAME, A, B, C, D, E, F) std::make_pair(C, D),
#include "loongarch_intrinsics_binop.inc"
#undef PROCESS
};

static constexpr std::pair<uint8_t, uint8_t> binop_shape_op1[] = {
#define PROCESS(NAME, A, B, C, D, E, F) std::make_pair(E, F),
#include "loongarch_intrinsics_binop.inc"
#undef PROCESS
};

static constexpr std::pair<uint8_t, uint8_t> binop_shape_ret[] = {
#define PROCESS(NAME, A, B, C, D, E, F) std::make_pair(A, B),
#include "loongarch_intrinsics_binop.inc"
#undef PROCESS
};

namespace IR {

vector<Value *> LoongArchIntrinBinOp::operands() const {
  return {a, b};
}

bool LoongArchIntrinBinOp::propagatesPoison() const {
  return true;
}

bool LoongArchIntrinBinOp::hasSideEffects() const {
  return false;
}

void LoongArchIntrinBinOp::rauw(const Value &what, Value &with) {
  RAUW(a);
  RAUW(b);
}

void LoongArchIntrinBinOp::print(ostream &os) const {
  const char *name;
  switch (op) {
#define PROCESS(NAME, A, B, C, D, E, F)                                        \
  case NAME:                                                                   \
    name = #NAME;                                                              \
    break;
#include "loongarch_intrinsics_binop.inc"
#undef PROCESS
  }
  os << getName() << " = " << name << ' ' << *a << ", " << *b;
}

StateValue LoongArchIntrinBinOp::toSMT(State &s) const {
  auto rty = getType().getAsAggregateType();
  auto aty = a->getType().getAsAggregateType();
  auto bty = b->getType().getAsAggregateType();
  auto &av = s[*a];
  auto &bv = s[*b];

  switch (op) {
  case loongarch_lsx_vsrl_b:
  case loongarch_lsx_vsrl_h:
  case loongarch_lsx_vsrl_w:
  case loongarch_lsx_vsrl_d:
  case loongarch_lasx_xvsrl_b:
  case loongarch_lasx_xvsrl_h:
  case loongarch_lasx_xvsrl_w:
  case loongarch_lasx_xvsrl_d: {
    unsigned elem_bw = bty->getChild(0).bits();

    expr shift_np = true;
    expr shift_v;
    for (unsigned i = 0, e = 64 / elem_bw; i != e; ++i) {
      StateValue vv = bty->extract(bv, i);
      shift_v = i == 0 ? std::move(vv.value) : vv.value.concat(shift_v);
      shift_np &= vv.non_poison;
    }
    function<expr(const expr &, const expr &)> fn;

    fn = [&](auto a, auto b) -> expr {
      return expr::mkIf(shift_v.uge(elem_bw), expr::mkUInt(0, a), a.lshr(b));
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = aty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, shift_v.trunc(elem_bw)),
                        shift_np && ai.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vsll_b:
  case loongarch_lsx_vsll_h:
  case loongarch_lsx_vsll_w:
  case loongarch_lsx_vsll_d:
  case loongarch_lasx_xvsll_b:
  case loongarch_lasx_xvsll_h:
  case loongarch_lasx_xvsll_w:
  case loongarch_lasx_xvsll_d: {
    unsigned elem_bw = bty->getChild(0).bits();

    expr shift_np = true;
    expr shift_v;
    for (unsigned i = 0, e = 64 / elem_bw; i != e; ++i) {
      StateValue vv = bty->extract(bv, i);
      shift_v = i == 0 ? std::move(vv.value) : vv.value.concat(shift_v);
      shift_np &= vv.non_poison;
    }
    function<expr(const expr &, const expr &)> fn;
    fn = [&](auto a, auto b) -> expr {
      return expr::mkIf(shift_v.uge(elem_bw), expr::mkUInt(0, a), a << b);
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = aty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, shift_v.trunc(elem_bw)),
                        shift_np && ai.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vsra_b:
  case loongarch_lsx_vsra_h:
  case loongarch_lsx_vsra_w:
  case loongarch_lsx_vsra_d:
  case loongarch_lasx_xvsra_b:
  case loongarch_lasx_xvsra_h:
  case loongarch_lasx_xvsra_w:
  case loongarch_lasx_xvsra_d: {
    unsigned elem_bw = bty->getChild(0).bits();

    expr shift_np = true;
    expr shift_v;
    for (unsigned i = 0, e = 64 / elem_bw; i != e; ++i) {
      StateValue vv = bty->extract(bv, i);
      shift_v = i == 0 ? std::move(vv.value) : vv.value.concat(shift_v);
      shift_np &= vv.non_poison;
    }
    function<expr(const expr &, const expr &)> fn;
    fn = [&](auto a, auto b) -> expr {
      return expr::mkIf(
          shift_v.uge(elem_bw),
          expr::mkIf(a.isNegative(), expr::mkInt(-1, a), expr::mkUInt(0, a)),
          a.ashr(b));
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = aty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, shift_v.trunc(elem_bw)),
                        shift_np && ai.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vsrli_b:
  case loongarch_lsx_vsrli_h:
  case loongarch_lsx_vsrli_w:
  case loongarch_lsx_vsrli_d:
  case loongarch_lasx_xvsrli_b:
  case loongarch_lasx_xvsrli_h:
  case loongarch_lasx_xvsrli_w:
  case loongarch_lasx_xvsrli_d: {
    expr (*fn)(const expr &, const expr &);
    fn = [](auto a, auto b) {
      unsigned sz_a = a.bits();
      expr check = b.uge(sz_a);
      expr outbounds = expr::mkUInt(0, a);
      expr inbounds = a.lshr(b.zextOrTrunc(sz_a));
      return expr::mkIf(b.uge(sz_a), outbounds, inbounds);
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, bv.value), ai.non_poison && bv.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vslli_b:
  case loongarch_lsx_vslli_h:
  case loongarch_lsx_vslli_w:
  case loongarch_lsx_vslli_d:
  case loongarch_lasx_xvslli_b:
  case loongarch_lasx_xvslli_h:
  case loongarch_lasx_xvslli_w:
  case loongarch_lasx_xvslli_d: {
    expr (*fn)(const expr &, const expr &);
    fn = [](auto a, auto b) {
      unsigned sz_a = a.bits();
      expr outbounds = expr::mkUInt(0, a);
      expr inbounds = a << b.zextOrTrunc(sz_a);
      return expr::mkIf(b.uge(sz_a), outbounds, inbounds);
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, bv.value), ai.non_poison && bv.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vsrai_b:
  case loongarch_lsx_vsrai_h:
  case loongarch_lsx_vsrai_w:
  case loongarch_lsx_vsrai_d:
  case loongarch_lasx_xvsrai_b:
  case loongarch_lasx_xvsrai_h:
  case loongarch_lasx_xvsrai_w:
  case loongarch_lasx_xvsrai_d: {
    expr (*fn)(const expr &, const expr &);

    fn = [](auto a, auto b) {
      unsigned sz_a = a.bits();
      expr outbounds =
          expr::mkIf(a.isNegative(), expr::mkInt(-1, a), expr::mkUInt(0, a));
      expr inbounds = a.ashr(b.zextOrTrunc(sz_a));
      return expr::mkIf(b.uge(sz_a), outbounds, inbounds);
    };
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; ++i) {
      auto ai = aty->extract(av, i);
      vals.emplace_back(fn(ai.value, bv.value), ai.non_poison && bv.non_poison);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vhaddw_h_b:
  case loongarch_lsx_vhaddw_w_h:
  case loongarch_lsx_vhaddw_d_w:
  // case loongarch_lsx_vhaddw_q_d:
  case loongarch_lasx_xvhaddw_h_b:
  case loongarch_lasx_xvhaddw_w_h:
  case loongarch_lasx_xvhaddw_d_w:
  // case loongarch_lasx_xvhaddw_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a1, p1] = aty->extract(av, 2 * i + 1);
      auto [b0, p0] = bty->extract(bv, 2 * i);
      vals.emplace_back((a1 + b0).sext(extSize), p1 && p0);
    }

    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vhsubw_h_b:
  case loongarch_lsx_vhsubw_w_h:
  case loongarch_lsx_vhsubw_d_w:
  // case loongarch_lsx_vhsubw_q_d:
  case loongarch_lasx_xvhsubw_h_b:
  case loongarch_lasx_xvhsubw_w_h:
  case loongarch_lasx_xvhsubw_d_w:
  // case loongarch_lasx_xvhsubw_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a1, p1] = aty->extract(av, 2 * i + 1);
      auto [b0, p0] = bty->extract(bv, 2 * i);
      vals.emplace_back((a1 - b0).sext(extSize), p1 && p0);
    }

    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vhaddw_hu_bu:
  case loongarch_lsx_vhaddw_wu_hu:
  case loongarch_lsx_vhaddw_du_wu:
  // case loongarch_lsx_vhaddw_qu_du:
  case loongarch_lasx_xvhaddw_hu_bu:
  case loongarch_lasx_xvhaddw_wu_hu:
  case loongarch_lasx_xvhaddw_du_wu:
  // case loongarch_lasx_xvhaddw_qu_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a1, p1] = aty->extract(av, 2 * i + 1);
      auto [b0, p0] = bty->extract(bv, 2 * i);
      vals.emplace_back((a1 + b0).zext(extSize), p1 && p0);
    }

    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vhsubw_hu_bu:
  case loongarch_lsx_vhsubw_wu_hu:
  case loongarch_lsx_vhsubw_du_wu:
  // case loongarch_lsx_vhsubw_qu_du:
  case loongarch_lasx_xvhsubw_hu_bu:
  case loongarch_lasx_xvhsubw_wu_hu:
  case loongarch_lasx_xvhsubw_du_wu:
  // case loongarch_lasx_xvhsubw_qu_du: 
  {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a1, p1] = aty->extract(av, 2 * i + 1);
      auto [b0, p0] = bty->extract(bv, 2 * i);
      vals.emplace_back((a1 - b0).zext(extSize), p1 && p0);
    }

    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vaddwev_h_b:
  case loongarch_lsx_vaddwev_w_h:
  case loongarch_lsx_vaddwev_d_w:
  // case loongarch_lsx_vaddwev_q_d:
  case loongarch_lasx_xvaddwev_h_b:
  case loongarch_lasx_xvaddwev_w_h:
  case loongarch_lasx_xvaddwev_d_w:
  // case loongarch_lasx_xvaddwev_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i);
      auto [b0, b0p] = bty->extract(bv, 2 * i);
      vals.emplace_back(a0.sext(extSize) + b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vaddwod_h_b:
  case loongarch_lsx_vaddwod_w_h:
  case loongarch_lsx_vaddwod_d_w:
  // case loongarch_lsx_vaddwod_q_d:
  case loongarch_lasx_xvaddwod_h_b:
  case loongarch_lasx_xvaddwod_w_h:
  case loongarch_lasx_xvaddwod_d_w:
  // case loongarch_lasx_xvaddwod_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i + 1);
      auto [b0, b0p] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(a0.sext(extSize) + b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vsubwev_h_b:
  case loongarch_lsx_vsubwev_w_h:
  case loongarch_lsx_vsubwev_d_w:
  // case loongarch_lsx_vsubwev_q_d:
  case loongarch_lasx_xvsubwev_h_b:
  case loongarch_lasx_xvsubwev_w_h:
  case loongarch_lasx_xvsubwev_d_w:
  // case loongarch_lasx_xvsubwev_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i);
      auto [b0, b0p] = bty->extract(bv, 2 * i);
      vals.emplace_back(a0.sext(extSize) - b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vsubwod_h_b:
  case loongarch_lsx_vsubwod_w_h:
  case loongarch_lsx_vsubwod_d_w:
  // case loongarch_lsx_vsubwod_q_d:
  case loongarch_lasx_xvsubwod_h_b:
  case loongarch_lasx_xvsubwod_w_h:
  case loongarch_lasx_xvsubwod_d_w:
  // case loongarch_lasx_xvsubwod_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i + 1);
      auto [b0, b0p] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(a0.sext(extSize) - b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vaddwev_h_bu:
  case loongarch_lsx_vaddwev_w_hu:
  case loongarch_lsx_vaddwev_d_wu:
  // case loongarch_lsx_vaddwev_q_du:
  case loongarch_lasx_xvaddwev_h_bu:
  case loongarch_lasx_xvaddwev_w_hu:
  case loongarch_lasx_xvaddwev_d_wu:
  // case loongarch_lasx_xvaddwev_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i);
      auto [b0, b0p] = bty->extract(bv, 2 * i);
      vals.emplace_back(a0.zext(extSize) + b0.zext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vaddwod_h_bu:
  case loongarch_lsx_vaddwod_w_hu:
  case loongarch_lsx_vaddwod_d_wu:
  // case loongarch_lsx_vaddwod_q_du:
  case loongarch_lasx_xvaddwod_h_bu:
  case loongarch_lasx_xvaddwod_w_hu:
  case loongarch_lasx_xvaddwod_d_wu:
  // case loongarch_lasx_xvaddwod_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i + 1);
      auto [b0, b0p] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(a0.zext(extSize) + b0.zext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vsubwev_h_bu:
  case loongarch_lsx_vsubwev_w_hu:
  case loongarch_lsx_vsubwev_d_wu:
  // case loongarch_lsx_vsubwev_q_du:
  case loongarch_lasx_xvsubwev_h_bu:
  case loongarch_lasx_xvsubwev_w_hu:
  case loongarch_lasx_xvsubwev_d_wu:
  // case loongarch_lasx_xvsubwev_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i);
      auto [b0, b0p] = bty->extract(bv, 2 * i);
      vals.emplace_back(a0.zext(extSize) - b0.zext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vsubwod_h_bu:
  case loongarch_lsx_vsubwod_w_hu:
  case loongarch_lsx_vsubwod_d_wu:
  // case loongarch_lsx_vsubwod_q_du:
  case loongarch_lasx_xvsubwod_h_bu:
  case loongarch_lasx_xvsubwod_w_hu:
  case loongarch_lasx_xvsubwod_d_wu:
  // case loongarch_lasx_xvsubwod_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i + 1);
      auto [b0, b0p] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(a0.zext(extSize) - b0.zext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vaddwev_h_bu_b:
  case loongarch_lsx_vaddwev_w_hu_h:
  case loongarch_lsx_vaddwev_d_wu_w:
  // case loongarch_lsx_vaddwev_q_du_d:
  case loongarch_lasx_xvaddwev_h_bu_b:
  case loongarch_lasx_xvaddwev_w_hu_h:
  case loongarch_lasx_xvaddwev_d_wu_w:
  // case loongarch_lasx_xvaddwev_q_du_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i);
      auto [b0, b0p] = bty->extract(bv, 2 * i);
      vals.emplace_back(a0.zext(extSize) + b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vaddwod_h_bu_b:
  case loongarch_lsx_vaddwod_w_hu_h:
  case loongarch_lsx_vaddwod_d_wu_w:
  // case loongarch_lsx_vaddwod_q_du_d:
  case loongarch_lasx_xvaddwod_h_bu_b:
  case loongarch_lasx_xvaddwod_w_hu_h:
  case loongarch_lasx_xvaddwod_d_wu_w:
  // case loongarch_lasx_xvaddwod_q_du_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    for (unsigned i = 0; i < laneCount; i++) {
      auto [a0, a0p] = aty->extract(av, 2 * i + 1);
      auto [b0, b0p] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(a0.zext(extSize) + b0.sext(extSize), a0p && b0p);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmul_b:
  case loongarch_lsx_vmul_h:
  case loongarch_lsx_vmul_w:
  case loongarch_lsx_vmul_d:
  case loongarch_lasx_xvmul_b:
  case loongarch_lasx_xvmul_h:
  case loongarch_lasx_xvmul_w:
  case loongarch_lasx_xvmul_d: {
    unsigned extSize = binop_shape_ret[op].second;
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; i++) {
      auto [ai, aip] = aty->extract(av, i);
      auto [bi, bip] = bty->extract(bv, i);
      vals.emplace_back(
          (ai.sext(extSize) * bi.sext(extSize)).extract(extSize - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmuh_b:
  case loongarch_lsx_vmuh_h:
  case loongarch_lsx_vmuh_w:
  case loongarch_lsx_vmuh_d:
  case loongarch_lasx_xvmuh_b:
  case loongarch_lasx_xvmuh_h:
  case loongarch_lasx_xvmuh_w:
  case loongarch_lasx_xvmuh_d: {
    unsigned extSize = binop_shape_ret[op].second;
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; i++) {
      auto [ai, aip] = aty->extract(av, i);
      auto [bi, bip] = bty->extract(bv, i);
      vals.emplace_back((ai.sext(extSize) * bi.sext(extSize))
                            .extract(extSize * 2 - 1, extSize),
                        aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmuh_bu:
  case loongarch_lsx_vmuh_hu:
  case loongarch_lsx_vmuh_wu:
  case loongarch_lsx_vmuh_du:
  case loongarch_lasx_xvmuh_bu:
  case loongarch_lasx_xvmuh_hu:
  case loongarch_lasx_xvmuh_wu:
  case loongarch_lasx_xvmuh_du: {
    unsigned extSize = binop_shape_ret[op].second;
    vector<StateValue> vals;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; i++) {
      auto [ai, aip] = aty->extract(av, i);
      auto [bi, bip] = bty->extract(bv, i);
      vals.emplace_back((ai.zext(extSize) * bi.zext(extSize))
                            .extract(extSize * 2 - 1, extSize),
                        aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwev_h_b:
  case loongarch_lsx_vmulwev_w_h:
  case loongarch_lsx_vmulwev_d_w:
  // case loongarch_lsx_vmulwev_q_d:
  case loongarch_lasx_xvmulwev_h_b:
  case loongarch_lasx_xvmulwev_w_h:
  case loongarch_lasx_xvmulwev_d_w:
  // case loongarch_lasx_xvmulwev_q_d:
   {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i);
      auto [bi, bip] = bty->extract(bv, 2 * i);
      vals.emplace_back(
          (ai.sext(extSize) * bi.sext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwod_h_b:
  case loongarch_lsx_vmulwod_w_h:
  case loongarch_lsx_vmulwod_d_w:
  // case loongarch_lsx_vmulwod_q_d:
  case loongarch_lasx_xvmulwod_h_b:
  case loongarch_lasx_xvmulwod_w_h:
  case loongarch_lasx_xvmulwod_d_w:
  // case loongarch_lasx_xvmulwod_q_d:
   {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i + 1);
      auto [bi, bip] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(
          (ai.sext(extSize) * bi.sext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwev_h_bu:
  case loongarch_lsx_vmulwev_w_hu:
  case loongarch_lsx_vmulwev_d_wu:
  // case loongarch_lsx_vmulwev_q_du:
  case loongarch_lasx_xvmulwev_h_bu:
  case loongarch_lasx_xvmulwev_w_hu:
  case loongarch_lasx_xvmulwev_d_wu:
  // case loongarch_lasx_xvmulwev_q_du:
   {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i);
      auto [bi, bip] = bty->extract(bv, 2 * i);
      vals.emplace_back(
          (ai.zext(extSize) * bi.zext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwod_h_bu:
  case loongarch_lsx_vmulwod_w_hu:
  case loongarch_lsx_vmulwod_d_wu:
  // case loongarch_lsx_vmulwod_q_du:
  case loongarch_lasx_xvmulwod_h_bu:
  case loongarch_lasx_xvmulwod_w_hu:
  case loongarch_lasx_xvmulwod_d_wu:
  // case loongarch_lasx_xvmulwod_q_du: 
  {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i + 1);
      auto [bi, bip] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(
          (ai.zext(extSize) * bi.zext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwev_h_bu_b:
  case loongarch_lsx_vmulwev_w_hu_h:
  case loongarch_lsx_vmulwev_d_wu_w:
  // case loongarch_lsx_vmulwev_q_du_d:
  case loongarch_lasx_xvmulwev_h_bu_b:
  case loongarch_lasx_xvmulwev_w_hu_h:
  case loongarch_lasx_xvmulwev_d_wu_w:
  // case loongarch_lasx_xvmulwev_q_du_d:
   {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i);
      auto [bi, bip] = bty->extract(bv, 2 * i);
      vals.emplace_back(
          (ai.zext(extSize) * bi.sext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmulwod_h_bu_b:
  case loongarch_lsx_vmulwod_w_hu_h:
  case loongarch_lsx_vmulwod_d_wu_w:
  // case loongarch_lsx_vmulwod_q_du_d:
  case loongarch_lasx_xvmulwod_h_bu_b:
  case loongarch_lasx_xvmulwod_w_hu_h:
  case loongarch_lasx_xvmulwod_d_wu_w:
  // case loongarch_lasx_xvmulwod_q_du_d:
   {
    unsigned laneCount = binop_shape_ret[op].first;
    unsigned extSize = binop_shape_ret[op].second / 2;

    vector<StateValue> vals;
    for (unsigned i = 0; i < laneCount; i++) {
      auto [ai, aip] = aty->extract(av, 2 * i + 1);
      auto [bi, bip] = bty->extract(bv, 2 * i + 1);
      vals.emplace_back(
          (ai.zext(extSize) * bi.sext(extSize)).extract(extSize * 2 - 1, 0),
          aip && bip);
    }
    return rty->aggregateVals(vals);
  }

  default: {
  }
  }

  UNREACHABLE();
}

expr LoongArchIntrinBinOp::getTypeConstraints(const Function &f) const {
  return Value::getTypeConstraints() &&
         (binop_shape_op0[op].first != 1
              ? a->getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(binop_shape_op0[op].second);
                }) &&
                    a->getType().getAsAggregateType()->numElements() ==
                        binop_shape_op0[op].first
              : a->getType().enforceIntType(binop_shape_op0[op].second)) &&
         (binop_shape_op1[op].first != 1
              ? b->getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(binop_shape_op1[op].second);
                }) &&
                    b->getType().getAsAggregateType()->numElements() ==
                        binop_shape_op1[op].first
              : b->getType().enforceIntType(binop_shape_op1[op].second)) &&
         (binop_shape_ret[op].first != 1
              ? getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(binop_shape_ret[op].second);
                }) &&
                    getType().getAsAggregateType()->numElements() ==
                        binop_shape_ret[op].first
              : getType().enforceIntType(binop_shape_ret[op].second));
}

unique_ptr<Instr> LoongArchIntrinBinOp::dup(Function &f,
                                            const string &suffix) const {
  return make_unique<LoongArchIntrinBinOp>(getType(), getName() + suffix, *a,
                                           *b, op);
}

// the shape of a vector is stored as <# of lanes, element bits>
static constexpr std::pair<uint8_t, uint8_t> terop_shape_op0[] = {
#define PROCESS(NAME, A, B, C, D, E, F, G, H) std::make_pair(C, D),
#include "loongarch_intrinsics_terop.inc"
#undef PROCESS
};

static constexpr std::pair<uint8_t, uint8_t> terop_shape_op1[] = {
#define PROCESS(NAME, A, B, C, D, E, F, G, H) std::make_pair(E, F),
#include "loongarch_intrinsics_terop.inc"
#undef PROCESS
};

static constexpr std::pair<uint8_t, uint8_t> terop_shape_op2[] = {
#define PROCESS(NAME, A, B, C, D, E, F, G, H) std::make_pair(G, H),
#include "loongarch_intrinsics_terop.inc"
#undef PROCESS
};

static constexpr std::pair<uint8_t, uint8_t> terop_shape_ret[] = {
#define PROCESS(NAME, A, B, C, D, E, F, G, H) std::make_pair(A, B),
#include "loongarch_intrinsics_terop.inc"
#undef PROCESS
};

void LoongArchIntrinTerOp::print(ostream &os) const {
  const char *name;
  switch (op) {
#define PROCESS(NAME, A, B, C, D, E, F, G, H)                                  \
  case NAME:                                                                   \
    name = #NAME;                                                              \
    break;
#include "loongarch_intrinsics_terop.inc"
#undef PROCESS
  }
  os << getName() << " = " << name << ' ' << *a << ", " << *b << ", " << *c;
}

StateValue LoongArchIntrinTerOp::toSMT(State &s) const {
  auto rty = getType().getAsAggregateType();
  auto aty = a->getType().getAsAggregateType();
  auto bty = b->getType().getAsAggregateType();
  auto cty = c->getType().getAsAggregateType();
  auto &av = s[*a];
  auto &bv = s[*b];
  auto &cv = s[*c];

  switch (op) {
  case loongarch_lsx_vmadd_b:
  case loongarch_lsx_vmadd_h:
  case loongarch_lsx_vmadd_w:
  case loongarch_lsx_vmadd_d:
  case loongarch_lasx_xvmadd_b:
  case loongarch_lasx_xvmadd_h:
  case loongarch_lasx_xvmadd_w:
  case loongarch_lasx_xvmadd_d: {
    vector<StateValue> vals;
    unsigned retTySize = terop_shape_ret[op].second;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, i);
      auto [c, cp] = cty->extract(cv, i);
      vals.emplace_back(a + (b * c).extract(retTySize - 1, 0), ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vmsub_b:
  case loongarch_lsx_vmsub_h:
  case loongarch_lsx_vmsub_w:
  case loongarch_lsx_vmsub_d:
  case loongarch_lasx_xvmsub_b:
  case loongarch_lasx_xvmsub_h:
  case loongarch_lasx_xvmsub_w:
  case loongarch_lasx_xvmsub_d: {
    vector<StateValue> vals;
    unsigned retTySize = terop_shape_ret[op].second;
    for (unsigned i = 0, e = rty->numElementsConst(); i != e; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, i);
      auto [c, cp] = cty->extract(cv, i);
      vals.emplace_back(a - (b * c).extract(retTySize - 1, 0), ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }
  case loongarch_lsx_vmaddwev_h_b:
  case loongarch_lsx_vmaddwev_w_h:
  case loongarch_lsx_vmaddwev_d_w:
  // case loongarch_lsx_vmaddwev_q_d:
  case loongarch_lasx_xvmaddwev_h_b:
  case loongarch_lasx_xvmaddwev_w_h:
  case loongarch_lasx_xvmaddwev_d_w:
  // case loongarch_lasx_xvmaddwev_q_d: 
  {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i);
      auto [c, cp] = cty->extract(cv, 2 * i);
      vals.emplace_back(
          a + ((b.sext(extSize) * c.sext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmaddwod_h_b:
  case loongarch_lsx_vmaddwod_w_h:
  case loongarch_lsx_vmaddwod_d_w:
  // case loongarch_lsx_vmaddwod_q_d:
  case loongarch_lasx_xvmaddwod_h_b:
  case loongarch_lasx_xvmaddwod_w_h:
  case loongarch_lasx_xvmaddwod_d_w:
  // case loongarch_lasx_xvmaddwod_q_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i + 1);
      auto [c, cp] = cty->extract(cv, 2 * i + 1);
      vals.emplace_back(
          a + ((b.sext(extSize) * c.sext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmaddwev_h_bu:
  case loongarch_lsx_vmaddwev_w_hu:
  case loongarch_lsx_vmaddwev_d_wu:
  // case loongarch_lsx_vmaddwev_q_du:
  case loongarch_lasx_xvmaddwev_h_bu:
  case loongarch_lasx_xvmaddwev_w_hu:
  case loongarch_lasx_xvmaddwev_d_wu:
  // case loongarch_lasx_xvmaddwev_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i);
      auto [c, cp] = cty->extract(cv, 2 * i);
      vals.emplace_back(
          a + ((b.zext(extSize) * c.zext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmaddwod_h_bu:
  case loongarch_lsx_vmaddwod_w_hu:
  case loongarch_lsx_vmaddwod_d_wu:
  // case loongarch_lsx_vmaddwod_q_du:
  case loongarch_lasx_xvmaddwod_h_bu:
  case loongarch_lasx_xvmaddwod_w_hu:
  case loongarch_lasx_xvmaddwod_d_wu:
  // case loongarch_lasx_xvmaddwod_q_du:
   {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i + 1);
      auto [c, cp] = cty->extract(cv, 2 * i + 1);
      vals.emplace_back(
          a + ((b.zext(extSize) * c.zext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmaddwev_h_bu_b:
  case loongarch_lsx_vmaddwev_w_hu_h:
  case loongarch_lsx_vmaddwev_d_wu_w:
  // case loongarch_lsx_vmaddwev_q_du_d:
  case loongarch_lasx_xvmaddwev_h_bu_b:
  case loongarch_lasx_xvmaddwev_w_hu_h:
  case loongarch_lasx_xvmaddwev_d_wu_w:
  // case loongarch_lasx_xvmaddwev_q_du_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i);
      auto [c, cp] = cty->extract(cv, 2 * i);
      vals.emplace_back(
          a + ((b.zext(extSize) * c.sext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  case loongarch_lsx_vmaddwod_h_bu_b:
  case loongarch_lsx_vmaddwod_w_hu_h:
  case loongarch_lsx_vmaddwod_d_wu_w:
  // case loongarch_lsx_vmaddwod_q_du_d:
  case loongarch_lasx_xvmaddwod_h_bu_b:
  case loongarch_lasx_xvmaddwod_w_hu_h:
  case loongarch_lasx_xvmaddwod_d_wu_w:
  // case loongarch_lasx_xvmaddwod_q_du_d:
   {
    vector<StateValue> vals;
    unsigned laneCount = terop_shape_ret[op].first;
    unsigned retTySize = terop_shape_ret[op].second;
    unsigned extSize = retTySize / 2;

    for (int i = 0; i < laneCount; i++) {
      auto [a, ap] = aty->extract(av, i);
      auto [b, bp] = bty->extract(bv, 2 * i + 1);
      auto [c, cp] = cty->extract(cv, 2 * i + 1);
      vals.emplace_back(
          a + ((b.zext(extSize) * c.sext(extSize)).extract(retTySize - 1, 0)),
          ap && bp && cp);
    }
    return rty->aggregateVals(vals);
  }

  default: {
  }
  }
  UNREACHABLE();
}

expr LoongArchIntrinTerOp::getTypeConstraints(const Function &f) const {
  return Value::getTypeConstraints() &&
         (terop_shape_op0[op].first != 1
              ? a->getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(terop_shape_op0[op].second);
                }) &&
                    a->getType().getAsAggregateType()->numElements() ==
                        terop_shape_op0[op].first
              : a->getType().enforceIntType(terop_shape_op0[op].second)) &&
         (terop_shape_op1[op].first != 1
              ? b->getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(terop_shape_op1[op].second);
                }) &&
                    b->getType().getAsAggregateType()->numElements() ==
                        terop_shape_op1[op].first
              : b->getType().enforceIntType(terop_shape_op1[op].second)) &&
         (terop_shape_op2[op].first != 1
              ? b->getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(terop_shape_op2[op].second);
                }) &&
                    b->getType().getAsAggregateType()->numElements() ==
                        terop_shape_op2[op].first
              : b->getType().enforceIntType(terop_shape_op2[op].second)) &&
         (terop_shape_ret[op].first != 1
              ? getType().enforceVectorType([this](auto &ty) {
                  return ty.enforceIntType(terop_shape_ret[op].second);
                }) &&
                    getType().getAsAggregateType()->numElements() ==
                        terop_shape_ret[op].first
              : getType().enforceIntType(terop_shape_ret[op].second));
}

unique_ptr<Instr> LoongArchIntrinTerOp::dup(Function &f,
                                            const string &suffix) const {
  return make_unique<LoongArchIntrinTerOp>(getType(), getName() + suffix, *a,
                                           *b, *c, op);
}

vector<Value *> LoongArchIntrinTerOp::operands() const {
  return {a, b, c};
}

bool LoongArchIntrinTerOp::propagatesPoison() const {
  return true;
}

bool LoongArchIntrinTerOp::hasSideEffects() const {
  return false;
}

void LoongArchIntrinTerOp::rauw(const Value &what, Value &with) {
  RAUW(a);
  RAUW(b);
  RAUW(c);
}
} // namespace IR