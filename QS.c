#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long ull;

const ull SIZE = 1 << 8;

struct Data
{
    char surname[21];
    char name[21];
    char patronymic[21];
    ull number;
};

void delData (struct Data *data, ull sz) {
    if (!data) {
        for (ull i = 0; i < sz; i++) {
            free(data[i].surname);
            free(data[i].name);
            free(data[i].patronymic);
        }
        free(data);
    }
}

struct Data* create (struct Data *data, ull sz) {
    data = malloc(sz * sizeof (struct Data));
    if (!data) {
        return NULL;
    }
    return data;
}

void swap(struct Data *a1, struct Data *a2) {
    struct Data a = *a1;
    *a1 = *a2;
    *a2 = a;
}

_Bool comp(struct Data *a1, struct Data *a2) {
    if (strcmp(a1->surname, a2->surname) != 0) {
        return strcmp(a1->surname, a2->surname) > 0;
    }
    if (strcmp(a1->name, a2->name) != 0) {
        return strcmp(a1->name, a2->name) > 0;
    }
    if (strcmp(a1->patronymic, a2->patronymic) != 0) {
        return strcmp(a1->patronymic, a2->patronymic) > 0;
    }
    return (a1->number > a2->number);
}

void qSort(struct Data *data, ull sz, FILE *out) {
    ull i, j, lb, rb, pos = 1, mid;
    struct Data pivot;
    ull lefts[SIZE], rights[SIZE];
    lefts[1] = 0;
    rights[1] = sz - 1;
    do {
        lb = lefts[pos];
        rb = rights[pos];
        pos--;
        mid = (lb + rb) / 2;
        i = lb;
        j = rb;
        pivot = data[mid];
        do {
            while (comp(&pivot, &data[i])) { i++; }
            while (comp(&data[j], &pivot)) { j--; }
            if (i <= j) {
                if (comp(&data[i], &data[j])) {
                    swap(&data[i], &data[j]);
                }
                i++;
                if (j > 0) {
                    j--;
                }
            }
        } while (i <= j);

        if (i < rb) {
            pos++;
            lefts[pos] = i;
            rights[pos] = rb;
        }
        if (j > lb) {
            pos++;
            lefts[pos] = lb;
            rights[pos] = j;
        }
    }  while (pos != 0);
}

int main(int argc, char** argv) {
    int code = 2;
    if (argc != 3) {
        printf("Expected string: \"lab2 <input file name> <output file name>\"");
        return 2;
    }
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        printf("Couldn't open input file");
        return code;
    }
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        printf("Couldn't open output file");
        fclose(in);
        return code;
    }
    /// files OK
    code = 0;
    ull n = 0;
    struct Data tmp;
    char *tmp4 = malloc(12 * sizeof(char));
    while(fscanf(in, "%s %s %s %s", tmp.surname, tmp.name, tmp.patronymic, tmp4) != EOF) {n++;}
    rewind(in);
    struct Data *data = create(data, n);
    if (!data) {
        printf("Allocation error");
        code = 1;
        goto clear;
    }

    for(ull i = 0; i < n; i++) {
        fscanf(in, "%s %s %s %s", data[i].surname, data[i].name, data[i].patronymic, tmp4);
        data[i].number = strtoull(tmp4, NULL, 10);
    }

    qSort(data, n, out);

    for (ull i = 0; i < n; i++) {
        fprintf(out, "%s ", data[i].surname);
        fprintf(out, "%s ", data[i].name);
        fprintf(out, "%s ", data[i].patronymic);
        fprintf(out, "%llu\n", data[i].number);
    }

    clear:
        delData(data, n);
        fclose(in);
        fclose(out);
        return code;
}
