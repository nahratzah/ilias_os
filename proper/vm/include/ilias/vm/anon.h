namespace ilias {
namespace vm {


class anon {
 public:
  anon() noexcept = default;
  anon(const anon&) noexcept;
  anon(anon&&) noexcept;
  anon& operator=(const anon&) noexcept;
  anon& operator=(anon&&) noexcept;

  bool has_page() const noexcept;
  bool is_swapped() const noexcept;

 private:
  // XXX page pointer
  // XXX reference to swap storage
};

class anonymous_memory {
 public:
  anonymous_memory(page_count<Arch> c)
  : anon_(c.get())
  {}

  template<arch Arch>
  anon& operator[](page_count<Arch> idx) { return anon_.at(idx.get()); }
  template<arch Arch>
  const anon& operator[](page_count<Arch> idx) const { return anon_.at(idx.get()); }

 private:
  std::vector<anon> anon_;
};


}} /* namespace ilias::vm */
