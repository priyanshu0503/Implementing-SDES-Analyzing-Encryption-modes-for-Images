/*
This file is the header file for the SDES algorithm
*/
#ifndef SDES_H
#define SDES_H
#include <stdint.h>

typedef uint8_t customByte;

//function declarations
void shift(int ar[], int size, int shifts);
void key_generation(int key[]);
customByte* function_(customByte* ar, int key[]);
customByte* swap(customByte* ar, int n);
customByte* encryption(customByte plaintext[]);
customByte* decryption(customByte ar[]);
char* binary_(int val);
customByte* convert_to_bits(uint8_t byte);
uint8_t convert_to_byte(customByte* bits);


extern int key[];
extern int P10[];
extern int P8[];
extern int key1[];
extern int key2[];
extern int IP[];
extern int EP[];
extern int P4[];
extern int IP_inv[];
extern int S0[][4];
extern int S1[][4];



#endif // SDES_H
