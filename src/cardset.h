#include "types.h"

constexpr CardSet CARDSET_NONE = 0;
constexpr CardSet CARDSET_ALL = 0xFFFF;
constexpr CardSetSuitless CARDSETSUITLESS_NONE = 0;
constexpr CardSetSuitless CARDSETSUITLESS_ALL = 0xFFFF;

namespace cs {

constexpr CardSet of_rank(Rank rank) {
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

constexpr CardSet of_suit(Suit suit) {
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

constexpr CardSetSuitless extract_suit(CardSet cards, Suit suit) {
    constexpr uint16_t mask = (1 << NUM_RANKS) - 1;
    return (cards.to_ullong() >> (suit * NUM_RANKS)) & mask;
}

constexpr CardSetSuitless union_suits(CardSet cards) {
    CardSetSuitless result = CARDSETSUITLESS_NONE;
    for (Suit suit = 0; suit < NUM_SUITS; ++suit) {
        result |= extract_suit(cards, suit);
    }
    return result;
}

} // namespace cs
