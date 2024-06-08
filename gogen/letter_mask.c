#include "letter_mask.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
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
        // Create mask of last 7 bits 1, then invert it to get desired value (all bits 1 and last 7 bits 0)
        // a 32 bit mask with last 7 bits 1 is 0...0111 1111, so 0x7F
        *mask = ~0x7F;
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

// ################################ QUESTION 6 ################################
void intersect_neighbourhoods(letter_mask_t *fst, letter_mask_t *snd) {
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
