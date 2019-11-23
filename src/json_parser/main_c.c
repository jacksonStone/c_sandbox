#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "../dynamic_structures/list.h"
// #include "../dynamic_structures/hash_map.h"

struct {
    long len;
    char* data;
} typedef file_contents;
file_contents get_file_contents(char * file_name) {
    FILE *f = fopen(file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    file_contents content;
    content.len = fsize;
    content.data = string;
    return content;
}

// //JSON node can either be
// // Value (num, string, boolean, null)
// // List of JSON nodes
// // Map of JSON nodes
// struct {
//     union json_value {
//         double num_value;
//         char* str_value;
//         int bool_value;
//         struct json * list_value;
//         hash_map * map_value;
//     } value;
// } typedef json;

enum token_type {
    //Special recursive signifiers
    open_brace,
    close_brace,
    open_bracket,
    close_bracket,
    //Seperators
    comma,
    colon_seperator,
     
    //Values
    number_value,
    string_value,
    boolean_value,
    null_value,

};

struct {
    char * text;
    int text_length;
    enum token_type type;
} typedef token;

struct {
    char * text;
    int len;
    enum token_type associated_token;
} typedef keyword;

#define keyword_len 7
keyword keywords[keyword_len] = {
    {
        "null",
        4,
        null_value
    },
    {
        "true",
        4,
        boolean_value
    },
    {
        "false",
        5,
        boolean_value
    },
    {
        ",",
        1,
        comma
    },
    {
        ":",
        1,
        colon_seperator
    },
    {
        "]",
        1,
        close_bracket
    },
    {
        "}",
        1,
        close_brace
    }
};

int is_num(char c) {
    return c >= '0' && c <= '9';
}

token handle_json_value(char * f, long len, token ** tokens, long * index) {
    //Skip them space
    token new_token;

    while (isspace(f[*index])) {
        *index = *index + 1;
        if ( *index > len ) {
            return new_token;
        }
    }

    int starting_index = *index;
    char starting_char = f[starting_index];
    new_token.text = &f[starting_index];
    //Check for numbers
    if(is_num(starting_char) || starting_char == '-') {
        new_token.type = number_value;
        int token_len = 0;
        int found_period = 0;
        if(starting_char == '-') {
            token_len++;
            *index = *index + 1;
        }
        while(*index <= len) {
            if(is_num(f[*index])) {
                *index = *index + 1;
                token_len++;
                continue;
            } else if(f[*index] == '.' && !found_period){
                *index = *index + 1;
                token_len++;
                found_period = 1;
                continue;
            }
            break;
        }
        new_token.text_length = token_len;
        append_to_list(*tokens, new_token);
        return new_token;
    }
    //Check for strings
    if(starting_char == '"') {
        new_token.type = string_value;
        int str_len = 1;
        *index = *index + 1;
        while(*index <= len && f[*index] != '"') {
            //Handle escape sequences
            if(f[*index] == '\\') {
                str_len+=2;
                *index = *index + 2;
            } else {
              str_len+=1;  
              *index = *index + 1;
            }
        } 
        if(*index <= len && f[*index] == '"') {
            str_len++;
            *index = *index + 1;
        }
        new_token.text_length = str_len;
        append_to_list(*tokens, new_token);
        return new_token;
    }
    //Check for keywords
    for(int i = 0; i < keyword_len; i++) {
        keyword cur_keyword = keywords[i];
        if((starting_index + cur_keyword.len) <= len && strncmp(&f[starting_index], cur_keyword.text, cur_keyword.len) == 0) {
            new_token.text_length = cur_keyword.len;
            new_token.type = cur_keyword.associated_token;
            append_to_list(*tokens, new_token);
            *index = *index + cur_keyword.len;
            return new_token;
        }
    }

    //Now we get into layered stuff
    if(starting_char == '[') {
        new_token.type = open_bracket;
        new_token.text_length = 1;
        *index = *index + 1;
        append_to_list(*tokens, new_token);
        token last_found;
        while(last_found.type != close_bracket && *index <= len) {
            last_found = handle_json_value(f, len, tokens, index);
        }
        return new_token;
    }
    if(starting_char == '{') {
        new_token.type = open_brace;
        new_token.text_length = 1;
        *index = *index + 1;
        append_to_list(*tokens, new_token);
        token last_found;
        while(last_found.type != close_brace && *index <= len) {
            last_found = handle_json_value(f, len, tokens, index);
        }
        return new_token;
    }
    assert(0);
    //None
    return new_token;
}

int main() {
    int file_count = 5;
    char * files[5] = {
        "./example4.json",
        "./example5.json",
        "./example6.json",
        "./example7.json",
        "./example.json"
    };
    for(int i = 0; i < file_count; i++) {
        char * file_name = files[i];
        printf("Tokenizing file: %s\n", file_name);
        file_contents content = get_file_contents(file_name);
        // json * current_node;
        // json * json_result = current_node;

        //Lexer
        token * tokens = make_list(token);
        char * f = content.data;
        long index = 0;
        handle_json_value(content.data, content.len, &tokens, &index);
        for(int j = 0; j < list_entry_count(tokens); j++) {
            token entry = tokens[j];
            printf("(%d) value: %.*s, type: %d\n", j, entry.text_length, entry.text, entry.type);
        }
        printf("\n");
    }
}
