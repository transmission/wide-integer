///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2018 - 2023.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <vector>

#include <examples/example_uintwide_t.h>
#include <math/wide_integer/uintwide_t.h>

#include <util/utility/util_pseudorandom_time_point_seed.h>

namespace local_timed_mul_8_by_8
{
  template<typename UnsignedIntegralIteratorType,
           typename RandomEngineType>
  auto get_random_big_uint(RandomEngineType& rng, UnsignedIntegralIteratorType it_out) -> void
  {
    using local_uint_type = typename std::iterator_traits<UnsignedIntegralIteratorType>::value_type;

    #if defined(WIDE_INTEGER_NAMESPACE)
    using distribution_type =
      WIDE_INTEGER_NAMESPACE::math::wide_integer::uniform_int_distribution<std::numeric_limits<local_uint_type>::digits, typename local_uint_type::limb_type>;
    #else
    using distribution_type =
      ::math::wide_integer::uniform_int_distribution<std::numeric_limits<local_uint_type>::digits, typename local_uint_type::limb_type>;
    #endif

    distribution_type distribution;

    *it_out = distribution(rng);
  }

  #if defined(WIDE_INTEGER_NAMESPACE)
  using big_uint_type = WIDE_INTEGER_NAMESPACE::math::wide_integer::uintwide_t<static_cast<WIDE_INTEGER_NAMESPACE::math::wide_integer::size_t>(UINT32_C(256))>;
  #else
  using big_uint_type = ::math::wide_integer::uintwide_t<static_cast<math::wide_integer::size_t>(UINT32_C(256))>;
  #endif

  auto local_a() -> std::vector<big_uint_type>&
  {
    static std::vector<big_uint_type>
      my_local_a
      (
        static_cast<typename std::vector<big_uint_type>::size_type>(UINT32_C(1024))
      );

    return my_local_a;
  }

  auto local_b() -> std::vector<big_uint_type>&
  {
    static std::vector<big_uint_type> my_local_b(local_a().size());

    return my_local_b;
  }
} // namespace local_timed_mul_8_by_8

#if defined(WIDE_INTEGER_NAMESPACE)
auto WIDE_INTEGER_NAMESPACE::math::wide_integer::example009b_timed_mul_8_by_8() -> bool
#else
auto ::math::wide_integer::example009b_timed_mul_8_by_8() -> bool
#endif
{
  using random_engine_type =
    std::linear_congruential_engine<std::uint32_t, UINT32_C(48271), UINT32_C(0), UINT32_C(2147483647)>;

  random_engine_type rng; // NOLINT(cert-msc32-c,cert-msc51-cpp)

  rng.seed(util::util_pseudorandom_time_point_seed::value<typename random_engine_type::result_type>());

  for(auto i = static_cast<typename std::vector<local_timed_mul_8_by_8::big_uint_type>::size_type>(0U); i < local_timed_mul_8_by_8::local_a().size(); ++i)
  {
    local_timed_mul_8_by_8::get_random_big_uint(rng, local_timed_mul_8_by_8::local_a().begin() + static_cast<typename std::vector<local_timed_mul_8_by_8::big_uint_type>::difference_type>(i));
    local_timed_mul_8_by_8::get_random_big_uint(rng, local_timed_mul_8_by_8::local_b().begin() + static_cast<typename std::vector<local_timed_mul_8_by_8::big_uint_type>::difference_type>(i));
  }

  std::uint64_t count = 0U;
  std::size_t   index = 0U;

  float total_time { };

  const auto begin = std::clock();

  for(;;)
  {
    local_timed_mul_8_by_8::local_a().at(index + 0U) * local_timed_mul_8_by_8::local_b().at(index + 0U);
    local_timed_mul_8_by_8::local_a().at(index + 1U) * local_timed_mul_8_by_8::local_b().at(index + 1U);
    local_timed_mul_8_by_8::local_a().at(index + 2U) * local_timed_mul_8_by_8::local_b().at(index + 2U);
    local_timed_mul_8_by_8::local_a().at(index + 3U) * local_timed_mul_8_by_8::local_b().at(index + 3U);

    const auto end = std::clock();

    total_time = static_cast<float>(static_cast<float>(end - begin) / CLOCKS_PER_SEC);

    count += 4U;
    index += 4U;

    if(index >= local_timed_mul_8_by_8::local_a().size())
    {
      index = 0U;
    }

    if(total_time > static_cast<float>(6.0L)) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    {
      break;
    }
  }

  const float kops_per_sec = static_cast<float>(count) / static_cast<float>(static_cast<float>(total_time * 1000.0F));

  {
    const auto flg = std::cout.flags();

    std::cout << "bits: "
              << std::numeric_limits<local_timed_mul_8_by_8::big_uint_type>::digits
              << ", kops_per_sec: "
              << std::fixed
              << std::setprecision(2)
              << kops_per_sec
              << ", count: "
              << count
              << std::endl;

    std::cout.flags(flg);
  }

  const auto result_is_ok = (kops_per_sec > (std::numeric_limits<float>::min)());

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if defined(WIDE_INTEGER_STANDALONE_EXAMPLE009B_TIMED_MUL_8_BY_8)

auto main() -> int
{
  #if defined(WIDE_INTEGER_NAMESPACE)
  const auto result_is_ok = WIDE_INTEGER_NAMESPACE::math::wide_integer::example009b_timed_mul_8_by_8();
  #else
  const auto result_is_ok = ::math::wide_integer::example009b_timed_mul_8_by_8();
  #endif

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;

  return (result_is_ok ? 0 : -1);
}

#endif
