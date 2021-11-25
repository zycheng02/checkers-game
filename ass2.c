/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Zhiyuan Cheng 1271641]
  Dated:     [10/10/2021]

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>


/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

#define WHITE_ROUND         1
#define BALCK_ROUND         0
#define MAX_COMMAND         20

/* one type definition from my sample solution -------------------------------*/
typedef char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

typedef struct{
  int r; // row, start from 0
  int c; // column, start from 0
} cell_t;

typedef struct{
  cell_t s; // start cell
  cell_t e; // end cell
} move_t;

typedef enum{
  SUCCESS = 0,
  E_SOURCE_OUT,
  E_TARGET_OUT,
  E_SOURCE_EMPTY,
  E_TARGET_OCC,
  E_WRONG_CELL,
  E_OTHER
} error_type_t; // for check valid move


// function declare
cell_t get_cell(char *);
move_t get_move(char *);

/* basic function for running the game */
void reset_board(board_t);
void print_board(board_t);
void take_move(board_t, move_t);
error_type_t is_move_valid(board_t, move_t, int);
void show_error_msg(error_type_t);

int compute_score(board_t);

/* for minmax rule */
// get all the possible moves of a cell
move_t * get_possible_moves(board_t, cell_t, int *, int);

int minmax_recursive(board_t, int round, int depth);



int
main(int argc, char *argv[]) {
    

    board_t board;
    reset_board(board);
    char command[MAX_COMMAND];
    int round = BALCK_ROUND;
    while (scanf("%s", command) > 0)
    {        
        if (strlen(command) == 0) break;
        
        if (strlen(command) >= 5){
            // move command
            move_t move = get_move(command);
            error_type_t etype = is_move_valid(board, move, round %2);

            if (etype != SUCCESS){
              show_error_msg(etype);
              break;
            }
            take_move(board, move);
            printf("=====================================\n");
            if (round % 2 == BALCK_ROUND)
                printf("BLACK ACTION #%d: %s\n", round + 1, command);
            else 
                printf("WHITE ACTION #%d: %s\n", round + 1, command);
            printf("BOARD COST: %d\n", compute_score(board));
            print_board(board);
        }else if (strcmp(command, "A") == 0) {
            // auto mate play
            printf("=====================================\n");
            minmax_recursive(board, round, 0);
            break;
        }else if (strcmp(command, "P") == 0) {
             // auto mate play ten rounds

            for (int i = 0; i < COMP_ACTIONS; i++)
            {
                printf("=====================================\n");
                int best_score = minmax_recursive(board, round, 0);
                round ++;            
                if (best_score == INT_MAX || best_score == INT_MIN) break;
                
            }
            
            break;
        }else{
          // unkown command, pass
          continue;
        }

        round = round + 1;
        
    }
    
    return EXIT_SUCCESS;            // exit program with the success code
}

cell_t get_cell(char *command){
  assert(strlen(command) >= 2);
  int r = command[1] - '1';
  int c = command[0] - 'A';
  cell_t cell = {r, c};
  return cell;
}

move_t get_move(char *line){
  cell_t start = get_cell(&line[0]);
  cell_t end = get_cell(&line[3]);
  move_t move = {start, end};
  return move;
}

void reset_board(board_t board){
  printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
  printf("#BLACK PIECES: %d\n", (ROWS_WITH_PIECES) *(BOARD_SIZE)/2);
  printf("#WHITE PIECES: %d\n", (ROWS_WITH_PIECES) *(BOARD_SIZE)/2);
  for (int i = 0; i < BOARD_SIZE; i++){

    char specical_cell = CELL_EMPTY;
    if (i < ROWS_WITH_PIECES ) specical_cell = CELL_WPIECE;
    if (i >= BOARD_SIZE - ROWS_WITH_PIECES) specical_cell = CELL_BPIECE;
    for (int j = 0; j < BOARD_SIZE; j++){
        if ((i + j) %2 > 0) board[i][j] = specical_cell;
        else board[i][j] = CELL_EMPTY;
    }        
  }
  print_board(board);
  
}

void print_board(board_t board){
    printf("     A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf(" %d |", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c |", board[i][j]);
        }
        printf("\n   +---+---+---+---+---+---+---+---+\n");        
    }
    
}


int compute_score(board_t board){
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            char c = board[i][j];
            if (c == CELL_BPIECE) {score ++;}
            else if (c == CELL_WPIECE) {score --;}
            else if (c == CELL_BTOWER) {score += 3;}
            else if (c == CELL_WTOWER) {score -= 3;}          
            
        }        
    }
    return score;    
}

int get_move_length(move_t move){
  if(abs(move.s.r - move.e.r) == abs(move.s.c - move.e.c))
    return abs(abs(move.s.r - move.e.r));
  else return -1;
}
cell_t middle_cell(move_t move){
  cell_t c = {(move.s.r + move.e.r)/2, (move.s.c + move.e.c)/2};
  return c;
}


void take_move(board_t board, move_t move){
  // we think the move is valid
  char c = board[move.s.r][move.s.c];
  board[move.s.r][move.s.c] = CELL_EMPTY;

  // if white touch the last row, or black touch the first row, 
  if (c == CELL_BPIECE && move.e.r == 0){
    board[move.e.r][move.e.c] = CELL_BTOWER;
  }else if (c == CELL_WPIECE && move.e.r == BOARD_SIZE - 1){
    board[move.e.r][move.e.c] = CELL_WTOWER;
  }else board[move.e.r][move.e.c] = c;

  if (get_move_length(move) == 2){
      cell_t cell = middle_cell(move);
      board[cell.r][cell.c] = CELL_EMPTY;
  }


}

int is_valid_cell(cell_t cell){
  return cell.r >= 0 && cell.c >= 0 && cell.r < BOARD_SIZE && \
    cell.c < BOARD_SIZE;
}
error_type_t is_move_valid(board_t board, move_t move, int round){
  if (!is_valid_cell(move.s)) return E_SOURCE_OUT;
  

  if(!is_valid_cell(move.e)) return E_TARGET_OUT;

  char source = board[move.s.r][move.s.c];
  char target = board[move.e.r][move.e.c];

  if (source == CELL_EMPTY)
    return E_SOURCE_EMPTY;
  
  if (target != CELL_EMPTY)
    return E_TARGET_OCC;

  if (round == BALCK_ROUND && (source == CELL_WPIECE || source == CELL_WTOWER)) 
    return E_WRONG_CELL;

  if (round == WHITE_ROUND && (source == CELL_BPIECE || source == CELL_BTOWER)) 
    return E_WRONG_CELL;

  if (source == CELL_BPIECE && move.s.r < move.e.r)
    return E_OTHER;
  
  if (source == CELL_WPIECE && move.s.r > move.e.r)
    return E_OTHER;
  

  if (get_move_length(move) != 1 && get_move_length(move) != 2) return E_OTHER;

  if (get_move_length(move) == 2){
    // middle cell should be the other color
    cell_t middle = middle_cell(move);
    char pass = board[middle.r][middle.c];
    if (pass == CELL_EMPTY) return E_OTHER;
    if (pass == source ) return E_OTHER;
    if (pass - source == 'B' - 'b' || pass - source == 'b' - 'B' ) \
      return E_OTHER;

  }
  return SUCCESS;

}


void show_error_msg(error_type_t etype){
  switch (etype)
  {
  case E_SOURCE_OUT:
     printf("ERROR: Source cell is outside of the board.\n");
    break;
  
  case E_TARGET_OUT:
     printf("ERROR: Target cell is outside of the board.\n");
    break;
  
  case E_SOURCE_EMPTY:
     printf("ERROR: Source cell is empty.\n");
    break;
  case E_TARGET_OCC:
     printf("ERROR: Target cell is not empty.\n");
    break;
  case E_WRONG_CELL:
     printf("ERROR: Source cell holds opponent's piece/tower.\n");
      break;
    
  case E_OTHER:
     printf("ERROR: Illegal action.\n");
      break;  
  
  default:
    break;
  }
}

/** return all possible moves in a array allocated
 * the length is return through *
 * */

move_t * get_possible_moves(board_t board, cell_t cell, int *size, int round){
    int valid_move[4] = {0};
    error_type_t etype;
    char source = board[cell.r][cell.c];
    
    // for each of the 4 direction
    
    cell_t diffs[4] = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
    for (int i = 0; i < 4; i++){
      for (int j = 1; j <= 2; j++)
      {     
      
        move_t move = {cell, {cell.r + j*diffs[i].r, cell.c + j*diffs[i].c}};
        etype = is_move_valid(board, move, round);
        if (etype == SUCCESS){
          valid_move[i] = j;
          break;
        }
      }
    }

    *size = 0;
    for (int i = 0; i < 4; i++)
    {
      if (valid_move[i]){
        *size += 1;
      }
    }
    if (*size == 0) return NULL;

    move_t * moves = (move_t *) malloc(sizeof(move_t)*(*size));
    int j = 0;
    for (int i = 0; i < 4; i++)
    {
      if (valid_move[i]){
        moves[j].s = cell;
        moves[j].e.r = cell.r + valid_move[i] * diffs[i].r;
        moves[j].e.c = cell.c + valid_move[i] * diffs[i].c;
        j ++;
      }
    }

    return moves;    
}

void board_copy(board_t dest, board_t src){
  for (int i = 0; i < BOARD_SIZE; i++) {
      for (int j = 0; j < BOARD_SIZE; j++){
        dest[i][j] = src[i][j];
      }
  }
}

int minmax_recursive(board_t board, int round, int depth){

  if (depth >= TREE_DEPTH) return compute_score(board);
  int size;
  move_t best_move;
  int best_score;
  if (round % 2 == BALCK_ROUND) best_score = INT_MIN; // find max, set as min
  else best_score = INT_MAX;
  board_t board_bak;
  
  for (int i = 0; i < BOARD_SIZE; i++) {
      for (int j = 0; j < BOARD_SIZE; j++){
          int cell = board[i][j];
          
          if (round == BALCK_ROUND && cell != CELL_BPIECE && \
              cell != CELL_BTOWER) continue;
          if (round == WHITE_ROUND && cell != CELL_WPIECE && \
              cell != CELL_WTOWER) continue;
          cell_t c = {i, j};
          move_t * moves = get_possible_moves(board, c, &size, round % 2);
          if (size == 0) continue;
          for (int l = 0; l < size; l++)
          {
            board_copy(board_bak, board);
            take_move(board_bak, moves[l]);
            int result = minmax_recursive(board_bak, (round + 1)%2, depth + 1);
            if (round % 2 == BALCK_ROUND && result > best_score){
              best_score = result;
              best_move = moves[l];
            }else if (round % 2 == WHITE_ROUND && result < best_score)
            {
              best_score = result;
              best_move = moves[l];
            }
            
          }
          free(moves);
          
      }
      
    }
  
  if (depth == 0 && best_score != INT_MAX && best_score != INT_MIN){
    // find a best move, 
    take_move(board, best_move);
    if (round % 2 == BALCK_ROUND) printf("*** BLACK ACTION");
    else printf("*** WHITE ACTION");
    printf(" #%d: ", round + 1);
    printf("%c%c-%c%c\n", best_move.s.c + 'A', best_move.s.r + '1',\
           best_move.e.c + 'A', best_move.e.r + '1');
    printf("BOARD COST: %d\n", compute_score(board));
    print_board(board); 
  }
  if (depth == 0 && best_score == INT_MAX){
     printf("BLACK WIN!\n");
  }
   if (depth == 0 && best_score == INT_MIN){
     printf("WHITE WIN!\n");
  }


  return best_score;
  

}
//"algorithms are fun"
/* THE END -------------------------------------------------------------------*/