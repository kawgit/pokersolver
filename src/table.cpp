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

Table::Table(size_t num_players, Chips init_stack)
  : num_players(num_players),
    player_cards(num_players, 0),
    player_stacks(num_players, 0),
    player_bets(num_players, 0),
    player_pots(num_players, 0),
    player_statuses(num_players, PLAYING),
    total_chips_at_table(num_players * init_stack)
{
  assert(num_players >= 2);
  assert(num_players <= 8);
  set_stacks(init_stack);
  assert_ok();
}

void Table::assert_ok() {
  assert(player_statuses.size() == num_players);
  assert(player_cards.size() == num_players);
  assert(player_bets.size() == num_players);
  assert(player_pots.size() == num_players);
  assert(player_stacks.size() == num_players);

  CardSet player_cards_union = 0;
  for (const CardSet cards : player_cards) {
    player_cards_union |= cards;
  }

  assert((player_cards_union | flop_cards | turn_cards | river_cards | deck_cards) == CARDSET_ALL);

  const int num_players_busted = std::count(player_statuses.begin(), player_statuses.end(), BUSTED);

  assert(deck_cards.all() || player_cards_union.count() == 2 * (num_players - num_players_busted));
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

  assert(max_bet == *std::max_element(player_bets.begin(), player_bets.end()));
  assert(pot == std::accumulate(player_pots.begin(), player_pots.end(), 0));
  assert(pot + std::accumulate(player_stacks.begin(), player_stacks.end(), 0) == total_chips_at_table);

  assert(player_to_move < num_players);
  assert(closer < num_players);

}

void Table::set_stack(Player player, Chips stack) {
  assert(player < num_players);
  assert(stack >= 0);

  total_chips_at_table -= player_stacks[player];
  total_chips_at_table += stack;
  player_stacks[player] = stack;
}

void Table::set_stacks(Chips stack) {
  assert(stack >= 0);

  for (Player player = 0; player < num_players; ++player) {
    player_stacks[player] = stack;
  }

  reset();
}

bool Table::is_over() {
  return std::count(player_statuses.begin(), player_statuses.end(), BUSTED) >= num_players - 1;
}

void Table::reset() {
  std::cout << "Reset" << std::endl;

  assert(player_cards.size() == num_players);
  assert(player_stacks.size() == num_players);
  assert(player_bets.size() == num_players);
  assert(player_pots.size() == num_players);
  assert(player_statuses.size() == num_players);

  deck_cards = CARDSET_ALL;
  flop_cards = CARDSET_NONE;
  turn_cards = CARDSET_NONE;
  river_cards = CARDSET_NONE;
  pot = 0;
  closer = 1;
  max_bet = 0;
  player_to_move = 0;

  std::fill(player_cards.begin(), player_cards.end(), CARDSET_NONE);
  std::fill(player_bets.begin(), player_bets.end(), 0);
  std::fill(player_pots.begin(), player_pots.end(), 0);
  std::fill(player_statuses.begin(), player_statuses.end(), PLAYING);
  
  for (Player player = 0; player < num_players; ++player) {
    if (player_stacks[player] == 0) {
      player_statuses[player] = BUSTED;
    }
  }

  std::cout << "  Player stacks: " << ints_to_string(player_stacks) << std::endl;
}

void Table::deal() {

  assert(!is_over());

  std::cout << "Deal" << std::endl;

  for (size_t i = 0; i < num_players; ++i) {
    if (player_statuses[i] == BUSTED) {
      continue;
    }

    player_cards[i].set(draw(deck_cards));
    player_cards[i].set(draw(deck_cards));

    std::cout << "  Player " << static_cast<int>(i) << " was dealt " << cardset_to_string(player_cards[i]) << std::endl;
  }

  bet(0, std::min(SMALL_BLIND, player_stacks[0]));
  bet(1, std::min(BIG_BLIND, player_stacks[1]));

  player_to_move = 2 % num_players;
  closer = 1;

}

void Table::flop() {
  std::cout << "Flop" << std::endl;

  assert(flop_cards.none());
  assert(turn_cards.none());
  assert(river_cards.none());

  flop_cards.set(draw(deck_cards));
  flop_cards.set(draw(deck_cards));
  flop_cards.set(draw(deck_cards));

  std::cout << "  Community Cards: " << cardset_to_string(flop_cards) << std::endl;

  player_to_move = num_players == 2 ? 1 : 0;
  closer = (player_to_move - 1) % num_players;
}

void Table::turn() {
  
  std::cout << "Turn" << std::endl;

  assert(flop_cards.count() == 3);
  assert(turn_cards.none());
  assert(river_cards.none());
  
  turn_cards = flop_cards;
  turn_cards.set(draw(deck_cards));

  std::cout << "  Community Cards: " << cardset_to_string(flop_cards);
  std::cout << " " << cardset_to_string(turn_cards & ~flop_cards) << std::endl;

  player_to_move = num_players == 2 ? 1 : 0;
  closer = (player_to_move - 1) % num_players;
}

void Table::river() {

  std::cout << "River" << std::endl;

  assert(flop_cards.count() == 3);
  assert(turn_cards.count() == 4);
  assert(river_cards.none());

  river_cards = turn_cards;
  river_cards.set(draw(deck_cards));

  std::cout << "  Community Cards: " << cardset_to_string(flop_cards);
  std::cout << " " << cardset_to_string(turn_cards & ~flop_cards);
  std::cout << " " << cardset_to_string(river_cards & ~turn_cards) << std::endl;

  player_to_move = num_players == 2 ? 1 : 0;
  closer = (player_to_move - 1) % num_players;
}

void Table::folds_to() {

  
  assert(std::count_if(player_statuses.begin(), player_statuses.end(), is_player_status_in) == 1);
  
  const Player player_still_in = std::find_if(player_statuses.begin(), player_statuses.end(), is_player_status_in) - player_statuses.begin();
  
  std::cout << "Folds to player " << player_still_in << std::endl;
  
  const Chips pot = collect_pots(player_pots[player_still_in]);
  award(player_still_in, pot);
  reset();
}

void Table::showdown() {

  std::cout << "Showdown" << std::endl;

  Leaderboard leaderboard = get_leaderboard(river_cards, player_cards, player_statuses);

  std::cout << leaderboard_to_string(leaderboard);

  for (size_t winning_level = 0; winning_level < leaderboard.size(); winning_level++) {
    
    std::vector<Player> winners = leaderboard[winning_level].players;

    std::sort(winners.begin(), winners.end(), [this](Player a, Player b) {
      return player_pots[a] < player_pots[b];
    });
    
    for (Player pot_limiter : winners) {
      Chips pot = collect_pots(player_pots[pot_limiter]);

      if (pot == 0) {
        continue;
      }

      for (Player player : winners) {
        award(player, pot / winners.size());
      }

      const Chips amount_awarded = pot / winners.size() * winners.size();

      if (amount_awarded != pot) {
        award(pot_limiter, pot - amount_awarded);
      }
    }
  }

  reset();
}

void Table::step() {

  assert_ok();

  if (deck_cards == CARDSET_ALL) {
    deal();
    return;
  }

  while (player_statuses[player_to_move] != PLAYING) {
    assert(player_to_move != closer);
    player_to_move = (player_to_move + 1) % num_players;
  }

  act();
  
  if (std::count(player_statuses.begin(), player_statuses.end(), PLAYING) < 2) {
    end_round();
    return;
  }

  do {
    if (player_to_move == closer) {
      end_round();
      return;
    }
    player_to_move = (player_to_move + 1) % num_players;
  } while (player_statuses[player_to_move] != PLAYING);
  
}

void Table::end_round() {
  collect_bets();

  if (std::count_if(player_statuses.begin(), player_statuses.end(), is_player_status_in) == 1) {
    folds_to();
  }
  else if (std::count(player_statuses.begin(), player_statuses.end(), PLAYING) < 2) {
    if (flop_cards.none()) flop();
    if (turn_cards.none()) turn();
    if (river_cards.none()) river();
    showdown();
  }
  else if (flop_cards.none()) {
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
}

void Table::act() {

  assert(player_bets[player_to_move] <= max_bet);
  assert(player_statuses[player_to_move] == PLAYING);

  const int action = get_random_int(0, 9);

  if (max_bet > player_bets[player_to_move] && action < 1) {
    fold(player_to_move);
  } else if (action < 8) {
    match(player_to_move);
  } else {
    raise(player_to_move, std::min(Chips(std::max(Chips(BIG_BLIND), Chips(2 * max_bet - player_bets[player_to_move]))), Chips(player_stacks[player_to_move])));
  }

}

void Table::fold(Player player) {
  std::cout << "  Player " << std::to_string(player) << " folds" << std::endl;

  assert(player_bets[player] < max_bet);
  assert(player_statuses[player_to_move] == PLAYING);

  player_statuses[player] = FOLDED;
}

void Table::match(Player player) {
  
  if (player_bets[player] == max_bet) {
    std::cout << "  Player " << std::to_string(player) << " checks" << std::endl;
    return;
  }
  
  assert(max_bet >= 2 * player_bets[player]);
  bet(player, std::min(max_bet - player_bets[player], player_stacks[player]));
  std::cout << "  Player " << std::to_string(player) << " calls. The pot is now " << std::to_string(pot) << std::endl;
}

void Table::raise(Player player, Chips amount) {
  assert(amount + player_bets[player] >= BIG_BLIND || amount == player_stacks[player]);
  assert(amount <= player_stacks[player]);
  assert(amount + player_bets[player] >= 2 * max_bet || amount == player_stacks[player]);

  bet(player, amount);
  closer = (player - 1) % num_players;
  std::cout << "  Player " << std::to_string(player) << " raises to " << std::to_string(player_bets[player]) << ". The pot is now " << std::to_string(pot) << std::endl;
}

void Table::bet(Player player, Chips amount) {
  assert(amount >= SMALL_BLIND || amount == player_stacks[player]);
  assert(amount <= player_stacks[player]);
  assert(amount + player_bets[player] == max_bet || amount + player_bets[player] >= 2 * max_bet || amount == player_stacks[player]);

  player_stacks[player] -= amount;
  player_bets[player] += amount;
  player_pots[player] += amount;

  max_bet = std::max(max_bet, player_bets[player]);
  pot += amount;

  if (player_stacks[player] == 0) {
    player_statuses[player] = ALL_IN;
  }
}

void Table::award(Player player, Chips amount) {
  player_stacks[player] += amount;

  std::cout << "  Player " << std::to_string(player) << " wins " << std::to_string(amount) << std::endl;
}

void Table::collect_bets() {
  for (Player player = 0; player < num_players; ++player) {
    player_bets[player] = 0;
  }
  max_bet = 0;
}

Chips Table::collect_pots(Chips amount) {
  Chips collected = 0;

  for (Player player = 0; player < num_players; ++player) {
    const Chips amount_to_collect = std::min(player_pots[player], amount);
    player_pots[player] -= amount_to_collect;
    pot -= amount_to_collect;
    collected += amount_to_collect;
  }

  return collected;
}

void Table::print() {
  std::cout << "--- Table State ---" << std::endl;
  std::cout << "Pot: " << pot << std::endl;
  std::cout << "Max Bet: " << max_bet << std::endl;
  std::cout << "Player to Move: " << static_cast<int>(player_to_move) << std::endl;
  std::cout << "Community Cards: ";
  std::cout << cardset_to_string(river_cards | turn_cards | flop_cards) << std::endl;

  std::cout << "Player Bets" << std::endl;
  for (Player i = 0; i < num_players; ++i) {
    std::cout << "  Player " << static_cast<int>(i) << " Bet: " << player_bets[i] << std::endl;
  }

  for (Player i = 0; i < num_players; ++i) {
    std::cout << "Player " << static_cast<int>(i) << ":" << std::endl;
    std::cout << "  Status: " << status_to_string(player_statuses[i]) << std::endl;
    std::cout << "  Hand: " << cardset_to_string(player_cards[i]) << std::endl;
    std::cout << "  Stack: " << player_stacks[i] << std::endl;
    std::cout << "  Current Bet: " << player_bets[i] << std::endl;
    std::cout << "  Total Pot Contribution: " << player_pots[i] << std::endl;
  }
  std::cout << "-------------------" << std::endl;
}
