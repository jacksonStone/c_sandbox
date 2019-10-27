#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define GET_ENTRY(r, index, type) (type) * (type *) getEntry(r, index)

typedef enum {
    CHAR,
    INT,
} TYPE;

typedef struct {
    void* contents;
    size_t entrySize;
    int length;
    TYPE type;
} list;

list fromString(char* str) {
    size_t size = sizeof(char);
    int length = strlen(str);
    char asArray[length];
    for(int i = 0; i < length; i++) {
        // Lol have to copy to array so it can be modifiable
        // don't use literals!
        asArray[i] = str[i];
    }
    return (list) { asArray, size, length, CHAR };
}

void* getEntry(list target, int index) {
    if(target.length <= index) {
        void* emptyResults;
        return emptyResults;
    }
    char* asChars = (char*) target.contents;
    int location = index * target.entrySize;
    int ratioOfSize = location/(sizeof(char));
    char* startOfContent = &asChars[ratioOfSize];
    return startOfContent;
}

void setEntry(list target, int index, void* value) {
    if(target.length <= index) {
        return;
    }
    char* asChars = (char*) target.contents;
    size_t size = target.entrySize;
    size_t sizeRatio = size/sizeof(char);
    int location = index * size;
    int ratioOfSize = location/(sizeof(char));
    char* startOfContent = (char *) &asChars[ratioOfSize];
    char* valuesToWrite = (char*) value;

    for(size_t i = 0; i < sizeRatio; i++) {
        startOfContent[i] = valuesToWrite[i];
    }
}

int main() {
    char * str = "HelloThere!";
    list foo = fromString(str);
    char asChar = GET_ENTRY(foo, 6, char);
    char p = 'c';
    printf("%c\n",asChar);

    setEntry(foo, 6, &p);
    
    printf("%c\n", GET_ENTRY(foo, 6, char));
}