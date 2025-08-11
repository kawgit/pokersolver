#include <iostream>

#include "showdown.h"
#include "table.h"
#include "types.h"
#include "util.h"

int main() {

    srand(0);

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
        assert(get_hand_type(get_hand_strength(cards)) == STRAIGHT_FLUSH);
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
        assert(get_hand_type(get_hand_strength(cards)) == FOUR_OF_A_KIND);
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
        assert(get_hand_type(get_hand_strength(cards)) == FULL_HOUSE);
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
        assert(get_hand_type(get_hand_strength(cards)) == FLUSH);
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
        assert(get_hand_type(get_hand_strength(cards)) == STRAIGHT);
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
        assert(get_hand_type(get_hand_strength(cards)) == THREE_OF_A_KIND);
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
        assert(get_hand_type(get_hand_strength(cards)) == TWO_PAIR);
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
        assert(get_hand_type(get_hand_strength(cards)) == PAIR);
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
        assert(get_hand_type(get_hand_strength(cards)) == HIGH_CARD);
    }

    // Hand kicker tests

    {
        // Community cards (river)
        CardSet river = 0;
        river.set(make_card(ACE, SPADES));
        river.set(make_card(FOUR, HEARTS));
        river.set(make_card(SEVEN, CLUBS));
        river.set(make_card(NINE, DIAMONDS));
        river.set(make_card(JACK, SPADES));

        // Player 0: Pair of 7s with kicker ACE, 9, 4
        CardSet hand0 = river;
        hand0.set(make_card(SEVEN, SPADES));
        hand0.set(make_card(TWO, CLUBS));

        // Player 1: Pair of 7s with kicker ACE, JACK, 9  (better kicker than player 0)
        CardSet hand1 = river;
        hand1.set(make_card(SEVEN, DIAMONDS));
        hand1.set(make_card(QUEEN, CLUBS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == PAIR);
        assert(get_hand_type(s1) == PAIR);

        assert(s1 > s0);  // Player 1 wins due to better kicker (queen > nine)
    }

    {
        CardSet river = 0;
        river.set(make_card(KING, HEARTS));
        river.set(make_card(KING, CLUBS));
        river.set(make_card(FOUR, DIAMONDS));
        river.set(make_card(FOUR, SPADES));
        river.set(make_card(TWO, CLUBS));

        // Player 0: Two pair (Kings and Fours) with kicker 9
        CardSet hand0 = river;
        hand0.set(make_card(NINE, HEARTS));
        hand0.set(make_card(THREE, CLUBS));

        // Player 1: Two pair (Kings and Fives) with kicker 2
        CardSet hand1 = river;
        hand1.set(make_card(FIVE, HEARTS));
        hand1.set(make_card(FIVE, CLUBS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == TWO_PAIR);
        assert(get_hand_type(s1) == TWO_PAIR);

        assert(s1 > s0);  // Player 1 wins because second pair (5s) > 4s
    }

    {
        CardSet river = 0;
        river.set(make_card(TEN, SPADES));
        river.set(make_card(TEN, DIAMONDS));
        river.set(make_card(TEN, HEARTS));
        river.set(make_card(FOUR, CLUBS));
        river.set(make_card(TWO, DIAMONDS));

        // Player 0: Trips Tens + kickers 9 and 8
        CardSet hand0 = river;
        hand0.set(make_card(NINE, SPADES));
        hand0.set(make_card(EIGHT, CLUBS));

        // Player 1: Trips Tens + kickers Ace and 3 (better kicker)
        CardSet hand1 = river;
        hand1.set(make_card(ACE, CLUBS));
        hand1.set(make_card(THREE, DIAMONDS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == THREE_OF_A_KIND);
        assert(get_hand_type(s1) == THREE_OF_A_KIND);

        assert(s1 > s0);  // Player 1 wins with better kickers
    }

    {
        // River cards containing 4 spades (not a flush yet)
        CardSet river = 0;
        river.set(make_card(TWO, SPADES));
        river.set(make_card(FIVE, SPADES));
        river.set(make_card(EIGHT, SPADES));
        river.set(make_card(THREE, SPADES));
        river.set(make_card(SIX, DIAMONDS));

        // Player 0: Flush spades with kicker Q high
        CardSet hand0 = river;
        hand0.set(make_card(QUEEN, SPADES));
        hand0.set(make_card(FOUR, DIAMONDS));

        // Player 1: Flush spades with kicker K high (better flush)
        CardSet hand1 = river;
        hand1.set(make_card(KING, SPADES));
        hand1.set(make_card(NINE, CLUBS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == FLUSH);
        assert(get_hand_type(s1) == FLUSH);

        assert(s1 > s0);  // Player 1 wins with higher flush card (King > Queen)
    }

    {
        CardSet river = 0;
        river.set(make_card(FIVE, HEARTS));
        river.set(make_card(SIX, DIAMONDS));
        river.set(make_card(SEVEN, CLUBS));
        river.set(make_card(EIGHT, SPADES));
        river.set(make_card(TEN, HEARTS));

        // Player 0: Straight 6-7-8-9-10 (9 high)
        CardSet hand0 = river;
        hand0.set(make_card(NINE, CLUBS));
        hand0.set(make_card(TWO, DIAMONDS));

        // Player 1: Straight 7-8-9-10-J (Jack high, better)
        CardSet hand1 = river;
        hand1.set(make_card(NINE, DIAMONDS));
        hand1.set(make_card(JACK, CLUBS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == STRAIGHT);
        assert(get_hand_type(s1) == STRAIGHT);

        assert(s1 > s0);  // Player 1 wins with higher straight (Jack high > 9 high)
    }

    {
        // Community cards: Full house possible: Three 9s and two 4s
        CardSet river = 0;
        river.set(make_card(NINE, HEARTS));
        river.set(make_card(NINE, CLUBS));
        river.set(make_card(NINE, DIAMONDS));
        river.set(make_card(FOUR, SPADES));
        river.set(make_card(FOUR, DIAMONDS));

        // Player 0 hole cards: 3♠ 2♣ (irrelevant, full house formed from community)
        CardSet hand0 = river;
        hand0.set(make_card(THREE, SPADES));
        hand0.set(make_card(TWO, CLUBS));

        // Player 1 hole cards: 5♠ 6♣ (irrelevant, same full house)
        CardSet hand1 = river;
        hand1.set(make_card(FIVE, SPADES));
        hand1.set(make_card(SIX, CLUBS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == FULL_HOUSE);
        assert(get_hand_type(s1) == FULL_HOUSE);

        assert(s0 == s1);  // Exact tie (chop)
    }

    {
        // Community cards: A♦ K♠ Q♣ J♣ 9♠
        CardSet river = 0;
        river.set(make_card(ACE, DIAMONDS));
        river.set(make_card(KING, SPADES));
        river.set(make_card(QUEEN, CLUBS));
        river.set(make_card(JACK, CLUBS));
        river.set(make_card(NINE, SPADES));

        // Player 0 hole cards: 8♦ 7♠
        CardSet hand0 = river;
        hand0.set(make_card(EIGHT, DIAMONDS));
        hand0.set(make_card(SEVEN, SPADES));

        // Player 1 hole cards: 8♣ 7♦ (same kickers)
        CardSet hand1 = river;
        hand1.set(make_card(EIGHT, CLUBS));
        hand1.set(make_card(SEVEN, DIAMONDS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == HIGH_CARD);
        assert(get_hand_type(s1) == HIGH_CARD);

        assert(s0 == s1);  // Exact tie (chop)
    }

    {
        // Community cards: K♠ K♦ 5♣ 5♦ 2♠
        CardSet river = 0;
        river.set(make_card(KING, SPADES));
        river.set(make_card(KING, DIAMONDS));
        river.set(make_card(FIVE, CLUBS));
        river.set(make_card(FIVE, DIAMONDS));
        river.set(make_card(TWO, SPADES));

        // Player 0 hole cards: 9♠ 3♣ (kicker 9)
        CardSet hand0 = river;
        hand0.set(make_card(NINE, SPADES));
        hand0.set(make_card(THREE, CLUBS));

        // Player 1 hole cards: 9♦ 3♦ (same kicker 9)
        CardSet hand1 = river;
        hand1.set(make_card(NINE, DIAMONDS));
        hand1.set(make_card(THREE, DIAMONDS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == TWO_PAIR);
        assert(get_hand_type(s1) == TWO_PAIR);

        assert(s0 == s1);  // Exact tie (chop)
    }

    {
        // Community cards: K♠ K♦ 5♣ 5♦ A♠ — two pair (Kings and Fives) + Ace kicker on board
        CardSet river = 0;
        river.set(make_card(KING, SPADES));
        river.set(make_card(KING, DIAMONDS));
        river.set(make_card(FIVE, CLUBS));
        river.set(make_card(FIVE, DIAMONDS));
        river.set(make_card(ACE, SPADES));  // High kicker on board

        // Player 0 hole cards: 9♠ 3♣ — no pair, hole cards lower than board kicker
        CardSet hand0 = river;
        hand0.set(make_card(NINE, SPADES));
        hand0.set(make_card(THREE, CLUBS));

        // Player 1 hole cards: 8♦ 4♦ — no pair, hole cards lower than board kicker
        CardSet hand1 = river;
        hand1.set(make_card(EIGHT, DIAMONDS));
        hand1.set(make_card(FOUR, DIAMONDS));

        HandStrength s0 = get_hand_strength(hand0);
        HandStrength s1 = get_hand_strength(hand1);

        assert(get_hand_type(s0) == TWO_PAIR);
        assert(get_hand_type(s1) == TWO_PAIR);

        // Both players have the same two pair and the same highest kicker (Ace on board),
        // so their hand strengths must be equal → chop.
        assert(s0 == s1);
    }

    // Leaderboard tests

    {

        CardSet community_cards = 0;

        community_cards.set(make_card(TEN, SPADES));
        community_cards.set(make_card(KING, SPADES));
        community_cards.set(make_card(QUEEN, SPADES));
        community_cards.set(make_card(FOUR, CLUBS));
        community_cards.set(make_card(FOUR, DIAMONDS));

        std::vector<CardSet> player_hands = {};
        std::vector<PlayerStatus> player_statuses = {};
        {
            CardSet cards = 0;
            cards.set(make_card(TEN, DIAMONDS));
            cards.set(make_card(TEN, CLUBS));
            player_hands.push_back(cards);
            player_statuses.push_back(PLAYING);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(NINE, HEARTS));
            cards.set(make_card(JACK, SPADES));
            player_hands.push_back(cards);
            player_statuses.push_back(PLAYING);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(ACE, SPADES));
            cards.set(make_card(THREE, SPADES));
            player_hands.push_back(cards);
            player_statuses.push_back(PLAYING);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(TWO, CLUBS));
            cards.set(make_card(THREE, CLUBS));
            player_hands.push_back(cards);
            player_statuses.push_back(PLAYING);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(TWO, DIAMONDS));
            cards.set(make_card(THREE, DIAMONDS));
            player_hands.push_back(cards);
            player_statuses.push_back(PLAYING);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(TWO, HEARTS));
            cards.set(make_card(THREE, HEARTS));
            player_hands.push_back(cards);
            player_statuses.push_back(FOLDED);
        }

        const Leaderboard leaderboard = get_leaderboard(community_cards, player_hands, player_statuses);
        std::string leaderboard_string = leaderboard_to_string(leaderboard);
        assert(leaderboard_string == "  Leaderboard\n    FULL_HOUSE (13647872): 0\n    FLUSH (10493186): 2\n    STRAIGHT (8390656): 1\n    PAIR (2362624): 3 4\n");
    }

    // Table tests
    
    {
        Table table(2);
        for (int i = 0; i < 1000000; i++) {
            table.step();

            if (table.is_over()) {
                table.set_stacks(100 * BIG_BLIND);
            }
        }
        table.print();
    }

    std::cout << "Tests Passed" << std::endl;
}
