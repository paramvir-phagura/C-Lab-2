#ifdef COMMON_H
#include "common.h"
#endif
#ifndef COMMON_H

#include <stdio.h>

#endif

#include <string.h>

/*
 * Helper function
 * Prints the binary representation of integer x
 */
void print_binary(int x) {
    int i;
    // start at MSB and go backwards (decrement)
    for (i = 31; i >= 0; --i) {
        // print the i-th bit (0 or 1)
        printf("%d", (x >> i) & 1);
        // add space for readability
        if (i % 8 == 0) printf(" ");
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
}

/*
 * Helper function
 * Prints the string representation of array arr
 */
void print_array(int *arr, int arrLength) {
    if (arrLength < 0) {
        printf("Invalid array, length < 0");
    } else if (arrLength == 1) {
        printf("[]");
    } else if (arrLength > 0) {
        char res[124];
        char buf[64];
        strcpy(res, "[");
        sprintf(buf, "%d", arr[0]);
        strcat(res, buf);
        int j;
        for (j = 1; j < arrLength; j++) {
            strcat(res, ", ");
            sprintf(buf, "%d", arr[j]);
            strcat(res, buf);
        }
        strcat(res, "]");
        printf("%s", res);
    }
}
