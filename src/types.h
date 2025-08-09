#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <deque>
#include <stdint.h>
#include <vector>

typedef uint8_t Card;
typedef uint8_t Rank;
typedef uint8_t Suit;
typedef uint8_t Player;
typedef int Stack;
typedef std::bitset<52> CardSet;
typedef std::bitset<13> CardSetSuitless;
typedef uint8_t HandType;
typedef uint32_t HandKickers;
typedef uint32_t HandStrength;
typedef uint8_t PlayerStatus;

struct LeaderboardLevel {
    HandStrength strength;
    std::vector<Player> players;
};

typedef std::deque<LeaderboardLevel> Leaderboard;

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
    NO_CARD = NUM_CARDS
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

enum PlayerStatuses : PlayerStatus {
    PLAYING,
    FOLDED,
    ALL_IN,
    BUSTED,
    NUM_PLAYER_STATUSES
};

const auto is_player_status_in = [](PlayerStatus status) {
    return status == PLAYING || status == ALL_IN;
};