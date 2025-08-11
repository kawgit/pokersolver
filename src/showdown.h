#pragma once

#include <stdint.h>

#include "types.h"

/*
    1 state for straight flush
    1 state for four of a kind
    1 state for full house
    1 state for flush
    1 state for straight
    1 state for three of a kind
    1 state for two pair
    1 state for pair
    1 state for high card

    3 bits for hand type
    4 bits for kicker 1
    4 bits for kicker 2
    13 bits for high card kickers afterwards
    Thus, we can use a 32 bit int to store the hand strength
*/

enum HandTypes : HandType {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    NUM_HANDTYPES
};

constexpr HandType get_hand_type(HandStrength strength) {
    return strength >> 21;
}

constexpr HandKickers get_hand_kickers(HandStrength strength) {
    return strength & ((1L << 21) - 1);
}

constexpr HandStrength make_hand_strength(HandType handtype, CardSetSuitless kickers) {
    return (handtype << 21) | kickers.to_ulong();
}

constexpr HandStrength make_hand_strength(HandType handtype, Rank kicker1, CardSetSuitless kickers) {
    return (handtype << 21) | (kicker1 << 17) | kickers.to_ulong();
}

constexpr HandStrength make_hand_strength(HandType handtype, Rank kicker1, Rank kicker2, CardSetSuitless kickers) {
    return (handtype << 21) | (kicker1 << 17) | (kicker2 << 13) | kickers.to_ulong();
}

HandStrength get_hand_strength(CardSet cards);
Leaderboard get_leaderboard(const CardSet river_cards, const std::vector<CardSet>& player_cards, const std::vector<PlayerStatus>& player_statuses);