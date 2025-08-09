Below I have pasted the contents of all the files in the src directory of my cpp poker solver project so far. Please refrain from using comments if possible within your code. Use snake casing rather than camel casing.
Please help me build table.cpp's print function.



CMakeLists.txt:
cmake_minimum_required(VERSION 3.20)
project(poker_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find LibTorch (set this to the absolute path of your libtorch directory)
set(Torch_DIR "/Volumes/Case-Sensitive/Packages/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)

add_executable(poker_solver
  main.cpp
  # table.cpp
  # util.cpp
)

add_executable(test
  test.cpp
  util.cpp
  showdown.cpp
  table.cpp
)

target_include_directories(poker_solver PUBLIC src)
target_include_directories(test PUBLIC src)

# Link LibTorch to your executable
target_link_libraries(poker_solver "${TORCH_LIBRARIES}")
set_property(TARGET poker_solver PROPERTY CXX_STANDARD 23)

target_link_libraries(test "${TORCH_LIBRARIES}")
set_property(TARGET test PROPERTY CXX_STANDARD 23)

# Optional: avoid missing symbols when using CUDA builds
set_property(TARGET poker_solver PROPERTY INTERFACE_LINK_LIBRARIES "${TORCH_LIBRARIES}")
set_property(TARGET test PROPERTY INTERFACE_LINK_LIBRARIES "${TORCH_LIBRARIES}")

cardset.h:
#include "types.h"

constexpr CardSet CARDSET_NONE = 0;
constexpr CardSet CARDSET_ALL = (1ULL << NUM_CARDS) - 1;
constexpr CardSetSuitless CARDSETSUITLESS_NONE = 0;
constexpr CardSetSuitless CARDSETSUITLESS_ALL = (1ULL << NUM_RANKS) - 1;

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

main.cpp:
#include <torch/torch.h>
#include <iostream>

int main() {
    torch::Tensor tensor = torch::rand({2, 3});
    std::cout << tensor << std::endl;
}

showdown.cpp:
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

showdown.h:
#pragma once

#include <stdint.h>

#include "types.h"

/*
    1 state for straight flush
    1 state for four of a kind
    1 state for full house
    1 state for flush
    1 state for straight
    1 state for three of a kind
    1 state for two pair
    1 state for pair
    1 state for high card

    3 bits for hand type
    20 bits for kickers (4 bits per card)
    22 bits total
    Thus, we can use a 32 bit int to store the hand strength
*/

enum HandTypes : HandType {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    NUM_HANDTYPES
};

constexpr HandType get_hand_type(HandStrength strength) {
    return strength >> 20;
}

constexpr HandKickers get_hand_kickers(HandStrength strength) {
    return strength & ((1L << 20) - 1);
}

constexpr HandStrength make_hand_strength(HandType handtype, HandKickers kickers) {
    return (handtype << 20) | kickers;
}

HandType get_hand_type(CardSet cards);
HandStrength get_hand_strength(CardSet cards);
Leaderboard get_leaderboard(CardSet river_cards, std::vector<CardSet> player_cards);

table.cpp:
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "cardset.h"
#include "showdown.h"
#include "table.h"
#include "util.h"

Table::Table(size_t num_players, Stack init_stack)
  : num_players(num_players),
    player_cards(num_players, 0),
    player_stacks(num_players, 0),
    player_bets(num_players, 0),
    player_pots(num_players, 0),
    player_folds(num_players, 0),
    total_chips_at_table(num_players * init_stack)
{
  assert(num_players >= 2);
  assert(num_players <= 8);
  reset();
  set_stacks(init_stack);
  assert_ok();
}

void Table::assert_ok() {
  assert(player_folds.size() == num_players);
  assert(player_cards.size() == num_players);
  assert(player_bets.size() == num_players);
  assert(player_pots.size() == num_players);
  assert(player_stacks.size() == num_players);

  CardSet player_cards_union = 0;
  for (const CardSet cards : player_cards) {
    player_cards_union |= cards;
  }

  assert(deck_cards.all() || player_cards_union.count() == 2 * num_players);
  assert((player_cards_union & deck_cards).none());
  
  if (flop_cards.none()) {
    assert(turn_cards.none());
    assert(river_cards.none());
    assert(deck_cards.count() + player_cards_union.count() == 52);
  }
  else if (turn_cards.none()) {
    assert(flop_cards.count() == 3);
    assert(river_cards.none());
    assert(deck_cards.count() + player_cards_union.count() == 49);
  }
  else if (river_cards.none()) {
    assert(flop_cards.count() == 3);
    assert(turn_cards.count() == 4);
    assert(deck_cards.count() + player_cards_union.count() == 48);
  }
  else {
    assert(flop_cards.count() == 3);
    assert(turn_cards.count() == 4);
    assert(river_cards.count() == 5);
  }
  std::cout << std::to_string(max_bet) << std::endl;
  std::cout << ints_to_string<Stack>(player_bets) << std::endl;
  assert(max_bet == *std::max_element(player_bets.begin(), player_bets.end()));
  
  Stack pot_total = 0;
  Stack stack_total = 0;
  for (int i = 0; i < num_players; ++i) {
    pot_total += player_bets[i];
    pot_total += player_pots[i];
    stack_total += player_stacks[i];
  }
  assert(pot == pot_total);

  assert(player_to_move < num_players);
  assert(closer < num_players);

  assert(stack_total + pot_total == total_chips_at_table);

}

void Table::set_stack(Player player, Stack stack) {
  assert(player < num_players);
  assert(stack >= 0);

  total_chips_at_table -= player_stacks[player];
  total_chips_at_table += stack;
  player_stacks[player] = stack;
}

void Table::set_stacks(Stack stack) {
  assert(stack >= 0);

  for (Player player = 0; player < num_players; ++player) {
    player_stacks[player] = stack;
  }
}

void Table::reset() {
  assert(player_cards.size() == num_players);
  assert(player_stacks.size() == num_players);
  assert(player_bets.size() == num_players);
  assert(player_pots.size() == num_players);
  assert(player_folds.size() == num_players);

  deck_cards = CARDSET_ALL;
  flop_cards = CARDSET_NONE;
  turn_cards = CARDSET_NONE;
  river_cards = CARDSET_NONE;
  pot = 0;

  std::fill(player_folds.begin(), player_folds.end(), true);
  std::fill(player_cards.begin(), player_cards.end(), CARDSET_NONE);
  std::fill(player_bets.begin(), player_bets.end(), 0);
  std::fill(player_pots.begin(), player_pots.end(), 0);
}

void Table::deal() {
  for (size_t i = 0; i < num_players; ++i) {
    player_cards[i].set(draw(deck_cards));
    player_cards[i].set(draw(deck_cards));
  }

  bet(0, SMALL_BLIND);
  bet(1, BIG_BLIND);

  player_to_move = 2 % num_players;
  closer = 1;
}

void Table::flop() {
  flop_cards.set(draw(deck_cards));
  flop_cards.set(draw(deck_cards));
  flop_cards.set(draw(deck_cards));

  player_to_move = num_players == 2 ? 1 : 0;
  closer = num_players - 1;
}

void Table::turn() {
  turn_cards = flop_cards;
  turn_cards.set(draw(deck_cards));

  player_to_move = num_players == 2 ? 1 : 0;
  closer = num_players - 1;
}

void Table::river() {
  river_cards = turn_cards;
  river_cards.set(draw(deck_cards));

  player_to_move = num_players == 2 ? 1 : 0;
  closer = num_players - 1;
}

void Table::showdown() {

  Leaderboard leaderboard = get_leaderboard(river_cards, player_cards);

  for (size_t winning_level = 0; winning_level < leaderboard.size(); winning_level++) {
    
    std::vector<Player> winners = leaderboard[winning_level].players;

    std::sort(winners.begin(), winners.end(), [this](Player a, Player b) {
      return player_pots[a] < player_pots[b];
    });
    
    std::cout << ints_to_string<Player>(winners) << std::endl;

    // for (size_t losing_level = winning_level; losing_level < leaderboard.size(); losing_level++) {
      
    // }
  }
}

void Table::step() {
  if (deck_cards == CARDSET_ALL) {
    deal();
  }
  
  act();

  if (player_to_move != closer) {
    player_to_move = (player_to_move + 1) % num_players;
    assert_ok();
    return;
  }

  collect_bets();
  if (flop_cards.none()) {
    flop();
  }
  else if (turn_cards.none()) {
    turn();
  }
  else if (river_cards.none()) {
    river();
  }
  else {
    showdown();
  }

  assert_ok();
}

void Table::act() {

  assert(player_bets[player_to_move] <= max_bet);

  if (is_all_in(player_to_move) || is_folded(player_to_move)) {
    return;
  }

  const int action = rand() % 10;

  if (max_bet > player_bets[player_to_move] && action <= 2) {
    fold(player_to_move);
  } else if (action <= 5) {
    match(player_to_move);
  } else {
    raise(player_to_move, std::max(int(BIG_BLIND), 2 * max_bet));
  }

}

void Table::fold(Player player) {
  std::cout << "Player " << std::to_string(player) << " folds" << std::endl;

  assert(player_bets[player] < max_bet);

  player_folds[player] = true;
}

void Table::match(Player player) {
  
  if (player_bets[player] == max_bet) {
    std::cout << "Player " << std::to_string(player) << " checks" << std::endl;
    
    return;
  }
  
  std::cout << "Player " << std::to_string(player) << " calls" << std::endl;
  
  assert(max_bet >= 2 * player_bets[player]);

  bet(player, max_bet - player_bets[player]);
}

void Table::raise(Player player, Stack amount) {
  std::cout << "Player " << std::to_string(player) << " raises " << std::to_string(amount) << std::endl;

  assert(amount >= BIG_BLIND);
  assert(amount >= 2 * max_bet);
  assert(amount <= player_stacks[player]);

  bet(player, amount);
}

void Table::bet(Player player, Stack amount) {
  
  assert(amount >= SMALL_BLIND);
  assert(amount <= player_stacks[player]);

  player_stacks[player] -= amount;
  player_bets[player] += amount;

  max_bet = std::max(max_bet, player_bets[player]);
  pot += amount;
  closer = (player - 1) % num_players;
}

void Table::collect_bets() {
  for (Player player = 0; player < num_players; ++player) {
    player_pots[player] += player_bets[player];
    player_bets[player] = 0;
  }
  max_bet = 0;
}

bool Table::is_folded(Player player) {
  assert(player < num_players);
  return !player_folds[player];
}

bool Table::is_all_in(Player player) {
  assert(player < num_players);
  return player_stacks[player] == 0 && player_folds[player];
}

void Table::print() {

}

table.h:
#pragma once

#include <deque>
#include <vector>

#include "types.h"

class Table {
    public:
        Table(size_t num_players, Stack init_stack=100*BIG_BLIND); // should initialize table without dealing cards
        void print();
        void step();
        void set_stack(Player player, Stack stack);
        void set_stacks(Stack stack);

    private:
        void assert_ok();

        // card revealing functions (the step function should call one of these each time its called)
        void reset();
        void deal();
        void flop();
        void turn();
        void river();
        void showdown(); // should be called at the end of the game, determining the winner and awarding the pot. step should only call showdown, not showdown and reset
        
        // action functions
        void act(); // should be used to allow a the current player to choose an action on their turn. for now, have them do some random action
        void fold(Player player);
        void match(Player player);
        void raise(Player player, Stack amount);

        // helpers
        void bet(Player player, Stack amount);
        void collect_bets();
        bool is_folded(Player player);
        bool is_all_in(Player player);

    private:
        CardSet deck_cards;
        CardSet flop_cards;
        CardSet river_cards;
        CardSet turn_cards;
        size_t num_players;
        Player closer; // if at any point this person calls / checks, the betting round ends
        Player player_to_move;
        Stack max_bet; // the max of all player bets
        Stack pot; // the sum of all player bets (including past betting rounds)
        std::vector<bool> player_folds;
        std::vector<CardSet> player_cards;
        std::vector<Stack> player_bets;
        std::vector<Stack> player_pots;
        std::vector<Stack> player_stacks;
        Stack total_chips_at_table;
        
};

test.cpp:
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

        CardSet community_cards = 0;

        community_cards.set(make_card(TEN, SPADES));
        community_cards.set(make_card(KING, SPADES));
        community_cards.set(make_card(QUEEN, SPADES));
        community_cards.set(make_card(FOUR, CLUBS));
        community_cards.set(make_card(FOUR, DIAMONDS));

        std::vector<CardSet> player_hands = {};
        {
            CardSet cards = 0;
            cards.set(make_card(TEN, DIAMONDS));
            cards.set(make_card(TEN, CLUBS));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(NINE, HEARTS));
            cards.set(make_card(JACK, SPADES));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(ACE, SPADES));
            cards.set(make_card(THREE, SPADES));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(TWO, CLUBS));
            cards.set(make_card(THREE, CLUBS));
            player_hands.push_back(cards);
        }

        {
            CardSet cards = 0;
            cards.set(make_card(TWO, DIAMONDS));
            cards.set(make_card(THREE, DIAMONDS));
            player_hands.push_back(cards);
        }

        const Leaderboard leaderboard = get_leaderboard(community_cards, player_hands);
        std::string leaderboard_string = leaderboard_to_string(leaderboard);
        assert(leaderboard_string == "Strength, Player List\nFULL_HOUSE (6291456): 0\nFLUSH (5242880): 2\nSTRAIGHT (4194304): 1\nPAIR (1048576): 3 4\n");
    }

    // Table tests
    
    {
        Table table(2);
        table.print();
        table.step();
        table.print();
        table.step();
        table.print();
        table.step();
        table.print();
        table.step();
        table.print();
    }

    std::cout << "Tests Passed" << std::endl;
}

types.h:
#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <deque>
#include <stdint.h>
#include <vector>

typedef uint8_t Card;
typedef uint8_t Rank;
typedef uint8_t Suit;
typedef uint8_t Player;
typedef int Stack;
typedef std::bitset<52> CardSet;
typedef std::bitset<13> CardSetSuitless;
typedef uint8_t HandType;
typedef uint32_t HandKickers;
typedef uint32_t HandStrength;

struct LeaderboardLevel {
    HandStrength strength;
    std::vector<Player> players;
};

typedef std::deque<LeaderboardLevel> Leaderboard;

enum Ranks : Rank {
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    NUM_RANKS
};

enum Suits : Suit {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES,
    NUM_SUITS
};

enum Cards : Card {
    NUM_CARDS = NUM_RANKS * NUM_SUITS,
    NO_CARD = NUM_CARDS
};

enum Stacks : Stack {
    SMALL_BLIND = 1,
    BIG_BLIND = 2
};

constexpr Card make_card(Rank rank, Suit suit) {
    assert(rank <= NUM_RANKS);
    assert(suit <= NUM_SUITS);
    return suit * NUM_RANKS + rank;
}

constexpr Rank get_rank(Card card) {
    return card % NUM_RANKS;
}

constexpr Suit get_suit(Card card) {
    return card / NUM_RANKS;
}

util.cpp:
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
    std::string result = set.to_string() + ": [";
    for (size_t i = 0; i < NUM_CARDS; i++) {
        if (set[i]) {
            result += card_to_string(i);
            result += ", ";
        }
    }
    result += "]";
    return result;
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

util.h:
#pragma once

#include <array>
#include <string>
#include <cassert>

#include "types.h"

template <typename T>
std::string ints_to_string(std::vector<T> players);

std::string rank_to_string(Rank rank);
std::string suit_to_string(Suit suit);
std::string card_to_string(Card card);
std::string cardset_to_string(CardSet set);
std::string leaderboard_to_string(const Leaderboard& leaderboard);

int get_random_int(int min, int max);

Card draw_and_replace(CardSet& set);
Card draw(CardSet& set);

constexpr std::array<std::array<uint64_t, 27>, 53> NCR_LOOKUP = []() {
    std::array<std::array<uint64_t, 27>, 53> lookup = { 0 };

    for (int n = 0; n < 53; ++n) {
        lookup[n][0] = 1;
    }

    for (int n = 1; n < 53; ++n) {
        for (int r = 1; r < 27; ++r) {
            lookup[n][r] = lookup[n-1][r-1] + lookup[n-1][r];
        }
    }

    return lookup;
}();

constexpr uint64_t ncr(int n, int r) {
    assert(0 <= n);
    assert(0 <= r);
    assert(n <= 52);
    assert(r <= 52);
    r = n >= r ? std::min(r, n - r) : r;
    assert(r <= 26);
    return NCR_LOOKUP[n][r];
}

constexpr Card pop_card(CardSet& x) {
    assert(x.any());
    const Card index = __builtin_ctzll(x.to_ullong());
    x.reset(index);
    return index;
}

