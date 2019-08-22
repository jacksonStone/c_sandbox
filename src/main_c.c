#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* text;
    int length;
} string;

string newString(char* text) {
    string foo;
    int size = strlen(text);
    int alloted_space = sizeof(char) * (size + 1);
        printf("%d\n", alloted_space);

    char* text_without_0 = (char *) malloc(alloted_space);
    for(int i = 0; i < size; i++) {
        text_without_0[i] = text[i];
    }
    foo.text = text_without_0;
    foo.length = size;
    return foo;
}
void printStr(string str) {
    char toPrint [str.length + 1];
    for(int i = 0; i < str.length; i++) {
        toPrint[i] = str.text[i];
    }
    toPrint[str.length] = '\0';
    printf("%s\n", toPrint);
}

char* to_c_str(string str) {
    char* toPrint = (char *) malloc(sizeof(char) * (str.length + 1));
    for(int i = 0; i < str.length; i++) {
        toPrint[i] = str.text[i];
    }
    toPrint[str.length] = '\0';
    return toPrint;
}

string* split(string toSplit, string pattern, int* count) {
    char* text = toSplit.text;
    int length = toSplit.length;
    int match_indexes [length];
    int match_counter = 0;
    char* match = pattern.text;
    for(int i = 0; i < toSplit.length; i++) {
        for(int j = 0; j < pattern.length; j++) {
            if(length <= (j + i)) {
                break;
            }
            if(text[i + j] != match[j]) {
                break;
            }
            if(j == (pattern.length - 1)) {
                //We have a match
                match_indexes[match_counter] = i;
                i+=j;
                match_counter++;
            }
        }
    }

    //TODO:: YOU ARE HERE
    printf("Printing matches found: %d\n", match_counter);
    string* ret = (string *) malloc(sizeof(string) * (match_counter + 1));
    if (match_counter == 0) {
        ret[0] = toSplit;
        return ret;
    }
    int lastSeenLocal = 0;
    for(int i = 0; i < match_counter + 1; i++) {
        int end_of_match;
        if(i == match_counter) {
            end_of_match = length;
        } else {
            end_of_match = match_indexes[i];
        }
        string split_result;
        split_result.text = &text[lastSeenLocal];
        split_result.length = end_of_match - lastSeenLocal;
        lastSeenLocal = end_of_match + pattern.length;
        ret[i] = split_result;
    }
    *count = match_counter + 1;
    return ret;
}

int main() {
    string myStr = newString("FOOAOOAOOAOEY TOOOEY");
    string match = newString("OO");
    int count;
    string* split_results = split(myStr, match, &count);
    int arrayEntries = count;
    printf("Array Entries: %d\n", arrayEntries);
    for(int i = 0; i < count; i++) {
        printStr(split_results[i]);
    }
    char* cstr = to_c_str(split_results[1]);
    printf("%s\n", cstr);
    free(cstr);
    free(split_results);
}