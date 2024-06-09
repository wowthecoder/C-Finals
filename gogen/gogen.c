#include "gogen.h"
#include "letter_mask.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// ################################## PART B ##################################

#define PTR_CHECK(ptr, msg) if (!(ptr)) perror(msg), exit(EXIT_FAILURE)
#define MAX_LINE_LENGTH (512)

void parse_gogen_file(const char *filename, board_t board, char ***words_ptr) {
    FILE *file = fopen(filename, "r");
    PTR_CHECK(file, "File could not be opened!");

    // Parse board
    for (int r = 0; r < HEIGHT; ++r) {
        for (int c = 0; c < WIDTH; ++c) {
            assert(fscanf(file, " %c", &board[r][c]) == 1);
            if (board[r][c] != '.') {
                assert(board[r][c] >= 'A' && board[r][c] <= 'Y');
            }
        }
    }

    // Parse number of words
    int num_words;
    assert(fscanf(file, "%d\r\n", &(num_words)) == 1);

    // Parse words as a NULL terminated array of strings
    char **words = calloc(num_words + 1, sizeof(char*));
    PTR_CHECK(words, "Could not allocate memory for word pointers!");
    for (int i = 0; i < num_words; i++) {
        char buffer[MAX_LINE_LENGTH];
        assert(fscanf(file, "%s\r\n", buffer) == 1);
        words[i] = calloc(strlen(buffer) + 1, sizeof(char));
        PTR_CHECK(words[i], "Could not allocate memory for word!");
        strcpy(words[i], buffer);
    }
    *words_ptr = words;
}

void free_null_terminated_array(char **words) {
    for (int i = 0; words[i]; i++)
        free(words[i]);
    free(words);
}

void print_board(board_t board) {
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH; c++) {
            printf("%c ", board[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_words(char **words) {
    for (int i = 0; words[i]; i++)
        printf("%s\n", words[i]);
}

void copy_board(board_t dest, board_t src) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            dest[r][c] = src[r][c];
}

// ################################ QUESTION 1 ################################
bool get_letter_position(board_t board, char letter, int *row, int *col) {
    for (int r = 0; r < HEIGHT; r++ ) {
        for (int c = 0; c < WIDTH; c++) {
            if (board[r][c] == letter) {
                *row = r;
                *col = c;
                return true;
            }
        }
    }
    return false;
}

// ################################ QUESTION 2 ################################
bool valid_solution(board_t board, char **words) {
    // 1st condition: all letters from A to Y appears once
    char appeared[25];
    for (int r = 0; r < HEIGHT; r++) {
        for (int c = 0; c < WIDTH; c++) {
            char ch = board[r][c];
            if (ch < 'A' || ch > 'Y' || strchr(appeared, ch) != NULL) {
                return false;
            }
            appeared[ch - 'A'] = ch;
        }
    }

    // 2nd condition: all words in board
    for (int i = 0; words[i] != NULL; i++) {
        // for the current word, do this:
        char *curr = words[i];
        // store position of prev character
        int r_prev = -1, c_prev = -1;
        // *curr is the current char
        while (*curr != 0) {
            int r_curr, c_curr;
            bool found = get_letter_position(board, *curr, &r_curr, &c_curr);
            if (!found) {
                return false;
            }
            if (r_prev != -1 && c_prev != -1 && (abs(r_prev - r_curr) > 1 || abs(c_prev - c_curr) > 1)) {
                return false;
            }
            r_prev = r_curr;
            c_prev = c_curr;
            curr++;
        }
    }
    return true;
}

// ################################ QUESTION 3 ################################
void update(board_t board, letter_mask_t masks[NUM_LETTERS]) {
    for (int i = 0; i < NUM_LETTERS; i++) {
        char ch = 'A' + i;
        // ch exists on the board
        int row, col;
        bool found = get_letter_position(board, ch, &row, &col);
        if (found) {
            set_all_bits(&masks[i], false);
            set_bit_value(&masks[i], row, col, true);
        } else { // ch does not exist on the board
            for (int r = 0; r < HEIGHT; r++) {
                for (int c = 0; c < WIDTH; c++) {
                    if (board[r][c] >= 'A' && board[r][c] <= 'Y') {
                        set_bit_value(&masks[i], r, c, false);
                    }
                }
            }
            // if the mask is now in a fixed position (only 1 bits set) 
            if (!is_free_letter(masks[i])) {
                int frow, fcol;
                get_fixed_letter_pos(masks[i], &frow, &fcol);
                board[frow][fcol] = ch;
            }
        }
    }
}

// ################################ QUESTION 4 ################################
static int num_free_masks(letter_mask_t masks[NUM_LETTERS]) {
    int count = 0;
    for (int i = 0; i < NUM_LETTERS; i++) {
        if (is_free_letter(masks[i])) {
            count++;
        }
    }
    return count;
}

bool solve_board(board_t board, char **words) {
    // step 1: initialise masks for every letter
    letter_mask_t masks[NUM_LETTERS];
    for (int i = 0; i < NUM_LETTERS; i++) {
        set_all_bits(&masks[i], true);
    }
    update(board, masks);

    // step 2: refine masks until no more decrease in number of free letters
    int prev_free_masks = 26;
    while (num_free_masks(masks) < prev_free_masks) {
        prev_free_masks = num_free_masks(masks);
        for (int i = 0; words[i] != NULL; i++) {
            char *curr = words[i];
            while (*curr != 0) {
                char prev = *curr++;
                if (*curr != 0) {
                    intersect_neighbourhoods(&masks[prev - 'A'], &masks[*curr - 'A']);
                    update(board, masks);
                }
            }
        }
    }

    // step 3: guess and backtracking and pray that you get a solution
    if (valid_solution(board, words)) {
        // print_board(board);
        // printf("The words are: \n");
        // print_words(words);
        return true;
    }
    for (int j = 0; j < NUM_LETTERS; j++) {
        if (is_free_letter(masks[j])) {
            // try to fix a possible position for this letter
            for (int r = 0; r < HEIGHT; r++) {
                for (int c = 0; c < WIDTH; c++) {
                    if (get_bit_value(masks[j], r, c)) {
                        board_t boardb4;
                        copy_board(boardb4, board);
                        board[r][c] = 'A' + j;
                        if (solve_board(board, words)) {
                            return true;
                        }
                        // reset the board
                        copy_board(board, boardb4);
                    }
                }
            }
            // if all positions exhausted, there is no solution
            return false;
        }
    }
    // basically won't reach here but wtv
    return false;
}

// ################################ QUESTION 5 ################################
// Write the solution to gogen-challenge.txt on the next lines! You are allowed
// to compute this manually if you didn't manage to get the code working.





// Solution to gogen-challenge.txt should be above!

#ifdef MAIN_GOGEN
int main (void) {
    board_t board;
    char **words;
    parse_gogen_file("gogen-easy.txt", board, &words);
    printf("Here is the easy gogen puzzle from Figure 1:\n");
    print_board(board);
    if (solve_board(board, words)) {
        printf("\nSolved!\n");
        print_board(board);
    } else {
        printf("\nImpossible!\n");
    }
    free_null_terminated_array(words);

    parse_gogen_file("gogen-challenge.txt", board, &words);
    printf("Here is a more challenging gogen puzzle:\n");
    print_board(board);
    if (solve_board(board, words)) {
        printf("\nSolved!\n");
        print_board(board);
    } else {
        printf("\nImpossible!\n");
    }
    free_null_terminated_array(words);
    return 0;
}
#endif
