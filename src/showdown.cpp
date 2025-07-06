#include <array>
#include <cassert>
#include <iostream>
#include <vector>

#include "cardset.h"
#include "showdown.h"
#include "types.h"
#include "util.h"

constexpr bool has_straight(CardSetSuitless cards) {
    const uint16_t mask = (cards.to_ulong() << 1) | (cards.to_ulong() >> ACE);
    return mask & (mask >> 1) & (mask >> 2) & (mask >> 3) & (mask >> 4);
}

HandType get_hand_type(CardSet cards) {
    assert(cards.count() == 7);

    bool has_flush = false;
    
    for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
        const CardSetSuitless cards_in_suit = cs::extract_suit(cards, suit);

        if (has_straight(cards_in_suit)) {
            return STRAIGHT_FLUSH;
        }

        has_flush |= cards_in_suit.count() >= 5;
    }

    size_t rank_count_freqs[5] = {0};
    for (Rank rank = 0; rank < NUM_RANKS; ++rank) {
        const CardSet cards_in_rank = cs::of_rank(rank) & cards;
        const size_t count = cards_in_rank.count();

        assert(count <= 4);

        rank_count_freqs[count]++;
    }

    if (rank_count_freqs[4]) {
        return FOUR_OF_A_KIND;
    }

    if (rank_count_freqs[3] && rank_count_freqs[2]) {
        return FULL_HOUSE;
    }

    if (has_flush) {
        return FLUSH;
    }

    const CardSetSuitless cards_union_suits = cs::union_suits(cards);
    if (has_straight(cards_union_suits)) {
        return STRAIGHT;
    }

    if (rank_count_freqs[3]) {
        return THREE_OF_A_KIND;
    }

    if (rank_count_freqs[2] >= 2) {
        return TWO_PAIR;
    }

    if (rank_count_freqs[2]) {
        return PAIR;
    }

    return HIGH_CARD;
}
HandStrength get_hand_strength(CardSet cards) {
    assert(cards.count() == 7);
    HandType type = get_hand_type(cards);
    HandType kickers = 0; // TODO: implement kickers
    return make_hand_strength(type, kickers);
}

void add_to_leaderboard(Leaderboard& leaderboard, Player player, HandStrength strength) {
    size_t i;
    for (i = 0; i < leaderboard.size(); i++) {
        LeaderboardLevel& level = leaderboard[i];
        if (level.strength == strength) {
            level.players.push_back(player);
            return;
        }
        else if (level.strength < strength) {
            break;
        }
    }

    LeaderboardLevel level;
    level.strength = strength;
    level.players = { player };
    leaderboard.insert(leaderboard.begin() + i, level);
}

Leaderboard get_leaderboard(CardSet river_cards, std::vector<CardSet> player_cards) {
    assert(river_cards.count() == 5);
    Leaderboard leaderboard = {};
    std::vector<HandStrength> level_strengths;
    for (Player player = 0; player < player_cards.size(); ++player) {
        assert(player_cards[player].count() == 2);
        const CardSet hand = river_cards | player_cards[player];
        assert(hand.count() == 7);
        const HandStrength strength = get_hand_strength(hand);
        add_to_leaderboard(leaderboard, player, strength);
    }
    return leaderboard;
}
