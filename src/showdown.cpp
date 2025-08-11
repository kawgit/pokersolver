#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "cardset.h"
#include "showdown.h"
#include "types.h"
#include "util.h"

// Returns a cardset containing the top card of each straight that exists
constexpr CardSetSuitless get_straights(CardSetSuitless cards) {
    const uint16_t mask = (cards.to_ulong() << 1) | (cards.to_ulong() >> ACE);
    return (mask >> 1) & (mask) & (mask << 1) & (mask << 2) & (mask << 3);
}

HandStrength get_hand_strength(CardSet cards) {
    assert(cards.count() == 7);

    CardSetSuitless cards_in_flush = CARDSETSUITLESS_NONE;
    
    for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
        const CardSetSuitless cards_in_suit = cs_extract_suit(cards, suit);

        if (cards_in_suit.count() < 5) {
            continue;
        }

        const CardSetSuitless straights = get_straights(cards_in_suit);
        if (straights.any()) {
            return make_hand_strength(STRAIGHT_FLUSH, css_keep_highest(straights, 1));
        }

        cards_in_flush = css_keep_highest(cards_in_suit, 5);
        assert(cards_in_flush.count() == 5);
        break;
    }

    std::vector<Rank> ranks_with_count[5] = {};
    for (Rank rank = 0; rank < NUM_RANKS; ++rank) {
        const CardSet cards_in_rank = cs_of_rank(rank) & cards;
        const size_t count = cards_in_rank.count();

        assert(count <= 4);

        ranks_with_count[count].push_back(rank);
    }

    for (size_t i = 0; i < 5; ++i) {
        std::sort(ranks_with_count[i].begin(), ranks_with_count[i].end(), std::greater<Rank>());
    }

    const CardSetSuitless cards_union_suits = cs_union_suits(cards);

    if (ranks_with_count[4].size()) {
        const Rank four_rank = ranks_with_count[4][0];
        const CardSetSuitless leftovers = css_without_rank(cards_union_suits, four_rank);
        const CardSetSuitless kickers = css_keep_highest(leftovers, 1);
        return make_hand_strength(FOUR_OF_A_KIND, four_rank, kickers);
    }

    if (ranks_with_count[3].size() && ranks_with_count[2].size()) {
        const Rank three_rank = ranks_with_count[3][0];
        const Rank two_rank = ranks_with_count[2][0];
        return make_hand_strength(FULL_HOUSE, three_rank, two_rank, CARDSETSUITLESS_NONE);
    }

    if (cards_in_flush.any()) {
        return make_hand_strength(FLUSH, cards_in_flush);
    }

    const CardSetSuitless straights = get_straights(cards_union_suits);
    if (straights.any()) {
        return make_hand_strength(STRAIGHT, css_keep_highest(straights, 1));
    }

    if (ranks_with_count[3].size()) {
        const Rank three_rank = ranks_with_count[3][0];
        const CardSetSuitless leftovers = css_without_rank(cards_union_suits, three_rank);
        const CardSetSuitless kickers = css_keep_highest(leftovers, 2);
        return make_hand_strength(THREE_OF_A_KIND, three_rank, kickers);
    }

    if (ranks_with_count[2].size() >= 2) {
        const Rank two_rank_0 = ranks_with_count[2][0];
        const Rank two_rank_1 = ranks_with_count[2][1];
        const CardSetSuitless leftovers = css_without_ranks(cards_union_suits, two_rank_0, two_rank_1);
        const CardSetSuitless kickers = css_keep_highest(leftovers, 1);
        return make_hand_strength(TWO_PAIR, two_rank_0, two_rank_1, kickers);
    }

    if (ranks_with_count[2].size()) {
        const Rank two_rank = ranks_with_count[2][0];
        const CardSetSuitless leftovers = css_without_rank(cards_union_suits, two_rank);
        const CardSetSuitless kickers = css_keep_highest(leftovers, 3);
        return make_hand_strength(PAIR, two_rank, kickers);
    }

    return make_hand_strength(HIGH_CARD, css_keep_highest(cards_union_suits, 5));
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

Leaderboard get_leaderboard(const CardSet river_cards, const std::vector<CardSet>& player_cards, const std::vector<PlayerStatus>& player_statuses) {
    assert(player_cards.size() == player_statuses.size());
    assert(river_cards.count() == 5);
    Leaderboard leaderboard = {};
    std::vector<HandStrength> level_strengths;
    for (Player player = 0; player < player_cards.size(); ++player) {
        if (!is_player_status_in(player_statuses[player])) {
            continue;
        }
        assert(player_cards[player].count() == 2);
        const CardSet hand = river_cards | player_cards[player];
        assert(hand.count() == 7);
        const HandStrength strength = get_hand_strength(hand);
        add_to_leaderboard(leaderboard, player, strength);
    }
    return leaderboard;
}
