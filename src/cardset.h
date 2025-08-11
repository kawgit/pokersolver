#include "types.h"

constexpr CardSet CARDSET_NONE = 0;
constexpr CardSet CARDSET_ALL = (1ULL << NUM_CARDS) - 1;
constexpr CardSetSuitless CARDSETSUITLESS_NONE = 0;
constexpr CardSetSuitless CARDSETSUITLESS_ALL = (1ULL << NUM_RANKS) - 1;

constexpr CardSet cs_of_rank(Rank rank) {
    constexpr std::array<CardSet, NUM_RANKS> CARDSETS_BY_RANK = []() {
        std::array<CardSet, NUM_RANKS> cardsets;
        for (Rank rank = 0; rank < NUM_RANKS; ++rank) {
            CardSet cardset = CARDSET_NONE;
            for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
                cardset.set(make_card(rank, suit));
            }
            cardsets[rank] = cardset;
        }
        return cardsets;
    }();

    return CARDSETS_BY_RANK[rank];
}

constexpr CardSet cs_of_suit(Suit suit) {
    constexpr std::array<CardSet, NUM_SUITS> CARDSETS_BY_SUIT = []() {
        std::array<CardSet, NUM_SUITS> cardsets;
        for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
            CardSet cardset = CARDSET_NONE;
            for (Rank rank = 0; rank < NUM_RANKS; ++rank) {
                cardset.set(make_card(rank, suit));
            }
            cardsets[suit] = cardset;
        }
        return cardsets;
    }();

    return CARDSETS_BY_SUIT[suit];
}

constexpr CardSetSuitless css_of_rank(Rank rank) {
    constexpr std::array<CardSetSuitless, NUM_RANKS> CARDSETSUITLESS_BY_RANK = []() {
        std::array<CardSetSuitless, NUM_RANKS> arr{};
        for (Rank r = 0; r < NUM_RANKS; ++r) {
            CardSetSuitless bs;
            bs.set(r);
            arr[r] = bs;
        }
        return arr;
    }();

    return CARDSETSUITLESS_BY_RANK[rank];
}

constexpr CardSetSuitless cs_extract_suit(CardSet cards, Suit suit) {
    constexpr uint16_t mask = (1 << NUM_RANKS) - 1;
    return (cards.to_ullong() >> (suit * NUM_RANKS)) & mask;
}

constexpr CardSetSuitless cs_union_suits(CardSet cards) {
    CardSetSuitless result = CARDSETSUITLESS_NONE;
    for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
        result |= cs_extract_suit(cards, suit);
    }
    return result;
}

constexpr Card cs_pop(CardSet& x) {
    assert(x.any());
    const Card index = __builtin_ctzll(x.to_ullong());
    x.reset(index);
    return index;
}

constexpr CardSetSuitless css_pop_lowest(CardSetSuitless x) {
    assert(x.any());
    const Card index = __builtin_ctzl(x.to_ulong());
    x.reset(index);
    return index;
}

constexpr CardSetSuitless css_remove_lowest(CardSetSuitless x, size_t n) {
    assert(n <= x.count());
    uint16_t mask = x.to_ulong();
    for (size_t i = 0; i < n; ++i) {
        mask &= mask - 1;
    }
    return CardSetSuitless(mask);
}

constexpr CardSetSuitless css_keep_highest(CardSetSuitless x, size_t n) {
    assert(n <= x.count());
    return css_remove_lowest(x, x.count() - n);
}

constexpr CardSetSuitless css_without_rank(CardSetSuitless x, Rank rank) {
    return x & ~css_of_rank(rank);
}

constexpr CardSetSuitless css_without_ranks(CardSetSuitless x, Rank rank1, Rank rank2) {
    return x & ~css_of_rank(rank1) & ~css_of_rank(rank2);
}