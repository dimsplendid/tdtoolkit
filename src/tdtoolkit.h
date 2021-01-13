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
    Str filename;
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
/* Config methods */
int new_Config(Config * self);
int del_Config(Config self);

/* each mesured point */
typedef struct _Opt_Data_at_Each_point {
    Str id;
    Str point;
    double cell_gap;
    Data_XY VT;
    Data_XY VRT, VTon, VToff;
} * Opt_data;
/* Opt_data methods */
int new_Opt_data(Opt_data * self);
int del_Opt_data(Opt_data self);
int Opt_data_print(Opt_data self);

typedef struct _Vw_cellgapRT {
    double Vw;
    Data_XY CellgapRT;
} * Vw_cellgapRT_data;
/* each condtion */
typedef struct _Condtion {
    Str desc;
    // Config configuration;
    List data; // each point opt data
    List Vw_cellgapRT, Vw_cellgapTon;
    double ref_Ton, given_cell_gap;
    double result_Vw, result_RT;
} * Cond;
/* Cond methods */
int new_Cond(Cond * self);
int del_Cond(Cond self);
bool Cond_comp_has_point(G_PTR, G_PTR);
int Cond_data_progress(Cond self);
double Cond_find_Vw_by(Cond self);
double Cond_find_RT_at(Cond self);
/* Condition print option, modified later */
typedef enum _Condition_print_type { OPT } Cond_print_type;
int Cond_print(Cond self, Cond_print_type opt);
/* init all data */
typedef enum _Input_file_type {
    Key_file,
    Axo_file,
    Opt_file,
    RT_file
} Input_file_type;
int data_read(List all_cond, Str filename, Input_file_type type);