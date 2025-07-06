#pragma once

#include <array>
#include <string>
#include <cassert>

#include "types.h"

template <typename T>
std::string ints_to_string(std::vector<T> players);

std::string rank_to_string(Rank rank);
std::string suit_to_string(Suit suit);
std::string card_to_string(Card card);
std::string cardset_to_string(CardSet set);
std::string leaderboard_to_string(const Leaderboard& leaderboard);

int get_random_int(int min, int max);

Card draw_and_replace(CardSet& set);
Card draw(CardSet& set);

constexpr std::array<std::array<uint64_t, 27>, 53> NCR_LOOKUP = []() {
    std::array<std::array<uint64_t, 27>, 53> lookup = { 0 };

    for (int n = 0; n < 53; ++n) {
        lookup[n][0] = 1;
    }

    for (int n = 1; n < 53; ++n) {
        for (int r = 1; r < 27; ++r) {
            lookup[n][r] = lookup[n-1][r-1] + lookup[n-1][r];
        }
    }

    return lookup;
}();

constexpr uint64_t ncr(int n, int r) {
    assert(0 <= n);
    assert(0 <= r);
    assert(n <= 52);
    assert(r <= 52);
    r = n >= r ? std::min(r, n - r) : r;
    assert(r <= 26);
    return NCR_LOOKUP[n][r];
}

constexpr Card pop_card(CardSet& x) {
    assert(x.any());
    const Card index = __builtin_ctzll(x.to_ullong());
    x.reset(index);
    return index;
}