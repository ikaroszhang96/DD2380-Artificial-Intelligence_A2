#include "player.hpp"
#include <cstdlib>
#include <math.h>
#include <set>
#include <algorithm>
#include <math.h>
#include <functional>
#include <unordered_set>
#include <string>
#include <string.h>
#include <map>
#define VALUE 999999999
#define MAX_DEPTH 8

namespace checkers
{
std::map<int, int> Max_map;  //key: hash value,  val: max value of sub layer.
std::map<int, int> Min_map;  //key: hash value,  val: max value of sub layer.
std::map<int, int> layer;
GameState Player::play(const GameState &pState,const Deadline &pDue)
{
    me = pState.getNextPlayer();
    opponent = me ^ (CELL_WHITE | CELL_RED);
    
    std::vector<GameState> lNextStates;
    pState.findPossibleMoves(lNextStates);

    if (lNextStates.size() == 0) return GameState(pState, Move());

	int num_nextState = lNextStates.size();
    int max_index = 0;
    int maxVal = -VALUE;
    int alpha = -VALUE;
    int beta  =  VALUE;
    int depth = MAX_DEPTH;
    int minimax_res;
    for(int index = 0; index < num_nextState; index++){
        minimax_res = minimax( lNextStates[index] , opponent, depth, alpha, beta, pDue);
        if(minimax_res >= maxVal){
            maxVal = minimax_res;
            max_index = index;
        }
        //alpha = std::max(maxVal, alpha);
    }

    return lNextStates[max_index];        

    /*
     * Here you should write your clever algorithms to get the best next move, ie the best
     * next state. This skeleton returns a random move instead.
     */
    //return lNextStates[rand() % lNextStates.size()];
}

int Player::minimax(const GameState &pState, const uint8_t player, int depth, int alpha, int beta, Deadline due){
    
    if(depth == 0){
        return heusticFunc(pState, player);
    }
    
    std::vector<GameState> NextStates;
    pState.findPossibleMoves(NextStates);
    
    if (NextStates.size() == 0){
        return heusticFunc(pState, player);
    }
    // when it is not over, iterate the sub-states.
    
    int v;
    //hashSet.clear();

    if(player == me){  // maximize
        v = -VALUE;
        orderStates(NextStates, false);
        //random_shuffle(NextStates.begin(), NextStates.end());
        for(int i = 0; i < NextStates.size(); i++){
        	if( due.getSeconds() - due.now().getSeconds() < 0.00001 ) break;
        	//if( hashSet.find( hashFunc( NextStates[i].toMessage() ) ) != hashSet.end() ) continue;
        	//else hashSet.insert( hashFunc( NextStates[i].toMessage() ) );
            int hash_val = hashFunc( NextStates[i].toMessage() );
            if(/*depth >= 6*/hashSet.find( hash_val ) == hashSet.end() ){
                
                hashSet.insert( hash_val );
                int map_val = minimax(NextStates[i], opponent, depth-1, alpha, beta, due);
                Max_map[hash_val] = map_val;
                layer[hash_val] = depth;
                v = std::max(v, map_val);

            }
            else{
                /*if (layer[hash_val] < depth) {
                    int map_val = minimax(NextStates[i], opponent, depth-1, alpha, beta, due);
                    Max_map[hash_val] = map_val;
                    v = std::max(v, map_val);
                }*/
                //if( hashSet.find( hash_val ) != hashSet.end() ){
                if (layer[hash_val]>=depth) {
                    v = std::max(v, Max_map[hash_val]);
                }
                //}
                else v = std::max(v, minimax(NextStates[i], opponent, depth-1, alpha, beta, due) );
            }

            alpha = std::max(alpha, v);
            if( alpha >= beta ){
                break;
            }
        }
    }

    else {  // minimize
        v = VALUE;
        orderStates(NextStates, true);
        //random_shuffle(NextStates.begin(), NextStates.end());
        for(int i = 0; i < NextStates.size(); i++){
        	if( due.getSeconds() - due.now().getSeconds() < 0.00001 ) break;
        	//if( hashSet.find( hashFunc( NextStates[i].toMessage() ) ) != hashSet.end() ) continue;
        	//else hashSet.insert( hashFunc( NextStates[i].toMessage() ) );
            int hash_val = hashFunc( NextStates[i].toMessage() );
            if(/*depth >= 6*/hashSet.find( hash_val ) == hashSet.end() ){
                
                hashSet.insert( hash_val );
                int map_val = minimax(NextStates[i], me, depth-1, alpha, beta, due);
                Min_map[hash_val] = map_val;
                layer[hash_val] = depth;
                v = std::min(v, map_val);

            }
            else{
                /*if (layer[hash_val] < depth) {
                    int map_val = minimax(NextStates[i], me, depth-1, alpha, beta, due);
                    Min_map[hash_val] = map_val;
                    v = std::min(v, map_val);
                }*/
                //if( hashSet.find( hash_val ) != hashSet.end() ){
                if (layer[hash_val]>=depth) {
                    v = std::min(v, Min_map[hash_val]);
                }
                //}
                else v = std::min(v, minimax(NextStates[i], me, depth-1, alpha, beta, due) );
            }

 
            beta = std::min(beta, v);
            if( beta <= alpha)
                break;
        }
    }
    return v; // default.
}

int Player::heusticFunc(const GameState &pState, const uint8_t player){
    int score = 0;
    
    uint8_t local_opponent = player ^ (CELL_WHITE | CELL_RED);
    
    if (pState.isEOG()){    // game over
        if (pState.isDraw()){
            score = 0;
        }
        else if ((pState.isRedWin() && me == CELL_RED) || (pState.isWhiteWin() && me == CELL_WHITE)){
            score = 1000;
        }
        else if ((pState.isRedWin() && me == CELL_WHITE) || (pState.isWhiteWin() && me == CELL_RED)){
            score = -1000;
        }
        return score;
    }
    
    for (int i = 0; i < GameState::cSquares; i++){
        if (pState.at(i) == player){
            score += 1;
        }
        else if (pState.at(i) == local_opponent){
            score -= 1;
        }
        else if (pState.at(i) == (player | CELL_KING)){
            score += 5;
        }
        else if (pState.at(i) == (local_opponent | CELL_KING)){
            score -= 5;
        }
    }
    if (me == player){
        return score;
    }
    else{
        return -score;
    }
    
}
    void Player::orderStates(std::vector<GameState> &NextStates, bool flag){
        auto sortRuleLambda = [this] (const GameState& s1, const GameState& s2) -> bool
        {
            return getOrderNumber(s1) < getOrderNumber(s2);
        };
        std::sort(NextStates.begin(), NextStates.end(), sortRuleLambda);
        if(!flag) reverse(NextStates.begin(), NextStates.end() );
    }
    
    int Player::getOrderNumber(const GameState &pState){
        if (pState.isEOG()){    // game over
            if (pState.isDraw()){
                return 0;
            }
            else if ((pState.isRedWin() && me == CELL_RED) || (pState.isWhiteWin() && me == CELL_WHITE)){
                return 1000;
            }
            else if ((pState.isRedWin() && me == CELL_WHITE) || (pState.isWhiteWin() && me == CELL_RED)){
                return -1000;
            }
            else return 0;
        }
        else{
            return heusticFunc(pState, me); //check.
        }
        
    }
    int Player::hashFunc(std::string s) {

        int hash = 7;
        for (int i = 0; i < s.length(); i++) {
            hash = hash*31 + s[i];
        }

        return hash;
}

/*
int calcu_max_pieces(const GameState &pState, int x, int y, uint8_t cell_color){
	if( x<0 || x>7 || y<0 || y>7 || pState.at(x,y) == CELL_EMPTY) return 0;
	else return 1 + max( calcu_max_pieces(pState, x+1, y-1, cell_color), calcu_max_pieces(pState, x+1, y+1, cell_color) );
	
}
*/

/*namespace checkers*/ }
