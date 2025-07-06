#include <cassert>
#include <string>

#include "showdown.h"
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
    if (card == NO_CARD) return "[]";
    assert(card <= NUM_CARDS);
    return rank_to_string(get_rank(card)) + suit_to_string(get_suit(card));
}

std::string handtype_to_string(HandType handtype) {
    switch (handtype) {
        case HIGH_CARD: return "HIGH_CARD";
        case PAIR: return "PAIR";
        case TWO_PAIR: return "TWO_PAIR";
        case THREE_OF_A_KIND: return "THREE_OF_A_KIND";
        case STRAIGHT: return "STRAIGHT";
        case FLUSH: return "FLUSH";
        case FULL_HOUSE: return "FULL_HOUSE";
        case FOUR_OF_A_KIND: return "FOUR_OF_A_KIND";
        case STRAIGHT_FLUSH: return "STRAIGHT_FLUSH";
        default: return "?";
    }
}

std::string leaderboard_to_string(const Leaderboard& leaderboard) {
    std::string result = "Strength, Player List\n";
    for (const LeaderboardLevel& level : leaderboard) {
        result += handtype_to_string(get_hand_type(level.strength)) + " (";
        result += std::to_string(level.strength) + "):";
        for (const Player player : level.players) {
            result += " " + std::to_string(player);
        }
        result += "\n";
    }
    return result;
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
