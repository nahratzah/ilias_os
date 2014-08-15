#ifndef _LOCALE_MISC_CODECVT_H_
#define _LOCALE_MISC_CODECVT_H_

#include <cdecl.h>
#include <locale_misc/locale.h>

_namespace_begin(std)


class codecvt_base {
 public:
  enum result {
    ok,
    partial,
    error,
    noconv
  };
};

template<typename Intern, typename Extern, typename St>
class codecvt
: public locale::facet,
  public codecvt_base
{
 public:
  using intern_type = Intern;
  using extern_type = Extern;
  using state_type = St;

  explicit codecvt(size_t = 0);

  result out(state_type&,
             const intern_type*, const intern_type*, const intern_type*&,
             extern_type*, extern_type*, extern_type*&) const;
  result in(state_type&,
            const extern_type*, const extern_type*, const extern_type*&,
            intern_type*, intern_type*, intern_type*&) const;
  result unshift(state_type&,
                 extern_type*, extern_type*, extern_type*&) const;
  int encoding() const noexcept;
  bool always_noconv() const noexcept;
  int length(state_type&, const extern_type*, const extern_type*,
             size_t) const;
  int max_length() const noexcept;

  static const locale::id id;

 protected:
  ~codecvt() noexcept override;

  virtual result do_out(state_type&,
                        const intern_type*, const intern_type*,
                        const intern_type*&,
                        extern_type*, extern_type*, extern_type*&) const = 0;
  virtual result do_in(state_type&,
                       const extern_type*, const extern_type*,
                       const extern_type*&,
                       intern_type*, intern_type*, intern_type*&) const = 0;
  virtual result do_unshift(state_type&,
                            extern_type*, extern_type*,
                            extern_type*&) const = 0;
  virtual int do_encoding() const noexcept = 0;
  virtual bool do_always_noconv() const noexcept = 0;
  virtual int do_length(state_type&, const extern_type*, const extern_type*,
                        size_t) const = 0;
  virtual int do_max_length() const noexcept = 0;
};

template<typename Intern, typename Extern, typename St>
const locale::id codecvt<Intern, Extern, St>::id{ locale::ctype };

#define _SPECIALIZE_CODECVT(Intern, Extern)				\
template<>								\
class codecvt<Intern, Extern, mbstate_t>				\
: public locale::facet,							\
  public codecvt_base							\
{									\
 public:								\
  using intern_type = Intern;						\
  using extern_type = Extern;						\
  using state_type = mbstate_t;						\
									\
  explicit codecvt(size_t = 0);						\
									\
  result out(state_type&,						\
             const intern_type*, const intern_type*,			\
             const intern_type*&,					\
             extern_type*, extern_type*, extern_type*&) const;		\
  result in(state_type&,						\
            const extern_type*, const extern_type*,			\
            const extern_type*&,					\
            intern_type*, intern_type*, intern_type*&) const;		\
  result unshift(state_type&,						\
                 extern_type*, extern_type*, extern_type*&) const;	\
  int encoding() const noexcept;					\
  bool always_noconv() const noexcept;					\
  int length(state_type&, const extern_type*, const extern_type*,	\
             size_t) const;						\
  int max_length() const noexcept;					\
									\
  static const locale::id id;						\
									\
 protected:								\
  ~codecvt() noexcept override;						\
									\
  virtual result do_out(state_type&,					\
                        const intern_type*, const intern_type*,		\
                        const intern_type*&,				\
                        extern_type*, extern_type*,			\
                        extern_type*&) const;				\
  virtual result do_in(state_type&,					\
                       const extern_type*, const extern_type*,		\
                       const extern_type*&,				\
                       intern_type*, intern_type*,			\
                       intern_type*&) const;				\
  virtual result do_unshift(state_type&,				\
                            extern_type*, extern_type*,			\
                            extern_type*&) const;			\
  virtual int do_encoding() const noexcept;				\
  virtual bool do_always_noconv() const noexcept;			\
  virtual int do_length(state_type&, const extern_type*,		\
                        const extern_type*,				\
                        size_t) const;					\
  virtual int do_max_length() const noexcept;				\
};

/* Declare specialization for identity conversion. */
_SPECIALIZE_CODECVT(char,     char)
_SPECIALIZE_CODECVT(char16_t, char16_t)
_SPECIALIZE_CODECVT(char32_t, char32_t)
_SPECIALIZE_CODECVT(wchar_t,  wchar_t)

/* Declare specialized codecvt. */
_SPECIALIZE_CODECVT(char16_t, char);
_SPECIALIZE_CODECVT(char32_t, char);
_SPECIALIZE_CODECVT(wchar_t,  char);

_SPECIALIZE_CODECVT(char,     char16_t);
_SPECIALIZE_CODECVT(char32_t, char16_t);
_SPECIALIZE_CODECVT(wchar_t,  char16_t);

_SPECIALIZE_CODECVT(char,     char32_t);
_SPECIALIZE_CODECVT(char16_t, char32_t);
_SPECIALIZE_CODECVT(wchar_t,  char32_t);

_SPECIALIZE_CODECVT(char,     wchar_t);
_SPECIALIZE_CODECVT(char16_t, wchar_t);
_SPECIALIZE_CODECVT(char32_t, wchar_t);

#undef _SPECIALIZE_CODECVT

template<typename Intern, typename Extern, typename St>
class codecvt_byname
: public codecvt<Intern, Extern, St>,
  private impl::facet_ref<codecvt<Intern, Extern, St>>
{
 public:
  using intern_type = typename codecvt<Intern, Extern, St>::intern_type;
  using extern_type = typename codecvt<Intern, Extern, St>::extern_type;
  using state_type = typename codecvt<Intern, Extern, St>::state_type;
  using result = typename codecvt<Intern, Extern, St>::result;

  explicit codecvt_byname(const char*, size_t = 0);
  explicit codecvt_byname(const string&, size_t = 0);
  explicit codecvt_byname(string_ref, size_t = 0);

 protected:
  ~codecvt_byname() noexcept override = default;

  result do_out(state_type&,
                const intern_type*, const intern_type*,
                const intern_type*&,
                extern_type*, extern_type*, extern_type*&) const override;
  result do_in(state_type&,
               const extern_type*, const extern_type*,
               const extern_type*&,
               intern_type*, intern_type*, intern_type*&) const override;
  result do_unshift(state_type&,
                    extern_type*, extern_type*, extern_type*&) const override;
  int do_encoding() const noexcept override;
  bool do_always_noconv() const noexcept override;
  int do_length(state_type&, const extern_type*, const extern_type*,
                size_t) const override;
  int do_max_length() const noexcept override;
};


extern template class codecvt_byname<char,     char,     mbstate_t>;
extern template class codecvt_byname<char16_t, char,     mbstate_t>;
extern template class codecvt_byname<char32_t, char,     mbstate_t>;
extern template class codecvt_byname<wchar_t,  char,     mbstate_t>;
extern template class codecvt_byname<char,     char16_t, mbstate_t>;
extern template class codecvt_byname<char16_t, char16_t, mbstate_t>;
extern template class codecvt_byname<char32_t, char16_t, mbstate_t>;
extern template class codecvt_byname<wchar_t,  char16_t, mbstate_t>;
extern template class codecvt_byname<char,     char32_t, mbstate_t>;
extern template class codecvt_byname<char16_t, char32_t, mbstate_t>;
extern template class codecvt_byname<char32_t, char32_t, mbstate_t>;
extern template class codecvt_byname<wchar_t,  char32_t, mbstate_t>;
extern template class codecvt_byname<char,     wchar_t,  mbstate_t>;
extern template class codecvt_byname<char16_t, wchar_t,  mbstate_t>;
extern template class codecvt_byname<char32_t, wchar_t,  mbstate_t>;
extern template class codecvt_byname<wchar_t,  wchar_t,  mbstate_t>;


_namespace_end(std)

#include <locale_misc/codecvt-inl.h>

#endif /* _LOCALE_MISC_CODECVT_H_ */
