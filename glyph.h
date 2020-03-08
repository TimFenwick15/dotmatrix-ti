/*
 * glyph.h
 *
 *  Created on: 4 Mar 2020
 *      Author: Tim
 */

#ifndef GLYPH_H_
#define GLYPH_H_

#define RED_1 0x01
#define GREEN_1 0x02
#define BLUE_1 0x04
#define RED_2 0x08
#define GREEN_2 0x10
#define BLUE_2 0x20

#define ROWS (32)
#define ROW_ADDRESSES (ROWS / 2)

#define SPRITE_UNIQUE_COLOURS (4)

typedef struct {
    Uint16 R:2;
    Uint16 G:2;
    Uint16 B:2;
    Uint16 unused:10;
} Colour;

typedef struct {
    Uint16 x;
    Uint16 y;
    Uint16 width;
    Uint16 height;
} Dimensions;

typedef struct {
    Uint32 pixel[ROW_ADDRESSES];           // 64 bytes
    Dimensions dimensions;                 //  8 bytes
    Colour colour[SPRITE_UNIQUE_COLOURS];  //  8 bytes: total 80 bytes per sprite
    Uint16 transparency;
} Sprite;

// top row first char - Omi-Maiko Station
Sprite glyph_0_0 = {
    {
        0b00010000000000000000000001010100,
        0b00000100000000010101010100000000,
        0b00000001000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010101010101010100,
        0b01010100000000010000000001000000,
        0b00000100000000010000000001000000,
        0b00000100000000010000000001000000,
        0b00000100000001000000000001000000,
        0b00000100000001000000000001000000,
        0b00000100000100000000000001000000,
        0b00000100000000000000000001000000,
        0b00010001010000000000000000000000,
        0b01000000000101010101010101010100,
        0b00000000000000000000000000000000
    },
    { 0, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b00, 0b11, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

// top row second char
Sprite glyph_1_0 = {
    {
        0b00010000000000000000000000000000,
        0b00000100000000000000000000000000,
        0b00000001000101010101010101010000,
        0b00000000000000000001000000000000,
        0b01000000000000000001000000000000,
        0b00010000000000000001000000000000,
        0b00000100000000000001000000000000,
        0b00000000000000000001000000000000,
        0b00000000000000000001000000000000,
        0b00000100000000000001000000000000,
        0b00000100000000000001000000000000,
        0b00010000000000000001000000000000,
        0b00010000000000000001000000000000,
        0b01000000010101010101010101010100,
        0b01000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 16, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b00, 0b11, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

// top row third char
Sprite glyph_2_0 = {
    {
        0b00000100000000000000000000000000,
        0b00010101010101010101010101010000,
        0b01000100000100000100000100000000,
        0b00000100000100000100000100000000,
        0b01010101010101010101010101010100,
        0b00000100000100000100000100000000,
        0b00000100000100000100000100000000,
        0b00010101010101010101010101010000,
        0b00000001000000000000000100000000,
        0b00000101010101000101010101010000,
        0b00010000000001000100000100000000,
        0b01000100000100000100000100000000,
        0b00000001010000010101010101010100,
        0b00000101000000000000000100000000,
        0b01010000000000000000000100000000,
        0b00000000000000000000000000000000
    },
    { 32, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b00, 0b11, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

// top row fourth char
Sprite glyph_3_0 = {
    {
        0b00000000000000000000000000000000,
        0b00000101010101010101010101000000,
        0b00000000000000000000000100000000,
        0b00000000000000000001010000000000,
        0b00000000000000010100000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b01010101010101010101010101010100,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000000010000000000000000,
        0b00000000000101010000000000000000,
        0b00000000000000000000000000000000
    },
    { 48, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b00, 0b11, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

// 20:12 twenty
Sprite glyph_1_1 = {
    {
        0b00000000000000000000000000000000,
        0b00000101000000000000010100000000,
        0b00010000010000000001000001000000,
        0b01000000000100000100000000010000,
        0b01000000000100000100000000010000,
        0b00000000000100000100000000010000,
        0b00000000010000000100000000010000,
        0b00000000010000000100000000010000,
        0b00000001000000000100000000010000,
        0b00000100000000000100000000010000,
        0b00000100000000000100000000010000,
        0b00010000000000000100000000010000,
        0b01000000000000000001000001000000,
        0b01010101010100000000010100000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 16, 16, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b11, 0b00, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

// 20:12 colon
Sprite glyph_2_1 = {
    {
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000001010000000000000000,
        0b00000000000001010000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000001010000000000000000,
        0b00000000000001010000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 32, 16, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0}, // b
        { 0b11, 0b00, 0b00, 0}, // c1
        { 0b00, 0b00, 0b00, 0}, // c2
        { 0b00, 0b00, 0b00, 0}  // c3
    },
    0
};

Sprite glyph_3_1 = {
    {
        0b00000000000000000000000000000000,
        0b00000001000000000000010100000000,
        0b00010101000000000001000001000000,
        0b00000001000000000100000000010000,
        0b00000001000000000100000000010000,
        0b00000001000000000000000000010000,
        0b00000001000000000000000001000000,
        0b00000001000000000000000001000000,
        0b00000001000000000000000100000000,
        0b00000001000000000000010000000000,
        0b00000001000000000000010000000000,
        0b00000001000000000001000000000000,
        0b00000001000000000100000000000000,
        0b00000001000000000101010101010000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 48, 16, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b
        { 0b01, 0b00, 0b11, 0 }, // c1        { 0b11, 0b00, 0b00, 0 }, // c1
        { 0b00, 0b00, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

Sprite agumon = {
    {
        0b00000000000101010101010000000000,
        0b00000000011010101010100100000000,
        0b00010101101010100101101001000000,
        0b01101010101010101001011001000000,
        0b01101010101010100101011001000000,
        0b01101010100101101010101001000000,
        0b00010101011010101010101001000000,
        0b00011010101010010110100100000000,
        0b00000101010101100101100100000000,
        0b00011010011010011010101001000000,
        0b00010101011010010101011001000000,
        0b00000000010110101010011010010000,
        0b00000101101001010101101010010000,
        0b00011001101001100110011001100100,
        0b01010101010101000101010101010100,
        0b00000000000000000000000000000000
    },
    { 0, 16, 16, 16 },
    { //     r     g     b
        { 0b11, 0b11, 0b11, 0 }, // b
        { 0b00, 0b00, 0b00, 0 }, // c1
        { 0b11, 0b11, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

Sprite _agumon = {
    {
        0b00000000000101010101010000000000,
        0b00000000011010101010100100000000,
        0b00010101101010100101101001000000,
        0b01101010101010101001011001000000,
        0b01101010101010100101011001000000,
        0b01101010100101101010101001000000,
        0b00010101011010101010101001000000,
        0b00011010101010010110100100000000,
        0b00000101010101100101100100000000,
        0b00011010011010011010101001000000,
        0b00010101011010010101011001000000,
        0b00000000010110101010011010010000,
        0b00000101101001010101101010010000,
        0b00011001101001100110011001100100,
        0b01010101010101000101010101010100,
        0b00000000000000000000000000000000
    },
    { 24, 8, 16, 16 },
    { //     r     g     b
        { 0b11, 0b11, 0b11, 0 }, // b  0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b00, 0b00, 0b00, 0 }, // c1
        { 0b11, 0b11, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

Sprite biyomonUpper = {
    {
        0b00000001000000000000010000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000001010,
        0b00000000000000000000000000101010,
        0b10000000000000000000000010101010,
        0b00000000000000000000000000000000,
        0b10100000000000000000000000000000,
        0b00001010000000000000000000010101,
        0b01000000111100000000000011000001,
        0b01010011110011000011111111001100,
        0b01001100001100110000110011000001,
        0b01010001010001000101000100010101,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 48, 8, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b11, 0b00, 0b00, 0 }, // c2 red
        { 0b11, 0b11, 0b00, 0 }  // c3 yellow
    },
    1
};

Sprite biyomonLower = {
    {
        0b01010100111111000011000100110001,
        0b01010100111111111100100000110001,
        0b01010100111111101010001100111000,
        0b01010101001110101010100010001000,
        0b01010000001110101010101010001000,
        0b01010010100010101010101010101010,
        0b01010010101010101010101010101010,
        0b00000100101010100000001010101010,
        0b00110001001010100011010010101010,
        0b11001000101010100011110010100000,
        0b10100000000010101011110010000000,
        0b00001010101000001010101000000000,
        0b00101010101010100000000000000000,
        0b00101010100010000010001000000000,
        0b00000010101000100000100010000101,
        0b00000000000010101010101000010101
    },
    { 48, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b0 black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b11, 0b00, 0b01, 0 }, // c2 pink
        { 0b00, 0b00, 0b11, 0 }  // c3 blue
    },
    0
};

#endif /* GLYPH_H_ */
