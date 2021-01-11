/**
 * tdtoolkit.c
 * 
 * copyright (c) 2021
 * author Wei-Hsiang Tseng
 * email: dimpslendid@gmail.com
 * All right reserved
 */
#include "tdtoolkit.h"

/* Config Methods Implement */
int new_Config(Config * self) {
    if (NULL == (*self = malloc(sizeof(Config)))) {
        exit(EXIT_FAILURE);
    }
    return 0;
}
int del_Config(Config self) {
    free(self)
    return 0;
}
/* Opt_data methods */
int new_Opt_data(Opt_data * self) {
    if (NULL == (*self = malloc(sizeof(Opt_data)))) {
        exit(EXIT_FAILURE);
    }
    Data_XY VT = NULL, VRT = NULL;
    new_Data_XY(&VT);
    new_Data_XY(&VRT);

    (*self)->VT = VT;
    (*self)->VRT = VRT;

    return 0;
}

int del_Opt_data(Opt_data self) {
    free(self->VT);
    free(self->VRT);
    free(self);
    
    return 0;
}


double Cond_find_RT_at(Cond self, double Vw, double cell_gap) {
    // 1. fitting cell_gap - RT for each Vw (linear regression)
    // 2. fitting voltage for certain Ton and cell gap
    // 3. fitting RT for certain voltage
    double RT;

    return RT;
}