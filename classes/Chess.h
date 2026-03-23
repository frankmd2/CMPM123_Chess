#pragma once

#include "Game.h"
#include "Grid.h"
#include "Bitboard.h"
#include <cassert>

constexpr int pieceSize = 80;

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    void setUpBoard() override;

    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    bool actionForEmptyHolder(BitHolder &holder) override;

    void stopGame() override;

    Player *checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;

    int boardEval(std::string state, int color);

    int negamax(std::string state, int alpha, int beta, int depth, int color);

    void updateAI() override;
    bool gameHasAI() override { return true; }

    Grid* getGrid() override { return _grid; }

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);

    bool isValidMove(BitMove move);

    void generateRookMoves(std::vector<BitMove> &moves, BitBoard rookBoard, BitBoard occupacy, BitBoard enemyPieces);

    void generateBishopMoves(std::vector<BitMove> &moves, BitBoard bishopBoard, BitBoard occupacy, BitBoard enemyPieces);

    void generateQueenMoves(std::vector<BitMove> &moves, BitBoard queenBoard, BitBoard occupacy, BitBoard enemyPieces);

    void generateMoves(std::string state, std::vector<BitMove> &moves);
    void generatePawnMoves(std::vector<BitMove> &moves, BitBoard pawnBoard, BitBoard occupacy, BitBoard enemyPieces);
    void generateKnightMoves(std::vector<BitMove> &moves, BitBoard knightBoard, BitBoard occupacy);
    void generateKingMoves(std::vector<BitMove> &moves, BitBoard kingBoard, BitBoard occupacy);

    void generateKnightAttacks();

    char pieceNotation(int x, int y) const;

    Grid* _grid;
    BitBoard occupacy;
    BitBoard occupacy_by_player[2];
    BitBoard piece_boards[6][2];
    BitBoard KnightAttacks[64];

    const int pawnTableW[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, -20, -20, 10, 10, 5,
        5, -5, -10, 0, 0, -10, -5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, 5, 10, 25, 25, 10, 5, 5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    const int pawnTableB[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        -50, -50, -50, -50, -50, -50, -50, -50,
        -10, -10, -20, -30, -30, -20, -10, -10,
        -5, -5, -10, -25, -25, -10, -5, -5,
        0, 0, 0, -20, -20, 0, 0, 0,
        -5, 5, 10, 0, 0, 10, 5, -5,
        -5, -10,-10, 20, 20,-10,-10, -5,
        0, 0 ,0 ,0 ,0 ,0 ,0 ,0
    };

    const int knightTableW[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const int knightTableB[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -40,-20 ,0 ,0 ,0 ,0 ,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const int bishopTableW[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10 ,10 ,10 ,10 ,10 ,0 , -10,
        -10 ,5 ,0 ,0 ,0 ,0 ,5 ,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const int bishopTableB[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -10, 10 ,10 ,10 ,10 ,10 ,0 , -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10 ,0 ,0 ,0 ,0 ,0 ,0 ,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const int rookTableW[64] = {
        0, 0, 0, 5, 5, 0, 0, 0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0 ,0 ,0 ,0 ,0 ,0 ,-5,
        -5 ,0 ,0 ,0 ,0 ,0 ,0 ,-5,
        5 ,10 ,10 ,10 ,10 ,10 ,10 ,5,
        0 ,0 ,0 ,5 ,5 ,0 ,0 ,0
    };

    const int rookTableB[64] = {
        0 ,0 ,0 ,5 ,5 ,0 ,0 ,0,
        5 ,10 ,10 ,10 ,10 ,10 ,10 ,5,
        -5 ,0 ,0 ,0 ,0 ,0 ,0 ,-5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
    };

    const int queenTableW[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5 ,0 ,5 ,5 ,5 ,5 ,0 ,-5,
        0 ,0 ,5 ,5 ,5 ,5 ,0 ,-5,
        -10 ,0 ,5 ,5 ,5 ,5 ,0 ,-10,
        -10 ,0 ,0 ,0 ,0 ,0 ,0 ,-10,
        -20,-10,-10,-5,-5,-10,-10,-20
    };

    const int queenTableB[64] = {
        -20,-10,-10,-5,-5,-10,-10,-20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5 ,0 ,5 ,5 ,5 ,5 ,0 ,-5,
        0 ,0 ,5 ,5 ,5 ,5 ,0 ,-5,
        -10 ,0 ,5 ,5 ,5 ,5 ,0 ,-10,
        -10 ,0 ,0 ,0 ,0 ,0 ,0 ,-10,
        -20,-10,-10,-5,-5,-10,-10,-20
    };

    const int kingTableW[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20 ,20 ,20 ,20 ,20 ,20 ,20 ,-20,
        20 ,20 ,0 ,0 ,0 ,0 ,20 ,20,
        20 ,-10,-10,-10,-10,-10,-10 ,20,
        20 ,30 ,30 ,0 ,0 ,30 ,30 ,20
    };

    const int kingTableB[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        20 ,30 ,30 ,0 ,0 ,30 ,30 ,20,
        20 ,-10,-10,-10,-10,-10,-10 ,20,
        20 ,20 ,0 ,0 ,0 ,0 ,20 ,20,
        -20 ,20 ,20 ,20 ,20 ,20 ,20 ,-20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
    };
};