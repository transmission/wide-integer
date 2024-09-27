///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2024.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// cd /mnt/c/Users/ckorm/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/wide_integer
// clang++ -std=c++20 -g -O2 -fsanitize=fuzzer,address,undefined -I. -I/mnt/c/boost/boost_1_85_0 test/fuzzing/test_fuzzing_prime.cpp -o test_fuzzing_prime
// ./test_fuzzing_prime -max_total_time=180

#include <math/wide_integer/uintwide_t.h>
#include <util/utility/util_pseudorandom_time_point_seed.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <random>

namespace fuzzing
{
  using boost_uint_backend_type =
    boost::multiprecision::cpp_int_backend<static_cast<unsigned>(UINT32_C(256)),
                                           static_cast<unsigned>(UINT32_C(256)),
                                           boost::multiprecision::unsigned_magnitude>;

  using boost_uint_type = boost::multiprecision::number<boost_uint_backend_type,
                                                        boost::multiprecision::et_off>;

  using local_uint_type = ::math::wide_integer::uint256_t;

  auto eval_prime(const std::uint8_t* data, std::size_t size) -> bool;
}

auto fuzzing::eval_prime(const std::uint8_t* data, std::size_t size) -> bool
{
  const std::size_t
    max_size
    {
      static_cast<std::size_t>
      (
        std::numeric_limits<fuzzing::local_uint_type>::digits / 8
      )
    };

  bool result_is_ok { true };

  if((size > std::size_t { max_size / 2U }) && (size <= max_size))
  {
    using random_engine1_type = std::linear_congruential_engine<std::uint32_t, UINT32_C(48271), UINT32_C(0), UINT32_C(2147483647)>;
    using random_engine2_type = std::mt19937;

    using distribution_type = ::math::wide_integer::uniform_int_distribution<local_uint_type::my_width2, typename local_uint_type::limb_type>;

    random_engine1_type generator1(util::util_pseudorandom_time_point_seed::value<typename random_engine1_type::result_type>());
    random_engine2_type generator2(util::util_pseudorandom_time_point_seed::value<typename random_engine2_type::result_type>());

    distribution_type distribution1;
    distribution_type distribution2;

    local_uint_type p0 { 0U };
    boost_uint_type pb { 0U };

    // Import the random data into the prime candidate.
    import_bits
    (
      p0,
      data,
      data + size,
      8U
    );

    // Import the random data into the boost control prime candidate.
    import_bits
    (
      pb,
      data,
      data + size,
      8U
    );

    // Ensure that both uintwide_t as well as boost obtain
    // the same prime (or non-prime) result.

    const bool miller_rabin_result_local { miller_rabin(p0, 25U, distribution2, generator2) };
    const bool miller_rabin_result_boost { boost::multiprecision::miller_rabin_test(pb, 25U, generator2) };

    const bool
      result_prime_or_not_prime_is_ok
      {
        miller_rabin_result_local == miller_rabin_result_boost
      };

    result_is_ok = (result_prime_or_not_prime_is_ok && result_is_ok);
  }

  // Assert the correct result.
  assert(result_is_ok);

  return result_is_ok;
}

// The fuzzing entry point.
extern "C"
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  const bool result_one_prime_is_ok { fuzzing::eval_prime(data, size) };

  return (result_one_prime_is_ok ? 0 : -1);
}