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
char* to_c_str(string str) {
    printf("Printing length and starting string\n");
    printf("%d\n", str.length);
    printf("%s\n", str.text);
    str.text[str.length] = '\0';
    return str.text;
}

string* split(string toSplit, string pattern) {
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
        lastSeenLocal = end_of_match;
        ret[i] = split_result;
    }
    return ret;
}

int main() {
    string myStr = newString("FOOAOOAOOAOEY TOOOEY");
    string match = newString("OAOO");
    string* split_results = split(myStr, match);
    printf("SIZE OF RESULTS: %lu\n", sizeof(split_results));
    printf("SINGLE ENTRY SIZE: %lu\n", sizeof(split_results[0]));
    int arrayEntries = sizeof(split_results)/sizeof(split_results[0]);
    printf("Array Entries: %d\n", arrayEntries);
    // Couple problems here. One is that when printing to_c_str
    // We 0 a value, but the split strings store pointers to original
    // so it puts 0 in the middle of the string
    // The next is we do not know the length of the split_results
    
    // printf("%s\n", to_c_str(split_results[0]));
    printf("%s\n", to_c_str(split_results[1]));
}