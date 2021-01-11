#include "tdtoolkit.h"
#include "tool.h"

#include <stdio.h>

int main(void) {
    List all_cond = NULL;
    new_List(&all_cond);
    // read panel condition
    data_read(all_cond, "AXOCellgap.txt", Axo_file);
    // read OPT data
    data_read(all_cond, "New-122143.csv", Opt_file);
    data_read(all_cond, "New-152358.csv", Opt_file);
    // read RT data
    data_read(all_cond, "RT_test.txt", RT_file);
    // data procession
    Node tmp = all_cond->head;
    for (uint32_t i = 0; i < all_cond->length; i++) {
        Cond_data_progress((Cond)(tmp->value));
        tmp = tmp->next;
    }
    // print compare table
    tmp = all_cond->head;
    for (uint32_t i = 0; i < all_cond->length; i++) {
        Cond_print((Cond)(tmp->value), OPT);
    }
    return 0;
}