#include <ilias/dwarf/dw_op.h>
#include <ilias/dwarf/read.h>
#include <cassert>
#include <stdexcept>
#include <tuple>

_namespace_begin(ilias)
namespace dwarf {


constexpr dw_op_stack::size_type dw_op_stack::max_stack_depth;

auto dw_op_stack::push(value_type v) -> void {
  using _namespace(std)::length_error;

  if (_predict_false(off_ == 0U)) {
    if (throw_)
      throw length_error("ilias::dwarf::dw_op_stack: stack exhausted");
    fail_ = true;
    return;
  }

  data_[--off_] = v;
}

auto dw_op_stack::pop() -> value_type {
  using _namespace(std)::length_error;

  if (_predict_false(off_ == data_.size())) {
    if (throw_) {
      throw length_error("ilias::dwarf::dw_op_stack: "
                         "cannot pop from empty stack");
    }
    fail_ = true;
    return 0;
  }

  return data_[off_--];
}

auto dw_op_stack::top() const -> value_type {
  using _namespace(std)::length_error;

  if (_predict_false(off_ == data_.size())) {
    if (throw_) {
      throw length_error("ilias::dwarf::dw_op_stack: "
                         "cannot pop from empty stack");
    }
    fail_ = true;
    return 0;
  }

  return data_[off_];
}

auto dw_op_stack::operator[](size_type idx) -> value_type& {
  using _namespace(std)::out_of_range;

  if (_predict_false(idx >= data_.size() - off_)) {
    if (throw_) {
      throw out_of_range("ilias::dwarf::dw_op_stack: "
                         "stack index out of range");
    }
    fail_ = true;
    return data_[0];
  }

  return data_[off_ + idx];
}

auto dw_op_stack::operator[](size_type idx) const -> const value_type& {
  using _namespace(std)::out_of_range;

  if (_predict_false(idx >= data_.size() - off_)) {
    if (throw_) {
      throw out_of_range("ilias::dwarf::dw_op_stack: "
                         "stack index out of range");
    }
    fail_ = true;
    return data_[0];
  }

  return data_[off_ + idx];
}

auto dw_op_stack::eval(const void __attribute__((aligned(1)))* begin,
                       const void __attribute__((aligned(1)))* end) ->
    void {
  while (begin != end && !terminated_ && !fail_)
    begin = eval1(begin);
}

auto dw_op_stack::eval1(const void __attribute__((aligned(1)))* ptr) ->
    const void* {
  using _namespace(std)::uint8_t;
  using _namespace(std)::uint16_t;
  using _namespace(std)::uint32_t;
  using _namespace(std)::uint64_t;
  using _namespace(std)::int8_t;
  using _namespace(std)::int16_t;
  using _namespace(std)::int32_t;
  using _namespace(std)::int64_t;
  using _namespace(std)::tie;

  opcode c;
  tie(c, ptr) = read_unaligned<opcode>(ptr);

  switch (c) {
  default:
    fail_ = true;
    break;

  case opcode::dw_op_addr:
    {
      address_type addr;
      tie(addr, ptr) = read_unaligned<address_type>(ptr);
      push(addr);
    }
    break;

  case opcode::dw_op_deref:
    {
      uintptr_t addr = pop();
      address_type val;
      tie(val, _namespace(std)::ignore) =
          read_unaligned<address_type>(reinterpret_cast<const void*>(addr));
      push(val);
    }
    break;

  case opcode::dw_op_const1u:
    {
      uint8_t val;
      tie(val, ptr) = read_unaligned<uint8_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const1s:
    {
      int8_t val;
      tie(val, ptr) = read_unaligned<int8_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const2u:
    {
      uint16_t val;
      tie(val, ptr) = read_unaligned<uint16_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const2s:
    {
      int16_t val;
      tie(val, ptr) = read_unaligned<int16_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const4u:
    {
      uint32_t val;
      tie(val, ptr) = read_unaligned<uint32_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const4s:
    {
      int32_t val;
      tie(val, ptr) = read_unaligned<int32_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const8u:
    {
      uint64_t val;
      tie(val, ptr) = read_unaligned<uint64_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_const8s:
    {
      int64_t val;
      tie(val, ptr) = read_unaligned<int64_t>(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_constu:
    {
      uint64_t val;
      tie(val, ptr) = read_uleb128(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_consts:
    {
      int64_t val;
      tie(val, ptr) = read_sleb128(ptr);
      push(val);
    }
    break;

  case opcode::dw_op_dup:
    push(top());
    break;

  case opcode::dw_op_drop:
    pop();
    break;

  case opcode::dw_op_over:
    push((*this)[1]);
    break;

  case opcode::dw_op_pick:
    {
      uint8_t arg;
      tie(arg, ptr) = read_unaligned<uint8_t>(ptr);
      push((*this)[arg]);
    }
    break;

  case opcode::dw_op_swap:
    _namespace(std)::swap((*this)[0], (*this)[1]);
    break;

  case opcode::dw_op_rot:
    if (_predict_true(size() >= 3U)) {
      _namespace(std)::rotate(data_.begin() + off_,
                              data_.begin() + off_ + 1U,
                              data_.begin() + off_ + 3U);
    }
    break;

  case opcode::dw_op_xderef:
    {
      address_type addr = pop();
      segment_type seg = pop();
      push(read_memory_at(addr, seg));
    }
    break;

  case opcode::dw_op_abs:
    {
      int64_t v = pop();
      push(v < 0U ? static_cast<uint64_t>(-(v + 1)) + 1U :
                    static_cast<uint64_t>(v));
    }
    break;

  case opcode::dw_op_and:
    push(pop() & pop());
    break;

  case opcode::dw_op_div:
    {
      int64_t top = static_cast<int64_t>(pop());
      int64_t second = static_cast<int64_t>(pop());
      push(second / top);
    }
    break;

  case opcode::dw_op_minus:
    {
      uint64_t top = pop();
      uint64_t second = pop();
      push(second - top);
    }
    break;

  case opcode::dw_op_mod:
    {
      uint64_t top = pop();
      uint64_t second = pop();
      push(second % top);
    }
    break;

  case opcode::dw_op_mul:
    push(pop() * pop());
    break;

  case opcode::dw_op_neg:
    push(-static_cast<int64_t>(pop()));
    break;

  case opcode::dw_op_not:
    push(~pop());
    break;

  case opcode::dw_op_or:
    push(pop() | pop());
    break;

  case opcode::dw_op_plus:
    push(pop() + pop());
    break;

  case opcode::dw_op_plus_uconst:
    {
      uint64_t v;
      tie(v, ptr) = read_uleb128(ptr);
      push(pop() + v);
    }
    break;

  case opcode::dw_op_shl:
    {
      uint64_t top = pop();
      uint64_t second = pop();
      push(second << top);
    }
    break;

  case opcode::dw_op_shr:
    {
      uint64_t top = pop();
      uint64_t second = pop();
      push(second >> top);
    }
    break;

  case opcode::dw_op_shra:
    {
      uint64_t top = pop();
      int64_t second = static_cast<int64_t>(pop());
      push(second >> top);
    }
    break;

  case opcode::dw_op_xor:
    push(pop() ^ pop());
    break;

  case opcode::dw_op_skip:
    {
      uint16_t skip;
      tie(skip, ptr) = read_unaligned<uint16_t>(ptr);
      ptr = static_cast<const uint8_t*>(ptr) + skip;
    }
    break;

  case opcode::dw_op_bra:
    {
      uint16_t skip;
      tie(skip, ptr) = read_unaligned<uint16_t>(ptr);
      if (pop() != 0U)
        ptr = static_cast<const uint8_t*>(ptr) + skip;
    }
    break;

  case opcode::dw_op_eq:
    push(static_cast<int64_t>(pop()) == static_cast<int64_t>(pop()) ? 1U : 0U);
    break;

  case opcode::dw_op_ge:
    {
      int64_t top = static_cast<int64_t>(pop());
      int64_t second = static_cast<int64_t>(pop());
      push(second >= top ? 1U : 0U);
    }
    break;

  case opcode::dw_op_gt:
    {
      int64_t top = static_cast<int64_t>(pop());
      int64_t second = static_cast<int64_t>(pop());
      push(second > top ? 1U : 0U);
    }
    break;

  case opcode::dw_op_le:
    {
      int64_t top = static_cast<int64_t>(pop());
      int64_t second = static_cast<int64_t>(pop());
      push(second <= top ? 1U : 0U);
    }
    break;

  case opcode::dw_op_lt:
    {
      int64_t top = static_cast<int64_t>(pop());
      int64_t second = static_cast<int64_t>(pop());
      push(second < top ? 1U : 0U);
    }
    break;

  case opcode::dw_op_ne:
    push(static_cast<int64_t>(pop()) != static_cast<int64_t>(pop()) ? 1U : 0U);
    break;

  case opcode::dw_op_lit0:
  case opcode::dw_op_lit1:
  case opcode::dw_op_lit2:
  case opcode::dw_op_lit3:
  case opcode::dw_op_lit4:
  case opcode::dw_op_lit5:
  case opcode::dw_op_lit6:
  case opcode::dw_op_lit7:
  case opcode::dw_op_lit8:
  case opcode::dw_op_lit9:
  case opcode::dw_op_lit10:
  case opcode::dw_op_lit11:
  case opcode::dw_op_lit12:
  case opcode::dw_op_lit13:
  case opcode::dw_op_lit14:
  case opcode::dw_op_lit15:
  case opcode::dw_op_lit16:
  case opcode::dw_op_lit17:
  case opcode::dw_op_lit18:
  case opcode::dw_op_lit19:
  case opcode::dw_op_lit20:
  case opcode::dw_op_lit21:
  case opcode::dw_op_lit22:
  case opcode::dw_op_lit23:
  case opcode::dw_op_lit24:
  case opcode::dw_op_lit25:
  case opcode::dw_op_lit26:
  case opcode::dw_op_lit27:
  case opcode::dw_op_lit28:
  case opcode::dw_op_lit29:
  case opcode::dw_op_lit30:
  case opcode::dw_op_lit31:
    {
      const unsigned int val = static_cast<uint8_t>(c) -
                               static_cast<uint8_t>(opcode::dw_op_lit0);
      push(val);
    }
    break;

  case opcode::dw_op_reg0:
  case opcode::dw_op_reg1:
  case opcode::dw_op_reg2:
  case opcode::dw_op_reg3:
  case opcode::dw_op_reg4:
  case opcode::dw_op_reg5:
  case opcode::dw_op_reg6:
  case opcode::dw_op_reg7:
  case opcode::dw_op_reg8:
  case opcode::dw_op_reg9:
  case opcode::dw_op_reg10:
  case opcode::dw_op_reg11:
  case opcode::dw_op_reg12:
  case opcode::dw_op_reg13:
  case opcode::dw_op_reg14:
  case opcode::dw_op_reg15:
  case opcode::dw_op_reg16:
  case opcode::dw_op_reg17:
  case opcode::dw_op_reg18:
  case opcode::dw_op_reg19:
  case opcode::dw_op_reg20:
  case opcode::dw_op_reg21:
  case opcode::dw_op_reg22:
  case opcode::dw_op_reg23:
  case opcode::dw_op_reg24:
  case opcode::dw_op_reg25:
  case opcode::dw_op_reg26:
  case opcode::dw_op_reg27:
  case opcode::dw_op_reg28:
  case opcode::dw_op_reg29:
  case opcode::dw_op_reg30:
  case opcode::dw_op_reg31:
    {
      const unsigned int reg = static_cast<uint8_t>(c) -
                               static_cast<uint8_t>(opcode::dw_op_reg0);
      push(get_reg(static_cast<dwarf_reg>(reg)));
    }
    break;

  case opcode::dw_op_breg0:
  case opcode::dw_op_breg1:
  case opcode::dw_op_breg2:
  case opcode::dw_op_breg3:
  case opcode::dw_op_breg4:
  case opcode::dw_op_breg5:
  case opcode::dw_op_breg6:
  case opcode::dw_op_breg7:
  case opcode::dw_op_breg8:
  case opcode::dw_op_breg9:
  case opcode::dw_op_breg10:
  case opcode::dw_op_breg11:
  case opcode::dw_op_breg12:
  case opcode::dw_op_breg13:
  case opcode::dw_op_breg14:
  case opcode::dw_op_breg15:
  case opcode::dw_op_breg16:
  case opcode::dw_op_breg17:
  case opcode::dw_op_breg18:
  case opcode::dw_op_breg19:
  case opcode::dw_op_breg20:
  case opcode::dw_op_breg21:
  case opcode::dw_op_breg22:
  case opcode::dw_op_breg23:
  case opcode::dw_op_breg24:
  case opcode::dw_op_breg25:
  case opcode::dw_op_breg26:
  case opcode::dw_op_breg27:
  case opcode::dw_op_breg28:
  case opcode::dw_op_breg29:
  case opcode::dw_op_breg30:
  case opcode::dw_op_breg31:
    {
      const unsigned int reg = static_cast<uint8_t>(c) -
                               static_cast<uint8_t>(opcode::dw_op_breg0);
      int64_t off;
      tie(off, ptr) = read_sleb128(ptr);
      push(get_reg(static_cast<dwarf_reg>(reg)) + off);
    }
    break;

  case opcode::dw_op_regx:
    {
      uint64_t reg;
      tie(reg, ptr) = read_uleb128(ptr);
      push(get_reg(static_cast<dwarf_reg>(reg)));
    }
    break;

  case opcode::dw_op_fbreg:
    {
      int64_t offset;
      tie(offset, ptr) = read_sleb128(ptr);
      push(static_cast<uint64_t>(static_cast<int64_t>(get_frame_base() +
                                                      offset)));
    }
    break;

  case opcode::dw_op_bregx:
    {
      uint64_t reg;
      int64_t off;
      tie(reg, ptr) = read_uleb128(ptr);
      tie(off, ptr) = read_sleb128(ptr);
      push(get_reg(static_cast<dwarf_reg>(reg)) + off);
    }
    break;

  case opcode::dw_op_piece:
    // XXX figure out how this interacts with dw_op_reg*...
    assert_msg(false, "XXX DW_OP_piece: implement me");
    break;

  case opcode::dw_op_deref_size:
    {
      const void __attribute__((aligned(1))) * addr;
      uint8_t sz;
      uint64_t val;
      addr = reinterpret_cast<const void*>(static_cast<uintptr_t>(pop()));
      tie(sz, ptr) = read_unaligned<uint8_t>(ptr);

      switch (sz) {
      default:
        fail_ = true;
      case 1:
        tie(val, _namespace(std)::ignore) = read_unaligned<uint8_t>(addr);
        break;
      case 2:
        tie(val, _namespace(std)::ignore) = read_unaligned<uint16_t>(addr);
        break;
      case 4:
        tie(val, _namespace(std)::ignore) = read_unaligned<uint32_t>(addr);
        break;
      case 8:
        tie(val, _namespace(std)::ignore) = read_unaligned<uint64_t>(addr);
        break;
      }
      push(val);
    }
    break;

  case opcode::dw_op_xderef_size:
    {
      uint8_t sz;
      uint64_t val;
      address_type addr = pop();
      segment_type seg = pop();
      tie(sz, ptr) = read_unaligned<uint8_t>(ptr);

      switch (sz) {
      default:
        fail_ = true;
      case 1:
        val = read_memory_at1(addr, seg);
        break;
      case 2:
        val = read_memory_at2(addr, seg);
        break;
      case 4:
        val = read_memory_at4(addr, seg);
        break;
      case 8:
        val = read_memory_at8(addr, seg);
        break;
      }
      push(val);
    }
    break;

  case opcode::dw_op_nop:
    /* SKIP: NOP operation. */
    break;

  case opcode::dw_op_push_object_address:
    // XXX figure this out
    assert_msg(false, "XXX DW_OP_push_object_address: implement me");
    break;

  case opcode::dw_op_call2:
    // XXX figure this out
    {
      uint16_t off;
      tie(off, ptr) = read_unaligned<uint16_t>(ptr);

      assert_msg(false, "XXX DW_OP_call2: implement me");
    }
    break;

  case opcode::dw_op_call4:
    // XXX figure this out
    {
      uint32_t off;
      tie(off, ptr) = read_unaligned<uint32_t>(ptr);

      assert_msg(false, "XXX DW_OP_call4: implement me");
    }
    break;

  case opcode::dw_op_call_ref:
    // XXX figure this out
    {
      uint64_t off;
      if (is_64bit())
        tie(off, ptr) = read_unaligned<uint64_t>(ptr);
      else
        tie(off, ptr) = read_unaligned<uint32_t>(ptr);

      assert_msg(false, "XXX DW_OP_call_ref: implement me");
    }
    break;

  case opcode::dw_op_form_tls_address:
    push(get_tls_base() + pop());
    break;

  case opcode::dw_op_call_frame_cfa:
    push(get_frame_base());
    break;

  case opcode::dw_op_bit_piece:
    // XXX figure out how this interacts with dw_op_reg*...
    {
      uint64_t size, off;
      tie(size, ptr) = read_uleb128(ptr);
      tie(off, ptr) = read_uleb128(ptr);

      assert_msg(false, "XXX DW_OP_piece: implement me");
    }
    break;

  case opcode::dw_op_implicit_value:
    {
      uint64_t size;
      uint64_t val;
      tie(size, ptr) = read_uleb128(ptr);

      val = 0;
      for (unsigned int sh = 0; size > 0; --size, sh += 8) {
        uint64_t read;
        tie(read, ptr) = read_unaligned<uint8_t>(ptr);
#if defined(__BIG_ENDIAN__)
        val <<= 8;
        val |= read;
#elif defined(__LITTLE_ENDIAN__)
        if (sh < 64) val |= read << sh;
#else
        static_assert(false, "Unrecognized endianness.");
#endif
      }

      push(val);
    }
    break;

  case opcode::dw_op_stack_value:
    use_value_ = true;
    terminated_ = true;
    break;
  }

  return ptr;
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)
