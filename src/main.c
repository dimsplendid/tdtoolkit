#include "tdtoolkit.h"
#include "tool.h"

#include <stdio.h>

int main(int argc, char ** argv) {
    if ( argc != 3) {
        printf("tdtoolkit cell_gap Ton\n");
        exit(EXIT_FAILURE);
    }
    List all_cond = NULL;
    new_List(&all_cond);
    // read panel condition
    printf("read axo file...\n");
    data_read(all_cond, "AXOCellgap.txt", Axo_file);
    // read OPT data
    printf("read opt file...\n");
    data_read(all_cond, "New-122143.csv", Opt_file);
    data_read(all_cond, "New-152358.csv", Opt_file);
    // read RT data
    printf("read RT file...\n");
    data_read(all_cond, "RT_test.txt", RT_file);
    // data procession
    Node tmp = all_cond->head;
    for (uint32_t i = 0; i < all_cond->length; i++) {
        ((Cond)(tmp->value))->given_cell_gap = atof(argv[1]);
        ((Cond)(tmp->value))->ref_Ton = atof(argv[2]);
        Cond_data_progress((Cond)(tmp->value));
        Cond_find_Vw_by((Cond)(tmp->value));
        Cond_find_RT_at((Cond)(tmp->value));
        tmp = tmp->next;
    }
    // print compare table
    tmp = all_cond->head;
    for (uint32_t i = 0; i < all_cond->length; i++) {
        Cond_print((Cond)(tmp->value), OPT);
    }
    return 0;
}