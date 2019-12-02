#ifndef _CHECKERS_PLAYER_HPP_
#define _CHECKERS_PLAYER_HPP_

#include "constants.hpp"
#include "deadline.hpp"
#include "move.hpp"
#include "gamestate.hpp"
#include <vector>
#include <functional>
#include <unordered_set>
#include <string>
#include <string.h>

namespace checkers
{

class Player
{
public:
    ///perform a move
    ///\param pState the current state of the board
    ///\param pDue time before which we must have returned
    ///\return the next state the board is in after our move
    GameState play(const GameState &pState, const Deadline &pDue);
    int minimax(const GameState &pState, uint8_t player, int depth, int alpha, int beta, Deadline due);
    int heusticFunc(const GameState &pState, uint8_t player);
    int getOrderNumber(const GameState &state);
    void orderStates(std::vector<GameState> &NextStates, bool flag);
    int hashFunc(std::string s);
    uint8_t me;
    uint8_t opponent;
    std::unordered_set<int> hashSet;
};

/*namespace checkers*/ }

#endif
