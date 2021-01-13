#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tool.h"
#include "tdtoolkit.h"

int main(void) {
    Opt_data A = NULL, B = NULL;
    new_Opt_data(&A);
    new_Opt_data(&B);

    A->id = "TAXXXAT1";
    A->point = "1";
    A->cell_gap = 2.8;

    B->id = "TAXXXAT1";
    B->point = "1";
    // Data_XY_add(B->VT, 0, 0);

    printf("A == B?: %d\n", Cond_comp_has_point(A, B));
    
    return 0;
}