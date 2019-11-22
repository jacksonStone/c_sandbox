#include <stdio.h>
#include <stdlib.h>
#define STRING const char*
static char* getFileContents(STRING fileName){
    FILE *f = fopen(fileName, "r");
    //Gets the size of the file
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    //Allots length + 1 for \0 room
    char * fileContent = (char*) malloc(fsize + 1);
    fread(fileContent, 1, fsize, f);
    fclose(f);
    fileContent[fsize] = 0;
    return fileContent;
}

int main() {
    char* contents = getFileContents("./example.json");
    printf("%s\n", contents);
    return(0);
}

typedef struct {
    char* value;
    int length;
} token;

void printToken(token t) {
    printf("%.*s", t.length, t.value);
}
token* getTokens(char* content) {
    int location = 0;
    token* foo;
    return foo;
}

