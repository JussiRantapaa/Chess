Base of a chess game for my Thesis, contains all chess rules except repetition and 50 rule draw, this far the bot just makes a random legal move. 
The .exe is available in the file but if your windows defender gets angry just compile it yourself, define #DEBUG for additional move info



  - main.c Handles the high level operations
  - setup.c contains board representation and variable initialization
  - gameLogic.c contains the core logic of move generation and validation
  - main.h definitions and function prototypes


Architecture overview:
1. Variables
    - Gamestate struct keeps track of all game variables and is passed into almost every function to avoid global variables
    
    - White's turn is represented with 0 and blacks turn with 1, same applies to all arrays that hold piece info
    
    - Moves and piece positions are represented with integers as follows:
      - Bits 0-5 : starting square
      - Bits 6-11: landing square
      - Bits 12- : extra info (Piece type, en passant, castling, capture moves etc.)
  

2. Move generation
    - We call compute_moves() which loops the squares and based on piece type it calls the correct compute_<PIECE_TYPE>_move function
    - If a move is within bounds we call is_king_threatened() to check it doesn't endanger the king
    - In is_king_threatened() we pretend to make the move and loop all enemy pieces to see if they can attack the square our king is located
    - If king is safe we call add_move() where the move is hashed into an integer and stored in the move_list[]
        
3. Making moves
    - Ask user for input, hash it and see if it matches a move on the move list in test_move() and only compare position bits (0-11)
    - If we get a match we enter make_move() and check for bits 12- to see if the move contains special manouvers and handle them separately
    - Finally we make the move on the gameboard and update piece positions
        
4. repeat until move_count is 0 and then handle game ending at display_winner



    

