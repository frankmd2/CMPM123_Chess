#pragma once

#include "Game.h"
#include "Grid.h"
#include "Bitboard.h"

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

    void generateMoves(std::vector<BitMove> &moves);
    void generatePawnMoves(std::vector<BitMove> &moves, BitBoard pawnBoard, BitBoard enemyPieces);
    void generateKnightMoves(std::vector<BitMove> &moves, BitBoard knightBoard, BitBoard occupacy);
    void generateKingMoves(std::vector<BitMove> &moves, BitBoard kingBoard);

    void generateKnightAttacks();

    char pieceNotation(int x, int y) const;

    Grid* _grid;
    BitBoard occupacy;
    BitBoard PieceBoards[6][2];
    BitBoard KnightAttacks[64];
};