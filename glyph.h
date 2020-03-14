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
        { 0b01, 0b01, 0b01, 0 }, // b
        { 0b00, 0b00, 0b00, 0 }, // c1
        { 0b11, 0b11, 0b00, 0 }, // c2
        { 0b00, 0b00, 0b00, 0 }  // c3
    },
    0
};

Sprite biyomonQuil = {
    {
        0b01010101010101010100000101010101,
        0b01010101010101010010110001010101,
        0b01010101010101001100001100010101,
        0b01010101010101001100010000010101,
        0b01010101010101001000010101010101,
        0b01010100000000000011000100000101,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 48, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b0 black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b01, 0b01, 0b01, 0 }, // c1 white
        { 0b11, 0b00, 0b01, 0 }, // c2 pink
        { 0b00, 0b00, 0b11, 0 }  // c3 blue
    },
    1
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
    { 48, 14, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b01, 0b01, 0b01, 0 }, // c1 white
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
    { 48, 6, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b0 black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b01, 0b01, 0b01, 0 }, // c1 white
        { 0b11, 0b00, 0b01, 0 }, // c2 pink
        { 0b00, 0b00, 0b11, 0 }  // c3 blue
    },
    1
};

Sprite red_front_still = {
    {
        0b01010101010000000000000101010101,
        0b01010101001111111111110001010101,
        0b01010100111111111111111100010101,
        0b01010100111111111111111100010101,
        0b01010000001101010101110000000101,
        0b01010000010000000000000100000101,
        0b01000100010101010101010100010001,
        0b01000101010100010100010101010001,
        0b01010000010100010100010100000101,
        0b01010000000101111101010000000101,
        0b01000101000000000000000001010001,
        0b01000101000000000000000001010001,
        0b01010000001111000011110000000101,
        0b01010100110000111100001100010101,
        0b01010100111111000011111100010101,
        0b01010101000000010100000001010101,
    },
    { 24, 8, 16, 16 }, // was 32 12
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b0 black 0b00 - off,      0b01 - on 50%,       0b10 or 0b11 - on 100%
        { 0b01, 0b01, 0b01, 0 }, // c1 white
        { 0b00, 0b00, 0b00, 0 }, // c2 black
        { 0b11, 0b00, 0b00, 0 }  // c3 red
    },
    0
};

Uint32 redFrames[4][16] = {
   {
        0b01010101010000000000000101010101,
        0b01010101001111111111110001010101,
        0b01010100111111111111111100010101,
        0b01010100111111111111111100010101,
        0b01010000001101010101110000000101,
        0b01010000010000000000000100000101,
        0b01000100010101010101010100010001,
        0b01000101010100010100010101010001,
        0b01010000010100010100010100000101,
        0b01010000000101111101010000000101,
        0b01000101000000000000000001010001,
        0b01000101000000000000000001010001,
        0b01010000001111000011110000000101,
        0b01010100110000111100001100010101,
        0b01010100111111000011111100010101,
        0b01010101000000010100000001010101
    },
    {
        0b01010101010101010101010101010101,
        0b01010101010000000000000101010101,
        0b01010101001111111111110001010101,
        0b01010100111111111111111100010101,
        0b01010100111111111111111100010101,
        0b01010000001101010101110000000101,
        0b01010000010000000000000100000101,
        0b01000100010101010101010100010001,
        0b01000101010100010100010101010001,
        0b01000000010100010100010100001101,
        0b01010000000101111101010000000101,
        0b01000000000000000000000000000001,
        0b01000000000000000000000101000001,
        0b01010000001100000000000101000101,
        0b01010100110000111100000000010101,
        0b01010100111111000000000000010101,
        //0b01010101000000010100000001010101
    },
    {
        0b01010101010000000000000101010101,
        0b01010101001111111111110001010101,
        0b01010100111111111111111100010101,
        0b01010100111111111111111100010101,
        0b01010000001101010101110000000101,
        0b01010000010000000000000100000101,
        0b01000100010101010101010100010001,
        0b01000101010100010100010101010001,
        0b01010000010100010100010100000101,
        0b01010000000101111101010000000101,
        0b01000101000000000000000001010001,
        0b01000101000000000000000001010001,
        0b01010000001111000011110000000101,
        0b01010100110000111100001100010101,
        0b01010100111111000011111100010101,
        0b01010101000000010100000001010101
    },
    {
        0b01010101010101010101010101010101,
        0b01010101010000000000000101010101,
        0b01010101001111111111110001010101,
        0b01010100111111111111111100010101,
        0b01010100111111111111111100010101,
        0b01010000001101010101110000000101,
        0b01010000010000000000000100000101,
        0b01000100010101010101010100010001,
        0b01000101010100010100010101010001,
        0b01110000010100010100010100000001,
        0b01010000000100111111010000000101,
        0b01000000000000000000000000000001,
        0b01000010100000000000000000000001,
        0b01010001010000000000110000000101,
        0b01010100000000111100001100010101,
        0b01010100000000000011111100010101,
    }
};


Sprite franxx_0_0 = {
    {
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000001010000010100000010100,
        0b00000000001010000010100000010100,
        0b00000000000010100000101010101111,
        0b00000000000010100000101010101111,
        0b00000000000000101000001010000001,
        0b00000000000000101000001010000001,
        0b00000000000000001010000010101010,
        0b00000000000000001010000010101010
    },
    { 32, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b00, 0b00, 0b11, 0 }, // c2 blue
        { 0b11, 0b00, 0b00, 0 }  // c3 red
    },
    0
};
Sprite franxx_1_0 = {
    {
        0b00000000000000000011110000111100,
        0b00000000000000000011110000111100,
        0b00000000000000001111000011110000,
        0b00000000000000001111000011110000,
        0b00000000000000111100001111000000,
        0b00000000000000111100001111000000,
        0b00000000000011110000111100000000,
        0b00000000000011110000111100000000,
        0b00010100001111000011110000000000,
        0b00010100001111000011110000000000,
        0b10101111111100001111000000000000,
        0b10101111111100001111000000000000,
        0b01000011110000111100000000000000,
        0b01000011110000111100000000000000,
        0b11110000111111110000000000000000,
        0b11110000111111110000000000000000
    },
    { 48, 0, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b00, 0b00, 0b11, 0 }, // c2 blue
        { 0b11, 0b00, 0b00, 0 }  // c3 red
    },
    0
};
Sprite franxx_0_1 = {
    {
        0b00000000000000000010100000101000,
        0b00000000000000000010100000101000,
        0b00000000000000001010101000001010,
        0b00000000000000001010101000001010,
        0b00000000000000101000001010000001,
        0b00000000000000101000001010000001,
        0b00000000000010100000101010101111,
        0b00000000000010100000101010101111,
        0b00000000001010000010100000010100,
        0b00000000001010000010100000010100,
        0b00000000101000001010000000000000,
        0b00000000101000001010000000000000,
        0b00000010100000101000000000000000,
        0b00000010100000101000000000000000,
        0b00001010000010100000000000000000,
        0b00001010000010100000000000000000
    },
    { 32, 16, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b00, 0b00, 0b11, 0 }, // c2 blue
        { 0b11, 0b00, 0b00, 0 }  // c3 red
    },
    0
};
Sprite franxx_1_1 = {
    {
        0b00111100001111000000000000000000,
        0b00111100001111000000000000000000,
        0b11111111000011110000000000000000,
        0b11111111000011110000000000000000,
        0b01000011110000111100000000000000,
        0b01000011110000111100000000000000,
        0b10101111111100001111000000000000,
        0b10101111111100001111000000000000,
        0b00010100001111000011110000000000,
        0b00010100001111000011110000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000,
        0b00000000000000000000000000000000
    },
    { 48, 16, 16, 16 },
    { //     r     g     b
        { 0b00, 0b00, 0b00, 0 }, // b  black
        { 0b11, 0b11, 0b11, 0 }, // c1 white
        { 0b00, 0b00, 0b11, 0 }, // c2 blue
        { 0b11, 0b00, 0b00, 0 }  // c3 red
    },
    0
};
#endif /* GLYPH_H_ */
