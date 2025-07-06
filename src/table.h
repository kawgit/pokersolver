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
