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