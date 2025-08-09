#include <cassert>
#include <string>

#include "showdown.h"
#include "types.h"
#include "util.h"

template <typename T>
std::string ints_to_string(std::vector<T> vector) {
    if (vector.empty()) {
        return "[]";
    }
    std::string result = "[";
    result += std::to_string(vector[0]);
    for (size_t i = 1; i < vector.size(); i++) {
        result += ", ";
        result += std::to_string(vector[i]);
    }
    result += "]";
    return result;
}

template std::string ints_to_string<uint8_t>(std::vector<uint8_t> vector);
template std::string ints_to_string<uint16_t>(std::vector<uint16_t> vector);
template std::string ints_to_string<uint32_t>(std::vector<uint32_t> vector);
template std::string ints_to_string<uint64_t>(std::vector<uint64_t> vector);

template std::string ints_to_string<int8_t>(std::vector<int8_t> vector);
template std::string ints_to_string<int16_t>(std::vector<int16_t> vector);
template std::string ints_to_string<int32_t>(std::vector<int32_t> vector);
template std::string ints_to_string<int64_t>(std::vector<int64_t> vector);

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

std::string cards_to_string(std::vector<Card> cards) {
    if (cards.empty()) {
        return "[]";
    }
    std::string result = "[";
    result += card_to_string(cards[0]);
    for (size_t i = 1; i < cards.size(); i++) {
        result += ", ";
        result += card_to_string(cards[i]);
    }
    result += "]";
    return result;
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

std::string cardset_to_string(CardSet set) {
    std::vector<Card> cards = {};
    for (size_t i = 0; i < NUM_CARDS; i++) {
        if (set[i]) {
            cards.push_back(i);
        }
    };
    return cards_to_string(cards);
}

std::string leaderboard_to_string(const Leaderboard& leaderboard) {
    std::string result = "Showdown\n";
    for (const LeaderboardLevel& level : leaderboard) {
        result += "  ";
        result += handtype_to_string(get_hand_type(level.strength)) + " (";
        result += std::to_string(level.strength) + "):";
        for (const Player player : level.players) {
            result += " " + std::to_string(player);
        }
        result += "\n";
    }
    return result;
}

std::string status_to_string(const PlayerStatus status) {
    switch (status) {
        case PLAYING: return "PLAYING";
        case FOLDED: return "FOLDED";
        case ALL_IN: return "ALL_IN";
        case BUSTED: return "BUSTED";
        default: return "?";
    }
}

std::string statuses_to_string(const std::vector<PlayerStatus>& statuses) {
    if (statuses.empty()) {
        return "[]";
    }
    std::string result = "[";
    result += status_to_string(statuses[0]);
    for (size_t i = 1; i < statuses.size(); i++) {
        result += ", ";
        result += status_to_string(statuses[i]);
    }
    result += "]";
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
