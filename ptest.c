/*
 * Test harness for pointer.c
 *
 * By: Arrvindh Shriraman and Dustin Lundquist
 */

#ifdef COMMON_H
#include "common.h"
#endif
#ifndef COMMON_H

#include <stdio.h>

#endif

#include "pointer.h"

typedef struct test_s {
    char *function_name;

    int (*function_under_test)();

    int expected_result;
} test;

typedef struct withinSameBlock_args_s {
    char *function_name;
    int *ptr1;
    int *ptr2;
    int expected_result;
} withinSameBlock_args;

typedef struct withinArray_args_s {
    char *function_name;
    int *base;
    int size;
    int *ptr;
    int expected_result;
} withinArray_args;

typedef struct swapInts_args_s {
    char *function_name;
    int *ptr1;
    int *ptr2;
} swapInts_args;

typedef struct stringLength_args_s {
    char *function_name;
    char *s;
    int expected_result;
} stringLength_args;

typedef struct endianExperiment_args_s {
    char *function_name;
    int *s;
    int expected_result;
} endianExperiment_args;

typedef struct selectionSort_args_s {
    char *function_name;
    int *arr;
    int *arr_copy;
    int arrLength;
    int *expected_result;
} selectionSort_args;

static test tests[] = {{"intSize()",     intSize,     sizeof(int)},
                       {"doubleSize()",  doubleSize,  sizeof(double)},
                       {"pointerSize()", pointerSize, sizeof(void *)},
                       {"changeValue()", changeValue, 295},
                       {NULL, NULL,                   0}};

static withinSameBlock_args box_args[] = {
        {"withinSameBlock", (int *) 0x1,        (int *) 72,         0},
        {"withinSameBlock", (int *) 0x1,        (int *) 4,          1},
        {"withinSameBlock", (int *) 0x12345678, (int *) 0x1,        0},
        {"withinSameBlock", (int *) 0x12345678, (int *) 0x12345658, 1},
        {"withinSameBlock", (int *) 0x12345678, (int *) 0x12345686, 0},
        {NULL, NULL, NULL,                                          0}};

static withinArray_args arr_args[] = {
        {"withinArray", (int *) 0x1,  4, (int *) 0xd,  1},
        {"withinArray", (int *) 0x1,  4, (int *) 0x11, 0},
        {"withinArray", (int *) 0x14, 4, (int *) 0xd,  0},
        {NULL, NULL,                  0, NULL,         0}};

static int int1 = 1;
static int int2 = 2;
static swapInts_args swap_args[] = {{"swapInts", &int1, &int2},
                                    {NULL, NULL, NULL}};

static char string1[] = "hello";
static char string2[] = "hello world";
static char string3[] = "";
static stringLength_args strlen_args[] = {{"stringLength", (char *) string1, 5},
                                          {"stringLength", (char *) string2, 11},
                                          {"stringLength", (char *) string3, 0},
                                          {NULL, NULL,                       0}};

static int ptr = 0;
static endianExperiment_args endianExp_args[] = {
        {"endianExperiment()", &ptr, 295295},
        {NULL, NULL,                 0}};

static int arr1[] = {2, 1};
static int arr1_copy[] = {2, 1};
static int expected_arr1[] = {1, 2};
static int arr2[] = {5, 2, 4, 3, 1};
static int arr2_copy[] = {5, 2, 4, 3, 1};
static int expected_arr2[] = {1, 2, 3, 4, 5};
static selectionSort_args selSort_args[] = {
        {"selectionSort", arr1, arr1_copy, 2, expected_arr1},
        {"selectionSort", arr2, arr2_copy, 5, expected_arr2},
        {NULL, NULL,            0}};

int main(int argc, char **argv) {
    int result;
    int score = 0;
    int flag = 0;
    test *t;

    for (t = tests; t->function_name; t++) {
        result = t->function_under_test();

        printf("%s = %d", t->function_name, result);

        if (result == t->expected_result) {
            printf("\t[ OK ]\n");
            score++;
        } else {
            printf("\t[ fail ] (expected %d)\n", t->expected_result);
        }
    }

    withinSameBlock_args *args;

    for (args = box_args; args->function_name != NULL; args++) {
        result = withinSameBlock(args->ptr1, args->ptr2);
        printf("%s(%p, %p) = %d", args->function_name, args->ptr1, args->ptr2,
               result);
        if (result == args->expected_result) {
            printf("\t[ OK ]\n");
        } else {
            flag = 1;
            printf("\t[ fail ] (expected %d)\n", args->expected_result);
        }
    }
    if (!flag) {
        score++;
    }
    flag = 0;

    withinArray_args *args2;

    for (args2 = arr_args; args2->function_name != NULL; args2++) {
        result = withinArray((args2->base), args2->size, args2->ptr);
        printf("%s(%p, %d, %p) = %d", args2->function_name, args2->base,
               args2->size, args2->ptr, result);
        if (result == args2->expected_result) {
            printf("\t[ OK ]\n");
        } else {
            flag = 1;
            printf("\t[ fail ] (expected %d)\n", args2->expected_result);
        }
    }

    if (!flag) {
        score++;
    }
    flag = 0;

    swapInts_args *args3;

    for (args3 = swap_args; args3->function_name != NULL; args3++) {
        int ptr1PrevVal = *(args3->ptr1);
        int ptr2PrevVal = *(args3->ptr2);
        printf("%s(*ptr1=%d, *ptr2=%d) = ", args3->function_name, *args3->ptr1,
               *args3->ptr2);
        swapInts(args3->ptr1, args3->ptr2);
        printf("*ptr1=%d, *ptr2=%d", *args3->ptr1, *args3->ptr2);
        if (*(args3->ptr1) == ptr2PrevVal && *(args3->ptr2) == ptr1PrevVal) {
            printf("\t[ OK ]\n");
        } else {
            printf("\t[ fail ] (expected *ptr1=%d, *ptr2=%d)\n", ptr2PrevVal,
                   ptr1PrevVal);
            flag = 1;
        }
    }
    if (!flag) {
        score++;
    }
    flag = 0;

    stringLength_args *args4;

    for (args4 = strlen_args; args4->function_name != NULL; args4++) {
        result = stringLength(args4->s);
        printf("%s(%s) = %d", args4->function_name, args4->s, result);
        if (result == args4->expected_result) {
            printf("\t[ OK ]\n");
        } else {
            printf("\t[ fail ] (expected %d)\n", args4->expected_result);
            flag = 1;
        }
    }

    if (!flag) {
        score++;
    }
    flag = 0;

    endianExperiment_args *args5;

    for (args5 = endianExp_args; args5->function_name != NULL; args5++) {
        result = endianExperiment(args5->s);
        printf("%s = %d", args5->function_name, result);
        if (result == args5->expected_result) {
            printf("\t[ OK ]\n");
        } else {
            printf("\t[ fail ] (expected %d)\n", args5->expected_result);
            flag = 1;
        }
    }
    if (!flag) {
        score++;
    }
    flag = 0;

    selectionSort_args *args6;

    for (args6 = selSort_args; args6->function_name != NULL; args6++) {
        selectionSort(args6->arr, args6->arrLength);

        printf("%s(", args6->function_name);
#ifdef COMMON_H
        print_array(args6->arr_copy, args6->arrLength);
#endif
        printf(", %d) = ", args6->arrLength);
#ifdef COMMON_H
        print_array(args6->arr, args6->arrLength);
#endif
        int i;
        result = 1;
        for (i = 0; i < args6->arrLength; i++) {
            if (args6->arr[i] != args6->expected_result[i]) {
                result = !result;
                break;
            }
        }
        if (result) {
            printf("\t[ OK ]\n");
        } else {
            printf("\t[ fail ] (expected ");
#ifdef COMMON_H
            print_array(args6->expected_result, args6->arrLength);
#endif
            printf(")\n");
            flag = 1;
        }
    }

    if (!flag) {
        score++;
    }
    flag = 0;

    printf("Final Score %d:", score);

    return 0;
}
