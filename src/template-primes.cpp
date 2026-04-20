#pragma once
#include <cstddef>

template <size_t N, size_t M = 2, bool C1 = (N < 2), bool C2 = (N % M == 0),
          bool C3 = (M * M <= N)>
const bool is_prime = is_prime<N, M + 1>;
template <size_t N, size_t M>
const bool is_prime<N, M, true> = false;
template <size_t N, size_t M>
const bool is_prime<N, M, false, true> = false;
template <size_t N, size_t M>
const bool is_prime<N, M, false, false, false> = true;

template <size_t N, size_t P = 2, size_t Cnt = 0>
const size_t nth_prime = nth_prime<N, P + 1, Cnt + is_prime<P>>;
template <size_t N, size_t P>
const size_t nth_prime<N, P, N> = P - 1;
