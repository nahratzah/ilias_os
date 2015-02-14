#ifndef _ILIAS_DWARF_EVAL_H_
#define _ILIAS_DWARF_EVAL_H_

#include <cdecl.h>
#include <array>
#include <bitset>
#include <cstdint>
#include <tuple>
#include <ilias/dwarf/cie.h>
#include <ilias/dwarf/registers.h>

_namespace_begin(ilias)
namespace dwarf {


struct state {
  struct cfa_t {
    _namespace(std)::uintptr_t addr;
    _namespace(std)::uint32_t seg;
    bool seg_set;
  };

  /*
   * Register values:
   * - undefined
   * - same_value -- previous_value = current_value
   * - offset(N) -- previous_value = *(cfa_ptr + N)
   * - val_offset -- previous_value = cfa_ptr + N
   * - register(R) -- previous_value = get_register(R)
   * - expression(E) -- previous_value = *(eval(E))
   * - val_expression(E) -- previous_value = eval(E)
   * - architectural -- defined by augmenter
   */
  enum class reg_val : _namespace(std)::uint8_t {
    undefined,
    same_value,
    offset,
    val_offset,
    register_,
    expression,
    val_expression,
    architectural
  };

  struct expression_range {
    const void* begin;
    const void* end;
  };

  union argument {
    _namespace(std)::uintptr_t off;
    dwarf_reg register_;
    expression_range expression;
  };

  cfa_t cfa;
  _namespace(std)::array<reg_val, dwarf_reg_count> val;
  _namespace(std)::array<argument, dwarf_reg_count> arg;

  void set_reg_undefined(_namespace(std)::uint64_t) noexcept;
  void set_reg_samevalue(_namespace(std)::uint64_t) noexcept;
  void set_reg_offsetrule(_namespace(std)::uint64_t,
                          _namespace(std)::uintptr_t) noexcept;
  void set_reg_valoffsetrule(_namespace(std)::uint64_t,
                             _namespace(std)::uintptr_t) noexcept;
  void set_reg_register(_namespace(std)::uint64_t,
                        _namespace(std)::uint64_t) noexcept;
  void set_reg_expression(_namespace(std)::uint64_t,
                          const void*, const void*) noexcept;
  void set_reg_valexpression(_namespace(std)::uint64_t,
                             const void*, const void*) noexcept;
  void set_reg_architectural(_namespace(std)::uint64_t) noexcept;
};


class eval_instr {
 public:
  using address_type = _namespace(std)::uintptr_t;  // XXX
  using segment_type = _namespace(std)::uint32_t;
  using segment_address_type = _namespace(std)::tuple<segment_type,
                                                      address_type>;

 private:
  struct block_type {};  // XXX

  static constexpr _namespace(std)::uint8_t dw_cfa_highmask_ = 0x3U << 6U;

  enum class dw_cfa : _namespace(std)::uint8_t {
    advance_loc        = 0x1U << 6U,
    offset             = 0x2U << 6U,
    restore            = 0x3U << 6U,

    nop                = 0x00U,
    set_loc            = 0x01U,
    advance_loc1       = 0x02U,
    advance_loc2       = 0x03U,
    advance_loc4       = 0x04U,
    offset_extended    = 0x05U,
    restore_extended   = 0x06U,
    undefined          = 0x07U,
    same_value         = 0x08U,
    register_          = 0x09U,
    remember_state     = 0x0aU,
    restore_state      = 0x0bU,
    def_cfa            = 0x0cU,
    def_cfa_register   = 0x0dU,
    def_cfa_offset     = 0x0eU,
    def_cfa_expression = 0x0fU,
    expression         = 0x10U,
    offset_extended_sf = 0x11U,
    def_cfa_sf         = 0x12U,
    def_cfa_offset_sf  = 0x13U,
    val_offset         = 0x14U,
    val_offset_sf      = 0x15U,
    val_expression     = 0x16U,
    lo_user            = 0x1cU,
    hi_user            = 0x3fU,
  };

  class operand {
   public:
    enum class type : _namespace(std)::uint8_t {
      none,
      uleb,
      address,
      delta1,
      delta2,
      delta4,
      block,
      sleb,
      segment_address,
    };

    template<type> class assign;

   private:
    union union_type {
      union_type() noexcept {}

      _namespace(std)::uint64_t uleb;
      address_type address;
      _namespace(std)::uint8_t delta1;
      _namespace(std)::uint16_t delta2;
      _namespace(std)::uint32_t delta4;
      block_type block;
      _namespace(std)::int64_t sleb;
      segment_address_type segment_address;
    };

   public:
    operand() noexcept;
    operand(const operand&) noexcept;
    operand& operator=(const operand&) noexcept;
    ~operand() noexcept;

    type get_type() const noexcept { return discriminant_; }

    _namespace(std)::uint64_t get_uleb() const noexcept;
    address_type get_address() const noexcept;
    _namespace(std)::uint8_t get_delta1() const noexcept;
    _namespace(std)::uint16_t get_delta2() const noexcept;
    _namespace(std)::uint32_t get_delta4() const noexcept;
    const block_type& get_block() const noexcept;
    _namespace(std)::int64_t get_sleb() const noexcept;
    const segment_address_type& get_segment_address() const noexcept;

   private:
    union_type val_;
    type discriminant_;
  };

 public:
  eval_instr() noexcept;
  eval_instr(const eval_instr&) noexcept;
  eval_instr& operator=(const eval_instr&) noexcept;

  static _namespace(std)::tuple<const void*, eval_instr> decode(
      const void __attribute__((aligned(1))) *,
      const cie::record&) noexcept;
  bool operator()(state&) noexcept;

 private:
  static auto decode_opcode_(_namespace(std)::uint8_t) noexcept ->
      _namespace(std)::tuple<dw_cfa, _namespace(std)::uint8_t>;
  static auto decode_(const _namespace(std)::uint8_t*,
                      const cie::record&) noexcept ->
      _namespace(std)::tuple<const _namespace(std)::uint8_t*,
                             dw_cfa, _namespace(std)::uint8_t,
                             operand, operand>;

  _namespace(std)::tuple<dw_cfa, _namespace(std)::uint8_t, operand, operand,
                         const cie::record*>
      data_;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#include <ilias/dwarf/eval-inl.h>

#endif /* _ILIAS_DWARF_EVAL_H_ */
