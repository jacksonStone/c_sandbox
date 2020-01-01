//Small malloc is about 200 nanoseconds
//Accessing JSON member is about 300 nanoseconds

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
    growing_heap* gh;
    char* error_message;
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
    char* error_message;
} json;
void skip_whitespace(json_parser* jp) {
    while(jp->index < jp->text_len && !jp->error_message) {
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
void attach_error_message(char* cstr, json_parser* jp) {
    if(jp->error_message) return;
    
    //Get region around where the error occured
    int start_index = 0;
    int end_index = 0;
    size_t len = jp->text_len;
    if(jp->text_len > jp->index + 20) {
        end_index = jp->index + 20;
    } else {
        end_index = jp->text_len;
    }
    if(jp->index - 20 > 0) {
        start_index = jp->index - 20;
    } else {
        start_index = 0;
    }
    int length_of_area = (end_index - start_index) + 6;
    char area_of_error[length_of_area];
    int char_index = 0;
    for (int i = start_index; i < end_index; i++) {
        if(i == jp->index) {
            area_of_error[char_index++] = '>';
            area_of_error[char_index++] = '>';
            area_of_error[char_index++] = jp->text[i];
            area_of_error[char_index++] = '<';
            area_of_error[char_index++] = '<';
            continue;
        }
        area_of_error[char_index] = jp->text[i];
        char_index++;
    }
    area_of_error[length_of_area] = '\0';


    char* error_message = gh_malloc(jp->gh, strlen(cstr) + 300);
    sprintf(error_message, "Error at index: %zu\nArea of error:\n%s\nMessage: %s\n", jp->index, area_of_error, cstr);
    jp->error_message = error_message;
};
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
            attach_error_message("Invalid token", jp);
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
    while(jp->index < jp->text_len && !jp->error_message) {
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
json_value get_next_json_value(json_parser* jp);
hashmap get_json_object_hashmap(json_parser* jp) {
    growing_heap* gh = jp->gh;
    hashmap hm = make_hashmap(gh);
    //Open brace
    jp->index++;
    int handled_all_values = 0;
    while(jp->index < jp->text_len && !jp->error_message) {
        switch(get_next_token(jp)) {
            case STRING_T: {
                if (handled_all_values) {
                    attach_error_message("Had an additional string key after value without comma", jp);
                }
                json_string string_key = get_next_json_string(jp);
                if (!string_key.beginning) {
                    attach_error_message("Failed to parse string key of object", jp);
                }
                if(get_next_token(jp) != COLON_T) {
                    attach_error_message("Did not have colon following string key", jp);
                }
                //Skip colon
                jp->index++;
                json_value map_value = get_next_json_value(jp);
                
                //TODO:: SPEED Probably an unnecessary copy!
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
                attach_error_message("Unrecognized token during object parsing", jp);
        }
    }
    attach_error_message("Never reached an object's close brace", jp);
    return hm;
}
void* get_json_list(json_parser* jp) {
    growing_heap* gh = jp->gh;
    //[
    jp->index++;
    json_value* value_list = make_list_with_allocator(json_value, gh);
    int done_with_values = 0;
    while(jp->index < jp->text_len && !jp->error_message) {
        switch(get_next_token(jp)) {
            case LIST_END_T: {
                jp->index++;
                return value_list;
            }
            default: {
                if(done_with_values) {
                    attach_error_message("More entries after no comma", jp);
                }
                json_value v = get_next_json_value(jp);
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
    attach_error_message("Never reached a list's close bracket", jp);
    return value_list;
    
}
json_value get_next_json_value(json_parser* jp) {
    json_value new_node = {};
    new_node.value_type = INVALID_V;
    if(jp->index >= jp->text_len) {
        attach_error_message("Tried to get JSON value but already at the end of the text", jp);
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
            while(jp->index < jp->text_len && !jp->error_message) {
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
                            attach_error_message("'-' must appear at the start of the number or after an 'e'", jp);
                        }
                        break;
                    case '+':
                        if(jp->text[jp->index - 1] == 'e') {
                            number_len++;
                            jp->index++;
                        } else {
                            attach_error_message("Can only have '+' immediatly after 'e'", jp);
                        }
                        break;
                    case 'e':
                        if(seen_e) {
                            attach_error_message("Only one 'e' is permissable in a valid number", jp);
                        }
                        seen_e = 1;
                        number_len++;
                        jp->index++;
                        break;
                    case '0':
                        if(number_len==0) {
                            if(jp->index+1 < jp->text_len) {
                                char next_char = jp->text[jp->index+1];
                                if (next_char >= '0' && next_char <= '9') {
                                    attach_error_message("Leading zeros are not permissable", jp);
                                }
                            }
                        }
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
                            attach_error_message("Inappropriate use of '.'", jp);
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
                attach_error_message("Invalid token literal", jp);
            }
            break;
        }
        case FALSE_T: {
            if(jp->index + 4 < jp->text_len && strncmp("false", &(jp->text[jp->index]), 5) == 0) {
               jp->index += 5;
               new_node.value_type = BOOLEAN_V;
               new_node.value.bool = 0;
            } else {
                attach_error_message("Invalid token literal", jp);
            }
            break;
        }
        case NULL_T: {
            if(jp->index + 3 < jp->text_len && strncmp("null", &(jp->text[jp->index]), 4) == 0) {
               jp->index += 4;
               new_node.value_type = NULL_V;
            } else {
                attach_error_message("Invalid token literal", jp);
            }
            break;
        }
        case OBJECT_BEGIN_T: {
            new_node.value.map_value = get_json_object_hashmap(jp);
            new_node.value_type = OBJECT_V;
            break;
        }
        case LIST_BEGIN_T:
            new_node.value.list_value = get_json_list(jp);
            new_node.value_type = LIST_V;
            break;
        default:
            attach_error_message("Unknown token", jp);

    }
    return new_node; 
}
json parse_json_with_allocator(char* data, size_t len, growing_heap* gh) {
    json_parser jp = {
        len,
        data,
        0,
        gh,
        NULL
    };
    json j = {};
    j.gh = jp.gh;
    j.data = get_next_json_value(&jp);
    if(jp.error_message) {
        j.error_message = jp.error_message;
        printf("%s\n", j.error_message);
    }
    return j;
}
json parse_json(char* text, int len) {
    growing_heap* gh_ptr = malloc(sizeof(growing_heap));
    *gh_ptr = make_growing_heap();
    json j = parse_json_with_allocator(text, strlen(text), gh_ptr);
    return j;
}
json parse_json_c_string(char* text) {
    return parse_json(text, strlen(text));
}
void free_json(json* j) {
    gh_free(j->gh);
}
json_value error_json_value() {
    assert(0);
    json_value jv = {};
    jv.value_type = INVALID_V;
    return jv;
}
json_value get_json_value(json_value j, char* path, int len) {
    if(!(*path)) {
        return j;
    }
    int index = 0;
    char* start_position = path;
    int is_list_entry = 0;
    int found_end_of_list = 0;
    int found_end = 0;
    int found_period = 0;
    int nonlist_chars = 0;
    while(index < len && path[index]) {
        if(found_end) {
            break;
        }
        switch(path[index]) {
            case '\\': {
                if(index + 1 < len) {
                    //Skip escaped char
                    index+=2;
                } else { 
                    //trailing "\"
                    index++;
                }
                break;
            }
            case '[': {
                if(nonlist_chars) {
                    //Ex bar[1]
                    found_end = 1;
                    break;
                }
                is_list_entry = 1;
                index++; // [
                int num_len = 0;
                while(index < len) {
                    if(found_end_of_list) {
                        break;
                    }
                    switch(path[index]) {
                        case '0': 
                            if(num_len == 0 && (index + 1 >= len || path[index+1] != ']')) {
                                return error_json_value(); // Cannot have numbers after a leading 0
                            }
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            num_len++;
                            break;
                        case ']':
                            if(num_len == 0) {
                                return error_json_value(); //No numeric value in index
                            }
                            found_end_of_list = 1;
                            break;
                        default:
                            return error_json_value(); //Non-numeric in list

                    }
                    index++;
                }
                break;
                //TODO::
            }
            case '.': {
                found_end = 1;
                found_period = 1;
                break;
                //TODO::
            }
            default:
                nonlist_chars = 1;
                index++;
        }
    }
    path = path + index;
    if(found_period) {
        //Skip period
        path = path + 1;
    }
    if(!is_list_entry) {
        if(j.value_type != OBJECT_V) {
            return error_json_value();//Not an object
        }
        json_value* child_ptr = (json_value*) get_hashmap_entry(&(j.value.map_value), start_position, index);
        if(found_period) {
            index++;
        }
        return get_json_value(*child_ptr, path, len - index);
    }
    if(!found_end_of_list && is_list_entry) {
        return error_json_value(); //Never found end of list and somehow got here
    }

    if(j.value_type != LIST_V) {
        return error_json_value(); //Attempted to fetch an index from a non-list
    }
    // Handle list entry
    // minus 2 for [ and ], plus one for '\0'
    char num_str[index - 1];
    for(int i = 1; i < index; i++) {
        num_str[i - 1] = start_position[i];
    }
    num_str[index-2] = '\0';
    int index_in_list = atoi(num_str);

    json_value* list_value = j.value.list_value;
    if(index_in_list >= get_list_len(list_value)) {
        return error_json_value(); // Out of bounds of list
    }
    if(found_period) {
        index++;
    }
    return get_json_value(list_value[index_in_list], path, len - index);


    assert(0);
    return j;

}
json_value get_json_value_c_string(json_value j, char* path) {
    return get_json_value(j, path, strlen(path));
}
// double get_number_from_json(char* path) {

// }

// void* get_list_from_json(char* path) {

// }

int main() {
    int allot_size = 500;    
    file_contents content = get_file_contents("./example6.json");
    int interations =  100000;
    //int interations = 1;
    start_timer();
    for(int i = 0; i < interations; i++) {
        json j = parse_json(content.data, content.len);
        json_value jv = get_json_value_c_string(j.data, "foo[1].bop[10]");
        free_json(&j);
    }
    printf("C: %f microsec for no allocator\n", (stop_timer()/interations));
   
    return 0;
}

