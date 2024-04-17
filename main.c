#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

int main(void){

    // Initialize gamestate variables
    Gamestate* gamestate = (Gamestate*)malloc(sizeof(Gamestate));
    if(gamestate == NULL)
        return EXIT_FAILURE;

    // Create the board
    char** board = (char**)malloc(8 * sizeof(char*));
    if(board == NULL)
        return EXIT_FAILURE;
    
    for(int i = 0;i < 8;i++){
        board[i] = (char*)malloc(8*sizeof(char));
        if(board[i] == NULL){
            for(int j = 0;j < i;j++){
                free(board[j]);
            }
            return EXIT_FAILURE;
        }
    }
    START:
    // Initialize the board and game variables
    init_board(board,gamestate);
    display(board);

    // Ask moves and display the board until game over
    while(!gamestate->game_over){
        gamestate->turn ^= 1;
        ask_move(board,gamestate);
        display(board);
    }
    
    display_winner(board,gamestate);

    int choice = 0;
    printf("\nPress 1 to play again\nPress anything else to exit\n");
    scanf("%d",&choice);
    if(choice == 1)
        goto START;

    for(int i=0;i<8;i++){
        free(board[i]);
    }
    board = NULL;
    free(gamestate);

    return 0;
}

