#include "laser.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

bool is_on_board(board_t board, int row, int col) {
    return row >= 0 && row < board.height && col >= 0 && col < board.width;
}

int find_start(board_t board) {
    for (int r = 0; r < board.height; r++) {
        if (board.cells[r][0] == '>') {
            return r;
        }
    }
    return -1;
}

char get_mirror_label(board_t board, int row, int col) {
    assert(is_on_board(board, row, col));
    if (strchr(MIRRORS, board.cells[row][col]) == NULL) {
        return '\0';
    }
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            char ch = board.cells[row + i][col + j];
            if (isalpha(ch)) {
                return ch;
            }
        }
    }
    return '\0';
}

board_t load_board(const char *filename) {
    board_t board;
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Cannot open file, gimme another\n");
        return board;
    }
    fscanf(f, "%d %d\n", &board.height, &board.width);
    int h = board.height, w = board.width;
    board.cells = malloc(h * sizeof(char *));
    if (board.cells == NULL) {
        fprintf(stderr, "Cannot alloc memory for board.cells ?!?!\n");
        return board;
    }
    board.cells[0] = malloc(h * w * sizeof(char));
    if (board.cells[0] == NULL) {
        fprintf(stderr, "Cannot alloc memory for board.cells[0] ?!?!\n");
        free ( board.cells );
        return board;
    }

    // now wire up the row pointers correctly
    for (int i = 1; i < h; i++) {
        board.cells[i] = board.cells[i-1] + w;
    }

    // now read the board
    char buffer[w + 2]; // read the 32 characters, plus \n and the NULL terminator
    int i = 0;
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        for (int j = 0; j < w; j++) {
            board.cells[i][j] = buffer[j];
        }
        // printf("i is %d, line : %s, strlen : %d\n", i, buffer, strlen(buffer));
        i++;
    }
    fclose(f);
    return board;
}

void free_cells(board_t board) {
    free ( board.cells[0] );
    free ( board.cells );
}

direction_t change_direction(direction_t direction, char cell) {
    if (cell == FWD_MIRROR) {
        switch (direction) {
            case EAST:
                return NORTH;
            case SOUTH:
                return WEST;
            case WEST: 
                return SOUTH;
            case NORTH:
                return EAST;
        }
    } else if (cell == BWD_MIRROR)  {
        switch (direction) {
            case EAST:
                return SOUTH;
            case SOUTH:
                return EAST;
            case WEST: 
                return NORTH;
            case NORTH:
                return WEST;
        }
    }
    // not a mirror
    return direction;
}

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

char step_laser(board_t board, laser_state_t *laser) {
    assert(is_on_board(board, laser->row, laser->col));
    // Change direction if current cell is a mirror
    char cell = board.cells[laser->row][laser->col];
    laser->direction = change_direction(laser->direction, cell);
    // advance laser beam 1 step
    switch (laser->direction) {
        case EAST: // to right side
            laser->col++;
            break;
        case SOUTH: // downwards
            laser->row++;
            break;
        case WEST: // to left side
            laser->col--;
            break;
        case NORTH: // upwards
            laser->row--;
            break;
    }
    // update laser message
    char label = get_mirror_label(board, laser->row, laser->col);
    if (label != '\0') {
        append(laser->msg, label);
    }
    return board.cells[laser->row][laser->col];
}

bool shoot(board_t board, laser_state_t *laser, bool trace) {
    assert(is_on_board(board, laser->row, laser->col));

    char curr = board.cells[laser->row][laser->col];
    if (strchr(BLOCKERS, curr) != NULL) { // if it starts with a blocker, go next step
        curr = step_laser(board, laser);
    }

    while (strchr(BLOCKERS, curr) == NULL) {
        if (trace && strchr(PASSTHROUGHS, curr) != NULL) {
            direction_t d = laser->direction;
            char tr = ' ';
            if (d == EAST || d == WEST) {
                tr = '-';
            } else {
                tr = '|';
            }
            // check if current cell already has a trace
            if ((curr == '-' && tr == '|') || (curr == '|' && tr == '-')) {
                board.cells[laser->row][laser->col] = '+';
            } else {
                board.cells[laser->row][laser->col] = tr;
            }
        }
        curr = step_laser(board, laser);
    }
    return curr == END;
}

bool solve(board_t board, laser_state_t laser, const char target[MAX_MSG_LEN]) {
    assert(is_on_board(board, laser.row, laser.col));
    return false;
}

#ifdef MAIN
int main(void) {
    board_t board = load_board("biscuit.txt");
    assert(find_start(board) != -1);
    printf("Initial board:\n");
    print_board(board);

    const char target[MAX_MSG_LEN] = "BIUC";
    printf("Attempt to construct word: %s\n", target);

    laser_state_t laser = {find_start(board), 0, EAST, ""};
    // Solve board
    bool success = solve(board, laser, target);
    // Trace board
    shoot(board, &laser, true);
    // Print traced board
    printf(success ? "Solution found!\n" : "Word cannot be constructed!\n");
    print_board(board);
    free_cells(board);
    return EXIT_SUCCESS;
}
#endif
