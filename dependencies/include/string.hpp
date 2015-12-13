#ifndef CORE_STRING_HPP
#define CORE_STRING_HPP

#include <initializer_list>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>
#include <limits>

namespace core {
inline namespace v1 {

template <class CharT, class Traits=::std::char_traits<CharT>>
struct basic_string_view {
  using difference_type = ::std::ptrdiff_t;
  using value_type = CharT;
  using size_type = ::std::size_t;

  using reference = value_type const&;
  using pointer = value_type const*;

  using const_reference = reference;
  using const_pointer = pointer;

  using const_iterator = pointer;
  using iterator = const_iterator;

  using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;

  using traits = Traits;

  static constexpr size_type npos = ::std::numeric_limits<size_type>::max();

  template <class Allocator>
  basic_string_view (
    ::std::basic_string<CharT, Traits, Allocator> const& that
  ) : str { that.data() }, len { that.size() } { }

  constexpr basic_string_view (pointer str, size_type len) noexcept :
    str { str },
    len { len }
  { }

  basic_string_view (pointer str) noexcept :
    basic_string_view { str, traits::length(str) }
  { }

  constexpr basic_string_view (basic_string_view const& that) noexcept :
    str { that.str },
    len { that.len }
  { }

  constexpr basic_string_view () noexcept :
    str { nullptr },
    len { 0 }
  { }

  basic_string_view& operator = (basic_string_view const& that) noexcept {
    basic_string_view { that }.swap(*this);
    return *this;
  }

  template <class Allocator>
  explicit operator ::std::basic_string<CharT, Traits, Allocator> () const {
    return ::std::basic_string<CharT, Traits, Allocator> {
      this->data(),
      this->size()
    };
  }

  template <class Allocator=std::allocator<CharT>>
  ::std::basic_string<CharT, Traits, Allocator> to_string (
    Allocator const& allocator = Allocator()
  ) const {
    return ::std::basic_string<CharT, Traits, Allocator> {
      this->data(),
      this->size(),
      allocator
    };
  }

  constexpr const_iterator begin () const noexcept { return this->data(); }
  constexpr const_iterator end () const noexcept {
    return this->data() + this->size();
  }

  constexpr const_iterator cbegin () const noexcept { return this->begin(); }
  constexpr const_iterator cend () const noexcept { return this->end(); }

  const_reverse_iterator rbegin () const noexcept {
    return const_reverse_iterator { this->end()};
  }

  const_reverse_iterator rend () const noexcept {
    return const_reverse_iterator { this->begin() };
  }

  const_reverse_iterator crbegin () const noexcept { return this->rbegin(); }
  const_reverse_iterator crend () const noexcept { return this->rend(); }

  constexpr size_type max_size () const noexcept { return this->size(); }
  constexpr size_type length () const noexcept { return this->size(); }
  constexpr size_type size () const noexcept { return this->len; }

  constexpr bool empty () const noexcept { return this->size() == 0; }

  constexpr reference operator [] (size_type idx) const {
    return this->str[idx];
  }

  constexpr reference front () const { return this->str[0]; }
  constexpr reference back () const { return this->str[this->size() - 1]; }
  constexpr pointer data () const { return this->str; }

  void remove_prefix (size_type n) {
    if (n > this->size()) { n = this->size(); }
    this->str += n;
    this->len -= n;
  }

  void remove_suffix (size_type n) {
    if (n > this->size()) { n = this->size(); }
    this->len -= n;
  }

  void clear () noexcept {
    this->str = nullptr;
    this->len = 0;
  }

  constexpr basic_string_view substr (size_type pos, size_type n=npos) const {
    return pos > this->size()
      ? throw ::std::out_of_range { "start position out of range" }
      : basic_string_view {
        this->data() + pos,
        n == npos or pos + n > this->size()
          ? (this->size() - pos)
          : n
      };
  }

  bool starts_with (value_type value) const noexcept {
    return not this->empty() and traits::eq(value, this->front());
  }

  bool ends_with (value_type value) const noexcept {
    return not this->empty() and traits::eq(value, this->back());
  }

  bool starts_with (basic_string_view that) const noexcept {
    return this->size() >= that.size() and
      traits::compare(this->data(), that.data(), that.size()) == 0;
  }

  bool ends_with (basic_string_view that) const noexcept {
    return this->size() >= that.size() and
      traits::compare(
        this->data() + this->size() - that.size(),
        that.data(),
        that.size()
      ) == 0;
  }

  difference_type compare (basic_string_view that) const {
    auto cmp = traits::compare(
      this->data(),
      that.data(),
      ::std::min(this->size(), that.size())
    );

    if (cmp != 0) { return cmp; }
    if (this->size() == that.size()) { return 0; }
    if (this->size() < that.size()) { return -1; }
    return 1;
  }

  reference at (size_type idx) const {
    if (idx >= this->size()) {
      throw ::std::out_of_range { "requested index out of range" };
    }
    return this->str[idx];
  }

  /* functions that take a string-ref */
  size_type find_first_not_of (basic_string_view that) const {
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
      if (traits::find(that.data(), that.size(), *iter)) { continue; }
      return ::std::distance(this->begin(), iter);
    }
    return npos;
  }

  size_type find_last_not_of (basic_string_view that) const {
    for (auto iter = this->rbegin(); iter != this->rend(); ++iter) {
      if (traits::find(that.data(), that.size(), *iter)) { continue; }
      return this->size() - ::std::distance(this->rbegin(), iter) - 1;
    }
    return npos;
  }

  size_type find_first_of (basic_string_view that) const {
    auto iter = ::std::find_first_of(
      this->begin(), this->end(),
      that.begin(), that.end(),
      traits::eq
    );
    if (iter == this->end()) { return npos; }
    return ::std::distance(this->begin(), iter);
  }

  size_type find_last_of (basic_string_view that) const {
    auto iter = ::std::find_first_of(
      this->rbegin(), this->rend(),
      that.rbegin(), that.rend(),
      traits::eq
    );
    if (iter == this->rend()) { return npos; }
    return this->size() - ::std::distance(this->rbegin(), iter) - 1;
  }

  size_type rfind (basic_string_view that) const {
    auto iter = ::std::search(
      this->rbegin(), this->rend(),
      that.rbegin(), that.rend(),
      traits::eq
    );
    if (iter == this->rend()) { return npos; }
    return this->size() - ::std::distance(this->rbegin(), iter) - 1;
  }

  size_type find (basic_string_view that) const {
    auto iter = ::std::search(
      this->begin(), this->end(),
      that.begin(), that.end(),
      traits::eq
    );
    if (iter == this->end()) { return npos; }
    return ::std::distance(this->begin(), iter);
  }

  /* functions that take a single CharT */
  size_type find_first_not_of (value_type value) const {
    auto end = this->end();
    auto iter = ::std::find_if_not(
      this->begin(),
      end,
      [value](value_type val) { return traits::eq(val, value); }
    );
    if (iter == end) { return npos; }
    return ::std::distance(this->begin(), iter);
  }

  size_type find_last_not_of (value_type value) const {
    auto end = this->rend();
    auto iter = ::std::find_if_not(
      this->rbegin(),
      end,
      [value](value_type val) { return traits::eq(val, value); }
    );
    if (iter == end) { return npos; }
    return this->size() - ::std::distance(this->rbegin(), iter) - 1;
  }

  size_type find_first_of (value_type value) const {
    return this->find(value);
  }

  size_type find_last_of (value_type value) const {
    return this->rfind(value);
  }

  size_type rfind (value_type value) const {
    auto end = this->rend();
    auto iter = ::std::find(this->rbegin(), end, value);
    if (iter == end) { return npos; }
    return this->size() - ::std::distance(this->rbegin(), iter) -1;
  }

  size_type find (value_type value) const {
    auto end = this->end();
    auto iter = ::std::find(this->begin(), end, value);
    if (iter == end) { return npos; }
    return ::std::distance(this->begin(), iter);
  }

  void swap (basic_string_view& that) noexcept {
    using ::std::swap;
    swap(this->str, that.str);
    swap(this->len, that.len);
  }

private:
  pointer str;
  size_type len;
};

using u32string_view = basic_string_view<char32_t>;
using u16string_view = basic_string_view<char16_t>;
using wstring_view = basic_string_view<wchar_t>;
using string_view = basic_string_view<char>;

template <class CharT, typename Traits>
bool operator == (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.size() == rhs.size() and lhs.compare(rhs) == 0; }

template <class CharT, typename Traits>
bool operator != (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.size() != rhs.size() or lhs.compare(rhs) != 0; }

template <class CharT, typename Traits>
bool operator >= (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.compare(rhs) >= 0; }

template <class CharT, typename Traits>
bool operator <= (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.compare(rhs) <= 0; }

template <class CharT, typename Traits>
bool operator > (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.compare(rhs) > 0; }

template <class CharT, typename Traits>
bool operator < (
  basic_string_view<CharT, Traits> lhs,
  basic_string_view<CharT, Traits> rhs
) noexcept { return lhs.compare(rhs) < 0; }

template <class CharT, class Traits>
::std::basic_ostream<CharT, Traits>& operator << (
  ::std::basic_ostream<CharT, Traits>& os,
  basic_string_view<CharT, Traits> const& str
) { return os << str.to_string(); }

template <class CharT, class Traits>
void swap (
  basic_string_view<CharT, Traits>& lhs,
  basic_string_view<CharT, Traits>& rhs
) noexcept { return lhs.swap(rhs); }

}} /* namespace core::v1 */

namespace std {

template <typename CharT, typename Traits>
struct hash<core::v1::basic_string_view<CharT, Traits>> {
  using argument_type = core::v1::basic_string_view<CharT, Traits>;
  using result_type = size_t;

  result_type operator ()(argument_type const& ref) const noexcept {
    return hash<typename argument_type::pointer> { }(ref.data());
  }
};

} /* namespace std */

#endif /* CORE_STRING_HPP */
