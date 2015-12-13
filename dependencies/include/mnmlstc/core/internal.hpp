#ifndef CORE_INTERNAL_HPP
#define CORE_INTERNAL_HPP

/* This is a header containing common implementation specific code, to
 * reduce the complexity of the other headers, especially those that are
 * closely intertwined, such as <core/functional.hpp> and <core/type_traits.hpp>
 *
 * Additionally, some of this code is duplicated elsewhere (such as class_of,
 * and meta::identity), but aliases are placed to lessen any impact that this
 * might have.
 */

#include <type_traits>
#include <functional>
#include <utility>

#include "meta.hpp"

namespace core {
inline namespace v2 {
namespace impl {

template <class T, class Void, template <class...> class, class...>
struct make_detect : meta::identity<T> { using value_t = ::std::false_type; };
template <class T, template <class...> class U, class... Args>
struct make_detect<T, meta::deduce<U<Args...>>, U, Args...> :
  meta::identity<U<Args...>>
{ using value_t = ::std::true_type; };

/* extremely useful custom type traits */
template <class T> struct class_of : meta::identity<T> { };
template <class Signature, class T>
struct class_of<Signature T::*> : meta::identity<T> { };

/* aliases */
template <class T> using class_of_t = typename class_of<T>::type;
template <class T> using decay_t = typename ::std::decay<T>::type;
template <class T>
using remove_reference_t = typename ::std::remove_reference<T>::type;
template <bool B, class T = void>
using enable_if_t = typename ::std::enable_if<B, T>::type;

/* is_nothrow_swappable plumbing */
using ::std::declval;
using ::std::swap;

template <class, class, class=void>
struct is_swappable : ::std::false_type { };

template <class T, class U>
struct is_swappable<
  T,
  U,
  meta::deduce<
    decltype(swap(declval<T&>(), declval<U&>())),
    decltype(swap(declval<U&>(), declval<T&>()))
  >
> : ::std::true_type { };

template <class T, class U=T>
struct is_nothrow_swappable : meta::all_t<
  is_swappable<T, U>::value,
  noexcept(swap(declval<T&>(), declval<U&>())),
  noexcept(swap(declval<U&>(), declval<T&>()))
> { };

/* used below but also for the functional objects defined later to cut down
 * on unnecessary code. Defined due to deprecation in C++11, and removal in
 * C++17. I remove the _function part of the name because it's an internal
 * API and never you mind.
 */
template <class T, class U, class R>
struct binary {
  using second_argument_type = U;
  using first_argument_type = T;
  using result_type = R;
};

template <class T, class R>
struct unary {
  using argument_type = T;
  using result_type = R;
};

/*
 * If I can't amuse myself when working with C++ templates, then life isn't
 * worth living. Bury me with my chevrons.
 */
template <class T>
constexpr T&& pass (remove_reference_t<T>& t) noexcept {
  return static_cast<T&&>(t);
}

template <class T>
constexpr T&& pass (remove_reference_t<T>&& t) noexcept {
  return static_cast<T&&>(t);
}

/* INVOKE pseudo-expression plumbing, *much* more simplified than previous
 * versions of Core
 */
struct undefined { constexpr undefined (...) noexcept { } };

/* We get some weird warnings under clang, so we actually give these functions
 * a body to get rid of it.
 */
template <class... Args>
constexpr undefined INVOKE (undefined, Args&&...) noexcept {
  return undefined { };
}

template <class Functor, class... Args>
constexpr auto INVOKE (Functor&& f, Args&&... args) -> enable_if_t<
  not ::std::is_member_pointer<decay_t<Functor>>::value,
  decltype(pass<Functor>(f)(pass<Args>(args)...))
> { return pass<Functor>(f)(pass<Args>(args)...); }

template <class Functor, class... Args>
auto INVOKE (Functor&& f, Args&&... args) -> enable_if_t<
  ::std::is_member_pointer<decay_t<Functor>>::value,
  decltype(::std::mem_fn(pass<Functor>(f))(pass<Args>(args)...))
> { return ::std::mem_fn(pass<Functor>(f))(pass<Args>(args)...); }

template <bool, class...> struct invoke_of { };
template <class... Args> struct invoke_of<true, Args...> :
  meta::identity<decltype(INVOKE(declval<Args>()...))>
{ };

/* Used to provide lambda based 'pattern matching' for variant and optional
 * types.
 *
 * Based off of Dave Abrahams C++11 'generic lambda' example (no longer
 * available on the internet)
 */

template <class... Lambdas> struct overload;
template <class Lambda> struct overload<Lambda> : Lambda {
  using call_type = Lambda;
  using call_type::operator ();
};

template <class Lambda, class... Lambdas>
struct overload<Lambda, Lambdas...> :
  private Lambda,
  private overload<Lambdas...>::call_type
{
  using base_type = typename overload<Lambdas...>::call_type;

  using lambda_type = Lambda;
  using call_type = overload;

  overload (Lambda&& lambda, Lambdas&&... lambdas) :
    lambda_type(pass<Lambda>(lambda)),
    base_type(pass<Lambdas>(lambdas)...)
  { }

  using lambda_type::operator ();
  using base_type::operator ();
};

template <class... Lambdas>
auto make_overload(Lambdas&&... lambdas) -> overload<Lambdas...> {
  return overload<Lambdas...> { pass<Lambdas>(lambdas)... };
}

/* union used for variant<Ts...> and implementing aligned_union, which is
 * not provided by gcc 4.8.x, but is provided by clang. (aligned_union_t is
 * the only alias missing from <type_traits>)
 */
template <class... Ts> union discriminate;
template <> union discriminate<> { };
template <class T, class... Ts>
union discriminate<T, Ts...> {
  T value;
  discriminate<Ts...> rest;
};

/* implementations of MurmurHash2 *Endian Neutral* (but not alignment!) */
template <::std::size_t=sizeof(::std::size_t)> struct murmur;
template <> struct murmur<4> {
  constexpr murmur () = default;

  ::std::size_t operator () (void const* p, ::std::size_t len) const noexcept {
    static constexpr ::std::uint32_t magic = UINT32_C(0x5BD1E995);
    static constexpr auto shift = 24;

    auto hash = len;
    auto data = static_cast<::std::uint8_t const*>(p);

    while (len >= sizeof(::std::uint32_t)) {
      ::std::uint32_t mix = data[0];
      mix |= ::std::uint32_t(data[1]) <<  8;
      mix |= ::std::uint32_t(data[2]) << 16;
      mix |= ::std::uint32_t(data[3]) << 24;

      mix *= magic;
      mix ^= mix >> shift;
      mix *= magic;

      hash *= magic;
      hash ^= mix;

      data += sizeof(::std::uint32_t);
      len -= sizeof(::std::uint32_t);
    }

    switch (len) {
      case 3: hash ^= ::std::uint32_t(data[2]) << 16; [[clang::fallthrough]];
      case 2: hash ^= ::std::uint32_t(data[1]) <<  8; [[clang::fallthrough]];
      case 1: hash ^= ::std::uint32_t(data[0]);
              hash *= magic;
    }

    hash ^= hash >> 13;
    hash *= magic;
    hash ^= hash >> 15;

    return hash;
  }
};

template <> struct murmur<8> {
  constexpr murmur () = default;

  ::std::size_t operator () (void const* p, ::std::size_t len) const noexcept {
    static constexpr ::std::uint64_t magic = UINT64_C(0xC6A4A7935BD1E995);
    static constexpr auto shift = 47;

    ::std::size_t hash = len * magic;

    auto data = static_cast<::std::uint8_t const*>(p);

    while (len >= sizeof(::std::uint64_t)) {
      ::std::uint64_t mix = data[0];
      mix |= ::std::uint64_t(data[1]) <<  8;
      mix |= ::std::uint64_t(data[2]) << 16;
      mix |= ::std::uint64_t(data[3]) << 24;
      mix |= ::std::uint64_t(data[4]) << 32;
      mix |= ::std::uint64_t(data[5]) << 40;
      mix |= ::std::uint64_t(data[6]) << 48;
      mix |= ::std::uint64_t(data[7]) << 54;

      mix *= magic;
      mix ^= mix >> shift;
      mix *= magic;

      hash ^= mix;
      hash *= magic;

      data += sizeof(::std::uint64_t);
      len -= sizeof(::std::uint64_t);
    }

    switch (len & 7) {
      case 7: hash ^= ::std::uint64_t(data[6]) << 48; [[clang::fallthrough]];
      case 6: hash ^= ::std::uint64_t(data[5]) << 40; [[clang::fallthrough]];
      case 5: hash ^= ::std::uint64_t(data[4]) << 32; [[clang::fallthrough]];
      case 4: hash ^= ::std::uint64_t(data[3]) << 24; [[clang::fallthrough]];
      case 3: hash ^= ::std::uint64_t(data[2]) << 16; [[clang::fallthrough]];
      case 2: hash ^= ::std::uint64_t(data[1]) << 8;  [[clang::fallthrough]];
      case 1: hash ^= ::std::uint64_t(data[0]);
              hash *= magic;
    }

    hash ^= hash >> shift;
    hash *= magic;
    hash ^= hash >> shift;

    return hash;
  }
};

}}} /* namespace core::v2::impl */

#endif /* CORE_INTERNAL_HPP */
