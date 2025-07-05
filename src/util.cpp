#include <cassert>
#include <string>

#include "types.h"
#include "util.h"

std::string rank_to_string(Rank rank) {
    switch (rank) {
        case ACE: return "A";
        case TWO: return "2";
        case THREE: return "3";
        case FOUR: return "4";
        case FIVE: return "5";
        case SIX: return "6";
        case SEVEN: return "7";
        case EIGHT: return "8";
        case NINE: return "9";
        case TEN: return "T";
        case JACK: return "J";
        case QUEEN: return "Q";
        case KING: return "K";
        default: return "?";
    }
}

std::string suit_to_string(Suit suit) {
    switch (suit) {
        case CLUBS: return "♣";
        case DIAMONDS: return "♦";
        case HEARTS: return "♥";
        case SPADES: return "♠";
        default: return "?";
    }
}

std::string card_to_string(Card card) {
    if (card > NUM_CARDS) return "[]";
    return rank_to_string(get_rank(card)) + suit_to_string(get_suit(card));
}

int get_random_int(int min, int max) {
  return rand() % (max - min + 1) + min;
}

Card draw_and_replace(CardSet& set) {
    const size_t num_cards = set.count();
    assert(num_cards > 0);

    const int index = rand() % num_cards;
    uint64_t setBits = set.to_ullong();

    for (int i = 0; i < index; ++i) {
        setBits &= setBits - 1;
    }

    const Card card = __builtin_ctzll(setBits);
    assert(set[card]);
    return card;
}

Card draw(CardSet& set) {
    const Card card = draw_and_replace(set);
    set.reset(card);
    return card;
}