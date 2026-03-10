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

    Grid* getGrid() override { return _grid; }

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);

    bool isValidMove(BitMove move);

    void generateRookMoves(std::vector<BitMove> &moves, BitBoard rookBoard, BitBoard occupacy);

    void generateBishopMoves(std::vector<BitMove> &moves, BitBoard bishopBoard, BitBoard occupacy);

    void generateQueenMoves(std::vector<BitMove> &moves, BitBoard queenBoard, BitBoard occupacy);

    void generateMoves(std::vector<BitMove> &moves);
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
};