#include <iostream>

#include "showdown.h"
#include "table.h"
#include "types.h"
#include "util.h"

int main() {

    // Hand type tests

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(TWO, SPADES));
        cards.set(make_card(THREE, SPADES));
        cards.set(make_card(FOUR, SPADES));
        cards.set(make_card(FIVE, SPADES));
        cards.set(make_card(EIGHT, SPADES));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == STRAIGHT_FLUSH);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(THREE, HEARTS));
        cards.set(make_card(THREE, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(THREE, DIAMONDS));
        cards.set(make_card(ACE, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == FOUR_OF_A_KIND);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(THREE, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(THREE, DIAMONDS));
        cards.set(make_card(ACE, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == FULL_HOUSE);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(TWO, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(FOUR, SPADES));
        cards.set(make_card(FIVE, SPADES));
        cards.set(make_card(EIGHT, SPADES));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == FLUSH);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(TWO, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(FOUR, SPADES));
        cards.set(make_card(FIVE, CLUBS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == STRAIGHT);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(THREE, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(THREE, DIAMONDS));
        cards.set(make_card(KING, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == THREE_OF_A_KIND);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(THREE, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(ACE, DIAMONDS));
        cards.set(make_card(KING, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == TWO_PAIR);
    }


    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(FOUR, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(ACE, DIAMONDS));
        cards.set(make_card(KING, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == PAIR);
    }

    {
        CardSet cards = 0;
        cards.set(make_card(ACE, SPADES));
        cards.set(make_card(FOUR, SPADES));
        cards.set(make_card(THREE, CLUBS));
        cards.set(make_card(QUEEN, DIAMONDS));
        cards.set(make_card(KING, DIAMONDS));
        cards.set(make_card(EIGHT, CLUBS));
        cards.set(make_card(NINE, SPADES));
        assert(get_hand_type(cards) == HIGH_CARD);
    }

    // Leaderboard tests

    {

        std::vector<CardSet> player_hands = {};
        {
            CardSet cards = 0;
            cards.set(make_card(ACE, SPADES));
            cards.set(make_card(TWO, SPADES));
            cards.set(make_card(THREE, SPADES));
            cards.set(make_card(FOUR, SPADES));
            cards.set(make_card(FIVE, SPADES));
            cards.set(make_card(EIGHT, SPADES));
            cards.set(make_card(NINE, SPADES));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(THREE, HEARTS));
            cards.set(make_card(THREE, SPADES));
            cards.set(make_card(THREE, CLUBS));
            cards.set(make_card(THREE, DIAMONDS));
            cards.set(make_card(ACE, DIAMONDS));
            cards.set(make_card(EIGHT, CLUBS));
            cards.set(make_card(NINE, SPADES));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(ACE, SPADES));
            cards.set(make_card(THREE, SPADES));
            cards.set(make_card(THREE, CLUBS));
            cards.set(make_card(THREE, DIAMONDS));
            cards.set(make_card(ACE, DIAMONDS));
            cards.set(make_card(EIGHT, CLUBS));
            cards.set(make_card(NINE, SPADES));
            player_hands.push_back(cards);
        }


        {
            CardSet cards = 0;
            cards.set(make_card(ACE, SPADES));
            cards.set(make_card(THREE, SPADES));
            cards.set(make_card(THREE, CLUBS));
            cards.set(make_card(THREE, DIAMONDS));
            cards.set(make_card(ACE, DIAMONDS));
            cards.set(make_card(EIGHT, SPADES));
            cards.set(make_card(FOUR, DIAMONDS));
            player_hands.push_back(cards);
        }

        const Leaderboard leaderboard = get_leaderboard(player_hands);
        std::string leaderboard_string = leaderboard_to_string(leaderboard);
        assert(leaderboard_string == "Strength, Player List\nSTRAIGHT_FLUSH (8388608): 0\nFOUR_OF_A_KIND (7340032): 1\nFULL_HOUSE (6291456): 2 3\n");
    }

    // // Table tests
    
    // {
    //     Table table(2);
    //     table.print();
    //     table.step();
    //     table.print();
    //     table.step();
    //     table.print();
    //     table.step();
    //     table.print();
    //     table.step();
    //     table.print();
    // }

    std::cout << "Tests Passed" << std::endl;
}
