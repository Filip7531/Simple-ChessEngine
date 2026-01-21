#include "DataTypes.h"
#include "Utils.h"
#include "Evaluation.h"
#include "Hash.h"
#include "Moves.h"
#include "BoardUtils.h"
#include "Bot.h"
#include<bits/stdc++.h>
using namespace std;
double TimeForMove = 5;//40 seconds
double Now() {
    auto now = std::chrono::system_clock::now();
    auto dur = now.time_since_epoch();
    return std::chrono::duration<double>(dur).count(); 
}
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
unordered_map<Pos,Computed,PosHash>Nemo;
double START;
pair<double, Move> alpha_beta(Pos pos, int depth, double alpha=-inf/2, double beta=inf/2){
    if(EndOfTheGame(pos)) return {-(inf - depth * eps),NO_MOVE};
    if(START + TimeForMove < Now())return {inf, END_OF_THE_TIME};
    if(depth == 0){
        return {Evaluate(pos), NO_MOVE};
    }
    if(Nemo[pos].depth >= depth){
        return {Nemo[pos].res, Nemo[pos].BestMove};
    }
    vector<Move> MOVES = generateMoves(pos);
    for(auto x : MOVES){
        if(EndOfTheGame(makeMove(pos,x)))return {inf,x};
    }
    const int K = 10;
    partial_sort(
        MOVES.begin(),
        MOVES.begin() + K,//top 10 moves
        MOVES.end(),
        [&](const Move &a, const Move &b) {
            double x, y;

            Pos A = makeMove(pos, a);
            Pos B = makeMove(pos, b);

            x = Nemo.count(A) ? Nemo[A].res : Evaluate(A);
            y = Nemo.count(B) ? Nemo[B].res : Evaluate(B);
            return x > y;
        }
    );
    if(MOVES.empty())return {0.0, NO_MOVE};//draw
    Move BestMove = NO_MOVE;
    double bestScore = -inf;

    int moveIdx = 0;
    for (const Move &m : MOVES) {
        moveIdx++;

        Pos nextPos = makeMove(pos, m);

        int newDepth = depth - 1;
        bool reduced = false;

        // LMR
        if (moveIdx > K && depth >= 3) {
            newDepth = depth - 2;
            reduced = true;
        }

        double score = -alpha_beta(nextPos, newDepth, -beta, -alpha).first;

        if (reduced && score > alpha) {
            score = -alpha_beta(nextPos, depth - 1, -beta, -alpha).first;
        }

        if (score > bestScore) {
            bestScore = score;
            BestMove = m;
        }

        alpha = max(alpha, score);

        if (alpha >= beta) {
            break; // cutoff
        }
    }


    Nemo[pos].BestMove = BestMove;
    Nemo[pos].res = bestScore;
    Nemo[pos].depth = depth;
    return {bestScore,BestMove};
}
pair<double,Move>IterativeDeepening(Pos a){
    
    START = Now();
    vector<pair<double,Move>>V;
    int D;
    for(int depth = 0;  ;depth++){
        D = depth;
        V.push_back(alpha_beta(a, depth));
        if(V.back().first >= inf / 2)break;
        if(START + TimeForMove <  Now())break;
        ofstream fout("RES.txt"); /
        fout << V.back().first << " "  << " " << D << " "
          << MoveToUCI(V[1].second)
         << endl;
         
        fout.close();
    }
    reverse(V.begin(),V.end());
    assert(V.size() >= 2);
    return V[1];
}
// --- Main ---
int main() {
    //write comunication
}
