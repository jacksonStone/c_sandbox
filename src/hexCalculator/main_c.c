#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* hexToBinary(char* hexString) {
    int lengthOfHex = strlen(hexString);
    int lenOfBinaryStr = lengthOfHex * 4;
    char * binaryString = (char *) malloc(lenOfBinaryStr * sizeof(char) + 1);
    for(int i = 0; i < lengthOfHex; i++) {
        char hexCode = hexString[i];
        int entry = i*4;
        char * chunk;
        switch (hexCode) {
            case '0':
                chunk = "0000";
                break;
            case '1':
                chunk = "0001";
                break;
            case '2':
                chunk = "0010";
                break;
            case '3':
                chunk = "0011";
                break;
            case '4':
                chunk = "0100";
                break;
            case '5':
                chunk = "0101";
                break;
            case '6':
                chunk = "0110";
                break;
            case '7':
                chunk = "0111";
                break;
            case '8':
                chunk = "1000";
                break;
            case '9':
                chunk = "1001";
                break;
            case 'A':
                chunk = "1010";
                break;
            case 'B':
                chunk = "1011";
                break;
            case 'C':
                chunk = "1100";
                break;
            case 'D':
                chunk = "1101";
                break;
            case 'E':
                chunk = "1110";
                break;
            case 'F':
                chunk = "1111";
                break;
                //Rest of cases
            default:
                chunk = "1111";
        }
        for(int j = 0; j < 4; j++) {
           binaryString[entry + j] = chunk[j];
        }
    }
    binaryString[lenOfBinaryStr] = '\0';
    return binaryString;
}

char* binaryToHex(char* binaryString) {
    int lengthOfBinary = strlen(binaryString);
    int lenOfHexStr = (lengthOfBinary + 4 - 1) / 4;
    char* binaryStringToUse;
    if(lenOfHexStr*4 > lengthOfBinary) {
        int paddedLength = lenOfHexStr * 4 + 1;
        char paddedBinaryString[paddedLength];
        int zerosToPad = lenOfHexStr*4 - lengthOfBinary;
        int i = 0;
        for(; i < zerosToPad; i++) {
            paddedBinaryString[i] = '0';
        }
        for(int j = 0; j < lengthOfBinary; j++) {
            paddedBinaryString[j+i] = binaryString[j];
        }
        paddedBinaryString[paddedLength] = '\0';       
    } else {
        binaryStringToUse = binaryString;
    }
    
    char * hexString = (char *) malloc(lenOfHexStr * sizeof(char) + 1);
    for(int i = 0; i < lenOfHexStr; i++) {
        int startPoint = i * 4;
        char chunk[4 + 1];
        for(int j = 0; j < 4; j++) {
            chunk[j] = binaryStringToUse[startPoint + j];
        }
        chunk[4] = '\0';
        if(strcmp(chunk, "0000") == 0) {
           hexString[i] = '0';
        } else if(strcmp(chunk, "0001") == 0){
           hexString[i] = '1'; 
        }
         else if(strcmp(chunk, "0010") == 0){
           hexString[i] = '2'; 
        }
        else if(strcmp(chunk, "0011") == 0){
           hexString[i] = '3'; 
        }
        else if(strcmp(chunk, "0100") == 0){
           hexString[i] = '4'; 
        }
        else if(strcmp(chunk, "0101") == 0){
           hexString[i] = '5'; 
        }
        else if(strcmp(chunk, "0110") == 0){
           hexString[i] = '6'; 
        }
        else if(strcmp(chunk, "0111") == 0){
           hexString[i] = '7'; 
        }
        else if(strcmp(chunk, "1000") == 0){
           hexString[i] = '8'; 
        } else if(strcmp(chunk, "1001") == 0){
           hexString[i] = '9'; 
        }
         else if(strcmp(chunk, "1010") == 0){
           hexString[i] = 'A'; 
        }
        else if(strcmp(chunk, "1011") == 0){
           hexString[i] = 'B'; 
        }
        else if(strcmp(chunk, "1100") == 0){
           hexString[i] = 'C'; 
        }
        else if(strcmp(chunk, "1101") == 0){
           hexString[i] = 'D'; 
        }
        else if(strcmp(chunk, "1110") == 0){
           hexString[i] = 'E'; 
        }
        else if(strcmp(chunk, "1111") == 0){
           hexString[i] = 'F'; 
        }
        else {
            //OTHER CASES
            hexString[i] = 'F'; 
        }
    }
    hexString[lenOfHexStr] = '\0';
    return hexString;
}

int main() {
    char* testString = "ABC0DEFFABC0DEF";
    printf("%s\n", hexToBinary(testString));
    printf("%s\n", binaryToHex(hexToBinary(testString)));
}