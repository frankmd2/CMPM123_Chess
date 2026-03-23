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

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

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

void Chess::generateMoves(std::string state, std::vector<BitMove> &moves) {
    for (int i = 0; i < 6; i ++) {
        piece_boards[i][0] = 0;
        piece_boards[i][1] = 0;
    }
    occupacy = BitBoard(0);
    occupacy_by_player[0] = BitBoard(0);
    occupacy_by_player[1] = BitBoard(0);

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
            piece_boards[piece - 1][playerNumber] |= (1ULL << i);
            occupacy |= (1ULL << i);
            occupacy_by_player[playerNumber] |= (1ULL << i);
        }
    }

    int currentPlayer = getCurrentPlayer()->playerNumber();
    generatePawnMoves(moves, piece_boards[Pawn - 1][currentPlayer], occupacy_by_player[currentPlayer], occupacy);
    generateKnightMoves(moves, piece_boards[Knight - 1][currentPlayer], occupacy_by_player[currentPlayer]);
    generateBishopMoves(moves, piece_boards[Bishop - 1][currentPlayer], occupacy_by_player[currentPlayer], occupacy);
    generateRookMoves(moves, piece_boards[Rook - 1][currentPlayer], occupacy_by_player[currentPlayer], occupacy);
    generateQueenMoves(moves, piece_boards[Queen - 1][currentPlayer], occupacy_by_player[currentPlayer], occupacy);
    generateKingMoves(moves, piece_boards[King - 1][currentPlayer], occupacy_by_player[currentPlayer]);

}

bool Chess::isValidMove(BitMove move) {
    return true;
}

void Chess::generateRookMoves(std::vector<BitMove> &moves, BitBoard rookBoard, BitBoard occupacy, BitBoard enemyPieces) {
    rookBoard.forEachBit([&](int fromSquare) {
        int rank = fromSquare / 8;
        int file = fromSquare % 8;

        // Generate moves in the four cardinal directions
        const int directions[4][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        for (const auto& direction : directions) {
            int newRank = rank + direction[0];
            int newFile = file + direction[1];

            while (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                int toSquare = newRank * 8 + newFile;
                if (occupacy.getData() & (1ULL << toSquare)) {
                    break;
                }
                if (enemyPieces.getData() & (1ULL << toSquare)) {
                    moves.emplace_back(fromSquare, toSquare, Rook);
                    break;
                }
                moves.emplace_back(fromSquare, toSquare, Rook);
                newRank += direction[0];
                newFile += direction[1];
            }
        }
    });
}

void Chess::generateBishopMoves(std::vector<BitMove> &moves, BitBoard bishopBoard, BitBoard occupacy, BitBoard enemyPieces) {
    bishopBoard.forEachBit([&](int fromSquare) {
        int rank = fromSquare / 8;
        int file = fromSquare % 8;

        // Generate moves in the four diagonal directions
        const int directions[4][2] = {
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (const auto& direction : directions) {
            int newRank = rank + direction[0];
            int newFile = file + direction[1];

            while (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                int toSquare = newRank * 8 + newFile;
                if (occupacy.getData() & (1ULL << toSquare)) {
                    break;
                }
                if (enemyPieces.getData() & (1ULL << toSquare)) {
                    moves.emplace_back(fromSquare, toSquare, Bishop);
                    break;
                }
                moves.emplace_back(fromSquare, toSquare, Bishop);
                newRank += direction[0];
                newFile += direction[1];
            }
        }
    });
}

void Chess::generateQueenMoves(std::vector<BitMove> &moves, BitBoard queenBoard, BitBoard occupacy, BitBoard enemyPieces) {
    queenBoard.forEachBit([&](int fromSquare) {
        int rank = fromSquare / 8;
        int file = fromSquare % 8;

        // Generate moves in all eight directions
        const int directions[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
        };

        for (const auto& direction : directions) {
            int newRank = rank + direction[0];
            int newFile = file + direction[1];

            while (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                int toSquare = newRank * 8 + newFile;
                if (occupacy.getData() & (1ULL << toSquare)) {
                    break;
                }
                if (enemyPieces.getData() & (1ULL << toSquare)) {
                    moves.emplace_back(fromSquare, toSquare, Queen);
                    break;
                }
                moves.emplace_back(fromSquare, toSquare, Queen);
                newRank += direction[0];
                newFile += direction[1];
            }
        }
    });
}

void Chess::generatePawnMoves(std::vector<BitMove> &moves, BitBoard pawnBoard, BitBoard occupacy, BitBoard enemyPieces) {
    pawnBoard.forEachBit([&](int fromSquare) {
        int rank = fromSquare / 8;
        int file = fromSquare % 8;
        int direction = (getCurrentPlayer()->playerNumber() == 0) ? 1 : -1;

        // Move forward
        int toSquare = (rank + direction) * 8 + file;
        if (toSquare >= 0 && toSquare < 64 && !(occupacy.getData() & (1ULL << toSquare)) && !(enemyPieces.getData() & (1ULL << toSquare))) {
            moves.emplace_back(fromSquare, toSquare, Pawn);
            // Double move from starting position
            if ((rank == 1 && direction == 1) || (rank == 6 && direction == -1)) {
                int doubleToSquare = (rank + 2 * direction) * 8 + file;
                if (doubleToSquare >= 0 && doubleToSquare < 64 && !(occupacy.getData() & (1ULL << doubleToSquare)) && !(enemyPieces.getData() & (1ULL << doubleToSquare))) {
                    moves.emplace_back(fromSquare, doubleToSquare, Pawn);
                }
            }
        }

        // Capture moves
        const int captureOffsets[2] = { -1, 1 };
        for (const auto& offset : captureOffsets) {
            int captureFile = file + offset;
            if (captureFile >= 0 && captureFile < 8) {
                int captureToSquare = (rank + direction) * 8 + captureFile;
                if (captureToSquare >= 0 && captureToSquare < 64 && (enemyPieces.getData() & (1ULL << captureToSquare))) {
                    moves.emplace_back(fromSquare, captureToSquare, Pawn);
                }
            }
        }
    });
}

void Chess::generateKnightMoves(std::vector<BitMove> &moves, BitBoard knightBoard, BitBoard occupacy) {
    knightBoard.forEachBit([&](int fromSquare) {
        BitBoard(KnightAttacks[fromSquare]).forEachBit([&](int toSquare) {
            if (occupacy.getData() & (1ULL << toSquare)) return;
            moves.emplace_back(fromSquare, toSquare, Knight);
        });
    });
}

void Chess::generateKingMoves(std::vector<BitMove> &moves, BitBoard kingBoard, BitBoard occupacy) {
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
            if (occupacy.getData() & (1ULL << toSquare)) return;
            moves.emplace_back(fromSquare, toSquare, King);
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
    generateMoves(stateString(), moves);

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
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int index = y * 8 + x;
            char c = s[index];

            ChessPiece piece = NoPiece;
            if (c == 'p' || c == 'P') piece = Pawn;
            else if (c == 'n' || c == 'N') piece = Knight;
            else if (c == 'b' || c == 'B') piece = Bishop;
            else if (c == 'r' || c == 'R') piece = Rook;
            else if (c == 'q' || c == 'Q') piece = Queen;
            else if (c == 'k' || c == 'K') piece = King;

            if (piece == NoPiece) {
                _grid->getSquare(x, y)->destroyBit();
                continue;
            }

            int playerNumber = std::isupper(c) ? 0 : 1;

            Bit* bit = PieceForPlayer(playerNumber, piece);
            bit->setPosition(_grid->getSquare(x, y)->getPosition());
            _grid->getSquare(x, y)->setBit(bit);
        }
    }
}

int Chess::boardEval(std::string state, int color) {
    int score = 0;
    for (int i = 0; i < state.size(); i++) {
        char c = state[i];
        if (c != '0') {
            int pieceValue = 0;
            if (std::isupper(c)) {
                switch (std::tolower(c)) {
                    case 'p':
                        pieceValue = 100;
                        score += pawnTableW[i];
                        break;
                    case 'n':
                        pieceValue = 300;
                        score += knightTableW[i];
                        break;
                    case 'b':
                        pieceValue = 300;
                        score += bishopTableW[i];
                        break;
                    case 'r':
                        pieceValue = 500;
                        score += rookTableW[i];
                        break;
                    case 'q':
                        pieceValue = 900;
                        score += queenTableW[i];
                        break;
                    case 'k':
                        pieceValue = 20000;
                        score += kingTableW[i];
                        break;
                }
            }
            else {
                switch (std::tolower(c)) {
                    case 'p':
                        pieceValue = 100;
                        score += pawnTableB[i];
                        break;
                    case 'n':
                        pieceValue = 300;
                        score += knightTableB[i];
                        break;
                    case 'b':
                        pieceValue = 300;
                        score += bishopTableB[i];
                        break;
                    case 'r':
                        pieceValue = 500;
                        score += rookTableB[i];
                        break;
                    case 'q':
                        pieceValue = 900;
                        score += queenTableB[i];
                        break;
                    case 'k':
                        pieceValue = 20000;
                        score += kingTableB[i];
                        break;
                }
            }
            score += (std::isupper(c) ? pieceValue : -pieceValue);
        }
    }
    if (color == 1) {
        score = -score;
    }
    return score;
}

int Chess::negamax(std::string state, int alpha, int beta, int depth, int color) {
    if (depth == 0) {
        return boardEval(state, 1 - color);
    }
    std::vector<BitMove> moves;
    generateMoves(state, moves);

    for (const auto& move : moves) {
        std::string newState = state;
        newState[move.to] = newState[move.from];
        newState[move.from] = '0';
        int eval = -negamax(newState, -beta, -alpha, depth - 1, 1 - color);
        if (eval > beta) return beta;
        alpha = std::max(alpha, eval);
    }
    return alpha;
}

void Chess::updateAI() {
    int DEPTH = 3;
    std::string state = stateString();

    std::vector<BitMove> moves;
    generateMoves(state, moves);

    int best_score = -1000000;
    int best_move_index = -1;

    for (const auto& move : moves) {
        std::string newState = state;
        std::swap(newState[move.from], newState[move.to]);
        int color = AI_PLAYER == 1 ? 1 : 0;
        int eval = -negamax(newState, -1000000, 1000000, DEPTH, color);

        if (eval > best_score) {
            best_score = eval;
            best_move_index = &move - &moves[0];
        }
    }
    std::cout << "Best move: " << best_move_index << " with score: " << best_score << std::endl;

    state[moves[best_move_index].to] = state[moves[best_move_index].from];
    state[moves[best_move_index].from] = '0';
    setStateString(state);
    endTurn();
}