 // Google and Mozzila's JSON parsing is about 5x faster than mine when
// my parser is running hot and theirs is running hot. 
// Theirs is 10x faster when mine is running cold and theirs is hot
// Testing theirs running cold is not really possible with browser APIs
// since it is sub-millisecond.
// 
// Considering my parser does a lot less error checking, I'm assuming
// I should be able to actually beat their performance. 
// At least in theory. So I have at least a 5x boost possibility.
// 
// Ideas I have to speed mine up:
//
// DONE 1. Create custom allocator rather than many tiny mallocs, use throughout
// Also preallocated some memory on stack and token list
// 2. modify map to handle non-zero terminated string keys during set to avoid
//    unnecessary mallocs.
// 3. Stream tokens rather than creating all upfront then parsing
// 4. Try to determine if it's a perfomance problem with map/list?
// 5. Figure out how to run a c-lang profiler? 
// 6. Consider looking at chromium's source code for guidance
// 7. Add a version of list that accepts a stack during creation
// 
//
//
//   C: 16.762022 microseconds per run
//   NODE: 5.978 microseconds per run
//   
//   35% as fast as Chromium
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/hash_map.h"
#include "./json_parser.h"

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
        null_token
    },
    {
        "true",
        4,
        boolean_token
    },
    {
        "false",
        5,
        boolean_token
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

static int is_num(char c) {
    return c >= '0' && c <= '9';
}
token tokenize_json(char * f, long len, token ** tokens, long * index, struct little_stack* stk) {
    //Skip them space
    token new_token;

    if(*index > len) {
        new_token.type = empty_token;
        return new_token;
    }
    while (isspace(f[*index])) {
        *index = *index + 1;
        if ( *index > len ) {
            new_token.type = empty_token;
            return new_token;
        }
    }

    int starting_index = *index;
    char starting_char = f[starting_index];
    new_token.text = &f[starting_index];
    //Check for numbers
    if(is_num(starting_char) || starting_char == '-') {
        new_token.type = number_token;
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
        new_token.type = string_token;
        //Skip quote
        int str_len = 0;
        *index = *index + 1;
        char * starting_str_point = &(f[*index]);
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
            *index = *index + 1;
        }
        new_token.text = starting_str_point;
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
        last_found.type = empty_token;
        while(last_found.type != close_bracket) {
            last_found = tokenize_json(f, len, tokens, index, stk);
        }
        return new_token;
    }
    if(starting_char == '{') {
        new_token.type = open_brace;
        new_token.text_length = 1;
        *index = *index + 1;
        append_to_list(*tokens, new_token);
        token last_found;
        last_found.type = empty_token;
        while(last_found.type != close_brace) {
            last_found = tokenize_json(f, len, tokens, index, stk);
        }
        return new_token;
    }
    // printf("index: %ld, starting_char: %c\n", *index, starting_char);
    assert(0);
    //None
    return new_token;
}

void convert_tokens_to_json(token * tokens, json * js, int * index, int token_count, struct little_stack* stk) {    
    if (*index > token_count) return;
    token cur_token = tokens[*index];
    
    switch (cur_token.type)
    {
        case colon_seperator:
            assert(0);
        case comma:
            assert(0);
        case close_brace:
            assert(0);
        case close_bracket:
            assert(0);
        case empty_token: 
            break;
        case string_token:
        {
            char * c_str = alloc_stack(stk, cur_token.text_length + 1);
            for(int i = 0; i < cur_token.text_length; i++) {
                c_str[i] = cur_token.text[i];
            }
            c_str[cur_token.text_length] = '\0';
            js->value.str_value = c_str;
            js->value_type = string_value;
            break;
        }
        case number_token:
        {
            char c_str[cur_token.text_length + 1];
            for(int i = 0; i < cur_token.text_length; i++) {
                c_str[i] = cur_token.text[i];
            }
            c_str[cur_token.text_length] = '\0';
            js->value.num_value = atof(c_str);
            js->value_type = number_value;
            break;
        }
        case null_token:
        {
            js->value_type = null_value;
            break;
        }
        case boolean_token:
        {
            int val = 0;
            if(cur_token.text_length == 4) {
                //AKA true
                val = 1;
            }
            js->value.bool_value = val;
            js->value_type = boolean_value;
            break;
        }
        case open_brace:
        {
            hash_map new_hashmap = make_hash_map(json*);
            js->value.map_value = new_hashmap;
            js->value_type = object_value;
            assert(*index + 4 <= token_count);
            token key_token = tokens[*index + 1];
            //Create map
            while(key_token.type != close_brace) {
                //Skip open brace or comma
                *index = *index + 1;
                key_token = tokens[*index];
                char * map_key = alloc_stack(stk, key_token.text_length + 1);
                for(int i = 0; i < key_token.text_length; i++) {
                    map_key[i] = key_token.text[i];
                }
                map_key[key_token.text_length] = '\0';
                json * child_json_p = alloc_stack(stk, sizeof(json));
                //Skip colon and key
                *index = *index + 2;
                //Recurse!
                convert_tokens_to_json(tokens, child_json_p, index, token_count, stk);
                add_hash_entry(new_hashmap, map_key, child_json_p);
                //Skip value
                *index = *index + 1;
                key_token = tokens[*index];
            }
            break;
        }
        case open_bracket:
        {
            json* list = make_list(json);
            js->value_type = list_value;
            assert(*index + 1 <= token_count);
            token next_value = tokens[*index + 1];
            
            while(next_value.type != close_bracket) {
                //Skip open bracket or comma
                *index = *index + 1;
                json child_json;
                //Recurse!
                convert_tokens_to_json(tokens, &child_json, index, token_count, stk);
                append_to_list(list, child_json);
                *index = *index + 1;
                next_value = tokens[*index];
            }
            js->value.list_value = list;
            break;
        }
    }
}

void get_json_from_string(char * f, long len, json* result) {
        token * tokens = make_list_with_starting_length(token, 100);
        struct little_stack stk = create_stack_with_size(300);
        long index = 0;
        tokenize_json(f, len, &tokens, &index, &stk);
        // assert(index == len);
        int index_of_tokens = 0;
        int token_len = list_entry_count(tokens);
        convert_tokens_to_json(tokens, result, &index_of_tokens, token_len, &stk);
}