#include "letter_mask.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Create mask of last 7 bits 1, then invert it to get desired value (all bits 1 and last 7 bits 0)
// a 32 bit mask with last 7 bits 1 is 0...0111 1111, so 0x7F
#define SEVEN_BITMASK ~0x7F

// ################################## PART A ##################################

void print_bitmask(letter_mask_t mask) {
    for (int row = 0; row < HEIGHT; ++row) {
        printf("row %d: ", row);
        for (int col = 0; col < WIDTH; ++col) {
            printf("%d ", MSB(mask));
            mask = mask << 1;
        }
        printf("\n");
    }
    printf("unused: ");
    for (int i = HEIGHT * WIDTH; i < NUM_BITS; i++) {
        printf("%d", MSB(mask));
        mask = mask << 1;
    }
    printf("\n");
}

// ################################ QUESTION 1 ################################
bool get_bit_value(letter_mask_t mask, int row, int col) {
    assert(row < 5 && col < 5);
    // index in 1D array is row * 5 + col, where 5 is the col width
    return MSB(mask << (row * WIDTH + col));
}

// ################################ QUESTION 2 ################################
void set_bit_value(letter_mask_t *mask, int row, int col, bool value) {
    assert(row < 5 && col < 5);
    int shift = NUM_BITS - (row * WIDTH + col) - 1;
    // & ~(1 << shift) basically sets a particular bit to 0
    *mask = (*mask & ~(1 << shift)) | (value << shift);
}

// ################################ QUESTION 3 ################################
void set_all_bits(letter_mask_t *mask, bool value) {
    if (value) {
        *mask = SEVEN_BITMASK;
    } else {
        *mask = 0;
    }
}

// ################################ QUESTION 4 ################################
bool is_free_letter(letter_mask_t mask) {
    int count = 0;
    // Get the useful bits
    mask >>= (NUM_BITS - NUM_LETTERS);
    // Test bits 1 by 1 and increment count whenever we see a 1
    while (mask != 0) {
        if (mask & 1) {
            count++;
        }
        mask >>= 1;
    }
    return count > 1;
}

// ################################ QUESTION 5 ################################
bool get_fixed_letter_pos(letter_mask_t mask, int *row, int *col) {
    if (is_free_letter(mask)) {
        return false;
    }
    int i;
    for (i = 0; i < NUM_LETTERS; i++) {
        if (MSB(mask)) {
            break;
        }
        mask <<= 1;
    }
    *row = i / 5;
    *col = i % 5;
    return true;
}

// helper method to shift the board left, right, up, down, and then left + up/down, right + up/down (mode 1-8)
static letter_mask_t shift_board(letter_mask_t mask, int mode) {
    switch (mode) {
        case 1: // shift left
            return mask << 1;
        case 2: // shift right
            return (mask >> 1) & SEVEN_BITMASK;
        case 3: // shift up
            return mask << WIDTH;
        case 4: // shift down
            return (mask >> WIDTH) & SEVEN_BITMASK;
        default:
            return mask;
    }
}

// ################################ QUESTION 6 ################################
void intersect_neighbourhoods(letter_mask_t *fst, letter_mask_t *snd) {
    letter_mask_t fst_neighbourhood = *fst, snd_neighbourhood = *snd;
    // 8 directions: left, right, up, down, left + up/down, right + up/down
    for (int i = 1; i <= 4; i++) {
        letter_mask_t f1 = shift_board(*fst, i);
        letter_mask_t s1 = shift_board(*snd, i);
        fst_neighbourhood |= f1;
        snd_neighbourhood |= s1;
        if (i < 3) {
            for (int j = 3; j <= 4; j++) {
                fst_neighbourhood |= shift_board(f1, j);
                snd_neighbourhood |= shift_board(s1, j);
            }
        }
    }
    *fst &= snd_neighbourhood;
    *snd &= fst_neighbourhood;
}

#ifdef MAIN_LETTER_MASK
int main(void) {
    print_bitmask(BOARD_ZEROS);
    print_bitmask(BOARD_ONES);
    print_bitmask(FIRST_ROW_SET);
    print_bitmask(LAST_ROW_SET);
    print_bitmask(FIRST_COL_SET);
    print_bitmask(LAST_COL_SET);
    print_bitmask(FIRST_CELL_SET);
    print_bitmask(0x8A88A8BA); // cross and 0111010 (0b10001010100010001010100010111010)
    print_bitmask(0x213E4257); // plus  and 1010111 (0b00100001001111100100001001010111)
    return 0;
}
#endif
