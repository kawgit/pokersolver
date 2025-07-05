#include <array>
#include <cassert>
#include <iostream>

#include "cardset.h"
#include "showdown.h"
#include "types.h"
#include "util.h"

constexpr CardSet get_hand_successor(CardSet cards) {

    constexpr CardSet first_hand = 0b1111111ULL;
    constexpr CardSet last_hand = first_hand << 45;

    if (cards.none()) {
        return first_hand;
    }

    assert(cards.count() == 7);
    assert(cards.to_ullong() != last_hand);

    CardSet copy = cards;
    Card incrementer;

    do {
        incrementer = pop_card(copy);
        assert(incrementer < NUM_CARDS);
    } while (copy.test(incrementer + 1));

    const uint64_t remove_mask = (1ULL << (incrementer + 1)) - 1;
    const uint64_t replacement_mask = (1ULL << (6 - copy.count())) - 1;
    cards = (cards.to_ullong() & ~remove_mask) | replacement_mask;
    cards.set(incrementer + 1);

    assert(cards.count() == 7);

    return cards;
}

constexpr size_t get_hand_index(CardSet cards) {
    assert(cards.count() == 7);

    size_t index = 0;

    for (size_t i = 1; i <= 7; ++i) {
        const Card card = pop_card(cards);
        index += ncr(card, i);
    }

    assert(cards.none());
    assert(index < ncr(52, 7));

    return index;
}

constexpr bool has_straight(CardSetSuitless cards) {
    const uint16_t mask = (cards.to_ulong() << 1) | (cards.to_ulong() >> ACE);
    return mask & (mask >> 1) & (mask >> 2) & (mask >> 3) & (mask >> 4);
}

constexpr HandType get_hand_type(CardSet cards) {
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

// std::array<HandType, ncr(52, 7)> hand_type_lookup = []() {
//     std::array<HandType, ncr(52, 7)> lookup = { HIGH_CARD };
//     CardSet cards = 0;
//     for (size_t i = 0; i < ncr(52, 7); i++) {
//         cards = get_hand_successor(cards);
//         lookup[i] = get_hand_type(cards);
//     }
//     return lookup;
// }();

// constexpr HandType get_hand_type_by_lookup(CardSet cards) {
//     assert(cards.count() == 7);
//     return hand_type_lookup[get_hand_index(cards)];
// }

HandStrength get_hand_strength(CardSet cards) {
    assert(cards.count() == 7);
    return get_hand_type(cards);
}
