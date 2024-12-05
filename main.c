/*
This file is the driver program for the SDES algorithm that also reads in a BMP image and encrypts it.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "SDES.h"

typedef unsigned char customByte;

void key_generation(int key[]);
customByte* encryption(customByte  plaintext[]);
customByte* decryption(customByte ar[]);


void parseKey(const char* keyString, int key[])
{
    for (int i = 0; i < 10; i++) {
        key[i] = keyString[i] - '0';
    }
}

int main(int argc, char *argv[])
{

    bool decrypt = false;
    char* fileName = NULL;
    char* outputName = "cypher.bmp";
    char* dOrE = NULL;
    char* tempKey = NULL;
    char key[11];

    int keyArray[10] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};

    if(argc == 2) {
        fileName = argv[1];
    }
    else if (argc == 3) {
        fileName = argv[1];
        outputName = argv[2];
    }
    else if (argc == 4) {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        decrypt = (strcmp(dOrE, "d") == 0);
    }
    else if (argc == 5) {
        fileName = argv[1];
        outputName = argv[2];
        dOrE = argv[3];
        decrypt = (strcmp(dOrE, "d") == 0);
        strncpy(key, argv[4], 10);
        key[10] = '\0';
        parseKey(key, keyArray);
    }
    else {
        fileName = "SD.bmp";
    }

    char mode[4];
    do {
        printf("Enter mode (ECB, CBC, CTR): ");
        if (fgets(mode, sizeof(mode), stdin) != NULL)
        {
            //Remove the newline character if present
            mode[strcspn(mode, "\n")] = 0;
            //Verify the mode is valid
            if (strcmp(mode, "ECB") == 0 || strcmp(mode, "CBC") == 0 || strcmp(mode, "CTR") == 0)
            {
                printf("Valid mode: %s\n", mode);
                break;
            } else
            {
                printf("Invalid mode. Please choose ECB, CBC, or CTR.\n\n");
            }
        }
        else
        {
            printf("Error reading input\n");
        }

    } while (1);

    if (decrypt) {
        printf("\nDecrypting %s\n", fileName);
    }
    else {
        printf("\nEncrypting %s\n", fileName);
    }

    FILE* picture;
    FILE* cypher;

    cypher = fopen(outputName, "wb");
    if (!(picture = fopen(fileName, "rb")))
    {
        printf("%s is missing, program will now exit\n", fileName);
        return 1;
    }


    unsigned char header[54];
    //Read the header
    fread(header, sizeof(unsigned char), 54, picture);
    fwrite(header, sizeof(unsigned char), 54, cypher);

    //Dealing with Padding Rules
    int width = 0;
    int height = 0;
    short int bitPerPixel = 24; // Assuming 24 bits per pixel for BMP

    // Read width, height, and bitPerPixel from the BMP header
    width = *(int*)&header[18];
    height = *(int*)&header[22];
    bitPerPixel = *(short*)&header[28];

    int paddedWidth = width;
    if (paddedWidth % 4 != 0)
    {
        paddedWidth += (4 - (paddedWidth % 4));
    }
    int multiplier = bitPerPixel / 8;
    int size = multiplier * paddedWidth * height;
    //Dealing with Padding Rules
    if (width % 4 != 0)
    {
        width += (4 - (width % 4));
    }

    size = multiplier * width * height;
    printf("SIZE: %d\n", size);
    unsigned char* pixelData = (unsigned char*)malloc(size);

    //Reading Pixel Data
    fread(pixelData, sizeof(unsigned char), size, picture);
    fclose(picture);

    //Generate keys
    key_generation(keyArray);


    //Decide which mode to implement
    if (strcmp(mode, "ECB") == 0)
    {
        for (int i=0; i<size; i++)
        {
            customByte plaintext[8];

            for (int j=0; j<8; j++)
            {
                plaintext[j] = (pixelData[i] >> (7 - j)) & 1;
            }

            customByte* processedData;
            if(decrypt)
            {
                processedData = decryption(plaintext);
            }
            else
            {
                processedData = encryption(plaintext);
            }

            unsigned char processedByte = 0;
            for (int j=0; j<8; j++)
            {
                processedByte |= (processedData[j] << (7 - j));
            }

            fwrite(&processedByte, sizeof(unsigned char), 1, cypher);
        }
    }
    else if (strcmp(mode, "CBC") == 0)
    {
        customByte iv[8] = {1, 1, 1, 0, 1, 1, 0, 1};
        customByte previousCiphertext[8] = {0};

        for (int i = 0; i < size; i++)//= 8)
        {   
            customByte plaintext[8];
            unsigned char byte = pixelData[i];
            
            for (int j=0; j<8; j++)
            {
                plaintext[j] = (byte >> (7-j)) & 1;
            }

            if (i == 0) {
                for (int j = 0; j < 8; j++) {
                    plaintext[j] ^= iv[j]; 
                }
            } else {
                for (int j = 0; j < 8; j++) {
                    plaintext[j] ^= previousCiphertext[j]; 
                }
            }

            //Encrypt or decrypt the plaintext
            customByte* processedData;
            if (decrypt) {
                processedData = decryption(plaintext);
            } else {
                processedData = encryption(plaintext);
            }

            // Store processed data as ciphertext and write it
            unsigned char processedByte = 0;
            for (int j = 0; j < 8; j++)
            {
                processedByte |= (processedData[j] << (7 - j));
            }

            fwrite(&processedByte, sizeof(unsigned char), 1, cypher);

            //Save processed data for the next block (to use as previous ciphertext)
            for (int j = 0; j < 8; j++) {
                previousCiphertext[j] = processedData[j];
            }
            

        }   
    }
    else if (strcmp(mode, "CTR") == 0)
    {
        customByte nonce[8] = {0, 0, 0, 0, 0, 0, 0, 1};
        int count = 0;

        for (int i=0; i<size; i++)
        {
            customByte plaintext[8];
            unsigned char byte = pixelData[i];

            for (int j=0; j<8; j++)
            {
                plaintext[j] = (byte >> (7-j)) & 1;
            }

            customByte counterBlock[8];
            for (int j = 0; j < 8; j++)
            {
                counterBlock[j] = nonce[j];
            }
            for (int j = 7; j >= 0; j--)
            {
                if (++counterBlock[j] != 0) break;
            }
            customByte* counterEncrypted = encryption(counterBlock);

            customByte processedData[8];
            for (int j = 0; j < 8; j++)
            {
                processedData[j] = plaintext[j] ^ counterEncrypted[j];
            }

            unsigned char processedByte = 0;
            for(int j=0; j<8; j++)
            {
                processedByte |= (processedData[j] << (7 - j));
            }

            fwrite(&processedByte, sizeof(unsigned char), 1, cypher);

            free(counterEncrypted);
        }
    }
    

    fclose(cypher);
    free(pixelData);

    printf("Operation completed successfully. \n");
    return 0;
}
