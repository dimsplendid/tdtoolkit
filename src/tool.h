/**
 * tool.h
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
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* Constant */
#define MAXDATASIZE 1000000

/* Data Structure */

/* fixed-length string type */
typedef struct { char x[100]; } Str; // Maybe a better way?

/* 2-D data */
typedef struct _Data_XY {
    uint32_t length;
    double X[MAXDATASIZE];
    double Y[MAXDATASIZE];
} * Data_XY;

typedef struct _F_linear_interpolation {
    double result;
} * F_linear_interp;

typedef struct _F_linear_regression {
    double result;
    double params[2]; // ax + b
    double r;
} * F_linear_reg;

typedef union _Result_Format {
    F_linear_interp linear_interp;
    F_linear_reg linear_reg;
} * F_result;

typedef enum _Data_XY_forcast_opt {
    Linear_interp,
    Linear_reg
} Data_XY_forcast_opt;

/* Result store methods */
int new_F_result(F_result * self, Data_XY_forcast_opt opt);
int del_F_result(F_result self, Data_XY_forcast_opt opt);
/* 2-D data methods */
int new_Data_XY(Data_XY * self);
int del_Data_XY(Data_XY self);
int Data_XY_add(Data_XY self, double x, double y);

int Data_XY_forcast(Data_XY self, double x, Data_XY_forcast_opt option, F_result result);

/* Linked-list */
typedef struct _Node * Node;
typedef void * G_PTR;
struct _Node {
    Node prev;
    Node next;
    G_PTR value;
    Str type;
};

typedef struct _Linked_List {
    uint32_t length;
    Node head;
} * List;
/* Linked-List Methods */
// List constructor
int new_List(List * self);
// List destructor
int del_List(List self);
int List_push(List self, G_PTR value, char * type);
Node List_pop(List self);

typedef bool (*List_compare_f)(G_PTR origin_node, G_PTR key);
// Return the first node if there is node fitting compare function,
// otherwise return NULL
Node List_find(List self, G_PTR key, List_compare_f f);
/* aux function */
// strick file name at 256 character
void rmfileAtrr(char file_name[256]);

