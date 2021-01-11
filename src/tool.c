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
        // Consider the code later, just copy...
        double sum_x = 0.0;
        double sum_x2 = 0.0;
        double sum_xy = 0.0;
        double sum_y = 0.0;
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