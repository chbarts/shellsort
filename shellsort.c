#include <stdlib.h>

void shellsort(void *arr[], size_t n, int (*comp) (void *, void *))
{
    int gaps[] = { 701, 301, 132, 57, 23, 10, 4, 1 };
    size_t i, j;
    void *temp;
    int g;

    if (n <= 1)
        return;

    for (g = 0; g < sizeof(gaps); g++) {
        for (i = gaps[g]; i < n; i++) {
            temp = arr[i];
            for (j = i;
                 (j >= gaps[g]) && (comp(arr[j - gaps[g]], temp) > 0);
                 j -= gaps[g]) {
                arr[j] = arr[j - gaps[g]];
            }

            arr[j] = temp;
        }
    }
}
