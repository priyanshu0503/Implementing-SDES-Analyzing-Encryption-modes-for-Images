# Implementing-SDES-Analyzing-Encryption-modes-for-Images

********************************************************
Project Implementing S-DES & Analyzing Encryption modes for Images - CSC 487

By Priyanshu Mittal and Taitlyn Gowlovech

This program encrypts BMPs using the SDES algorithm.

********************************************************

CREDIT:
The file reading of the bmp image in main.c is credited to Nathanial Dobb's Image Processing Code "simple_SDES_BMP.cpp".


********************************************************

FILES INCLUDED:
main.c - main driver program that reads from a bmp image (normal.bmp the defaul), asks which form of encryption to use (ECB, CBC, or CTR), and calls the encryption funtion or decryption funtion (encryption default). Then writes the encrypted text to specified file or cypher.bmp(default)

SDES.c - program that contains all the backend logic and funcitonality of the encryption/decryption. This encludes key generation, swapping, substitution, and permuation

SDES.h - header file that contains all the function declarations and external variables

********************************************************

HOW TO COMPILE:
Using gcc from the command line, the project can be compiled as followed: (same as from file given by Nathanial Dobbs image processsing code)

Command Line Argument Options

./exe [input file]
	--Defaults to cypher.bmp as output
	--Defaults to encrypting
	--Prompts for key

./exe [input file] [output file]
	--Defaults to encrypting
	--Prompts for key

./exe [input file] [output file] [d or e]
	--Prompts for key

./exe [input file] [output file] [d or e] [key]

example: 
C:\...filepath...\FolderOfContents> gcc -o sdes main.c SDES.c
C:\...filepath...\FolderOfContents> ./sdes penguin.bmp cypher.bmp e 0100000000  

********************************************************

NOTES:
-All files must be in the same folder and file path for them to correctly function with one another
-There must be a normal.bmp file or another specified bmp file for the program to correctly funtion
-Cypher.bmp will be overwritten if the image or key are updated and the program is ran again
