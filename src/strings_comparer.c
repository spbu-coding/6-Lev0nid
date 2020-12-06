#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sortings.h"

int compare_strings_ascending(const char* string1, const char* string2) {
    return strcmp(string2, string1);
}

int compare_strings_descending(const char* string1, const char* string2) {
    return strcmp(string1, string2);
}

int parse_params(char** argv, array_size_t* amount_of_strings, comparator_func_t* cmp) {
    char* end_pointer;
    *amount_of_strings = strtoul(argv[1], &end_pointer, 10);
    if(*end_pointer != '\0') {
        fprintf(stderr, "Second param is not a positive integer");
        return -1;
    }

    static const char* asc = "asc";
    static const char* des = "des";

    if(strcmp(asc, argv[5]) == 0) {
        *cmp = compare_strings_ascending;
    } else if(strcmp(des, argv[5]) == 0) {
        *cmp = compare_strings_descending;
    } else {
        fprintf(stderr, "Unsupported comparator");
        return -1;
    }
    return 0;
}

int sort_strings(char** argv, array_size_t amount_of_strings, strings_array_t strings, comparator_func_t cmp) {
    static const char* word_bubble = "bubble";
    static const char* word_merge = "merge";
    static const char* word_quick = "quick";
    static const char* word_insertion = "insertion";
    static const char* word_radix = "radix";

    if(amount_of_strings == 0) return 0;

    if(strcmp(word_bubble, argv[4]) == 0) {
        bubble(strings, amount_of_strings, cmp);
    } else if(strcmp(word_merge, argv[4]) == 0) {
        merge(strings, amount_of_strings, cmp);
    } else if(strcmp(word_quick, argv[4]) == 0) {
        quick(strings, amount_of_strings, cmp);
    } else if(strcmp(word_insertion, argv[4]) == 0) {
        insertion(strings, amount_of_strings, cmp);
    } else if(strcmp(word_radix, argv[4]) == 0) {
        radix(strings, amount_of_strings, cmp);
    } else {
        fprintf(stderr, "Unsupported sorting algorithm");
        return -1;
    }
    return 0;
}

int read_strings_from_file(FILE* file, array_size_t amount_of_strings, strings_array_t strings) {
    size_t i;
    if(amount_of_strings == 0) return 0;
    for(i = 0; i < amount_of_strings && !feof(file); i++){
        if(fgets(strings[i], MAX_INPUT_STRING_SIZE, file) == NULL) {
            fprintf(stderr, "Reading string #%zu error", i + 1);
            return -1;
        }
    }
    if(i < amount_of_strings) {
        fprintf(stderr, "Not enough strings in file");
        return -1;
    }
    return 0;
}

int put_strings_in_file(FILE* file, array_size_t amount_of_strings, strings_array_t strings) {
    if(amount_of_strings == 0) fputs("\n", file);
    for(size_t i = 0; i < amount_of_strings; i++) {
        if(fputs(strings[i], file) == EOF) {
            fprintf(stderr, "Printing string #%zu error", (i + 1));
            return -1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if(argc != 6) {
        fprintf(stderr, "Incorrect amount of params");
        return -1;
    }
    array_size_t amount_of_strings;
    comparator_func_t cmp = NULL;
    FILE* input_file;
    FILE* output_file;

    if(parse_params(argv, &amount_of_strings, &cmp) == -1) {
        return -1;
    }

    input_file = fopen(argv[2], "r");
    if(input_file == NULL) {
        fclose(input_file);
        fprintf(stderr, "Could not open %s file", argv[2]);
        return -1;
    }

    strings_array_t strings = malloc(amount_of_strings * sizeof(char*));
    if (strings == NULL) {
        fprintf(stderr, "Strings array memory allocation error\n");
        return -1;
    }
    for(size_t i = 0; i < amount_of_strings; i++) {
        strings[i] = (char*)malloc(sizeof(char)*(MAX_INPUT_STRING_SIZE + 1));
        if(strings[i] == NULL) {
            fprintf(stderr, "String #%zu of strings array memory allocation error\n", i + 1);
            for(size_t j = 0; j < i; j++)
                free(strings[j]);
            free(strings);
            return -1;
        }
    }

    if(read_strings_from_file(input_file, amount_of_strings, strings) == -1) {
        for(size_t i = 0; i < amount_of_strings; i++)
            free(strings[i]);
        free(strings);
        fclose(input_file);
        return -1;
    }
    fclose(input_file);

    if(sort_strings(argv, amount_of_strings, strings, cmp) == -1) {
        return -1;
    }

    output_file = fopen(argv[3], "w");
    if(output_file == NULL) {
        fclose(output_file);
        fprintf(stderr, "Could not open %s file", argv[3]);
        return -1;
    }

    if(put_strings_in_file(output_file, amount_of_strings, strings) == -1) {
        for(size_t i = 0; i < amount_of_strings; i++)
            free(strings[i]);
        free(strings);
        fclose(output_file);
        return -1;
    }

    for (size_t i = 0; i < amount_of_strings; i++)
        free(strings[i]);
    free(strings);
    fclose(output_file);
    return 0;
}