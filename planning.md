  Planning!

- Board
  - 1D Array (but actually 2D) of Pieces?

- enum PieceType
  - Pawn = 1
  - King = 2
  - etc.
 
- Piece
  - enum of type
  - moved? (some way to handle pawns moving 2 sqaures/castling)
  - movement system?
    - hard code movements?

- AI
  - some sort of API to interact with the board (make_move()?)

```
# Quick and dirty danger map.
# Before
110
0P0
0K0

# After
110
P10
011
```

- Sqaure (holds pieces) ?
  - has knowledge of pieces that are controlling it
  - would be a part of Board
  - Properties:
    - list of pieces controling this sqaure

- Some sort of Analysis Struct
  - would make a quick bool map 
  - let analysis = Analysis::new(potential_state);
  - methods:
    - check_if_checked()
    - get_legal_moves()

- Move struct?
  - start
  - end

- Checklist
  - Draw Board
  - Draw Pieces
  - Drag Pieces
  - Pawns moves twice initially
  - Capture
  - Check
  - Promotion
  - Pins
  - Castling
  - En Passant
  - Checkmate
  - AI (1000)
    - needs to be able to at least beat chess.com's Emir bot

- Checkist (Low Priority)
  - premoves
  - import a game (FEN/PGN)
  - time control
  - themes (2D/3D?)
  - online multiplayer!!
