#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

const float eps = FLT_EPSILON * 10;

void swap(float *a1, float *a2) {
    float a = *a1;
    *a1 = *a2;
    *a2 = a;
}

/// function that divides string ()
void divide(float *mx, float *a, float div, int sz) {
    for (int k = 0; k < sz; k++) {
        mx[k] /= div;
    }
    *a /= div;
}

/// for operation str1 - c * str2
void subtract(float *str1, float *str2, float *a1, const float *a2, float c, int sz) {
    for (int i = 0; i < sz; i++) {
        if (fabsf(str2[i]) < eps) {
            continue;
        }
        str1[i] -= c * str2[i];
    }
    *a1 -= c * *a2;
}

/// check whether system has none/many solutions
void check(FILE* out, float *a, int i, int sz) {
    for (int k = i; k < sz; k++) {
        if (fabsf(a[k]) > eps) {
            fprintf(out, "no solution");
            return;
        }
    }
    fprintf(out, "many solutions");
}

/// return 1 - malloc error
/// return 2 - file error

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Expected string: \"lab3 <input file name> <output file name>\"");
        return 2;
    }
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        printf("Couldn't open input file");
        return 2;
    }
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        printf("Couldn't open output file");
        fclose(in);
        return 2;
    }
    /// files OK
    int n;
    fscanf(in, "%i", &n);
    int code = 1;
    float *a = (float*) malloc(n * sizeof(float));
    if (!a) {
        printf("Couldn't malloc array a");
        goto clear;
    }
    float **mx = (float**) malloc(n * sizeof(float*));
    if (!mx) {
        printf("Couldn't malloc array of arrays mx[][]");
        goto clear;
    }
    for (int i = 0; i < n; i++) {
        mx[i] = (float*) malloc(n * sizeof(float));
        if (!mx[i]) {
            printf("Couldn't malloc array mx[i]");
            goto clear;
        }
    }
    /// malloc OK
    code = 0;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            fscanf(in, "%f", &mx[j][i]);
        }
        fscanf(in, "%f", &a[j]);
    }
    /// got mx & a correctly

    int i = 0, j = 0;
    for (; i < n && j < n; j++) {
        int b = i;
        float pivot = fabsf(mx[i][j]);
        for (int k = i; k < n; k++) {
            if (fabsf(mx[k][j]) > pivot) {
                pivot = mx[k][j];
                b = i;
            }
        }
        if (pivot < eps) {
            continue;
        }
        for (int k = 0; k < n; k++) {
            swap(&mx[i][k], &mx[b][k]);
        }
        swap(&a[i], &a[b]);
        divide(mx[i], &a[i], mx[i][j], n);
        for (int k = 0; k < n; k++) {
            if (k == i) {continue;}
            subtract(mx[k], mx[i], &a[k], &a[i], mx[k][j], n);
        }
        i++;
    }

    if (i != j) {
        check(out, a, i, n);
        goto clear;
    }

    /// has 1 solution:

    for (int k = 0; k < n; k++) {
        fprintf(out, "%.4f\n", a[k]);
    }

    clear:
        free(a);
        for (int k = 0; k < n; k++) {
            free(mx[k]);
        }
        free(mx);
        fclose(in);
        fclose(out);
        return code;
}
