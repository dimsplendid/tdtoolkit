/**
 * tool.c
 * 
 * copyright (c) 2021
 * author Wei-Hsiang Tseng
 * email: dimpslendid@gmail.com
 * All right reserved
 */

#include "tool.h"

/* String method */
int new_Str(Str * self) {
    if(NULL == (*self = calloc(100, sizeof(char)))) {
        exit(EXIT_FAILURE);
    }
    return 0;
}
int del_Str(Str self) {
    free(self);
    return 0;
}

/* Result store methods */
int new_F_result(F_result * self, Data_XY_forcast_opt option) {
    if (NULL == (*self = malloc(sizeof(F_result)))) {
        exit(EXIT_FAILURE);
    }
    switch (option) {
    case Linear_interp: {
            F_linear_interp f_linear_interp = malloc(sizeof(F_linear_interp));
            (*self)->linear_interp = f_linear_interp;
        }
        break;
    case Linear_reg: {
            F_linear_reg f_linear_reg = malloc(sizeof(F_linear_reg));
            (*self)->linear_reg = f_linear_reg;
        }
        break;
    default:
        break;
    }
    return 0;
}
int del_F_result(F_result self, Data_XY_forcast_opt option) {
    switch (option) {
    case Linear_interp:
        free(self->linear_interp);
        break;
    case Linear_reg:
        free(self->linear_reg);
        break;
    default:
        break;
    }
    free(self);
    return 0;
}
/* 2-D data methods */
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

int Data_XY_add(Data_XY self, double x, double y) {
    self->X[self->length] = x;
    self->Y[self->length] = y;
    self->length++;
    return 0;
}

int Data_XY_forcast(Data_XY self, double x, Data_XY_forcast_opt opt, F_result result) {
    switch (opt) {
    case Linear_interp: {
        // Consider the code later, just copy...
            double sum_x  = 0.0;
            double sum_x2 = 0.0;
            double sum_xy = 0.0;
            double sum_y  = 0.0;
            double sum_y2 = 0.0;

            for (uint32_t i = 0; i < self->length; i++) {
                sum_x  += self->X[i];
                sum_x2 += self->X[i] * self->X[i];
                sum_xy += self->X[i] * self->Y[i];
                sum_y  += self->Y[i];
                sum_y2 += self->Y[i] * self->Y[i];
            }
            double denom = (self->length * sum_x2 - sum_x * sum_x);
            if (denom == 0) {
                // Singular matrix, can't solve the problem
                result->linear_reg->params[0] = 0;
                result->linear_reg->params[1] = 0;
                result->linear_reg->r = 0;
                result->linear_reg->result = 0;
                return 1;
            }
            result->linear_reg->params[0] = (self->length * sum_xy - sum_x * sum_y) / denom;
            result->linear_reg->params[1] = (sum_y * sum_x2 - sum_x * sum_xy) / denom;
            result->linear_reg->r = (sum_xy - sum_x * sum_y / self->length) / sqrt((sum_x2 - sum_x * sum_x / self->length) * (sum_y2 - sum_y * sum_y / self->length));
            result->linear_reg->result = x * result->linear_reg->params[0] + result->linear_reg->params[1];
            return 0;
        }
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

/* Linked-List Methods */
int new_List(List * self){
    if (NULL == (*self = malloc(sizeof(List)))) {
        exit(EXIT_FAILURE);
    }
    (* self)->head = NULL;
    (* self)->length = 0;
#ifdef DEBUG
    // printf("List OK\n");
#endif
    return 0;
}
int del_List(List self) {
    Node tmp_node;
    while (self->length > 0) {
        tmp_node = List_pop(self);
        free(tmp_node);
    }
    free(self);
    return 0;
}
int List_push(List self, G_PTR value, Str type) {
    Node new_node;
    if (NULL == (new_node = calloc(1, sizeof(List)))) {
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    // printf("node calloc sucess\n");
    #endif
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->value = value;
    new_Str(&(new_node->type));
    strncpy(new_node->type,type,100);
    // go to the last node
    Node tmp_node = self->head;
    if (tmp_node == NULL) {
        tmp_node = new_node;
    } else {
        for(uint32_t i = 0; i < self->length; i++) {
            tmp_node = tmp_node->next;
        }
        tmp_node->next = new_node;
        new_node->prev = tmp_node;
    }
    self->length += 1;
    return 0;
}
Node List_pop(List self) {
    // go to the last node
    Node tmp_node = self->head;
    for(uint32_t i = 0; i < self->length; i++) {
        tmp_node = tmp_node->next;
    }
    tmp_node->prev->next = NULL;
    tmp_node->prev = NULL;
    self->length -= 1;
    return tmp_node;
}

Node List_find(List self, G_PTR key, List_compare_f f) {
    Node tmp_node = self->head;
    if (tmp_node == NULL) {
        return NULL;
    }
    for(uint32_t i = 0; i < self->length; i++) {
        if (f(tmp_node->value, key)) {
            return tmp_node;
        }
        tmp_node = tmp_node->next;
    }
    return NULL;
}
/* aux function */
void rmFileAtrr (char file_name[80]) {
    char *pch = NULL;
    if(NULL != (pch = strrchr(file_name,'.'))) {
        *pch = '\0';
    }
}