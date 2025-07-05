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

typedef uint32_t HandStrength;
typedef uint8_t HandType;

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

constexpr CardSet get_hand_successor(CardSet cards);
constexpr size_t get_hand_index(CardSet cards);
constexpr HandType get_hand_type(CardSet cards);
HandStrength get_hand_strength(CardSet cards);
