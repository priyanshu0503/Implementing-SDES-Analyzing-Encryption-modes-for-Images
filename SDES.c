/*
This following algorithm performs SDES
*/
#include "SDES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//Create and define P10 and P8
int P10[] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
int P8[] = {6, 3, 7, 4, 8, 5 ,10 ,9};

int key1[8], key2[8];
//defining positions
int IP[] = {2 ,6 ,3 ,1 ,4 ,8 ,5 ,7};
int EP[] = {4 ,1 ,2 ,3 ,2 ,3 ,4 ,1};
int P4[] = {2, 4, 3, 1};
int IP_inv[] = {4 ,1 ,3 ,5 ,7 ,2 ,8,6};

//S-boxes
int S0[][4] = {{1,0,3,2},
               {3,2,1,0},
               {0,2,1,3},
               {3,1,3,2}};

int S1[][4] = {{0,1,2,3},
               {2,0,1,3},
               {3,0,1,0},
               {2,1,0,3}};




/*
key generatiion function:
this function basically generates the key(key1 and
key2) using P10 and P8 with (1 and 2)left shifts
*/
void key_generation(int key[]){
    int key_[10];
    int Ls[5], Rs[5];

    //Apply the P10 permuation
    for(int i=0; i<10; i++)
    {
        key_[i] = key[P10[i]-1];
    }


    //Split into left and right halves
    for(int i=0; i<5; i++){
        Ls[i] = key_[i];
        Rs[i] = key_[i+5];
    }

    //KEY 1 GENERATION
    //Circular left shift by 1
    shift(Ls, 5, 1);
    shift(Rs, 5, 1);
    int combined[10];

    //Combine the two sides together
    for(int i=0; i<5; i++){
        combined[i] = Ls[i];
        combined[i+5] = Rs[i];
    }

    //Apply P8 to get Key1
    for(int i=0; i<8; i++){
        key1[i] = combined[P8[i]-1];
    }

    //KEY 2 GENERATION
    //Circular left shift by 2
    shift(Ls, 5, 2);
    shift(Rs, 5, 2);

    //Combine the two sides together
    for(int i=0; i<5; i++)
    {
        combined[i] = Ls[i];
        combined[i+5] = Rs[i];
    }

    //Apply P8 to get Key2
    for(int i=0; i<8; i++){
        key2[i] = combined[P8[i]-1];
    }

    //PRINT KEYS
    printf("KEY1 : ");
    for(int i=0; i<8; i++)
    {
        printf("%d ",key1[i]);
    }
    printf("\n");

    printf("KEY2 : ");
    for(int i=0;i<8;i++)
    {
        printf("%d ",key2[i]);
    }
    printf("\n");
}

/*
This function is useful for implementing the circular left shift
ar[] - array
n - size of array
shifts - number of shifts (1 for Key1 and 2 for Key2)
*/
void shift(int ar[], int n, int shifts)
{
   while (shifts--)
   {
       int temp = ar[0];
       for (int i=0; i< n-1; i++)
       {
           ar[i] = ar[i+1];
       }
       ar[n-1] = temp;
   }
}

/*
This function is implementing expansion, then xor with desired key,
then S0 and S1 substitution, P4 permutation,
and again xor. We have used this function 2 times
(key-1 and key-2) during encryption and 2 times(key-1&2)
during decryption
*/
customByte* function_(customByte ar[], int key[]){
    static customByte output[8];
    int l[4], r[4];

    for (int i = 0; i < 4; ++i) {
        l[i] = ar[i];
        r[i] = ar[i + 4];
    }

    int ep[8];
    for (int j = 0;j < 8; ++j) {
        ep[j] = r[EP[j] - 1];
    }

    for (int j = 0;j < 8; ++j) {
        ar[j] = key[j] ^ ep[j];
    }

    int l_1[4], r_1[4];
    for (int i = 0;i < 4; ++i) {
        l_1[i] = ar[i];
        r_1[i] = ar[i + 4];
    }

    int row, col, val;
        row = (l_1[0]<<1) + l_1[3];
        col = (l_1[1]<<1) + l_1[2];
        val = S0[row][col];
        customByte* str_l = convert_to_bits(val);

        row = (r_1[0]<<1) + r_1[3];
        col = (r_1[1]<<1) + r_1[2];
        val = S1[row][col];
        customByte* str_r= convert_to_bits(val);

    int r_[4];
    for(int i=0;i<2;++i){
        //r_[i] = str_l[i];
        //r_[i+2] = str_r[i];
        char c;
        c=str_l[i];
        r_[i]=c-'0';
        c=str_r[i];
        r_[i+2]=c-'0';
    }

    int r_p4[4];

    for(int i=0;i<4;++i){
        r_p4[i]=r_[P4[i]-1];
    }

    for(int i=0;i<4;++i){
        l[i]=l[i]^r_p4[i];
    }


    for(int i=0;i<4;++i){
        output[i]=l[i];
        output[i+4]=r[i];
    }

    return output;
}

/*
This function performs swapping
array - array being swapped
n - size of array
*/
customByte* swap(customByte* ar, int n){
    static customByte output[8];

    for(int i=0;i<n;++i){
        output[i]=ar[i+n];
        output[i+n]=ar[i];
    }

    return output;
}

/*
This is main encryption function takes plain text as
input uses another functions and returns the array of
cipher text
*/
customByte* encryption(customByte plaintext[]){
  static customByte arr[8];
  for (int i = 0; i < 8; ++i) {
      arr[i] = plaintext[IP[i] - 1];
  }
  customByte* arr1 = function_(arr, key1);
  customByte* after_swap = swap(arr1,4);
  customByte* arr2 = function_(after_swap, key2);

  static customByte ciphertext[8];
  for (int i = 0; i < 8; ++i) {
      ciphertext[i] = arr2[IP_inv[i] - 1];
  }
  return ciphertext;
}

customByte* decryption(customByte ar[]){
   static customByte arr[8];
   for (int i = 0; i < 8; ++i) {
       arr[i] = ar[IP[i] - 1];
   }

   customByte* arr1 = function_(arr, key2);
   customByte* after_swap = swap(arr1,4);
   customByte* arr2 = function_(after_swap, key1);

   static customByte decrypted[8];
   for (int i = 0; i < 8; ++i) {
       decrypted[i] = arr2[IP_inv[i] - 1];
   }
   return decrypted;
}

customByte* convert_to_bits(uint8_t byte)
{
    static customByte bits[8];
    for (int i = 0; i < 8; i++) {
        bits[7 - i] = (byte >> i) & 1;
    }
    return bits;
}

uint8_t convert_to_byte(customByte* bits) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte |= (bits[i] << (7 - i));
    }
    return byte;
}

/*char* binary_(int val)
{
    char *binary=(char *)malloc(3*sizeof(char));
    if(val==0){ binary="00";}
    else if(val==1){ binary="01";}
    else if(val==2){binary="10";}
    else binary="11";
    return binary;
}*/

