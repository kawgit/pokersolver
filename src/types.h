#pragma once

#include <array>
#include <cassert>
#include <bitset>
#include <stdint.h>

typedef uint8_t Card;
typedef uint8_t Rank;
typedef uint8_t Suit;
typedef uint8_t Player;
typedef int Stack;
typedef std::bitset<52> CardSet;
typedef std::bitset<13> CardSetSuitless;


enum Ranks : Rank {
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    NUM_RANKS
};

enum Suits : Suit {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES,
    NUM_SUITS
};

enum Cards : Card {
    NUM_CARDS = NUM_RANKS * NUM_SUITS,
};

enum Stacks : Stack {
    SMALL_BLIND = 1,
    BIG_BLIND = 2
};

constexpr Card make_card(Rank rank, Suit suit) {
    assert(rank <= NUM_RANKS);
    assert(suit <= NUM_SUITS);
    return suit * NUM_RANKS + rank;
}

constexpr Rank get_rank(Card card) {
    return card % NUM_RANKS;
}

constexpr Suit get_suit(Card card) {
    return card / NUM_RANKS;
}