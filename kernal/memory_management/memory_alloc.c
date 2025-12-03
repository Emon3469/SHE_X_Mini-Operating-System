#include <stdio.h>
#include "memory_alloc.h"

void calculateInternalFragmentation() {
    int ms, bs, nob, eif, n, mp[10], tif = 0;
    int i, p = 0;

    printf("Enter total memory size: ");
    scanf("%d", &ms);

    printf("Enter block size: ");
    scanf("%d", &bs);

    nob = ms / bs;
    eif = ms - nob * bs;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter memory required for each process:\n");
    for(i = 0; i < n; i++) {
        printf("Process %d: ", i+1);
        scanf("%d", &mp[i]);
    }

    for(i = 0; i < n && p < nob; i++) {
        if(mp[i] > bs) {
            printf("Process %d cannot be allocated (Too large)\n", i+1);
        } else {
            tif += (bs - mp[i]);
            p++;
        }
    }

    if(p == n) {
        eif = 0;
    }

    printf("\nTotal Internal Fragmentation = %d bytes\n", tif);
    printf("External Fragmentation = %d bytes\n", eif);

    if(i < n) {
        printf("Memory is Full — Remaining processes cannot be allocated.\n");
    }

    printf("Total processes allocated: %d\n", p);
    printf("Unused Blocks: %d\n", nob - p);
}

int main() {
    calculateInternalFragmentation();
    return 0;
}
