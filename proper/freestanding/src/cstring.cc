#include <cstring>
#include <endian.h>
#include <cstddef_misc/intptr.h>
#include <cstddef_misc/size_ptrdiff.h>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <utility>


namespace std {
namespace ilias {
namespace {


typedef _TYPES(uint8_t) uint8_t;


template<typename T>
inline void
prefetch_r(const T* addr)
{
	__builtin_prefetch(addr, 0, 0);
}

template<typename T>
inline void
prefetch_w(T* addr)
{
	__builtin_prefetch(addr, 1, 0);
}

template<typename T>
constexpr unsigned int
alignment()
{
	return (sizeof(T) == 0 ? 1 : sizeof(T));
}
template<typename T>
constexpr unsigned int
type_size()
{
	return sizeof(T);
}
template<>
constexpr unsigned int
type_size<void>()
{
	return 0;
}

template<bool Writeable>
class buffer_ptr
{
public:
	typedef size_t size_type;
	typedef uintptr_t intpointer;

protected:
	const void* m_addr;

public:
	buffer_ptr(const void* addr = nullptr) noexcept
	:	m_addr(addr)
	{
		/* Empty body. */
	}

	template<typename T>
	const T*
	addr() const noexcept
	{
		assert(this->aligned<T>());
		return reinterpret_cast<const T*>(this->m_addr);
	}

	template<typename T>
	const T&
	deref() const noexcept
	{
		auto p = this->addr<T>();
		prefetch_r(p);
		return *p;
	}

	template<typename RelType>
	size_type
	offset() const noexcept
	{
		return this->offset(alignment<RelType>());
	}

	size_type
	offset(size_type rel) const noexcept
	{
		return reinterpret_cast<intpointer>(this->m_addr) % rel;
	}

	template<typename RelType>
	bool
	aligned() const noexcept
	{
		return this->offset<RelType>() == 0;
	}
};

template<>
class buffer_ptr<true>
{
public:
	typedef size_t size_type;
	typedef uintptr_t intpointer;

protected:
	void* m_addr;

public:
	buffer_ptr(void* addr = nullptr) noexcept
	:	m_addr(addr)
	{
		/* Empty body. */
	}

	template<typename T>
	T*
	addr() const noexcept
	{
		assert(this->aligned<T>());
		return reinterpret_cast<T*>(this->m_addr);
	}

	template<typename T>
	T&
	deref() const noexcept
	{
		auto p = this->addr<T>();
		prefetch_w(p);
		return *p;
	}

	template<typename RelType>
	size_type
	offset() const noexcept
	{
		return this->offset(alignment<RelType>());
	}

	size_type
	offset(size_type rel) const noexcept
	{
		return reinterpret_cast<intpointer>(this->m_addr) % rel;
	}

	template<typename RelType>
	bool
	aligned() const noexcept
	{
		return this->offset<RelType>() == 0;
	}
};

template<bool Writeable>
class buffer :
	public buffer_ptr<Writeable>
{
public:
	typedef typename buffer_ptr<Writeable>::size_type size_type;
	typedef typename buffer_ptr<Writeable>::intpointer intpointer;

private:
	const void* m_addr;
	size_type m_len;

public:
	constexpr buffer() noexcept
	:	buffer(nullptr, 0)
	{
		/* Empty body. */
	}

	constexpr buffer(const void* addr, size_t len) noexcept
	:	buffer_ptr<Writeable>(addr),
		m_len(len)
	{
		/* Empty body. */
	}

	constexpr buffer(void* addr, size_t len) noexcept
	:	buffer_ptr<Writeable>(addr),
		m_len(len)
	{
		/* Empty body. */
	}

	buffer&
	operator+=(size_type delta) noexcept
	{
		if (delta > this->m_len)
			delta = this->m_len;

		this->m_addr = reinterpret_cast<void*>(reinterpret_cast<intpointer>(this->m_addr) + delta);
		this->m_len -= delta;
		return *this;
	}

	buffer
	operator+(size_type delta) const noexcept
	{
		buffer clone = *this;
		return clone += delta;
	}

	buffer&
	operator-=(size_type delta) noexcept
	{
		this->m_addr = reinterpret_cast<void*>(reinterpret_cast<intpointer>(this->m_addr) - delta);
		this->m_len += delta;
		return *this;
	}

	buffer
	operator-(size_type delta) const noexcept
	{
		buffer clone = *this;
		return clone -= delta;
	}

	buffer&
	operator++() noexcept
	{
		return *this += 1;
	}

	buffer
	operator++(int) noexcept
	{
		buffer clone = *this;
		++*this;
		return clone;
	}

	buffer&
	operator--() noexcept
	{
		return *this -= 1;
	}

	buffer
	operator--(int) noexcept
	{
		buffer clone = *this;
		--*this;
		return clone;
	}

	template<typename RelType>
	size_type
	read_skip() const noexcept
	{
		return this->template offset<RelType>();
	}

	bool
	empty() const noexcept
	{
		return (this->m_len == 0);
	}

	size_type
	size() const noexcept
	{
		return this->m_len;
	}

	friend void
	swap(buffer& a, buffer& b) noexcept
	{
		swap(a.m_addr, b.m_addr);
		swap(a.m_len, b.m_len);
	}
};

template<bool Writeable>
class reverse_buffer
{
private:
	typedef buffer<Writeable> impl_type;

public:
	typedef typename impl_type::size_type size_type;
	typedef typename impl_type::intpointer intpointer;

private:
	impl_type m_impl;
	size_type m_len;

public:
	constexpr reverse_buffer() noexcept
	:	m_impl(),
		m_len(0)
	{
		/* Empty body. */
	}

	reverse_buffer(const void* addr, size_t len) noexcept
	:	m_impl(reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(addr) + len), 0),
		m_len(len)
	{
		/* Empty body. */
	}

	reverse_buffer(void* addr, size_t len) noexcept
	:	m_impl(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) + len), 0),
		m_len(len)
	{
		/* Empty body. */
	}

	template<typename T>
	impl_type
	base() const noexcept
	{
		return this->m_impl - type_size<T>();
	}

	template<typename T>
	auto
	addr() const noexcept -> decltype(this->m_impl.template addr<T>())
	{
		return this->base<T>().template addr<T>();
	}

	template<typename T>
	auto
	deref() const noexcept -> decltype(this->m_impl.template deref<T>())
	{
		return this->base<T>().template deref<T>();
	}

	template<typename RelType>
	size_type
	offset() const noexcept
	{
		return this->base<RelType>().template offset<RelType>();
	}

	size_type
	offset(size_type rel) const noexcept
	{
		return (this->base<void>() - rel).offset(rel);
	}

	template<typename RelType>
	size_type
	read_skip() const noexcept
	{
		return alignment<RelType>() - this->offset(alignment<RelType>());
	}

	template<typename RelType>
	bool
	aligned() const noexcept
	{
		return this->base<RelType>().template aligned<RelType>();
	}

	reverse_buffer&
	operator+=(size_type delta) noexcept
	{
		assert(delta <= this->m_len);

		this->m_impl -= delta;
		this->m_len -= delta;
		return *this;
	}

	reverse_buffer
	operator+(size_type delta) const noexcept
	{
		reverse_buffer clone = *this;
		return clone += delta;
	}

	reverse_buffer&
	operator-=(size_type delta) noexcept
	{
		assert(delta <= this->m_impl.size());

		this->m_impl += delta;
		this->m_len += delta;
		return *this;
	}

	reverse_buffer
	operator-(size_type delta) const noexcept
	{
		reverse_buffer clone = *this;
		return clone -= delta;
	}

	reverse_buffer&
	operator++() noexcept
	{
		return *this += 1;
	}

	reverse_buffer
	operator++(int) noexcept
	{
		reverse_buffer clone = *this;
		++*this;
		return clone;
	}

	reverse_buffer&
	operator--() noexcept
	{
		return *this -= 1;
	}

	reverse_buffer
	operator--(int) noexcept
	{
		reverse_buffer clone = *this;
		--*this;
		return clone;
	}

	bool
	empty() const noexcept
	{
		return (this->m_len == 0);
	}

	size_type
	size() const noexcept
	{
		return this->m_len;
	}

	friend void
	swap(reverse_buffer& a, reverse_buffer& b) noexcept
	{
		swap(a.m_impl, b.m_impl);
		swap(a.m_len, b.m_len);
	}
};


template<typename UInt>
UInt
read_buffer(buffer<false> b)
{
	auto shift = b.offset<UInt>();
	if (shift == 0)
		return b.deref<UInt>();

	b -= shift;
	auto b_ = b + sizeof(UInt);
	return addr_shift<UInt>(b.deref<UInt>(), b_.deref<UInt>(), bits_per_byte * shift);
}

template<typename UInt>
UInt
read_buffer(reverse_buffer<false> b)
{
	return read_buffer<UInt>(b.base<UInt>());
}

inline int
mismatch(uint8_t v1, uint8_t v2) noexcept
{
	return int(v2) - int(v1);
}

template<typename UInt>
int
mismatch(UInt v1, UInt v2) noexcept
{
	auto b1 = reinterpret_cast<const uint8_t*>(&v1);
	auto b2 = reinterpret_cast<const uint8_t*>(&v2);

	for (size_t i = 0; i < sizeof(UInt); ++i) {
		auto mm = mismatch(*b1++, *b2++);
		if (mm)
			return mm;
	}
	return 0;
}


template<typename CmpInt, template<bool> class Buffer>
int
compare(Buffer<false> b1, Buffer<false> b2) noexcept
{
	/* Integer type used for comparison. */
	typedef CmpInt comparison_int;

	const auto b1_len = b1.size();
	const auto b2_len = b2.size();

	/* Result if both buffers are identical. */
	const int ident_rv = (b1_len < b2_len ? -1 : b1_len > b2_len);

	/* Use byte comparison until one of the buffers is aligned. */
	while (!b1.template aligned<comparison_int>() && !b2.template aligned<comparison_int>() &&
	    !b1.empty() && !b2.empty()) {
		typedef uint8_t type;

		type v1 = (b1++).template deref<type>();
		type v2 = (b2++).template deref<type>();
		if (v1 != v2)
			return int(v2) - int(v1);
	}

	/* Ensure the aligned buffer is always b1. */
	int mul = 1;
	if (!b1.template aligned<comparison_int>()) {
		swap(b1, b2);
		mul = -1;
	}

	/* Use aligned access to read the bulk of the data. */
	while (b1.size() >= sizeof(comparison_int) && b2.size() >= sizeof(comparison_int) + b2.template read_skip<comparison_int>()) {
		comparison_int v1 = b1.template deref<comparison_int>();
		comparison_int v2 = read_buffer<comparison_int>(b2);

		if (v1 != v2) {
			/* Mismatch found. */
			return mul * mismatch(v1, v2);
		}

		b1 += sizeof(comparison_int);
		b2 += sizeof(comparison_int);
	}

	/* Use byte access for the tail of the buffers. */
	while (!b1.empty() && !b2.empty()) {
		typedef uint8_t type;

		type v1 = (b1++).template deref<type>();
		type v2 = (b2++).template deref<type>();
		if (v1 != v2)
			return int(v2) - int(v1);
	}

	/* Both are identical, up to the point where at least one ends. */
	return ident_rv;
}

template<typename CmpInt, template<bool> class Buffer>
void*
copy(Buffer<true> dst, Buffer<false> src) noexcept
{
	/* Integer type used for comparison. */
	typedef CmpInt copy_int;

	/* Use byte comparison until dst is aligned. */
	while (!dst.template aligned<copy_int>() &&
	    !dst.empty() && !src.empty()) {
		typedef uint8_t type;

		(dst++).template deref<type>() = (src++).template deref<type>();
	}

	/* Use aligned access to read the bulk of the data. */
	while (dst.size() >= sizeof(copy_int) && src.size() >= sizeof(copy_int) + src.template read_skip<copy_int>()) {
		dst.template deref<copy_int>() = read_buffer<copy_int>(src);

		dst += sizeof(copy_int);
		src += sizeof(copy_int);
	}

	/* Use byte access for the tail of the buffers. */
	while (!dst.empty() && !src.empty()) {
		typedef uint8_t type;

		(dst++).template deref<type>() = (src++).template deref<type>();
	}

	return dst.template addr<void>();
}

template<typename UInt>
inline bool
has_zero_byte(UInt v) noexcept
{
	if (!is_pow2(sizeof(UInt))) {
		/* Fast check (O(log n)), may yield false positives. */
		UInt smash = v;
		for (size_t i = sizeof(UInt) / 2; i > 0; i /= 2)
			smash &= (smash >> i);
		if (smash & ((1 << bits_per_byte) - 1))
			return false;	/* Definately not present. */
	}

	/* Slow check (O(n)), no false positives. */
	auto b1 = reinterpret_cast<const uint8_t*>(&v);

	for (size_t i = 0; i < sizeof(UInt); ++i, ++b1)
		if (!*b1)
			return true;
	return false;
}

template<typename CmpInt, template<bool> class Buffer>
const void*
find_byte(Buffer<false> haystack, uint8_t needle, bool stop_at_0) noexcept
{
	typedef CmpInt search_int;

	/* Use byte comparison until we reach alignment. */
	while (!haystack.template aligned<search_int>() &&
	    !haystack.empty()) {
		typedef uint8_t type;

		auto v = haystack.template deref<type>();
		if (v == needle || (stop_at_0 && v == 0U))
			return haystack.template addr<void>();
		++haystack;
	}

	/* Declare big needle and look for a place where it seems to match. */
	const auto big_needle = repeat(search_int(needle), bits_per_byte);
	while (haystack.size() >= sizeof(search_int)) {
		auto v = haystack.template deref<search_int>();
		if ((stop_at_0 && has_zero_byte(v)) || has_zero_byte(v ^ big_needle))
			break;

		haystack += sizeof(search_int);
	}

	/* Use byte access to find exact spot of match, or work on the tail of the buffer. */
	bool found = false;
	while (!haystack.template aligned<search_int>() &&
	    !haystack.empty()) {
		typedef uint8_t type;

		auto v = haystack.template deref<type>();
		if (v == needle || (stop_at_0 && v == 0U)) {
			found = (v == needle);
			break;
		}
		++haystack;
	}

	/* Return pointer to found byte. */
	return (found ? haystack.template addr<uint8_t>() : nullptr);
}


}} /* namespace std::ilias::<unnamed> */


void*
memccpy(void* dst, const void* src, int c, size_t len) noexcept
{
	auto end = ilias::find_byte<uintptr_t, ilias::buffer>({ src, len }, c & 0xff, false);
	size_t cplen = len;
	if (end)
		cplen = reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(src);

	auto cp_rv = ilias::copy<uintptr_t, ilias::buffer>({ dst, cplen }, { src, cplen });
	return (end ? cp_rv : nullptr);
}

void*
memchr(const void* b, int c, size_t len) noexcept
{
	return const_cast<void*>(ilias::find_byte<uintptr_t, ilias::buffer>({ b, len }, c & 0xff, false));
}

void*
memrchr(const void* b, int c, size_t len) noexcept
{
	return const_cast<void*>(ilias::find_byte<uintptr_t, ilias::reverse_buffer>({ b, len }, c & 0xff, false));
}

int
memcmp(const void* s1, const void* s2, size_t sz) noexcept
{
	return ilias::compare<uintptr_t, ilias::buffer>({ s1, sz }, { s2, sz });
}

void*
memcpy(void* dst, const void* src, size_t len) noexcept
{
	ilias::copy<uintptr_t, ilias::buffer>({ dst, len }, { src, len });
	return dst;
}

void*
memmove(void* dst, const void* src, size_t len) noexcept
{
	if (reinterpret_cast<uintptr_t>(dst) > reinterpret_cast<uintptr_t>(src) &&
	    reinterpret_cast<uintptr_t>(dst) < reinterpret_cast<uintptr_t>(src) + len) {
		ilias::copy<uintptr_t, ilias::reverse_buffer>({ dst, len }, { src, len });
		return dst;
	} else
		return memcpy(dst, src, len);
}

char*
strcat(char* dst, const char* append) noexcept
{
	return strcpy(dst + strlen(dst), append);
}

char*
strchr(const char* s, int c) noexcept
{
	return const_cast<char*>(static_cast<const char*>(
	    ilias::find_byte<uintptr_t, ilias::buffer>({ s, SIZE_MAX }, c & 0xff, true)));
}

char*
strrchr(const char* s, int c) noexcept
{
	/* Use a forward-search only algorithm. */
	char* rv = nullptr;
	while (char* found = strchr(s, c)) {
		rv = found;
		s = found + 1;
	}
	return rv;
}

int
strcmp(const char* s1, const char* s2) noexcept
{
	return ilias::compare<uintptr_t, ilias::buffer>({ s1, strlen(s1) }, { s2, strlen(s2) });
}

int
strncmp(const char* s1, const char* s2, size_t len) noexcept
{
	return ilias::compare<uintptr_t, ilias::buffer>({ s1, len }, { s2, len });
}

char*
strcpy(char* dst, const char* src) noexcept
{
	return reinterpret_cast<char*>(memcpy(dst, src, strlen(src) + 1));
}

char*
strdup(const char* s) noexcept
{
	const auto len = strlen(s) + 1;
	const auto p = malloc(len);
	if (p)
		memcpy(p, s, len);
	return static_cast<char*>(p);
}

size_t
strlen(const char* s) noexcept
{
	auto found = ilias::find_byte<uintptr_t, ilias::buffer>({ s, SIZE_MAX }, 0, false);
	return reinterpret_cast<uintptr_t>(found) - reinterpret_cast<uintptr_t>(s);
}

void*
memset(void* b, int assign_, size_t len) noexcept
{
	using namespace ilias;
	typedef uintptr_t search_int;
	const uint8_t assign = assign_ & 0xff;

	buffer<true> buf(b, len);

	/* Use byte comparison until we reach alignment. */
	while (!buf.template aligned<search_int>() &&
	    !buf.empty()) {
		buf.deref<uint8_t>() = assign;
		++buf;
	}

	/* Declare big needle and look for a place where it seems to match. */
	const auto big_assign = repeat(search_int(assign), bits_per_byte);
	while (buf.size() >= sizeof(search_int)) {
		buf.template deref<search_int>() = big_assign;
		buf += sizeof(big_assign);
	}

	/* Use byte access to find exact spot of match, or work on the tail of the buffer. */
	while (!buf.empty()) {
		buf.deref<uint8_t>() = assign;
		++buf;
	}

	/* Return pointer to buffer. */
	return b;
}


} /* namespace std */
