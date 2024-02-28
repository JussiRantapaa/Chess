#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

void ask_move(char **board){
    char start_pos[3];
    char end_pos[3];
    int correct_symbols = 0;
    int is_legal = 1;

    #ifdef DEBUG
        printf("\n\nWhite %d %d\nBlack %d %d\nrook moves: %d\nking moves: %d\n",white_king.row,white_king.col,black_king.row,black_king.col,rook_moves,king_moves);
        printf("Last move was %d %d to %d %d\n",last_move.start_col,last_move.start_row,last_move.end_col,last_move.end_row);
    #endif

        if(turn==0){
        printf("\n%d. White's turn\n",move_count);
    }else{
        printf("\n%d. Black's turn\n",move_count);
        move_count++;
    }

    // Ask for a move until a legal move is entered 
    while(is_legal != 0){   
        correct_symbols = scanf(" %[a-h] %[1-8]",&start_pos[0],&start_pos[1]);
        correct_symbols += scanf(" %[a-h] %[1-8]",&end_pos[0],&end_pos[1]);

        // Clear input buffer
        while (getchar() != '\n');

        // Prompt the user until a valid input is reached
        while(correct_symbols != 4){  
            printf("Invalid input, enter in format (a2 a3)\n");
            correct_symbols = scanf(" %[a-h] %[1-8]",&start_pos[0],&start_pos[1]);
            correct_symbols += scanf(" %[a-h] %[1-8]",&end_pos[0],&end_pos[1]);
           
           // Clear input buffer
            while (getchar() != '\n');   
            }
        
        is_legal = check_move_legality(board,start_pos,end_pos,turn);
        if(is_legal != 0){
            printf("INVALID MOVE\n");
        }
        // After a legal movetest for checkmate
        test_for_mate(board);
    }
}
int check_move_legality(char **board,char start[],char end[],int turn){
    // Convert string coordinates to integers that match the board array
    // CHAR_TO_INT is used to convert the column value (a-h) to a char number, then atoi for integer conversion
    int start_col = (int)(start[0]-CHAR_TO_INT); 
    int start_row = atoi(&(start[1]))-1;
    int end_col = (int)(end[0]-CHAR_TO_INT);
    int end_row = atoi(&(end[1]))-1;
    int is_legal = 1;

    // Check there is a piece on the starting square
    if(board[start_row][start_col] == EMPTY_SQUARE){
        return 1;
    }
    // Check that there isn't our own piece in the landing square and we are not trying to move opponents piece
    // WHITE_PIECES(96) is used to check the color of the piece on the landing square, white pieces are uppercase ascii (below 96)
    // For whites move we have to have an extra check for the empty squares
    if(turn == 0 && (((board[end_row][end_col] < WHITE_PIECES) && (board[end_row][end_col] != EMPTY_SQUARE)) || (board[start_row][start_col] > WHITE_PIECES))){
        return 1;
    // Same For black
    }else if(turn == 1 && ((board[end_row][end_col] > WHITE_PIECES) || (board[start_row][start_col] < WHITE_PIECES ))){
       return 1;
    }
    // Convert the piece we are moving to lowercase for the enum variable used in switch
    char char_piece = board[start_row][start_col];
    char_piece = (char_piece >= 'A' && char_piece <= 'Z') ? char_piece + ('a'-'A'):char_piece;
    enum pieces piece = char_piece;

    // Based on what piece we are moving check if the move is legal
    switch (piece){
        case PAWN:
            is_legal = test_pawn_move(start_row,start_col,end_row,end_col,board);
            // Check for pawn promotion;
            if(is_legal == 0 && (end_row == 7 || end_row == 0)){
                char queen_promotion = (turn == 0) ? 'Q': 'q';

                // The pawn is first turned in to a queen since the actual gameboard updating happens in finalize_move()
                board[start_row][start_col] = queen_promotion;
            }
            break;
        case ROOK:
            is_legal = test_rook_move(start_row,start_col,end_row,end_col,board);
            break;
        case KNIGHT:
            is_legal = test_knight_move(start_row,start_col,end_row,end_col);
            break;
        case BISHOP:
            is_legal = test_bishop_move(start_row,start_col,end_row,end_col,board);
            break;
        case QUEEN:
            is_legal = test_queen_move(start_row,start_col,end_row,end_col,board);
            break;
        case KING:
            is_legal = test_king_move(start_row,start_col,end_row,end_col,board);
            break;
    }
    // Check that the king isn't threatend resulting in the move
    is_legal += test_for_checks(start_row, start_col, end_row, end_col,board);

    // Update the board, last move variable and king position variable
    if(is_legal == 0){
        finalize_move(start_row, start_col, end_row, end_col,board);
    }
    // Finally return if the move is legal 0 = legal, 1 = illegal
    return is_legal;
} 
int test_for_checks(int start_row,int start_col,int end_row,int end_col,char **board){
    // Choose which king we are interested in
    Piece *king_ptr = (turn == 0) ? &white_king : &black_king;
    int check = 1;

    // Store the king position in a temporary variable
    int king_position_row = king_ptr->row;
    int king_position_col = king_ptr->col;

    // Create a temporary board to test the move
    char **temp_board = NULL;
    temp_board = (char**)malloc(8 * sizeof(char*));
      if(temp_board == NULL){
        printf("Memory allocating failed, exitting...");
        return 5;
    }
        for(int i=0;i<8;i++){
           temp_board[i] = (char*) malloc(8*sizeof(char));
            if(board[i] == NULL){
                printf("Memory allocating failed\n");
                for(int j=0;j<i;j++){
                    free(board[j]);
                }
            }
        }
    // Copy main board to temp board
    for(int i = 0;i<8;i++){
        for(int j = 0;j<8; j++){
            temp_board[i][j] = board[i][j];
        }
    }
    // Make the move in the temp board
    temp_board[end_row][end_col] = temp_board[start_row][start_col];
    temp_board[start_row][start_col] = EMPTY_SQUARE;

    // Since king position is updated in the next function, if we are testing a king move...
    // We have to update the temp variable, otherwise we reference the previous king position
    if(temp_board[end_row][end_col] == 'k' || temp_board[end_row][end_col] == 'K'){
        king_position_col = end_col;
        king_position_row = end_row;
    }
    // Check if any enemy piece can attack the file the king is in
    for(int i = 0;i < 8;i++){
        for(int j = 0;j < 8;j++){
            if((turn == 0 && temp_board[i][j] > WHITE_PIECES) || (turn == 1 && temp_board[i][j] < WHITE_PIECES && temp_board[i][j] != EMPTY_SQUARE)){
                check = test_for_threat(i,j,king_position_row,king_position_col,temp_board);
                if(check == 0){
                      // Destroy temp board
                    for(int i=0;i<8;i++){
                        free(temp_board[i]);  
                    }
                    return 1;
                } 
            }
        }
    }
     // Destroy temp board
    for(int i=0;i<8;i++){
        free(temp_board[i]);
    }
    return 0;
}
void finalize_move(int start_row,int start_col,int end_row,int end_col,char **board){
    // Update the game board
    board[end_row][end_col] = board[start_row][start_col];
    board[start_row][start_col ]= EMPTY_SQUARE;
    // Update last move variable (Used for en passant)
    last_move.start_col = start_col;
    last_move.start_row = start_row;
    last_move.end_col = end_col;
    last_move.end_row = end_row;
    last_move.piece = board[end_row][end_col];

    // Update king position
    if(board[end_row][end_col] == 'K'){
        white_king.row = end_row;
        white_king.col = end_col;
        king_moves |= (1U<<0);

    }else if(board[end_row][end_col] == 'k'){
        black_king.row = end_row;
        black_king.col = end_col;
        king_moves |= (1U<<1);

    }
}
int test_pawn_move(int start_row,int start_col,int end_row,int end_col,char **board){
    int first_move = (turn == 0) ? 1 : 6;
    int move_dir = (turn == 0) ? 1 : -1;
    int col_change = abs(end_col - start_col);
    int landing_square_empty = (board[end_row][end_col] == EMPTY_SQUARE) ? 1 : 0;

    // If the landing square is empty and we are staying in the same column, check if pawn is in starting square
    // also check that pawn wont jump over a piece 
    if(landing_square_empty){
        if(col_change == 0){
            if(((start_row == first_move) && (end_row == start_row + move_dir * 2)) && (board[start_row + move_dir][start_col] == EMPTY_SQUARE)){
                return 0;
            }
            // Else check that we are only moving one square
            if(end_row == start_row + move_dir){
                return 0;
            }    
        return 1;
        // Check for en passant
        }else if(col_change == 1 && end_col == last_move.end_col){
            if((turn == 0 && last_move.piece == 'p' )|| (turn == 1 && last_move.piece == 'P')){
                if(last_move.end_row == start_row && (last_move.end_row == (last_move.start_row + 2) || last_move.end_row == (last_move.start_row - 2) )){
                    board[last_move.end_row][last_move.end_col] = ' ';
                    return 0;
                }
            }
        }
        return 1;
    }
    // If we are eating a piece, check that its done diagonally
    if(col_change == 1 && end_row == (start_row + move_dir)){
            return 0;
    }
    return 1;
}
int test_rook_move(int start_row,int start_col,int end_row,int end_col,char **board){
    int col_change = abs(end_col - start_col);
    int row_change = abs(end_row - start_row);
    int col_dir = (end_col > start_col) ? 1 : -1;
    int row_dir = (end_row > start_row) ? 1 : -1;
    int col_delta = (col_change == 0) ? 0 : 1;
    int row_delta = (row_change == 0) ? 0 : 1;

    // Check that we move only horizontally or vertically
   if((col_change == 0) || (row_change == 0)){
        //Check for obstruction
        for(int i=1;i<col_change + row_change;i++){
            if(board[start_row + (row_delta * row_dir * i) ][start_col + (col_delta * col_dir * i)] !=  EMPTY_SQUARE){
                return 1;
            }
        }
        // Update the rook_moves variable that is used to check for castling rights
        // 0001 white a-rook, 0010 white h-rook, 0100 black a-rook,1000 = black h-rook 
        if((start_col == 0 || start_col == 7) && (start_row == 0 || start_row == 7)){
            int rook_column = (start_col == 0) ? 0 : 1;
            int rook_color = (start_row == 0) ? 0 : 2;

            rook_moves |= (1U<<(rook_column + rook_color));
        }
        return 0;
   }
   return 1; 
}
int test_knight_move(int start_row,int start_col,int end_row,int end_col){
    int col_change = abs(end_col - start_col);
    int row_change = abs(end_row - start_row);

    // Knight can only move two suqares vertically and one horizontally or vice versa
    if((col_change == 1 && row_change == 2) || (col_change == 2 && row_change == 1)){
        return 0;
    }
    return 1;
}
int test_bishop_move(int start_row,int start_col,int end_row,int end_col,char **board){
    int col_change = abs(end_col - start_col);
    int row_change = abs(end_row - start_row);

    // If we aren't moving as much horizontally as vertically its not diagonal
    if(col_change == row_change){
        int col_dir = (end_col > start_col) ? 1 : -1;
        int row_dir = (end_row > start_row) ? 1 : -1;

        for(int i = 1;i< row_change;i++){
            if(board[start_row + (i * row_dir)][start_col + (i * col_dir)] != EMPTY_SQUARE){
                return 1;
            }
        }
        return 0;
    }
    return 1;
}
int test_queen_move(int start_row,int start_col,int end_row,int end_col,char **board){
    int col_change = abs(end_col - start_col);
    int row_change = abs(end_row - start_row);
    int col_dir = (end_col > start_col) ? 1 : -1;
    int row_dir = (end_row > start_row) ? 1 : -1;
    int col_delta = (col_change == 0) ? 0 : 1;
    int row_delta = (row_change == 0) ? 0 : 1;

    // For diagonal moves
    if(col_change == row_change){
        // Check for obsruction
        for(int i = 1;i< row_change;i++){
            if(board[start_row + (i * row_dir)][start_col + (i * col_dir)] != EMPTY_SQUARE){
                return 1;
            }
        }
        return 0;
    }
    // For rook moves
    else if((col_change == 0) || (row_change == 0)){
        //Check for obstruction
        for(int i=1;i<col_change + row_change;i++){
            if(board[start_row + (row_delta * row_dir * i) ][start_col + (col_delta * col_dir * i)] !=  EMPTY_SQUARE){
                return 1;
            }
        } 
        return 0;
    }
    return 1;
}
int test_king_move(int start_row,int start_col,int end_row,int end_col,char **board){
    int col_change = abs(end_col - start_col);
    int row_change = abs(end_row - start_row);

    // For regular moves
    if((col_change <= 1) && (row_change <= 1)){
        // Update the king_moves variable used to check for castling rights
        return 0;
    }

    // Check if castling is allowed
    if(row_change == 0  && col_change == 2 && start_col == 4 ){
       return test_castling(start_row,start_col,end_row,end_col,board);
    }

    return 1;
}
int test_castling(int start_row,int start_col,int end_row,int end_col,char **board){
    int col_dir = (start_col < end_col) ? 1 : -1;
    int castling_distance = (end_col == 2) ? 4 : 3;
    int rook_col = (end_col == 2) ? 0 : 1;
    int square_is_threatened = 1;

    // Check if the king has moved
    if(king_moves & (1U<<turn)){
        printf("King has moved, fuck you");
        return 1;
    }
    
    //Check if the rook has moved
    if(rook_moves & (1U<<(rook_col + turn*2))){
        return 1;
    }
    // Check for obstuction
    for(int i = 1; i < castling_distance;i++){
        if(board[start_row][start_col + (col_dir * i)] != EMPTY_SQUARE){
            return 1;
        }
    }
    // Check that no enemy piece is threatening squares the king moves through, pretty shit but fine by now.
    for(int i = 0;i<8;i++){
        for(int j = 0;j<8;j++){
            if((turn == 0 && board[i][j] > WHITE_PIECES) || (turn == 1 && board[i][j] < WHITE_PIECES && board[i][j] != EMPTY_SQUARE)){
                square_is_threatened = test_for_threat(i,j,end_row,start_col + col_dir,board);
                if(square_is_threatened == 0){
                    return 1;
                } 
                square_is_threatened = test_for_threat(i,j,end_row,end_col,board);
                if(square_is_threatened == 0){
                    return 1;
                } 
            }
        }
    }
    // Get the rooks actual position from the earlier used rook_col variable
    rook_col = (rook_col == 0) ? 0 : 7;

    // Update the rooks position and the king_moves variable
    board[end_row][rook_col] = EMPTY_SQUARE;
    board[end_row][end_col - col_dir] = (turn == 0) ? 'R' : 'r';
    king_moves |= (1U<<turn);

    return 0;
}
int test_for_threat(int start_row,int start_col,int end_row,int end_col,char **board){
    int threat = 1;

    // ***This needs to be combined with the previous switch somehow***
    char char_piece = (int)board[start_row][start_col];
    char_piece = (char_piece>= 'A' && char_piece<= 'Z') ? char_piece+('a'-'A'):char_piece;
    enum pieces piece = char_piece;

    // Based on what piece we are moving we check if the move is legal
    switch (piece){
        case PAWN:
            threat = test_pawn_move(start_row,start_col,end_row,end_col,board);
            break;
        case ROOK:
            threat = test_rook_move(start_row,start_col,end_row,end_col,board);
            break;
        case KNIGHT:
            threat = test_knight_move(start_row,start_col,end_row,end_col);
            break;
        case BISHOP:
            threat = test_bishop_move(start_row,start_col,end_row,end_col,board);
            break;
        case QUEEN:
            threat = test_queen_move(start_row,start_col,end_row,end_col,board);
            break;
        case KING:
            threat = test_king_move(start_row,start_col,end_row,end_col,board);
            break;
    }
    return threat;
}
void test_for_mate(char **board){


}