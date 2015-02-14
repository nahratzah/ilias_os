#ifndef _ILIAS_DWARF_EVAL_INL_H_
#define _ILIAS_DWARF_EVAL_INL_H_

#include <ilias/dwarf/eval.h>
#include <new>

_namespace_begin(ilias)
namespace dwarf {


inline auto state::set_reg_undefined(_namespace(std)::uint64_t idx) noexcept ->
    void {
  assert(idx < dwarf_reg_count);
  val[idx] = reg_val::undefined;
}

inline auto state::set_reg_samevalue(_namespace(std)::uint64_t idx) noexcept ->
void {
  assert(idx < dwarf_reg_count);
  val[idx] = reg_val::same_value;
}

inline auto state::set_reg_offsetrule(_namespace(std)::uint64_t idx,
                                      _namespace(std)::uintptr_t off)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  val[idx] = reg_val::offset;
  arg[idx].off = off;
}

inline auto state::set_reg_valoffsetrule(_namespace(std)::uint64_t idx,
                                         _namespace(std)::uintptr_t off)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  val[idx] = reg_val::val_offset;
  arg[idx].off = off;
}

inline auto state::set_reg_register(_namespace(std)::uint64_t idx,
                                    _namespace(std)::uint64_t regno)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  assert(regno < dwarf_reg_count);
  val[idx] = reg_val::register_;
  arg[idx].register_ = static_cast<dwarf_reg>(regno);
}

inline auto state::set_reg_expression(_namespace(std)::uint64_t idx,
                                      const void* begin, const void* end)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  assert(begin <= end);
  val[idx] = reg_val::expression;
  arg[idx].expression = expression_range{ begin, end };
}

inline auto state::set_reg_valexpression(_namespace(std)::uint64_t idx,
                                         const void* begin, const void* end)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  assert(begin <= end);
  val[idx] = reg_val::val_expression;
  arg[idx].expression = expression_range{ begin, end };
}

inline auto state::set_reg_architectural(_namespace(std)::uint64_t idx)
    noexcept -> void {
  assert(idx < dwarf_reg_count);
  val[idx] = reg_val::architectural;
}


inline eval_instr::eval_instr() noexcept {
  using _namespace(std)::get;

  get<0>(data_) = dw_cfa::nop;
  get<1>(data_) = 0;
}

inline eval_instr::eval_instr(const eval_instr& o) noexcept
: data_(o.data_)
{}

inline auto eval_instr::operator=(const eval_instr& o) noexcept ->
    eval_instr& {
  data_ = o.data_;
  return *this;
}

inline auto eval_instr::decode_opcode_(_namespace(std)::uint8_t v) noexcept ->
    _namespace(std)::tuple<dw_cfa, _namespace(std)::uint8_t> {
  using result_type = _namespace(std)::tuple<dw_cfa, _namespace(std)::uint8_t>;
  using _namespace(std)::uint8_t;
  using _namespace(std)::get;

  result_type rv;
  if (v & dw_cfa_highmask_) {
    get<0>(rv) = static_cast<dw_cfa>(v & dw_cfa_highmask_);
    get<1>(rv) = (v & ~dw_cfa_highmask_);
  } else {
    get<0>(rv) = static_cast<dw_cfa>(v);
    get<1>(rv) = 0;
  }
  return rv;
}


inline eval_instr::operand::operand() noexcept
: discriminant_(type::none)
{}

inline eval_instr::operand::operand(const operand& o) noexcept
: discriminant_(o.discriminant_)
{
  switch (discriminant_) {
  case type::none:
    break;
  case type::uleb:
    new (&val_.uleb) uint64_t(o.val_.uleb);
    break;
  case type::address:
    new (&val_.address) address_type(o.val_.address);
    break;
  case type::delta1:
    new (&val_.delta1) uint8_t(o.val_.delta1);
    break;
  case type::delta2:
    new (&val_.delta2) uint16_t(o.val_.delta2);
    break;
  case type::delta4:
    new (&val_.delta4) uint32_t(o.val_.delta4);
    break;
  case type::block:
    new (&val_.block) block_type(o.val_.block);
    break;
  case type::sleb:
    new (&val_.sleb) int64_t(o.val_.sleb);
    break;
  case type::segment_address:
    new (&val_.segment_address) segment_address_type(o.val_.segment_address);
    break;
  }
}

inline auto eval_instr::operand::operator=(const operand& o) noexcept ->
    operand& {
  switch (_namespace(std)::exchange(discriminant_, type::none)) {
  case type::none:
    break;
  case type::uleb:
    val_.uleb.~uint64_t();
    break;
  case type::address:
    val_.address.~address_type();
    break;
  case type::delta1:
    val_.delta1.~uint8_t();
    break;
  case type::delta2:
    val_.delta2.~uint16_t();
    break;
  case type::delta4:
    val_.delta4.~uint32_t();
    break;
  case type::block:
    val_.block.~block_type();
    break;
  case type::sleb:
    val_.sleb.~int64_t();
    break;
  case type::segment_address:
    val_.segment_address.~segment_address_type();
    break;
  }

  switch (discriminant_) {
  case type::none:
    break;
  case type::uleb:
    new (&val_.uleb) uint64_t(o.val_.uleb);
    break;
  case type::address:
    new (&val_.address) address_type(o.val_.address);
    break;
  case type::delta1:
    new (&val_.delta1) uint8_t(o.val_.delta1);
    break;
  case type::delta2:
    new (&val_.delta2) uint16_t(o.val_.delta2);
    break;
  case type::delta4:
    new (&val_.delta4) uint32_t(o.val_.delta4);
    break;
  case type::block:
    new (&val_.block) block_type(o.val_.block);
    break;
  case type::sleb:
    new (&val_.sleb) int64_t(o.val_.sleb);
    break;
  case type::segment_address:
    new (&val_.segment_address) segment_address_type(o.val_.segment_address);
  }
  discriminant_ = o.discriminant_;
  return *this;
}

inline eval_instr::operand::~operand() noexcept {
  switch (discriminant_) {
  case type::none:
    break;
  case type::uleb:
    val_.uleb.~uint64_t();
    break;
  case type::address:
    val_.address.~address_type();
    break;
  case type::delta1:
    val_.delta1.~uint8_t();
    break;
  case type::delta2:
    val_.delta2.~uint16_t();
    break;
  case type::delta4:
    val_.delta4.~uint32_t();
    break;
  case type::block:
    val_.block.~block_type();
    break;
  case type::sleb:
    val_.sleb.~int64_t();
    break;
  case type::segment_address:
    val_.segment_address.~segment_address_type();
  }
}

inline auto eval_instr::operand::get_uleb() const noexcept ->
    _namespace(std)::uint64_t {
  assert(get_type() == type::uleb);
  return val_.uleb;
}

inline auto eval_instr::operand::get_address() const noexcept ->
    address_type {
  assert(get_type() == type::address);
  return val_.address;
}

inline auto eval_instr::operand::get_delta1() const noexcept ->
    _namespace(std)::uint8_t {
  assert(get_type() == type::delta1);
  return val_.delta1;
}

inline auto eval_instr::operand::get_delta2() const noexcept ->
    _namespace(std)::uint16_t {
  assert(get_type() == type::delta2);
  return val_.delta2;
}

inline auto eval_instr::operand::get_delta4() const noexcept ->
    _namespace(std)::uint32_t {
  assert(get_type() == type::delta4);
  return val_.delta4;
}

inline auto eval_instr::operand::get_block() const noexcept ->
    const block_type& {
  assert(get_type() == type::block);
  return val_.block;
}

inline auto eval_instr::operand::get_sleb() const noexcept ->
    _namespace(std)::int64_t {
  assert(get_type() == type::sleb);
  return val_.sleb;
}

inline auto eval_instr::operand::get_segment_address() const noexcept ->
    const segment_address_type& {
  assert(get_type() == type::segment_address);
  return val_.segment_address;
}


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_DWARF_EVAL_INL_H_ */
