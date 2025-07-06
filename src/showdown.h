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
    20 bits for kickers (4 bits per card)
    22 bits total
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
    return strength >> 20;
}

constexpr HandKickers get_hand_kickers(HandStrength strength) {
    return strength & ((1L << 20) - 1);
}

constexpr HandStrength make_hand_strength(HandType handtype, HandKickers kickers) {
    return (handtype << 20) | kickers;
}

HandType get_hand_type(CardSet cards);
HandStrength get_hand_strength(CardSet cards);
Leaderboard get_leaderboard(CardSet river_cards, std::vector<CardSet> player_cards);