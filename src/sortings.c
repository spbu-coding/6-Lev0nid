#include "sortings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static size_t min(size_t a, size_t b) {
    if(a < b) return a;
    return b;
}

static void swap(char** a, char** b) {
    char* tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

static int partition(strings_array_t strings, size_t left, size_t right, comparator_func_t cmp) {
    char* middle_string = strings[(left + right) / 2];
    while(left <= right) {
        while(cmp(strings[left], middle_string) > 0)
            left++;
        while(cmp(middle_string, strings[right]) > 0)
            right--;
        if(left >= right) break;
        swap(&strings[left], &strings[right]);
        left++;
        right--;
    }
    return right;
}

static void quick_sort_internal(strings_array_t strings, size_t left, size_t right, comparator_func_t cmp) {
    if(left < right) {
        size_t mid = partition(strings, left, right, cmp);
        quick_sort_internal(strings, left, mid, cmp);
        quick_sort_internal(strings, mid + 1, right, cmp);
    }
}

void quick(strings_array_t strings, array_size_t amount_of_strings, comparator_func_t cmp) {
    quick_sort_internal(strings, 0, amount_of_strings - 1, cmp);
}

void bubble(strings_array_t strings, array_size_t amount_of_strings, comparator_func_t cmp) {
    for(size_t i = 0; i < amount_of_strings; i++) {
        for(size_t j = i + 1; j < amount_of_strings; j++) {
            if(cmp(strings[i], strings[j]) <= 0)
                swap(&strings[i], &strings[j]);
        }
    }
}

void insertion(strings_array_t strings, array_size_t amount_of_strings, comparator_func_t cmp) {
    for(size_t i = 0; i < amount_of_strings; i++) {
        for(size_t j = i; j > 0 && cmp(strings[j - 1], strings[j]) <= 0; j--)
            swap(&strings[j-1], &strings[j]);
    }
}

static void merge_arrays(strings_array_t strings, strings_array_t buffer, size_t left,
                         size_t middle, size_t right, comparator_func_t cmp) {
    size_t i1 = 0, i2 = 0;

    while(left + i1 < middle && middle + i2 < right) {
        if(cmp(strings[left + i1], strings[middle + i2]) > 0) {
            buffer[i1 + i2] = strings[left + i1];
            i1++;
        } else {
            buffer[i1 + i2] = strings[middle + i2];
            i2++;
        }
    }

    for(; left + i1 < middle; i1++)
        buffer[i1 + i2] = strings[left + i1];

    for(; middle + i2 < right; i2++)
        buffer[i1 + i2] = strings[middle + i2];

    for(size_t i = 0; i < i1 + i2; i++)
        strings[left + i] = buffer[i];
}

void merge(strings_array_t strings, array_size_t amount_of_strings, comparator_func_t cmp) {
    strings_array_t buffer = malloc(amount_of_strings * sizeof(char*));
    if (buffer == NULL) {
        fprintf(stderr, "Merge sort buffer memory allocation error\n");
        free(buffer);
        exit(-1);
    }
    for(size_t i = 1; i < amount_of_strings; i *= 2) {
        for(size_t j = 0; j < amount_of_strings - i; j += i * 2) {
            merge_arrays(strings, buffer, j, j + i, min(j + 2 * i, amount_of_strings), cmp);
        }
    }
    free(buffer);
}

static void radix_sort_internal(strings_array_t strings, array_size_t amount_of_strings,
                      size_t left, size_t right, size_t position){
    if(position >= MAX_INPUT_STRING_SIZE || left + 1 >= right) return;

    int count[UCHAR_MAX + 1];
    for(size_t i = 0; i < UCHAR_MAX + 1; i++){
        count[i] = 0;
    }

    for(size_t i = left; i < right; i++){
        count[(unsigned char)strings[i][position]]++;
    }
    for(size_t i = 1; i < UCHAR_MAX + 1; i++){
        count[i] += count[i - 1];
    }
    int count_copy[UCHAR_MAX + 1];
    memcpy(count_copy, count, (UCHAR_MAX + 1)*sizeof(int));

    strings_array_t bucket = malloc((right - left) * sizeof(char*));
    if(bucket == NULL) {
        fprintf(stderr, "Radix sort bucket memory allocation error\n");
        free(bucket);
        exit(-1);
    }

    for(size_t i = left; i < right; i++) {
        bucket[--count[(unsigned char) strings[i][position]]] = strings[i];
    }
    memcpy(strings + left, bucket, (right - left)*sizeof(char*));

    radix_sort_internal(strings, amount_of_strings, left, left + count_copy[0], position + 1);
    for(int i = 1; i < UCHAR_MAX + 1; i++){
        radix_sort_internal(strings, amount_of_strings, left + count_copy[i - 1],
                            left + count_copy[i], position + 1);
    }

    free(bucket);
}


void radix(strings_array_t string, array_size_t amount_of_strings, comparator_func_t cmp){
    radix_sort_internal(string, amount_of_strings, 0, amount_of_strings, 0);
    if(cmp("1", "2") < 0) {
        for(size_t i = 0; i < amount_of_strings / 2; i++) {
            swap(&string[i], &string[amount_of_strings - i - 1]);
        }
    }
}