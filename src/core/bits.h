#ifndef BITS_H
#define BITS_H

// Helper header for bit operations 

#define BIT_0 0x1
#define BIT_1 0x2
#define BIT_2 0x4
#define BIT_3 0x8
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80

// Given bit 0-7 works out 8 bit value for it
#define BIT(n) (0x1 << n)

#endif //BITS_H
