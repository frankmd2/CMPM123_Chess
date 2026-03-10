#include "Chess.h"
#include <limits>
#include <cmath>

Chess::Chess()
{
    _grid = new Grid(8, 8);
    occupacy = BitBoard(0);
    generateKnightAttacks();
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    bit->setGameTag(playerNumber * 128 + piece);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)

    int file = 0;
    int rank = 7;
    for (char c : fen) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += c - '0';
        } else {
            ChessPiece piece = NoPiece;
            if (c == 'p' || c == 'P') piece = Pawn;
            else if (c == 'n' || c == 'N') piece = Knight;
            else if (c == 'b' || c == 'B') piece = Bishop;
            else if (c == 'r' || c == 'R') piece = Rook;
            else if (c == 'q' || c == 'Q') piece = Queen;
            else if (c == 'k' || c == 'K') piece = King;

            int playerNumber = std::isupper(c) ? 0 : 1;
            Bit* bit = PieceForPlayer(playerNumber, piece);
            bit->setPosition(_grid->getSquare(file, rank)->getPosition());
            _grid->getSquare(file, rank)->setBit(bit);

            file++;
        }
    }
}

void Chess::generateMoves(std::vector<BitMove> &moves) {
    for (int i = 0; i < 6; i ++) {
        PieceBoards[i][0] = 0;
        PieceBoards[i][1] = 0;
    }

    std::string state = stateString();
    for (int i = 0; i < state.size(); i++) {
        if (state[i] != '0') {
            char c = state[i];
            ChessPiece piece = NoPiece;
            if (c == 'p' || c == 'P') piece = Pawn;
            else if (c == 'n' || c == 'N') piece = Knight;
            else if (c == 'b' || c == 'B') piece = Bishop;
            else if (c == 'r' || c == 'R') piece = Rook;
            else if (c == 'q' || c == 'Q') piece = Queen;
            else if (c == 'k' || c == 'K') piece = King;

            int playerNumber = std::isupper(c) ? 0 : 1;
            PieceBoards[piece - 1][playerNumber] |= (1ULL << i);
        }
    }

    int currentPlayer = getCurrentPlayer()->playerNumber();
    //generatePawnMoves(moves, PieceBoards[Pawn - 1][currentPlayer], occupacy);
    generateKnightMoves(moves, PieceBoards[Knight - 1][currentPlayer], TODO);
    //generateKingMoves(moves, PieceBoards[King - 1][currentPlayer]);

    std::cout << "Generated " << moves.size() << " moves" << std::endl;
}

bool Chess::isValidMove(BitMove move) {
    ChessSquare fromSquare = *_grid->getSquare(move.from % 8, move.from / 8);
    ChessSquare toSquare = *_grid->getSquare(move.to % 8, move.to / 8);

    // if the target location is empty, return true
    if (toSquare.empty()) return true;

    // if the color are the same, return false
    if ((fromSquare.bit()->gameTag() & 128) == (toSquare.bit()->gameTag() & 128)) return false;

    return true;
}

void Chess::generatePawnMoves(std::vector<BitMove> &moves, BitBoard pawnBoard, BitBoard enemyPieces) {

}

void Chess::generateKnightMoves(std::vector<BitMove> &moves, BitBoard knightBoard, BitBoard occupacy) {
    knightBoard.forEachBit([&](int fromSquare) {
        BitBoard(KnightAttacks[fromSquare]).forEachBit([&](int toSquare) {
            if (isValidMove(BitMove(fromSquare, toSquare, Knight))) {
                moves.emplace_back(fromSquare, toSquare, Knight);
            }
        });
    });
}

void Chess::generateKingMoves(std::vector<BitMove> &moves, BitBoard kingBoard) {
    return;
    kingBoard.forEachBit([&](int fromSquare) {
        BitBoard kingAttacks = BitBoard(0);
        int rank = fromSquare / 8;
        int file = fromSquare % 8;

        const int kingMoves[8][2] = {
            {1, 0}, {1, 1}, {0, 1}, {-1, 1},
            {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
        };

        for (const auto& move : kingMoves) {
            int newRank = rank + move[0];
            int newFile = file + move[1];
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                kingAttacks |= (1ULL << (newRank * 8 + newFile));
            }
        }

        kingAttacks.forEachBit([&](int toSquare) {
            if (isValidMove(BitMove(fromSquare, toSquare, King))) {
                moves.emplace_back(fromSquare, toSquare, King);
            }
        });
    });
}

void Chess::generateKnightAttacks() {
    for (int square = 0; square < 64; square++) {
        BitBoard attacks;
        int rank = square / 8;
        int file = square % 8;

        const int knightMoves[8][2] = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
        };

        for (const auto& move : knightMoves) {
            int newRank = rank + move[0];
            int newFile = file + move[1];
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                attacks |= (1ULL << (newRank * 8 + newFile));
            }
        }
        KnightAttacks[square] = attacks;
    }
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber();
    int pieceColor = (bit.gameTag() & 128) / 128;

    if (pieceColor != currentPlayer) return false;
    return true;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    int currentPlayer = getCurrentPlayer()->playerNumber();
    std::vector<BitMove> moves;
    generateMoves(moves);

    auto *new_src = dynamic_cast<ChessSquare *>(&src);
    auto *new_dst = dynamic_cast<ChessSquare *>(&dst);

    for (const auto& move : moves) {
        if (move.from == new_src->getSquareIndex() && move.to == new_dst->getSquareIndex()) {
            return true;
        }
    }
    return false;
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;
}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}
