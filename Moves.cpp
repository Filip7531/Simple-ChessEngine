#include "DataTypes.h"
#include "Utils.h"
#include <vector>
#include <cstdint>

using namespace std;

void addMove(std::vector<Move> &moves, int fr, int ff, int tr, int tf) {
    moves.push_back({fr, ff, tr, tf});
}
Pos makeMove(const Pos &pos, const Move &m) {
    Pos newPos = pos;
    bool white = pos.IsWhiteMove;
    newPos.IsWhiteMove = !white;//zmiana kolejki

    Board &own  = white ? newPos.white : newPos.black;
    Board &enemy = white ? newPos.black : newPos.white;
    

    //castle
    if(own.King & (1ULL << (m.fromRank*8 + m.fromFile))
     && abs(m.toFile - m.fromFile) >= 2) { 
        
        int deltaFile = m.toFile - m.fromFile;
        if(deltaFile == 2) { 
            int rank = m.fromRank;
            assert(own.Rook & (1ULL << (rank*8 + 7)));
            own.Rook &= ~(1ULL << (rank*8 + 7)); 
            own.Rook |=  (1ULL << (rank*8 + 5)); 
        } else if(deltaFile == -2) { 
            int rank = m.fromRank;
            assert(own.Rook & (1ULL << (rank*8 + 0)));

            own.Rook &= ~(1ULL << (rank*8 + 0));
            own.Rook |=  (1ULL << (rank*8 + 3)); 
        }
        own.King &= ~(1ULL << (m.fromRank*8 + m.fromFile));
        own.King |= (1ULL << (m.toRank * 8 + m.toFile));
        return newPos;   
    }

    if (m.promotion != '0') {
        own.Pawn &= ~(1ULL << (m.toRank*8 + m.toFile));
        if (m.promotion == 'q') own.Queen |=  (1ULL << (m.toRank*8 + m.toFile));
        else if (m.promotion == 'r') own.Rook |=   (1ULL << (m.toRank*8 + m.toFile));
        else if (m.promotion == 'b') own.Bishop |= (1ULL << (m.toRank*8 + m.toFile));
        else if (m.promotion == 'n') own.Knight |= (1ULL << (m.toRank*8 + m.toFile));
        return newPos;   

    }

    auto movePiece = [&](ull &bb){
        if(getBit(bb, m.fromRank, m.fromFile)){
            bb &= ~(1ULL << (m.fromRank*8 + m.fromFile));
            bb |= (1ULL << (m.toRank*8 + m.toFile));    
            return true;
        }
        return false;
    };

    // sprawdzamy każdą figurę
    if(movePiece(own.Pawn))   {}
    else if(movePiece(own.Rook)) {}
    else if(movePiece(own.Knight)) {}
    else if(movePiece(own.Bishop)) {}
    else if(movePiece(own.Queen)) {}
    else if(movePiece(own.King)) {}
    else {
        cout << "ERRORRRRRRRRRR" << endl;
    }
    auto capturePiece = [&](ull &bb){
        if(getBit(bb, m.toRank, m.toFile)){
            bb &= ~(1ULL << (m.toRank*8 + m.toFile));
        }
    };
    capturePiece(enemy.Pawn);
    capturePiece(enemy.Rook);
    capturePiece(enemy.Knight);
    capturePiece(enemy.Bishop);
    capturePiece(enemy.Queen);
    capturePiece(enemy.King);

    
    return newPos;
}
void addPawnPromotionMoves(vector<Move>& moves,int fr, int ff, int tr, int tf, bool white){
    const char promos[4] = {'q','r','b','n'};
    for(char p : promos) {
        Move m;
        m.fromRank = fr;
        m.fromFile = ff;
        m.toRank   = tr;
        m.toFile   = tf;
        m.promotion = white ? toupper(p) : p;
        moves.push_back(m);
    }
}

void generatePawnMoves(const Pos &pos, bool white, vector<Move> &moves) {
    ull pawns = white ? pos.white.Pawn : pos.black.Pawn;
    ull own   = white ? (pos.white.Pawn|pos.white.Rook|pos.white.Knight|
                         pos.white.Bishop|pos.white.Queen|pos.white.King)
                      : (pos.black.Pawn|pos.black.Rook|pos.black.Knight|
                         pos.black.Bishop|pos.black.Queen|pos.black.King);

    ull enemy = white ? (pos.black.Pawn|pos.black.Rook|pos.black.Knight|
                         pos.black.Bishop|pos.black.Queen|pos.black.King)
                      : (pos.white.Pawn|pos.white.Rook|pos.white.Knight|
                         pos.white.Bishop|pos.white.Queen|pos.white.King);

    int dr = white ? -1 : 1;
    int promoRank = white ? 0 : 7;
    int startRank = white ? 6 : 1;

    for(int rank=0; rank<8; rank++) {
        for(int file=0; file<8; file++) {
            if(!getBit(pawns, rank, file)) continue;

            int toRank = rank + dr;
            if(toRank < 0 || toRank >= 8) continue;

            if(!getBit(own|enemy, toRank, file)) {
                if(toRank == promoRank)
                    addPawnPromotionMoves(moves, rank, file, toRank, file, white);
                else
                    addMove(moves, rank, file, toRank, file);
            }
//----------------------------------------------------------------------
            if(file > 0 && getBit(enemy, toRank, file-1)) {
                if(toRank == promoRank)
                    addPawnPromotionMoves(moves, rank, file, toRank, file-1, white);
                else
                    addMove(moves, rank, file, toRank, file-1);
            }

            if(file < 7 && getBit(enemy, toRank, file+1)) {
                if(toRank == promoRank)
                    addPawnPromotionMoves(moves, rank, file, toRank, file+1, white);
                else
                    addMove(moves, rank, file, toRank, file+1);
            }

            // --- podwójny ruch ---
            if(rank == startRank) {
                int toRank2 = rank + 2*dr;
                if(!getBit(own|enemy, rank+dr, file) &&
                   !getBit(own|enemy, toRank2, file))
                    addMove(moves, rank, file, toRank2, file);
            }
        }
    }
}

// ================= KNIGHT =================
void generateKnightMoves(const Pos &pos, bool white, std::vector<Move> &moves) {
    ull knights = white ? pos.white.Knight : pos.black.Knight;
    ull own     = white ? pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King
                        : pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King;
    [[maybe_unused]] ull enemy   = white ? pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King
                        : pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King;

    int dr[8] = {2,1,-1,-2,-2,-1,1,2};
    int df[8] = {1,2,2,1,-1,-2,-2,-1};

    for(int rank=0; rank<8; rank++){
        for(int file=0; file<8; file++){
            if(!getBit(knights, rank, file)) continue;
            for(int i=0;i<8;i++){
                int tr = rank + dr[i];
                int tf = file + df[i];
                if(tr<0||tr>=8||tf<0||tf>=8) continue;
                if(!getBit(own, tr, tf))
                    addMove(moves, rank, file, tr, tf);
            }
        }
    }
}

// ================= SLIDING PIECES =================
void generateSlidingMoves(const Pos &pos, ull pieces, bool white, const std::vector<std::pair<int,int>> &dirs, std::vector<Move> &moves) {
    ull own   = white ? pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King
                      : pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King;
    ull enemy = white ? pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King
                      : pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King;

    for(int rank=0; rank<8; rank++){
        for(int file=0; file<8; file++){
            if(!getBit(pieces, rank, file)) continue;
            for(auto [dr, df]: dirs){
                int tr = rank + dr;
                int tf = file + df;
                while(tr>=0 && tr<8 && tf>=0 && tf<8){
                    if(getBit(own, tr, tf)) break;
                    addMove(moves, rank, file, tr, tf);
                    if(getBit(enemy, tr, tf)) break;
                    tr += dr; tf += df;
                }
            }
        }
    }
}

// ================= KING =================
void generateKingMoves(const Pos &pos, bool white, std::vector<Move> &moves){
    ull king = white ? pos.white.King : pos.black.King;
    ull own  = white ? pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King
                     : pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King;
    [[maybe_unused]]ull enemy = white ? pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King
                      : pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King;

    int dr[8]={1,1,0,-1,-1,-1,0,1};
    int df[8]={0,1,1,1,0,-1,-1,-1};

    for(int rank=0; rank<8; rank++){
        for(int file=0; file<8; file++){
            if(!getBit(king, rank, file)) continue;
            for(int i=0;i<8;i++){
                int tr = rank + dr[i];
                int tf = file + df[i];
                if(tr<0||tr>=8||tf<0||tf>=8) continue;
                if(!getBit(own, tr, tf))
                    addMove(moves, rank, file, tr, tf);
            }
        }
    }
}

// ================= GENERATE ALL MOVES =================
std::vector<Move> generateMoves(const Pos &pos){
    std::vector<Move> moves;
    bool white = pos.IsWhiteMove;

    generatePawnMoves(pos, white, moves);
    generateKnightMoves(pos, white, moves);
    generateSlidingMoves(pos, white ? pos.white.Bishop : pos.black.Bishop, white, {{1,1},{1,-1},{-1,1},{-1,-1}}, moves);
    generateSlidingMoves(pos, white ? pos.white.Rook   : pos.black.Rook,   white, {{1,0},{-1,0},{0,1},{0,-1}}, moves);
    generateSlidingMoves(pos, white ? pos.white.Queen  : pos.black.Queen,  white, {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}}, moves);
    generateKingMoves(pos, white, moves);

    return moves;
}

// ================= QUIET / LOUD =================
std::vector<Move> generateQuietMoves(const Pos &pos){
    std::vector<Move> all = generateMoves(pos);
    std::vector<Move> quiet;
    for(auto &m: all){
        if(!getBit(pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King
                   |pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King,
                   m.toRank, m.toFile))
            quiet.push_back(m);
    }
    return quiet;
}

std::vector<Move> generateLoudMoves(const Pos &pos){
    std::vector<Move> all = generateMoves(pos);
    std::vector<Move> loud;
    for(auto &m: all){
        if(getBit(pos.black.Pawn|pos.black.Rook|pos.black.Knight|pos.black.Bishop|pos.black.Queen|pos.black.King
                  |pos.white.Pawn|pos.white.Rook|pos.white.Knight|pos.white.Bishop|pos.white.Queen|pos.white.King,
                  m.toRank, m.toFile))
            loud.push_back(m);
    }
    return loud;
}
