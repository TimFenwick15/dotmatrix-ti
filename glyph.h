/*
 * glyph.h
 *
 *  Created on: 4 Mar 2020
 *      Author: Tim
 */

#ifndef GLYPH_H_
#define GLYPH_H_

#define ROWS (32)
#define ROW_ADDRESSES (ROWS / 2)

// 20:12 first char
const Uint16 glyph_0_0[ROW_ADDRESSES] = {
    0b0111000000000010,
    0b0000111110000100,
    0b0000000010001000,
    0b0000000010000000,
    0b0000000010000000,
    0b0111111110000000,
    0b0001000010000111,
    0b0001000010000100,
    0b0001000010000100,
    0b0001000001000100,
    0b0001000001000100,
    0b0001000000100100,
    0b0001000000000100,
    0b0000000000011010,
    0b0111111111100001,
    0b0000000000000000
};

// 20:12 second char
const Uint16 glyph_1_0[ROW_ADDRESSES] = {
    0b0000000000000010,
    0b0000000000000100,
    0b0011111111101000,
    0b0000001000000000,
    0b0000001000000001,
    0b0000001000000010,
    0b0000001000000100,
    0b0000001000000000,
    0b0000001000000000,
    0b0000001000000100,
    0b0000001000000100,
    0b0000001000000010,
    0b0000001000000010,
    0b0111111111110001,
    0b0000000000000001,
    0b0000000000000000
};

// 20:12 third char
const Uint16 glyph_2_0[ROW_ADDRESSES] = {
    0b0000000000000100,
    0b0011111111111110,
    0b0000100100100101,
    0b0000100100100100,
    0b0111111111111111,
    0b0000100100100100,
    0b0000100100100100,
    0b0011111111111110,
    0b0000100000001000,
    0b0011111101111100,
    0b0000100101000010,
    0b0000100100100101,
    0b0111111110011000,
    0b0000100000001100,
    0b0000100000000011,
    0b0000000000000000
};

// 20:12 fourth char
const Uint16 glyph_3_0[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0001111111111100,
    0b0000100000000000,
    0b0000011000000000,
    0b0000000110000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0111111111111111,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000010000000,
    0b0000000011100000,
    0b0000000000000000
};

// 20:12 blank
const Uint16 glyph_0_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 twenty
const Uint16 glyph_1_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000110000001100,
    0b0001001000010010,
    0b0010000100100001,
    0b0010000100100001,
    0b0010000100100000,
    0b0010000100010000,
    0b0010000100010000,
    0b0010000100001000,
    0b0010000100000100,
    0b0010000100000100,
    0b0010000100000010,
    0b0001001000000001,
    0b0000110000111111,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 colon
const Uint16 glyph_2_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000011000000,
    0b0000000011000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000011000000,
    0b0000000011000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

// 20:12 twelve
const Uint16 glyph_3_1[ROW_ADDRESSES] = {
    0b0000000000000000,
    0b0000110000001000,
    0b0001001000001110,
    0b0010000100001000,
    0b0010000100001000,
    0b0010000000001000,
    0b0001000000001000,
    0b0001000000001000,
    0b0000100000001000,
    0b0000010000001000,
    0b0000010000001000,
    0b0000001000001000,
    0b0000000100001000,
    0b0011111100001000,
    0b0000000000000000,
    0b0000000000000000
};

#endif /* GLYPH_H_ */
