/**
 * tool.c
 * 
 * copyright (c) 2021
 * author Wei-Hsiang Tseng
 * email: dimpslendid@gmail.com
 * All right reserved
 */

#include "tool.h"

int new_Data_XY(Data_XY * self) {
    if (NULL == (*self = malloc(sizeof(Data_XY)))) {
        exit(EXIT_FAILURE);
    }
    (* self)->length = 0;
    return 0;
}
int del_Data_XY(Data_XY self) {
    free(self);
    return 0;
}

int Data_XY_forcast(Data_XY self, double x, Data_XY_forcast_opt opt, F_result result) {
    switch (opt) {
    case Linear_interp:
        
        return 0;
        break;
    case Linear_reg:

        return 0;
        break;
    default:
        printf("Do Not have this option, please check the parameters\n");
        exit(EXIT_FAILURE);
        break;
    }
}