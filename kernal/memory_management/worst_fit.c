#include <stdio.h>
#include "worst_fit.h"

#define MAX 50

void worstFitAllocation() {
    int frag[MAX], b[MAX], f[MAX], bf[MAX], ff[MAX];
    int i, j, nb, nf, temp, highest = 0;

    for(i = 0; i < MAX; i++) {
        b[i] = f[i] = bf[i] = ff[i] = frag[i] = 0;
    }

    printf("Enter number of blocks: ");
    scanf("%d", &nb);

    printf("Enter number of files: ");
    scanf("%d", &nf);

    printf("Enter block sizes:\n");
    for(i = 0; i < nb; i++) {
        printf("Block %d: ", i+1);
        scanf("%d", &b[i]);
    }

    printf("Enter file sizes:\n");
    for(i = 0; i < nf; i++) {
        printf("File %d: ", i+1);
        scanf("%d", &f[i]);
    }

    for(i = 0; i < nf; i++) {
        highest = -1;
        ff[i] = -1;

        for(j = 0; j < nb; j++) {
            if(bf[j] == 0) {
                temp = b[j] - f[i];

                if(temp >= 0 && temp > highest) {
                    highest = temp;
                    ff[i] = j;
                }
            }
        }

        if(ff[i] != -1) {
            frag[i] = highest;
            bf[ff[i]] = 1;
        }
    }

    printf("\nFile No.\tFile Size\tBlock No.\tBlock Size\tFragment\n");
    for(i = 0; i < nf; i++) {
        if(ff[i] != -1)
            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
                   i+1, f[i], ff[i]+1, b[ff[i]], frag[i]);
        else
            printf("%d\t\t%d\t\tNot Allocated\n", i+1, f[i]);
    }
}

int main() {
    worstFitAllocation();
    return 0;
}
