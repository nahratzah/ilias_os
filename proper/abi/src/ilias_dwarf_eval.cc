#include <ilias/dwarf/eval.h>
#include <ilias/dwarf/read.h>

_namespace_begin(ilias)
namespace dwarf {


template<>
class eval_instr::operand::assign<eval_instr::operand::type::uleb> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(uint64_t v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::address> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(address_type v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::delta1> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(uint8_t v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::delta2> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(uint16_t v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::delta4> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(uint32_t v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::block> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(const block_type& v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::sleb> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(int64_t v) const noexcept;

 private:
  operand& self_;
};

template<>
class eval_instr::operand::assign<eval_instr::operand::type::segment_address> {
 public:
  assign(operand& self) noexcept : self_(self) {}
  assign() noexcept = delete;
  assign(const assign&) = default;
  assign& operator=(const assign&) = delete;
  operand& operator=(const segment_address_type& v) const noexcept;

 private:
  operand& self_;
};


inline auto eval_instr::operand::assign<eval_instr::operand::type::uleb>::
    operator=(uint64_t v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.uleb) uint64_t(v);
  tmp.discriminant_ = eval_instr::operand::type::uleb;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::address>::
    operator=(address_type v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.address) address_type(v);
  tmp.discriminant_ = eval_instr::operand::type::address;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::delta1>::
    operator=(uint8_t v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.delta1) uint8_t(v);
  tmp.discriminant_ = eval_instr::operand::type::delta1;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::delta2>::
    operator=(uint16_t v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.delta2) uint16_t(v);
  tmp.discriminant_ = eval_instr::operand::type::delta2;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::delta4>::
    operator=(uint32_t v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.delta4) uint32_t(v);
  tmp.discriminant_ = eval_instr::operand::type::delta4;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::block>::
    operator=(const block_type& v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.block) block_type(v);
  tmp.discriminant_ = eval_instr::operand::type::block;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<eval_instr::operand::type::sleb>::
    operator=(int64_t v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.sleb) int64_t(v);
  tmp.discriminant_ = eval_instr::operand::type::sleb;
  return self_ = tmp;
}

inline auto eval_instr::operand::assign<
                eval_instr::operand::type::segment_address>::
    operator=(const segment_address_type& v) const noexcept -> operand& {
  operand tmp;
  new (&tmp.val_.segment_address) segment_address_type(v);
  tmp.discriminant_ = eval_instr::operand::type::segment_address;
  return self_ = tmp;
}


auto eval_instr::decode(const void __attribute__((aligned(1))) * ptr,
                        const cie::record& cie_record)
    noexcept -> _namespace(std)::tuple<const void*, eval_instr> {
  using _namespace(std)::get;
  using _namespace(std)::tie;
  using _namespace(std)::make_tuple;

  eval_instr rv;
  if (ptr) {
    tie(ptr, get<0>(rv.data_), get<1>(rv.data_),
             get<2>(rv.data_), get<3>(rv.data_)) =
        decode_(static_cast<const uint8_t*>(ptr), cie_record);
    get<4>(rv.data_) = &cie_record;
  }
  return make_tuple(ptr, rv);
}

auto eval_instr::operator()(state& s) noexcept -> bool {
  using _namespace(std)::get;
  using _namespace(std)::tie;
  using _namespace(std)::make_tuple;
  using _namespace(std)::tuple_cat;
  using type = operand::type;
  using _namespace(std)::intptr_t;

  if (!get<4>(data_)) return false;
  const cie::record& cie_record = *get<4>(data_);
  const uint8_t& oparg = get<1>(data_);
  const operand& op1 = get<2>(data_);
  const operand& op2 = get<3>(data_);
  const auto& data_alignment_factor = cie_record.data_alignment_factor;
  const auto& code_alignment_factor = cie_record.code_alignment_factor;

  bool ok;
  switch (get<0>(data_)) {
  default:
    ok = false;  // Instruction not recognized.
    break;

  case dw_cfa::advance_loc:
    s.cfa.addr += oparg * code_alignment_factor;
    ok = true;
    break;

  case dw_cfa::offset:
    s.set_reg_offsetrule(oparg,
                         op1.get_uleb() * data_alignment_factor);
    ok = true;
    break;

  case dw_cfa::restore:
    assert_msg(false, "XXX implement");
    ok = false;
    break;

  case dw_cfa::nop:
    ok = true;
    break;

  case dw_cfa::set_loc:
    if (_predict_true(op1.get_type() == type::address)) {
      tie(s.cfa.addr, s.cfa.seg, s.cfa.seg_set) =
          make_tuple(op1.get_address(), 0, false);
      ok = true;
    } else {
      tie(s.cfa.addr, s.cfa.seg, s.cfa.seg_set) =
          tuple_cat(op1.get_segment_address(), make_tuple(true));
      ok = true;
    }
    break;

  case dw_cfa::advance_loc1:
    s.cfa.addr += op1.get_delta1() * code_alignment_factor;
    ok = true;
    break;

  case dw_cfa::advance_loc2:
    s.cfa.addr += op1.get_delta2() * code_alignment_factor;
    ok = true;
    break;

  case dw_cfa::advance_loc4:
    s.cfa.addr += op1.get_delta4() * code_alignment_factor;
    ok = true;
    break;

  case dw_cfa::offset_extended:
    s.set_reg_offsetrule(op1.get_uleb(),
                         op2.get_uleb() * data_alignment_factor);
    ok = true;
    break;

  case dw_cfa::restore_extended:
    assert_msg(false, "XXX implement");
    ok = false;
    break;

  case dw_cfa::undefined:
    s.set_reg_undefined(op1.get_uleb());
    ok = true;
    break;

  case dw_cfa::same_value:
    s.set_reg_samevalue(op1.get_uleb());
    ok = true;
    break;

  case dw_cfa::register_:
  case dw_cfa::remember_state:
  case dw_cfa::restore_state:
  case dw_cfa::def_cfa:
  case dw_cfa::def_cfa_register:
  case dw_cfa::def_cfa_offset:
  case dw_cfa::def_cfa_expression:
  case dw_cfa::expression:
    assert_msg(false, "XXX implement");
    ok = false;
    break;

  case dw_cfa::offset_extended_sf:
    s.set_reg_offsetrule(op1.get_uleb(),
                         static_cast<intptr_t>(op2.get_sleb() *
                                               data_alignment_factor));
    ok = true;
    break;

  case dw_cfa::def_cfa_sf:
  case dw_cfa::def_cfa_offset_sf:
    assert_msg(false, "XXX implement");
    ok = false;
    break;

  case dw_cfa::val_offset:
    s.set_reg_valoffsetrule(op1.get_uleb(),
                            op2.get_uleb() * data_alignment_factor);
    ok = true;
    break;

  case dw_cfa::val_offset_sf:
    s.set_reg_valoffsetrule(op1.get_uleb(),
                            static_cast<intptr_t>(op2.get_sleb() *
                                                  data_alignment_factor));
    ok = true;
    break;

  case dw_cfa::val_expression:
  case dw_cfa::lo_user:
  case dw_cfa::hi_user:
    assert_msg(false, "XXX implement");
    ok = false;
    break;
  }
  return ok;
}

auto eval_instr::decode_(
    const _namespace(std)::uint8_t __attribute__((aligned(1))) * p,
    const cie::record& cie_record) noexcept ->
    _namespace(std)::tuple<const _namespace(std)::uint8_t*,
                           dw_cfa, _namespace(std)::uint8_t,
                           operand, operand> {
  using _namespace(std)::uint8_t;
  using _namespace(std)::uint16_t;
  using _namespace(std)::uint32_t;
  using _namespace(std)::get;
  using _namespace(std)::make_tuple;
  using _namespace(std)::tie;
  using type = operand::type;

  _namespace(std)::tuple<const uint8_t*, dw_cfa, uint8_t, operand, operand> rv;
  tie(get<0>(rv), get<1>(rv), get<2>(rv)) = make_tuple(p, dw_cfa::nop, 0);
  if (p == nullptr) return rv;

  tie(get<1>(rv), get<2>(rv)) = decode_opcode_(*p);
  const void __attribute__((aligned(1))) * ptr = p + 1U;

  switch (get<1>(rv)) {
  case dw_cfa::offset:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::set_loc:
    if (cie_record.segment_size == 0) {
      auto op1 = operand::assign<type::address>(get<3>(rv));
      tie(op1, ptr) = read_unaligned<address_type>(ptr);
    } else if (cie_record.segment_size == 1) {
      uint32_t seg = 0, rd;
      for (unsigned int i = 0; i < cie_record.segment_size; ++i) {
        tie(rd, ptr) = read_unaligned<uint8_t>(ptr);
#if defined(__BIG_ENDIAN__)
        seg <<= 8;
        seg |= rd;
#elif defined(__LITTLE_ENDIAN__)
        rd <<= 8 * i;
        seg |= rd;
#else
        static_assert(false, "Unknown endianness.");
#endif
      }

      address_type addr;
      tie(addr, ptr) = read_unaligned<address_type>(ptr);

      operand::assign<type::segment_address>(get<3>(rv)) = tie(seg, addr);
    }
    break;
  case dw_cfa::advance_loc1:
    {
      auto op1 = operand::assign<type::delta1>(get<3>(rv));
      tie(op1, ptr) = read_unaligned<uint8_t>(ptr);
    }
    break;
  case dw_cfa::advance_loc2:
    {
      auto op1 = operand::assign<type::delta2>(get<3>(rv));
      tie(op1, ptr) = read_unaligned<uint16_t>(ptr);
    }
    break;
  case dw_cfa::advance_loc4:
    {
      auto op1 = operand::assign<type::delta4>(get<3>(rv));
      tie(op1, ptr) = read_unaligned<uint32_t>(ptr);
    }
    break;
  case dw_cfa::offset_extended:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::uleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::restore_extended:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::undefined:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::same_value:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::register_:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::uleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::uleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa_register:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa_offset:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa_expression:
    {
      auto op1 = operand::assign<type::block>(get<3>(rv));
      tie(op1, ptr) = read_unaligned<block_type>(ptr);
    }
    break;
  case dw_cfa::expression:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::block>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_unaligned<block_type>(ptr);
    }
    break;
  case dw_cfa::offset_extended_sf:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::sleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_sleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa_sf:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::sleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_sleb128(ptr);
    }
    break;
  case dw_cfa::def_cfa_offset_sf:
    {
      auto op1 = operand::assign<type::sleb>(get<4>(rv));
      tie(op1, ptr) = read_sleb128(ptr);
    }
    break;
  case dw_cfa::val_offset:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::uleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_uleb128(ptr);
    }
    break;
  case dw_cfa::val_offset_sf:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::sleb>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_sleb128(ptr);
    }
    break;
  case dw_cfa::val_expression:
    {
      auto op1 = operand::assign<type::uleb>(get<3>(rv));
      auto op2 = operand::assign<type::block>(get<4>(rv));
      tie(op1, ptr) = read_uleb128(ptr);
      tie(op2, ptr) = read_unaligned<block_type>(ptr);
    }
    break;
  default:
    break;
  }
  get<0>(rv) = static_cast<const uint8_t*>(ptr);

  return rv;
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
