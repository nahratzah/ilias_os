#ifndef _ILIAS_DWARF_DW_OP_H_
#define _ILIAS_DWARF_DW_OP_H_

#include <cdecl.h>
#include <cstdint>
#include <array>
#include <ilias/dwarf/registers.h>

_namespace_begin(ilias)
namespace dwarf {


class dw_op_stack {
 public:
  enum class opcode : _namespace(std)::uint8_t {
    dw_op_addr                = 0x03,
    dw_op_deref               = 0x06,
    dw_op_const1u             = 0x08,
    dw_op_const1s             = 0x09,
    dw_op_const2u             = 0x0a,
    dw_op_const2s             = 0x0b,
    dw_op_const4u             = 0x0c,
    dw_op_const4s             = 0x0d,
    dw_op_const8u             = 0x0e,
    dw_op_const8s             = 0x0f,
    dw_op_constu              = 0x10,
    dw_op_consts              = 0x11,
    dw_op_dup                 = 0x12,
    dw_op_drop                = 0x13,
    dw_op_over                = 0x14,
    dw_op_pick                = 0x15,
    dw_op_swap                = 0x16,
    dw_op_rot                 = 0x17,
    dw_op_xderef              = 0x18,
    dw_op_abs                 = 0x19,
    dw_op_and                 = 0x1a,
    dw_op_div                 = 0x1b,
    dw_op_minus               = 0x1c,
    dw_op_mod                 = 0x1d,
    dw_op_mul                 = 0x1e,
    dw_op_neg                 = 0x1f,
    dw_op_not                 = 0x20,
    dw_op_or                  = 0x21,
    dw_op_plus                = 0x22,
    dw_op_plus_uconst         = 0x23,
    dw_op_shl                 = 0x24,
    dw_op_shr                 = 0x25,
    dw_op_shra                = 0x26,
    dw_op_xor                 = 0x27,
    dw_op_skip                = 0x2f,
    dw_op_bra                 = 0x28,
    dw_op_eq                  = 0x29,
    dw_op_ge                  = 0x2a,
    dw_op_gt                  = 0x2b,
    dw_op_le                  = 0x2c,
    dw_op_lt                  = 0x2d,
    dw_op_ne                  = 0x2e,
    dw_op_lit0                = 0x30,
    dw_op_lit1                = 0x31,
    dw_op_lit2                = 0x32,
    dw_op_lit3                = 0x33,
    dw_op_lit4                = 0x34,
    dw_op_lit5                = 0x35,
    dw_op_lit6                = 0x36,
    dw_op_lit7                = 0x37,
    dw_op_lit8                = 0x38,
    dw_op_lit9                = 0x39,
    dw_op_lit10               = 0x3a,
    dw_op_lit11               = 0x3b,
    dw_op_lit12               = 0x3c,
    dw_op_lit13               = 0x3d,
    dw_op_lit14               = 0x3e,
    dw_op_lit15               = 0x3f,
    dw_op_lit16               = 0x40,
    dw_op_lit17               = 0x41,
    dw_op_lit18               = 0x42,
    dw_op_lit19               = 0x43,
    dw_op_lit20               = 0x44,
    dw_op_lit21               = 0x45,
    dw_op_lit22               = 0x46,
    dw_op_lit23               = 0x47,
    dw_op_lit24               = 0x48,
    dw_op_lit25               = 0x49,
    dw_op_lit26               = 0x4a,
    dw_op_lit27               = 0x4b,
    dw_op_lit28               = 0x4c,
    dw_op_lit29               = 0x4d,
    dw_op_lit30               = 0x4e,
    dw_op_lit31               = 0x4f,
    dw_op_reg0                = 0x50,
    dw_op_reg1                = 0x51,
    dw_op_reg2                = 0x52,
    dw_op_reg3                = 0x53,
    dw_op_reg4                = 0x54,
    dw_op_reg5                = 0x55,
    dw_op_reg6                = 0x56,
    dw_op_reg7                = 0x57,
    dw_op_reg8                = 0x58,
    dw_op_reg9                = 0x59,
    dw_op_reg10               = 0x5a,
    dw_op_reg11               = 0x5b,
    dw_op_reg12               = 0x5c,
    dw_op_reg13               = 0x5d,
    dw_op_reg14               = 0x5e,
    dw_op_reg15               = 0x5f,
    dw_op_reg16               = 0x60,
    dw_op_reg17               = 0x61,
    dw_op_reg18               = 0x62,
    dw_op_reg19               = 0x63,
    dw_op_reg20               = 0x64,
    dw_op_reg21               = 0x65,
    dw_op_reg22               = 0x66,
    dw_op_reg23               = 0x67,
    dw_op_reg24               = 0x68,
    dw_op_reg25               = 0x69,
    dw_op_reg26               = 0x6a,
    dw_op_reg27               = 0x6b,
    dw_op_reg28               = 0x6c,
    dw_op_reg29               = 0x6d,
    dw_op_reg30               = 0x6e,
    dw_op_reg31               = 0x6f,
    dw_op_breg0               = 0x70,
    dw_op_breg1               = 0x71,
    dw_op_breg2               = 0x72,
    dw_op_breg3               = 0x73,
    dw_op_breg4               = 0x74,
    dw_op_breg5               = 0x75,
    dw_op_breg6               = 0x76,
    dw_op_breg7               = 0x77,
    dw_op_breg8               = 0x78,
    dw_op_breg9               = 0x79,
    dw_op_breg10              = 0x7a,
    dw_op_breg11              = 0x7b,
    dw_op_breg12              = 0x7c,
    dw_op_breg13              = 0x7d,
    dw_op_breg14              = 0x7e,
    dw_op_breg15              = 0x7f,
    dw_op_breg16              = 0x80,
    dw_op_breg17              = 0x81,
    dw_op_breg18              = 0x82,
    dw_op_breg19              = 0x83,
    dw_op_breg20              = 0x84,
    dw_op_breg21              = 0x85,
    dw_op_breg22              = 0x86,
    dw_op_breg23              = 0x87,
    dw_op_breg24              = 0x88,
    dw_op_breg25              = 0x89,
    dw_op_breg26              = 0x8a,
    dw_op_breg27              = 0x8b,
    dw_op_breg28              = 0x8c,
    dw_op_breg29              = 0x8d,
    dw_op_breg30              = 0x8e,
    dw_op_breg31              = 0x8f,
    dw_op_regx                = 0x90,
    dw_op_fbreg               = 0x91,
    dw_op_bregx               = 0x92,
    dw_op_piece               = 0x93,
    dw_op_deref_size          = 0x94,
    dw_op_xderef_size         = 0x95,
    dw_op_nop                 = 0x96,
    dw_op_push_object_address = 0x97,
    dw_op_call2               = 0x98,
    dw_op_call4               = 0x99,
    dw_op_call_ref            = 0x9a,
    dw_op_form_tls_address    = 0x9b,
    dw_op_call_frame_cfa      = 0x9c,
    dw_op_bit_piece           = 0x9d,
    dw_op_implicit_value      = 0x9e,
    dw_op_stack_value         = 0x9f,
  };

  using value_type = _namespace(std)::uint64_t;
  using size_type = _namespace(std)::uint8_t;
  using address_type = _namespace(std)::uintptr_t;
  using segment_type = _namespace(std)::uint32_t;

  static constexpr size_type max_stack_depth = 64;

  void push(value_type);
  value_type pop();
  value_type top() const;
  const value_type& operator[](size_type) const;
  value_type& operator[](size_type);
  size_type size() const noexcept { return data_.size() - off_; }
  bool empty() const noexcept { return data_.size() == off_; }

  void eval(const void __attribute__((aligned(1))) *,
            const void __attribute__((aligned(1))) *);
  const void* eval1(const void __attribute__((aligned(1))) *);

  virtual address_type read_memory_at(segment_type, address_type) = 0;
  virtual uint8_t read_memory_at1(segment_type, address_type) = 0;
  virtual uint16_t read_memory_at2(segment_type, address_type) = 0;
  virtual uint32_t read_memory_at4(segment_type, address_type) = 0;
  virtual uint64_t read_memory_at8(segment_type, address_type) = 0;
  virtual uint64_t get_reg(dwarf_reg) = 0;
  virtual address_type get_frame_base() = 0;
  virtual address_type get_tls_base() = 0;
  virtual bool is_64bit() const noexcept = 0;
  bool is_32bit() const noexcept { return !is_64bit(); }

  bool fail() const noexcept { return fail_; }
  bool use_value() const noexcept { return use_value_; }
  bool terminated() const noexcept { return terminated_; }

 private:
  _namespace(std)::array<value_type, max_stack_depth> data_;
  size_type off_ = data_.size();
  bool use_value_ = false;
  bool terminated_ = false;
  const bool throw_ = true;

 protected:
  mutable bool fail_ = false;
};


} /* namespace ilias::dwarf */
_namespace_end(ilias)

#endif /* _ILIAS_DWARF_DW_OP_H_ */
