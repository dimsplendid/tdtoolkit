/**
 * tdtoolkit.h
 * 
 * copyright (c) 2021
 * author Wei-Hsiang Tseng
 * email: dimpslendid@gmail.com
 * All right reserved
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "tool.h"

/* Constant */

/* Data Structure */

/* key file */
typedef struct _TD_toolkit_key {
    /* */

} TD_key;

/* exp configuration */
typedef struct _Configuration_of_panel {
    // cell
    Str LC;
    Str PI;
    Str seal;
    // design
    Str name;
    Str desc;
} * Config;

/* each mesured point */
typedef struct _Opt_Data_at_Each_point {
    Str id;
    Str point;
    double cell_gap;
    Data_XY VT;
    Data_XY VRT;
} * Opt_data;
/* Opt_data methods */

/* each condtion */
typedef struct _Condtion{
    Str desc;
    Config configuration;
    List data;
    double V90, V99; // average of all data
} * Cond;
/* Cond methods */
int new_Cond(Cond * self);
int del_Cond(Cond self);
bool Cond_comp_has_point(G_PTR, G_PTR);
double Cond_data_progress(Cond self);
double Cond_find_RT_at(Cond self, double Vw, double cell_gap);
/* init all data */
int data_read(List all_cond, Str filename, Str type);