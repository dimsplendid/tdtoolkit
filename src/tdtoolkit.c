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
    free(self);
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
/* Cond methods */
int new_Cond(Cond * self) {
    if (NULL == (*self = malloc(sizeof(Cond)))) {
        exit(EXIT_FAILURE);
    }
    Config new_cfg = NULL;
    List new_data = NULL;
    List new_Vw_cellgapRT = NULL, new_Vw_cellgapTon = NULL;
    new_Config(&new_cfg);
    new_List(&new_data);
    new_List(&new_Vw_cellgapRT);
    new_List(&new_Vw_cellgapTon);

    (* self)->configuration = new_cfg;
    (* self)->data = new_data;
    (* self)->Vw_cellgapRT = new_Vw_cellgapRT;
    (* self)->Vw_cellgapTon = new_Vw_cellgapTon;

    return 0;
}

int del_Cond(Cond self) {
    del_Config(self->configuration);
    del_List(self->data);
    del_List(self->Vw_cellgapRT);
    del_List(self->Vw_cellgapTon);
    free(self);
    return 0;
}
bool Cond_comp_has_point(G_PTR A, G_PTR B){
    Opt_data opt_data_A = (Opt_data)A;
    Opt_data opt_data_B = (Opt_data)B;
    return (strcmp(opt_data_A->id, opt_data_B->id) == 0) && (strcmp(opt_data_A->point, opt_data_B->point) == 0);
}

bool volt_cmp_f(G_PTR Vw_cellgapRT, G_PTR number) {
    // maybe use some kind of Anonymous functions later?
    return (((Vw_cellgapRT_data)Vw_cellgapRT)->Vw ) == (*(double *)number);
}

// 1. fitting cell_gap - RT for each Vw (linear regression)
int Cond_data_progress(Cond self) {
    if(self->data->length < 1) {
        printf("There is no data in \"%s\" condition", self->desc);
        return 1;
    }
    uint32_t count_measure_V = ((Opt_data)(self->data))->VRT->length;
    Node tmp_node = NULL;
    Node tmp_opt_data_node = self->data->head;
    Opt_data tmp_opt_data = NULL;
    for(uint32_t i = 0; i < count_measure_V; i++) {
        for(uint32_t j = 0; j < self->data->length; j++) {
            tmp_opt_data = tmp_opt_data_node->value;
            tmp_node = List_find(self->Vw_cellgapRT, &(tmp_opt_data->VRT->X[i]), volt_cmp_f);
            if (tmp_node == NULL) {
                Vw_cellgapRT_data new_volt_data_RT = NULL, new_volt_data_Ton = NULL;
                if(NULL == (new_volt_data_RT = malloc(sizeof(Vw_cellgapRT_data)))) { exit(EXIT_FAILURE); }
                if(NULL == (new_volt_data_Ton = malloc(sizeof(Vw_cellgapRT_data)))) { exit(EXIT_FAILURE); }
                new_Data_XY(&(new_volt_data_RT->CellgapRT));
                new_Data_XY(&(new_volt_data_Ton->CellgapRT));
                new_volt_data_RT->Vw = tmp_opt_data->VRT->X[i];
                new_volt_data_Ton->Vw = tmp_opt_data->VRT->X[i];
                Data_XY_add(new_volt_data_RT->CellgapRT, tmp_opt_data->cell_gap, tmp_opt_data->VRT->Y[i]);
                Data_XY_add(new_volt_data_Ton->CellgapRT, tmp_opt_data->cell_gap, tmp_opt_data->VTon->Y[i]);
                List_push(self->Vw_cellgapRT,new_volt_data_RT, "Vw_cellgapRT_data");
            } else {
                Data_XY_add(((Vw_cellgapRT_data)(tmp_node->value))->CellgapRT, tmp_opt_data->cell_gap, tmp_opt_data->VRT->Y[i]);
            }
            tmp_opt_data_node = tmp_opt_data_node->next;
        }
    }
    return 0;
}
// 2. fitting voltage for certain Ton and cell gap
double Cond_find_Vw_by(Cond self) {
    double cell_gap = self->given_cell_gap;
    double Ton = self->ref_Ton;
    if (self->Vw_cellgapTon->length < 1) {
        printf("The %s condition do not have Ton data now.\n", self->desc);
    }
    double Vw = 0.0;
    Data_XY Vw_Ton_at_certain_cell_gap = NULL;
    new_Data_XY(&Vw_Ton_at_certain_cell_gap);
    Node tmp_vw_cellgap_Ton_node = self->Vw_cellgapTon->head;
    Vw_cellgapRT_data tmp_cellgap_Ton_data = NULL;
    F_result tmp_linreg_result = NULL;
    new_F_result(&tmp_linreg_result, Linear_reg);
    for(uint32_t i = 0; i < self->Vw_cellgapTon->length; i++) {
        tmp_cellgap_Ton_data = tmp_vw_cellgap_Ton_node->value;
        Data_XY_forcast(tmp_cellgap_Ton_data->CellgapRT, cell_gap, Linear_reg, tmp_linreg_result);
        Data_XY_add(Vw_Ton_at_certain_cell_gap, tmp_linreg_result->linear_reg->result ,tmp_cellgap_Ton_data->Vw); // need filter broken fitting data later
        tmp_vw_cellgap_Ton_node = tmp_vw_cellgap_Ton_node->next;
    }
    Data_XY_forcast(Vw_Ton_at_certain_cell_gap, Ton, Linear_reg, tmp_linreg_result);
    Vw = tmp_linreg_result->linear_reg->result;
    del_F_result(tmp_linreg_result, Linear_reg);
    self->result_Vw = Vw;
    return Vw;
}

// 3. fitting RT for certain voltage
double Cond_find_RT_at(Cond self){
    if (self->Vw_cellgapRT->length < 1) {
        printf("The %s condition do not have RT data now.\n", self->desc);
    }
    double cell_gap = self->given_cell_gap;
    double Vw = self->result_Vw;
    double RT = 0.0;
    Data_XY Vw_RT_at_certain_cell_gap = NULL;
    new_Data_XY(&Vw_RT_at_certain_cell_gap);
    Node tmp_vw_cellgap_RT_node = self->Vw_cellgapRT->head;
    Vw_cellgapRT_data tmp_cellgap_RT_data = NULL;
    F_result tmp_linreg_result = NULL;
    new_F_result(&tmp_linreg_result, Linear_reg);
    for(uint32_t i = 0; i < self->Vw_cellgapTon->length; i++) {
        tmp_cellgap_RT_data = tmp_vw_cellgap_RT_node->value;
        Data_XY_forcast(tmp_cellgap_RT_data->CellgapRT, cell_gap, Linear_reg, tmp_linreg_result);
        Data_XY_add(Vw_RT_at_certain_cell_gap ,tmp_cellgap_RT_data->Vw, tmp_linreg_result->linear_reg->result); // need filter broken fitting data later
        tmp_vw_cellgap_RT_node = tmp_vw_cellgap_RT_node->next;
    }
    Data_XY_forcast(Vw_RT_at_certain_cell_gap, Vw, Linear_reg, tmp_linreg_result);
    RT = tmp_linreg_result->linear_reg->result;
    self->result_RT = RT;
    del_F_result(tmp_linreg_result, Linear_reg);
    return RT;
}


int Cond_print(Cond self, Cond_print_type option) {
    printf("Cond: %s, RT = %.2lf when Vw is %.2f at the given ref Ton = %.2lf",self->desc, self->result_RT, self->result_Vw, ->ref_Ton);
    return 0;
}

int data_read(List all_cond, Str filename, Input_file_type file_type) {
    switch (file_type) {
    case Key_file:
        break;
    case Axo_file: 
        {
            
        }
        break;
    case Opt_file:
        {

        }
        break;
    case RT_file:
        {

        }
        break;
    default:
        printf("There is no such input file\n");
        break;
    }
    return 0;
}
