#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../timer/timer.h"
#include "./types.h"
#include "./static_heap.h"
#include "./growing_heap.h"
#include "./list.h"
#include "./hashmap.h"
#include "../file_reader/file_reader.h"

typedef struct {
    size_t text_len;
    char* text;
    size_t index;
} json_parser;
typedef struct {
    size_t len;
    char* beginning;
} json_string;
enum json_value_type {
    INVALID_V,
    NULL_V,
    STRING_V,
    NUMBER_V,
    BOOLEAN_V,
    LIST_V,
    OBJECT_V,
};
//Inspired by Chromium's tokens
 enum json_token {
    INVALID_T,
    OBJECT_BEGIN_T,
    OBJECT_END_T,
    LIST_BEGIN_T,
    LIST_END_T,
    STRING_T,
    NUMBER_T,
    TRUE_T,
    FALSE_T,
    NULL_T,
    COMMA_T,
    COLON_T,
    EOF_T,
};
typedef struct {
    union {
        json_string str;
        double num;
        int bool;
        void* list_value;
        hashmap map_value;
    } value;
    enum json_value_type value_type;
} json_value;
typedef struct {
    json_value data;
    growing_heap* gh;
} json;
void skip_whitespace(json_parser* jp) {
    while(jp->index < jp->text_len) {
        switch (jp->text[jp->index]) {
            case '\r':
            case '\n':
            case ' ':
            case '\t':
                jp->index++;
                break;
            default:
                return;
        }
    }
}
enum json_token get_next_token(json_parser* jp) {
    skip_whitespace(jp);
    if (jp->index >= jp->text_len) return EOF_T;
    switch (jp->text[jp->index]) {
        case '{':
            return OBJECT_BEGIN_T;
        case '}':
            return OBJECT_END_T;
        case '[':
            return LIST_BEGIN_T;
        case ']':
            return LIST_END_T;
        case '"':
            return STRING_T;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return NUMBER_T;
        case 't':
            return TRUE_T;
        case 'f':
            return FALSE_T;
        case 'n':
            return NULL_T;
        case ',':
            return COMMA_T;
        case ':':
            return COLON_T;
        default:
            return INVALID_T;
    }
}
//Expects to be passed a json_parser with an index
//pointing to an open quote, and increments index to be after
//the closing quote 
json_string get_next_json_string(json_parser* jp) {
    jp->index++; // skip quote
    char* str_start = (jp->text + jp->index);
    int strlen = 0;
    json_string js = {};
    while(jp->index < jp->text_len) {
        if(jp->text[jp->index] == '"') {
            //Skip over closed quote
            jp->index++;
            //Valid string
            // new_node.value_type = STRING_V;
            js.len = strlen;
            js.beginning = str_start;
            // new_node.value.str = str;
            return js;
        }
        //Handle char escapes
        if(jp->text[jp->index] == '\\') {
            jp->index+=2;
            strlen+=2;  
        } else {
            jp->index+=1;
            strlen+=1; 
        }
    }
    return js;
}
json_value get_next_json_value(json_parser* jp, growing_heap* gh);
hashmap get_json_object_hashmap(json_parser* jp, growing_heap* gh) {
    hashmap hm = make_hashmap(gh);
    //Open brace
    jp->index++;

    while(jp->index < jp->text_len) {
        int handled_all_values = 0;
        switch(get_next_token(jp)) {
            case STRING_T: {
                if (handled_all_values) {
                    assert(0);//Had a string key without a seperating ,
                }
                json_string string_key = get_next_json_string(jp);
                if (!string_key.beginning) {
                    assert(0); //Failed to parse string key of object
                }
                if(get_next_token(jp) != COLON_T) {
                    assert(0); //Did not have colon following string key
                }
                //Skip colon
                jp->index++;
                json_value map_value = get_next_json_value(jp, gh);
                //TODO:: SPEED Probably an unnecessary copy!
                if(map_value.value_type == INVALID_V) {
                    assert(0); // Failed to parse value attribute for object
                }
                json_value* map_value_ptr = gh_malloc(gh, sizeof(json_value));
                memcpy(map_value_ptr, &map_value, sizeof(json_value));
                add_to_hashmap(&hm, string_key.beginning, string_key.len, map_value_ptr);
                if(get_next_token(jp) != COMMA_T) {
                    handled_all_values = 1;
                } else {
                    //Skip comma, and trigger another cycle of key/value pair handling
                    jp->index++;
                }
                
                break;
            }
            case OBJECT_END_T: {
                //Close brace
                jp->index++;
                return hm;
            }
            default: 
                assert(0);//Is not one of our specified tokens
        }
    }
    assert(0); //Never reached the Object's End token
    return hm;
}
void* get_json_list(json_parser* jp, growing_heap* gh) {
    //[
    jp->index++;
    json_value* value_list = make_list_with_allocator(json_value, gh);
    int done_with_values = 0;
    while(jp->index < jp->text_len) {
        switch(get_next_token(jp)) {
            case LIST_END_T: {
                jp->index++;
                return value_list;
            }
            case INVALID_T: 
                assert(0); //Failed to parse list entry token
                return value_list;
            default: {
                if(done_with_values) {
                    assert(0); //More entries after no comma
                }
                json_value v = get_next_json_value(jp, gh);
                if(v.value_type == INVALID_V) {
                    assert(0); //Something went wrong with the list entry creation
                }
                value_list = append_to_list(value_list, v);
                if(get_next_token(jp) != COMMA_T) {
                    done_with_values = 1;
                } else {
                    //Skip comma, and cycle again
                    jp->index++;
                }
            }
        }
    }
    assert(0); //Never reached list end
    return value_list;
    
}
json_value get_next_json_value(json_parser* jp, growing_heap* gh) {
    json_value new_node = {};
    new_node.value_type = INVALID_V;
    if(jp->index >= jp->text_len) {
        assert(0); //ran out of space
    }
    switch(get_next_token(jp)) {
        case STRING_T: {
            json_string js = get_next_json_string(jp);
            if (js.beginning) {
                new_node.value_type = STRING_V;
                new_node.value.str = js;
            }
            break;
        }            
        case NUMBER_T: {
            //YOU ARE HERE
            int number_len = 0;
            int seen_period = 0;
            int seen_e = 0;
            int done = 0;
            char* num_start = &(jp->text[jp->index]);
            while(jp->index < jp->text_len) {
                if (done) break;
                switch (jp->text[jp->index]) {
                    
                    case '-':
                        if(number_len == 0) {
                            number_len++;
                            jp->index++;
                        } 
                        else if(jp->text[jp->index - 1] == 'e') {
                            number_len++;
                            jp->index++;
                        } else {
                            //Negative sign must appear at the start or after an e
                            assert(0);
                        }
                        break;
                    case '+':
                        if(jp->text[jp->index - 1] == 'e') {
                            number_len++;
                            jp->index++;
                        } else {
                            assert(0); //Can only have + after e
                        }
                        break;
                    case 'e':
                        if(seen_e) {
                            assert(0); // more than one e in number
                        }
                        seen_e = 1;
                        number_len++;
                        jp->index++;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        number_len++;
                        jp->index++;
                        break;
                    case '.': 
                        if(!seen_period && !seen_e) {
                            seen_period = 1;
                            number_len++;
                            jp->index++;
                        } else {
                            //Two periods in number or period after e
                            assert(0);
                        }
                        break;
                    default:
                        done = 1;
                }
            }
            // TODO::See if there is a way to prevent this copying
            char c_str[number_len + 1];
            memcpy(c_str,num_start,number_len);
            c_str[number_len] = '\0';
            double num = atof(c_str);

            new_node.value.num = num;
            new_node.value_type = NUMBER_V;
            break;
        }
        case TRUE_T: {
            if(jp->index + 3 < jp->text_len && strncmp("true", &(jp->text[jp->index]), 4) == 0) {
               jp->index += 4;
               new_node.value.bool = 1;
               new_node.value_type = BOOLEAN_V;
            } else {
                assert(0);
            }
            break;
        }
        case FALSE_T: {
            if(jp->index + 4 < jp->text_len && strncmp("false", &(jp->text[jp->index]), 5) == 0) {
               jp->index += 5;
               new_node.value_type = BOOLEAN_V;
               new_node.value.bool = 0;
            } else {
                assert(0);
            }
            break;
        }
        case NULL_T: {
            if(jp->index + 3 < jp->text_len && strncmp("null", &(jp->text[jp->index]), 4) == 0) {
               jp->index += 4;
               new_node.value_type = NULL_V;
            } else {
                assert(0);
            }
            break;
        }
        case OBJECT_BEGIN_T: {
            new_node.value.map_value = get_json_object_hashmap(jp, gh);
            new_node.value_type = OBJECT_V;
            break;
        }
        //TODO::
        case LIST_BEGIN_T:
            new_node.value.list_value = get_json_list(jp, gh);
            new_node.value_type = LIST_V;
            break;
        default:
            assert(0);
    }
    assert(new_node.value_type != INVALID_V);
    return new_node; 
}
json parse_json_with_allocator(json_parser* jp, growing_heap* gh) {
    json j = {
        get_next_json_value(jp, gh),
        gh
    };
    
    //Read the whole thing
    skip_whitespace(jp);
    assert(jp->index == jp->text_len);

    return j;
}


int main() {
    int allot_size = 500;    
    file_contents content = get_file_contents("./example6.json");
    int interations =  100000;
    start_timer();
    for(int i = 0; i < interations; i++) {
        growing_heap gh = make_growing_heap();
        json_parser jp = {
            content.len,
            content.data,
            0
        };
        json j = parse_json_with_allocator(&jp, &gh);
        gh_free(&gh);
    }
   
    //    int* my_ints = make_list_with_allocator(int, &gh)
    //    my_ints = append_to_list(my_ints, 4);
    //    hashmap hm = make_hashmap(&gh);
    //    int k = 4;
    //    add_to_hashmap_c_string(&hm, "zz1", &k);
    //    int * res = get_hashmap_entry_c_string(&hm, "zz1");
    //    printf("Just retrieved: %d\n", *res);
    //    free_list(my_ints);
    //    gh_free(&gh);


    //Is a no-op if you pass an allocator during list creation

    printf("C: %f microsec\n", (stop_timer()/interations));
    
    
    return 0;
}

