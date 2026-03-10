## Chess.

Frank Zeng

Platform: windows

As usual, cmakelist has been modified for mingw tool chain.

The moves are generate with bitboards, and use the naive way to iterate through all possible moves.
The kightmoves uses an attack board, while the others just using loops.
The queen moves uses directly the rook and bishop moves, which is kind of bad practice.