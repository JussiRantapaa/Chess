/*
    CORE MOVE GENERATION AND VALIDATION LOGIC
    1. Generate all legal moves and store them in a move list.
        1.1 Based on a pieces ruleset compute all of its moves
        1.2 If a move is within bounds we call the is_king_threatened function
        1.3 In is_king_threatened we make the move and loop all enemy pieces and see if they can attack the square the king is located
        1.4 If king is safe we add the move and continue looping
    2. Move information is stored into a 32bit integer as follows:
    - bits 0-5 starting square
    - bits 6-11 landing square
    - bits 12- store specific information of the move such as rookmoves,castling,capturing pieces etc..
    3. Ask user for input, hash it and see if it matches a move on the move list
        3.1 If we get a match check for bits 12- to see if the move contains special manouvers and handle them separately
        3.2 Make the move on the gameboard and update piece position
    4. repeat until move_count is 0 and then handle game ending
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"
#include <time.h>

void ask_move(char **board,Gamestate* gamestate){
    char start_pos[3];
    char end_pos[3];
    int correct_symbols = 0;
    int move_list[100];
    int move_count = 0;
    int row,col,end_row,end_col;
    int hash = 0;

    if(gamestate->turn == WHITE)
        printf("\n%d. White's turn\n",gamestate->move_number/2 + 1);
    else
        printf("\n%d. Black's turn\n",gamestate->move_number/2 + 1);

    // Compute all the moves for a palyer and store them in the move list
    compute_moves(board,move_list,&move_count,gamestate);
    printf("MOVE COUNT: %d\n",move_count);
    if(!move_count){
        gamestate->game_over = 1;
        return;
    }
    #ifdef DEBUG
         printf("ALL MOVES: ");
         for(int i=0;i < move_count;i++){
            printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nPAWN MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & PAWN_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nROOK MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & ROOK_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nKNIGHT MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & KNIGHT_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nBISHOP MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & BISHOP_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nQUEEN MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & QUEEN_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
        printf("\nKING MOVES: ");
        for(int i=0;i < move_count;i++){
            if(move_list[i] & KING_MOVE)
                printf("[%c%d %c%d]",(move_list[i] & 63) % 8 + 'A',((move_list[i] & 63)/ 8)+1,((move_list[i]>>6) & 63) % 8 + 'A',(( move_list[i]>>6) & 63) / 8 +1);
        }
    #endif
    // If blacks move, make random move
    if(gamestate->turn == BLACK){
        int rand_move_index = make_random_move(move_count);
        make_move(board,move_list[rand_move_index],gamestate);
        row = decode(move_list[rand_move_index],ROW);
        col = decode(move_list[rand_move_index],COL);
        end_row = decode((move_list[rand_move_index]>>6),ROW);
        end_col = decode((move_list[rand_move_index]>>6),COL);
        printf("\n\n---------------------------------\nOPPONENTS MOVE %c%d to %c%d",col + 'A',row + 1,end_col + 'A',end_row + 1);
    }else{
    // Ask for a move until a legal move is entered 
        while(!hash){
        printf("\n\nEnter move in format(a2 a4): "); 
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
            // Decode the user input into row and col values
            col = (int)(start_pos[0]-CHAR_TO_INT); 
            row = atoi(&(start_pos[1]))-1;
            end_col = (int)(end_pos[0]-CHAR_TO_INT);
            end_row = atoi(&(end_pos[1]))-1;

            // Test if the user entered move matches a move on the move list (returns 0 if invalid)
            hash = test_move(board,row,col,end_row,end_col,&move_count,move_list);
            if(hash){
                make_move(board,hash,gamestate);
            }else{
                printf("ILLEGAL MOVE!!!");
            }
        }
    }
    // Add the move to the move history
    gamestate->move_history[gamestate->move_number++] = hash;
}
int make_random_move(int move_count){
    time_t t;
    srand((unsigned) time(&t));
    int index = rand() % move_count;

    return index;
}
// Returns the hash of a row and column
int encode(int row,int col){
    return ((row * 8 + col));
}
// Returns either the row or column of a hash
int decode(int hash, int choice){
    if(choice == ROW)
        return (hash & 0x3F ) / 8;
    else
        return (hash & 0x3F) % 8;
}
// Hash the input and check if it matches with a move in the move list
int test_move(char **board,int row,int col,int end_row,int end_col,int* move_count,int* move_list){
    // Bits 0-5 are for the starting square and 6-12 for the landing square
    int hash = encode(row,col) | (encode(end_row,end_col)<<6);
    for(int i = 0;i < *move_count;i++){
        if((move_list[i] & 0xFFF) == hash)
            return move_list[i];
    }
    // If no match return 0
    return 0;
}
void swap(int* a,int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
// Make the move on the board and update game variables
void make_move(char** board,int hash,Gamestate* gamestate){
    int row = decode(hash,ROW);
    int col = decode(hash,COL);
    int end_row = decode(hash>>6,ROW);
    int end_col = decode(hash>>6,COL);
    int turn = gamestate->turn;
    board[end_row][end_col] = board[row][col];
    board[row][col] = EMPTY_SQUARE;
    
    // Check for special moves and update their respected variables 
    if(hash & CAPTURE_MOVE){
        int captured_piece_index = get_piece_index(hash,gamestate,ENEMY_PIECE);
        // This looks horrible but really we just find the index of the captured piece, swap it with the index of the last piece
        // And decrement the piece_count
        swap(&gamestate->pieces[!turn][captured_piece_index],&gamestate->pieces[!turn][gamestate->piece_count[!turn] - 1]);
        gamestate->piece_count[!turn]--;
       // printf("Deleting piece from hash %d\n",(hash>>6) & 0x3F);
    }
    if(hash & CASTLING_MOVE){
        make_castling_move(board,row,col,end_row,end_col,gamestate);
    }else if(hash & ROOK_MOVE){
        int rook_bit = (col == 0) ? 0 : 2;
        // Update the rook_moves variable. Bit 0 = white A rook, bit 1 = black A rook, bit 2 = white h rook, bit 3 = black h rook
        gamestate->rook_moves &=~ (1U<<(turn + rook_bit));
    }else if(hash & KING_MOVE){
        gamestate->king_moves &=~(1U<<turn);
        gamestate->kingpos[turn] = (hash>>6) & 0x3F;
    }else if(hash & EN_PASSANT_MOVE){
        // Get the hash of the piece that got En passanted
        int dir = (turn == WHITE) ? -1 : 1;
        int en_passant_hash = encode(end_row-dir,end_col);

        // Find the index of the piece that got captured,swap it with the last index and decrement piece count
        swap(&gamestate->pieces[!turn][get_piece_index(en_passant_hash,gamestate,ENEMY_PIECE)],&gamestate->pieces[!turn][gamestate->piece_count[!turn] - 1]);
        gamestate->piece_count[!turn]--;
        board[end_row + dir][end_col] = EMPTY_SQUARE;
    }else if(hash & PAWN_MOVE && (end_row == 7 || end_row == 0)){
        // Handle pawn promotion
        board[end_row][end_col] = (turn == WHITE) ? 'Q' : 'q';
    }

    // Update the position of the piece we moved
    for(int i = 0;i < gamestate->piece_count[turn];i++){
        if(gamestate->pieces[turn][i] == (hash & 0x3F)){
            gamestate->pieces[turn][i] = ((hash>>6) & 0x3F);
            return;
        }
    }
}
int get_piece_index(int hash,Gamestate* gamestate,int piece_color){
    // Piece_color determines which color pieces we access
    int color = gamestate->turn;
    if(piece_color == ENEMY_PIECE)
        color = !color;

    // Loop the piece array and return the index when we find a match
    for(int i = 0;i < gamestate->piece_count[color];i++){
        if((gamestate->pieces[color][i] & 0x3F) == ((hash>>6) & 0x3F)){
            return i;
        }
    }
   return EXIT_FAILURE;
}
void make_castling_move(char** board,int row,int col,int end_row,int end_col,Gamestate* gamestate){
    int rook_hash = 0;
    int new_rook_col = 0;
    // Based on the end_col we move the rook on the board and create it's hash so we can find it from the piece list
    // Queens side castling
    if(end_col == 2){
        board[row][3] = board[row][0];
        board[row][0] = EMPTY_SQUARE;
        rook_hash = encode(row,0);
        new_rook_col = 3;
    }else{
    // Kings side castling
        board[row][5] = board[row][7];
        board[row][7] = EMPTY_SQUARE;
        rook_hash = encode(row,7);
        new_rook_col = 5;
    }
    int rook_index = get_piece_index(rook_hash,gamestate,FRIENDLY_PIECE);
    // Update the rook position in the pieces array and the king moves variable
    gamestate->pieces[gamestate->turn][rook_index] = encode(row,new_rook_col);
    gamestate->king_moves &=~(1U<<gamestate->turn);
}
void add_move(int row,int col,int end_row,int end_col,int* moves,int* move_count,int info){
    // Hash the move and add it to the list
    // Bits 0-5 point the starting square and 6-12 the landing square
    int move = (row * 8 + col) | ((end_row * 8 + end_col)<<6) | info;
    moves[*move_count] = move;
    (*move_count)++;
}
// Returns the status of a square on the gameboard
int get_square_status(char** board,int row, int col,Gamestate* gamestate){
    int turn = gamestate->turn;
    int piece = board[row][col];
    if((turn == WHITE && piece > WHITE_PIECES) || (turn == BLACK && piece < WHITE_PIECES && piece != EMPTY_SQUARE))
        return ENEMY_PIECE;
    else if(piece == EMPTY_SQUARE)
        return EMPTY_SQUARE;
    else
        return FRIENDLY_PIECE;
}

/****RAY FUNCTIONS****
    -Each function uses a ray to test if it can attack a specific square on the board
    -These functions are used when testing king threats*/
int vertical_ray(int row,int col,int new_row,int new_col,char** board){
    int dir = (new_row > row) ? 1 : -1;
    row += dir;
    //nIf we are not in same column early exit
    if(col != new_col)
        return 0;
    // Check the status of all squares in between, if we hit something return 0
    while(row != new_row && row < 8 && row >= 0){
        if(board[row][col] != EMPTY_SQUARE)
            return 0;
        row += dir;
    }
    // Return 1 if we are in the target square
    if(row == new_row)
        return 1;
    return 0;
}
int horizontal_ray(int row,int col,int new_row,int new_col,char** board){
    int dir = (new_col > col) ? 1 : -1;
    col += dir;
    // If not we are not in the same row early exit
    if(row != new_row)
        return 0;
    // Check the status of all squares in between, if we hit something return 1
    while(col != new_col && col < 8 && col >= 0){
        if(board[row][col] != EMPTY_SQUARE)
            return 0;
        col += dir;
    }
    // Return 1 if we are in the target square
    if(col == new_col)
        return 1;
    return 0;
}
int diagonal_ray(int row,int col,int new_row,int new_col,char** board){
    int row_dir = (new_row > row) ? 1 : -1;
    int col_dir = (new_col > col) ? 1 : -1;
    col += col_dir;
    row += row_dir;
    // If we land in the same row or column we stop looping or if we hit a piece we can exit
    while(col != new_col && row != new_row && col < 8 && col >= 0 && row < 8 && row  >= 0 ){
        if(board[row][col] != EMPTY_SQUARE){
            return 0;
        }
        col += col_dir;
        row += row_dir;
    }
    // If we get into target square we have a hit
    if(row == new_row && col == new_col)
        return 1;
    return 0;
}
int test_rook_threat(int row,int col,int new_row,int new_col,char** board){
    return vertical_ray(row,col,new_row,new_col,board) || horizontal_ray(row,col,new_row,new_col,board);
}
int test_bishop_threat(int row,int col,int new_row,int new_col,char** board){
    return diagonal_ray(row,col,new_row,new_col,board);
}
int test_queen_threat(int row,int col,int new_row,int new_col,char** board){
    return test_bishop_threat(row,col,new_row,new_col,board) || test_rook_threat(row,col,new_row,new_col,board);
}
int test_pawn_threat(int row,int col,int new_row,int new_col,char** board,Gamestate* gamestate){
    int dir = (gamestate->turn == WHITE) ? -1 : 1;
    // Check one square diagonally for both directions 
    if(new_row == row + dir && (new_col == col + 1 || new_col == col - 1))
        return 1;
    return 0;
}
int test_knight_threat(int row,int col,int king_row,int king_col,char** board){
    int row_distance = abs(king_row - row);
    int col_distance = abs(king_col - col);
    // if distance is 1 & 2 or 2 & 1 we have a hit
    if((row_distance == 1 && col_distance == 2) || (row_distance == 2 && col_distance == 1))
        return 1;
    return 0;
}
int test_king_threat(int row,int col,int king_row,int king_col,char** board){
    int row_distance = abs(king_row - row);
    int col_distance = abs(king_col - col);
    // If distance is 1 in any direction we have a hit
    if(row_distance <= 1 && col_distance <= 1)
        return 1;
    return 0;
}
// Check if king is threatened by a move 0 = no threat, 1 = threat
int is_king_threatened(int row,int col,int new_row,int new_col,char** board,int move_info,Gamestate* gamestate){
    // If a move is valid according to the pieces ruleset, we still have to ensure it doesn't endanger our king
    // We make the move under test on the board, run through enemy pieces and see if our king is in danger
    // Finally we restore the board to it's original state
    
    // Get our kings coordinates from the gamestate variable
    int turn = gamestate->turn;
    int king_row = decode(gamestate->kingpos[turn],ROW);
    int king_col = decode(gamestate->kingpos[turn],COL);

    // En passant stuff
    int dir = (turn == WHITE) ? -1 : 1;
    char en_passant_piece = EMPTY_SQUARE;

    // If we are moving the king, we have to refererence the new squares
    if(move_info == KING_MOVE){
       king_row = new_row;
       king_col = new_col;
    }else if(move_info == EN_PASSANT_MOVE){
        // If we en passant, the piece that gets EN PASSANTE'D has to be stored
        en_passant_piece = board[row - dir][col];
        board[row - dir][col] = EMPTY_SQUARE;
    }
    int threat = 0;
    // Pretend to make the move on the board and store the information of the square we are attacking
    char previous_piece = board[new_row][new_col];
    board[new_row][new_col] = board[row][col];
    board[row][col] = EMPTY_SQUARE;

    int enemy_pieces = (turn == WHITE) ? 1 : 0;

    // Run through the enemy pieces on the board
    for(int i = 0; i < gamestate->piece_count[enemy_pieces];i++){
        int enemy_row = decode(gamestate->pieces[enemy_pieces][i],ROW);
        int enemy_col = decode(gamestate->pieces[enemy_pieces][i],COL);
        if(enemy_col == new_col && enemy_row == new_row){
            // If we are testing the piece that got eaten we do nothing
        }else{
            char piece = board[enemy_row][enemy_col];

            if(piece == 'R' || piece == 'r')
                threat = test_rook_threat(enemy_row,enemy_col,king_row,king_col,board);
            else if(piece == 'B' || piece == 'b')
                threat = test_bishop_threat(enemy_row,enemy_col,king_row,king_col,board);
            else if(piece == 'Q' || piece == 'q')
                threat = test_queen_threat(enemy_row,enemy_col,king_row,king_col,board);
            else if(piece == 'P' || piece == 'p')
                threat = test_pawn_threat(enemy_row,enemy_col,king_row,king_col,board,gamestate);
            else if(piece == 'N' || piece == 'n')
                threat = test_knight_threat(enemy_row,enemy_col,king_row,king_col,board);
            else 
                threat = test_king_threat(enemy_row,enemy_col,king_row,king_col,board);
        }
        if(threat){
            goto EXIT_LOOP;
        }
    }
    EXIT_LOOP:
    // Place the board to its original position
    board[row][col] = board[new_row][new_col];
    board[new_row][new_col] = previous_piece;

    if(move_info == EN_PASSANT_MOVE)
        board[row - dir][col] = en_passant_piece;
    
    return threat;
}
// Computes all the moves for a said piece, this function is only called once every turn so the time lost by looping is not
// that expensive
void compute_moves(char** board,int* moves,int* move_count,Gamestate* gamestate){
    for(int i = 0;i < 8;i++){
        for(int j = 0;j < 8;j++){
            int current_square = get_square_status(board,i,j,gamestate);
            if(current_square == FRIENDLY_PIECE){
                int current_piece = board[i][j];
                current_piece = (current_piece >= 'A' && current_piece <= 'Z') ? current_piece + ('a'-'A'):current_piece;
                enum pieces piece = current_piece;
                switch (piece){
                case PAWN:
                   compute_pawn_moves(i,j,board,moves,move_count,gamestate);
                    break;
                case ROOK:
                    compute_rook_moves(i,j,board,moves,move_count,gamestate);
                    break;
                case KNIGHT:
                   compute_knight_moves(i,j,board,moves,move_count,gamestate);
                    break;
                case BISHOP:
                    compute_bishop_moves(i,j,board,moves,move_count,gamestate);
                    break;
                case QUEEN:
                    compute_queen_moves(i,j,board,moves,move_count,gamestate);
                    break;
                case KING:
                    compute_king_moves(i,j,board,moves,move_count,gamestate);
                    break;
                }
            }
        }
    }
}
void compute_pawn_moves(int row,int col,char** board,int* moves, int* move_count,Gamestate* gamestate){
    compute_forward_moves(row,col,board,moves,move_count,gamestate);
    compute_capture_moves(row,col,board,moves,move_count,gamestate);
    compute_en_passant_moves(row,col,board,moves,move_count,gamestate);
}
void compute_forward_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    int turn = gamestate->turn;
    int first_move = ((turn == WHITE && row == 1) || (turn == BLACK && row == 6)) ? 1 : 0;
    int move_dir = (turn == WHITE) ? 1 : -1;
    // Single row moves
    if(board[row + move_dir][col] == EMPTY_SQUARE  && !is_king_threatened(row,col,row + move_dir,col,board,0,gamestate)){
        add_move(row,col,row + move_dir,col,moves,move_count,PAWN_MOVE);
     // Two row moves
        if(first_move && board[row + move_dir*2][col] == EMPTY_SQUARE && !is_king_threatened(row,col,row + move_dir*2,col,board,0,gamestate))
            add_move(row,col,row + move_dir*2,col,moves,move_count,PAWN_DOUBLE_MOVE | PAWN_MOVE);
    }
}
void compute_capture_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    int turn = gamestate->turn;
    int move_dir = (turn == WHITE) ? 1 : -1;
    int left_col = col - 1;
    int right_col = col + 1;
    // Test left diagonal
    if(left_col < 8 && left_col >=0){
        int left_piece = get_square_status(board,row + move_dir,left_col,gamestate);
        if(left_piece == ENEMY_PIECE && !is_king_threatened(row,col,row + move_dir,left_col,board,0,gamestate)){
            //printf("Added move %d %d %d %d\n",row,col,row + move_dir,left_col);
            add_move(row,col,row + move_dir,left_col,moves,move_count,CAPTURE_MOVE | PAWN_MOVE);
        }
    // Test right diagonal
    }if(right_col < 8 && right_col >= 0){
        int right_piece = get_square_status(board,row + move_dir,right_col,gamestate);
        if((right_piece == ENEMY_PIECE && !is_king_threatened(row,col,row + move_dir,right_col,board,0,gamestate))){
            //printf("Added move %d %d %d %d\n",row,col,row + move_dir,right_col);
            add_move(row,col,row + move_dir,right_col,moves,move_count,CAPTURE_MOVE | PAWN_MOVE);
        }
    }
}
void compute_en_passant_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    // If the last move was not a double pawn move, early exit
    if(!(gamestate->move_history[gamestate->move_number-1] & PAWN_DOUBLE_MOVE)){
        return;
    }
    int prev_move_row = decode((gamestate->move_history[gamestate->move_number-1])>>6,ROW);
    int prev_move_col = decode((gamestate->move_history[gamestate->move_number-1])>>6,COL);
    int turn = gamestate->turn;
    int dir = (turn == WHITE) ? 1 : -1;
    // If previous move was a pawn double move and the column is adjacent to ours its legal
    if(abs(prev_move_col - col) == 1 && prev_move_row == row &&
        !is_king_threatened(row,col,row + dir,prev_move_col,board,EN_PASSANT_MOVE,gamestate)){
        add_move(row,col,row + dir,prev_move_col,moves,move_count,EN_PASSANT_MOVE | PAWN_MOVE);
    }
}
void compute_knight_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    // We can get every direction with two loops that go from negative to positive
    for(int dir1 = -1; dir1 <= 1;dir1 +=2){
        for(int dir2 = -1; dir2 <= 1;dir2 +=2){
            int new_row = row + 2 * dir1;
            int new_col = col + 1 * dir2;
            if(new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8){
                int current_square = get_square_status(board,new_row,new_col,gamestate);
                //printf("COMPUTING KNIGHT MOVE %d %d to %d %d\n",row,col,new_row,new_col);
                int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | KNIGHT_MOVE : KNIGHT_MOVE;
                if(current_square != FRIENDLY_PIECE && !is_king_threatened(row,col,new_row,new_col,board,0,gamestate))
                    add_move(row,col,new_row,new_col,moves,move_count,move_info);
            }
            new_row = row + 1 * dir1;
            new_col = col + 2 * dir2;
            if(new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8){
                int current_square = get_square_status(board,new_row,new_col,gamestate);
                //printf("COMPUTING KNIGHT MOVE %d %d to %d %d\n",row,col,new_row,new_col);
                int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | KNIGHT_MOVE : KNIGHT_MOVE;
                if(current_square != FRIENDLY_PIECE && !is_king_threatened(row,col,new_row,new_col,board,0,gamestate))
                    add_move(row,col,new_row,new_col,moves,move_count,move_info);
            }
        }
    }
}
// Computing horizontal, vertical and diagonal moves all follow the same pattern:
// 1. Loop the desired direction until we either hit our own piece or a enemy piece
// 2. Check that its not occupied by our own piece
// 3. Test if making the move opens our king
// Move_info variable is used to carry out if we move a rook so that we can handle castling related variables later on.
void compute_horizontal_moves(int row,int col,char** board,int* moves,int* move_count,int info,Gamestate* gamestate){
    for(int dir = -1; dir <= 1;dir += 2){
        int new_col = col + dir;
        int current_square = EMPTY_SQUARE;
        while(new_col < 8 && new_col >= 0 && current_square == EMPTY_SQUARE){
            current_square = get_square_status(board,row,new_col,gamestate);
            int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | info : info;
            if(current_square != FRIENDLY_PIECE  && !is_king_threatened(row,col,row,new_col,board,0,gamestate)){
                add_move(row,col,row,new_col,moves,move_count,move_info);
            }
            new_col += dir;
        }
    }
}
void compute_vertical_moves(int row,int col,char** board,int* moves,int* move_count, int info,Gamestate* gamestate){
    // Loop possible moves downwards and upwards
    for(int dir = -1; dir <= 1;dir += 2){
        int new_row = row + dir;
        int current_square = EMPTY_SQUARE;
        // While move is in bounds and current square is empty continue looping
        while(new_row < 8 && new_row >= 0 && current_square == EMPTY_SQUARE){
            current_square = get_square_status(board,new_row,col,gamestate);
            int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | info : info;
        if(current_square != FRIENDLY_PIECE && !is_king_threatened(row,col,new_row,col,board,0,gamestate)){
                add_move(row,col,new_row,col,moves,move_count,move_info);
            }
            new_row += dir;
        }
    }
}
void compute_diagonal_moves(int row,int col,char** board,int* moves,int* move_count,int info,Gamestate* gamestate){
    for(int dir1 = -1;dir1 <= 1; dir1 += 2){
        for(int dir2 = -1;dir2 <= 1;dir2 += 2){
            int current_square = EMPTY_SQUARE;
            int new_row = row + dir1;
            int new_col = col + dir2;
            while(new_row < 8 && new_row >= 0 && new_col < 8 && new_col >= 0 && current_square == EMPTY_SQUARE){

                current_square = get_square_status(board,new_row,new_col,gamestate);
                int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | info : info;
                if(current_square != FRIENDLY_PIECE && !is_king_threatened(row,col,new_row,new_col,board,0,gamestate)){
                    add_move(row,col,new_row,new_col,moves,move_count,move_info);
                }   
                new_row += dir1;
                new_col += dir2;
            }
        }
    }
}
void compute_bishop_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    compute_diagonal_moves(row,col,board,moves,move_count,BISHOP_MOVE,gamestate);
}
void compute_queen_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    compute_vertical_moves(row,col,board,moves,move_count,QUEEN_MOVE,gamestate);
    compute_horizontal_moves(row,col,board,moves,move_count,QUEEN_MOVE,gamestate);
    compute_diagonal_moves(row,col,board,moves,move_count,QUEEN_MOVE,gamestate);
}
void compute_rook_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    compute_horizontal_moves(row,col,board,moves,move_count,ROOK_MOVE,gamestate);
    compute_vertical_moves(row,col,board,moves,move_count,ROOK_MOVE,gamestate);
}
void compute_king_moves(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    for(int dir1 = -1; dir1 <= 1;dir1++){
        for(int dir2 = -1;dir2 <= 1;dir2++){
            int current_square = EMPTY_SQUARE;
            int new_row = row + dir1;
            int new_col = col + dir2;
            // If the move is in bounds and not the kings current square
            if(new_row < 8 && new_row >= 0 && new_col < 8 && new_col >= 0  && !(new_row == row && new_col == col)){
                current_square = get_square_status(board,new_row,new_col,gamestate);
                int move_info = (current_square == ENEMY_PIECE) ? CAPTURE_MOVE | KING_MOVE: KING_MOVE;
                // If the piece is not occupied by our own piece and moving to the square doesn't threaten king add the move
                if(current_square != FRIENDLY_PIECE && !is_king_threatened(row,col,new_row,new_col,board,KING_MOVE,gamestate)){
                add_move(row,col,new_row,new_col,moves,move_count,move_info);
                }
            }
        }
    }
    // If our king has not been moved we compute castling moves
    if(gamestate->king_moves & (1U<<gamestate->turn))
        compute_castling(row,col,board,moves,move_count,gamestate);
}
void compute_castling(int row,int col,char** board,int* moves,int* move_count,Gamestate* gamestate){
    int current_square = EMPTY_SQUARE;
    int castling_rights = 1;
    int move_info = KING_MOVE | CASTLING_MOVE;
    int turn = gamestate->turn;
    
    // Test if our left rook has been moved and that all squares between are empty
    if(gamestate->rook_moves & (1U<<turn)){
        for(int new_col = col - 1; new_col >= 1;new_col--){
            current_square = get_square_status(board,row,new_col,gamestate);
            if(current_square != EMPTY_SQUARE || is_king_threatened(row,col,row,new_col,board,KING_MOVE,gamestate))
                castling_rights = 0;
        }
        // If all squares meet the rules we add the mvoe
        if(castling_rights)
            add_move(row,col,row,col - 2,moves,move_count,move_info);
    }
    castling_rights = 1;
    // Test for other direction
    if(gamestate->rook_moves & 1U<<(turn + 2)){
    for(int new_col = col + 1; new_col <= 6;new_col++){
            current_square = get_square_status(board,row,new_col,gamestate);
            if(current_square != EMPTY_SQUARE || is_king_threatened(row,col,row,new_col,board,KING_MOVE,gamestate))
                castling_rights = 0;
        }
        if(castling_rights)
            add_move(row,col,row,col + 2,moves,move_count,move_info);
    }
}