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
    Str id, point;
    Data_XY VT = NULL, VRT = NULL;
    new_Str(&id);
    new_Str(&point);
    new_Data_XY(&VT);
    new_Data_XY(&VRT);
    (*self)->id = id;
    (*self)->point = point;
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

int Opt_data_print(Opt_data self) {
    printf("id-point\tcell gap\n");
    printf("%s-%s\t%lf\n",self->id, self->point, self->cell_gap);
    return 0;
}

/* Cond methods */
int new_Cond(Cond * self) {
#ifdef DEBUG
    // printf("sizeof cond: %lu\n", sizeof(Cond));
#endif
    if (NULL == (*self = (Cond)calloc(1,sizeof(Cond)))) {
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    // printf("cond calloc ok\n");
#endif
    // Config new_cfg = NULL;
    List new_data = NULL;
    List new_Vw_cellgapRT = NULL, new_Vw_cellgapTon = NULL;
    // new_Config(&new_cfg);
    // printf("new cfg success\n");
    new_List(&new_data);
    new_List(&new_Vw_cellgapRT);
    new_List(&new_Vw_cellgapTon);

    // (* self)->configuration = new_cfg;
    new_Str(&((*self)->desc));
    (* self)->data = new_data;
    (* self)->Vw_cellgapRT = new_Vw_cellgapRT;
    (* self)->Vw_cellgapTon = new_Vw_cellgapTon;

    return 0;
}

int del_Cond(Cond self) {
    // del_Config(self->configuration);
    del_Str(self->desc);
    del_List(self->data);
    del_List(self->Vw_cellgapRT);
    del_List(self->Vw_cellgapTon);
    free(self);
    return 0;
}
bool Cond_comp_has_point(G_PTR A, G_PTR B){
    Opt_data opt_data_A = (Opt_data)A;
    Opt_data opt_data_B = (Opt_data)B;
    return (strncmp(opt_data_A->id, opt_data_B->id,100) == 0) && (strncmp(opt_data_A->point, opt_data_B->point,100) == 0);
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
    printf("Cond: %s, RT = %.2lf when Vw is %.2f at the given ref Ton = %.2lf",self->desc, self->result_RT, self->result_Vw, self->ref_Ton);
    return 0;
}

bool axo_cond_cmp_f(G_PTR ori_val, G_PTR key) {
    if(strncmp(((Cond)ori_val)->desc, key,100) == 0) {
        return true;
    }
    return false;
}

int data_read(List all_cond, Str filename, Input_file_type file_type) {
    FILE * f;
    f = fopen(filename, "r");
    // skip first line
    char buf[1000];
    fgets(buf, 1000, f);

    // then do some parser thing here
    // need to separate later...
    switch (file_type) {
    case Key_file:
        break;
    case Axo_file: 
        {
            Str id_point = NULL, index = NULL, LC = NULL, id = NULL, id_short = NULL, point = NULL, cell_gap = NULL;
            new_Str(&id_point);
            new_Str(&index);
            new_Str(&LC);
            new_Str(&id);
            new_Str(&id_short);
            new_Str(&point);
            new_Str(&cell_gap);
            while(fgets(buf, sizeof(buf), f) != NULL) {
                sscanf(buf, "%s %s %s %s %s %s %s",id_point, index, LC, id, id_short, point, cell_gap);
                #ifdef VERBOSE
                printf("%s\t%s\t%s\t%s\n", LC, id, point, cell_gap);
                #endif
                // create new measure point in each cond
                Opt_data new_data_point = NULL;
                new_Opt_data(&new_data_point);
                strncpy(new_data_point->id, id, 100);
                strncpy(new_data_point->point, point, 100);
                new_data_point->cell_gap = atof(cell_gap);
                #ifdef DEBUG
                printf("new_data:\n");
                Opt_data_print(new_data_point);
                printf("die here?\n");
                #endif
                // find if LC in the list, otherwise create one
                Node tmp_cond_node = NULL;
                tmp_cond_node = List_find(all_cond, LC, axo_cond_cmp_f);
                if (tmp_cond_node) {
                    printf("no such cond\n");
                    List_push(((Cond)(tmp_cond_node->value))->data,new_data_point, "Opt_data");
                } else {
                    Cond new_cond = NULL;
                    new_Cond(&new_cond);
                    strncpy(new_cond->desc, LC, 100);
                    List_push(new_cond->data,new_data_point,"Opt_data");
                    List_push(all_cond, new_cond, "Cond");
                }
            }
            #ifdef DEBUG
            // maybe do a List iteration?
            Node tmp_cond_node = all_cond->head;
            printf("all_cond length: %" PRIu32 "\n", all_cond->length);
            for(uint32_t i = 0; i < all_cond->length; i++) {
                printf("Condition: %s\n:", ((Cond)(tmp_cond_node->value))->desc);
                Node tmp_opt_node = ((Cond)(tmp_cond_node->value))->data->head;
                for(uint32_t j = 0; j < ((Cond)(tmp_cond_node->value))->data->length; j++) {
                    Opt_data tmp_opt_data = tmp_opt_node->value;
                    printf("Opt id-point: %s-%s, %.3lf um\n", tmp_opt_data->id, tmp_opt_data->point, tmp_opt_data->cell_gap);
                    tmp_opt_node = tmp_opt_node->next;
                }
                tmp_cond_node = tmp_cond_node->next;
            }
            #endif
        }
        break;
    case Opt_file:
        {
            Str Data = NULL, M_Time = NULL, id = NULL, point = NULL, Station = NULL, Operator = NULL, Voltage = NULL, Itime = NULL, AR_T_1 = NULL, AR_T_2 = NULL, LCM_X = NULL, LCM_Y = NULL, LCM_Z = NULL, RX = NULL, RY = NULL, RZ = NULL, GX = NULL, GY = NULL, GZ = NULL, BX = NULL, BY = NULL, BZ = NULL, WX = NULL, WY = NULL, WZ = NULL, CG = NULL, R_x = NULL, R_y = NULL, G_x = NULL, G_y = NULL, B_x = NULL, B_y = NULL, W_x = NULL, W_y = NULL, RX_max = NULL, GY_max = NULL, BZ_max = NULL, V_RX_max = NULL, V_GY_max = NULL, V_BZ_max = NULL, WX_ = NULL, WY_ = NULL, WZ_ = NULL, W_x_1 = NULL, W_x_2 = NULL;
            
            new_Str(&Data);
            new_Str(&M_Time);
            new_Str(&id);
            new_Str(&point);
            new_Str(&Station);
            new_Str(&Operator);
            new_Str(&Voltage);
            new_Str(&Itime);
            new_Str(&AR_T_1);
            new_Str(&AR_T_2);
            new_Str(&LCM_X);
            new_Str(&LCM_Y);
            new_Str(&LCM_Z);
            new_Str(&RX);
            new_Str(&RY);
            new_Str(&RZ);
            new_Str(&GX);
            new_Str(&GY);
            new_Str(&GZ);
            new_Str(&BX);
            new_Str(&BY);
            new_Str(&BZ);
            new_Str(&WX);
            new_Str(&WY);
            new_Str(&WZ);
            new_Str(&CG);
            new_Str(&R_x);
            new_Str(&R_y);
            new_Str(&G_x);
            new_Str(&G_y);
            new_Str(&B_x);
            new_Str(&B_y);
            new_Str(&W_x);
            new_Str(&W_y);
            new_Str(&RX_max);
            new_Str(&GY_max);
            new_Str(&BZ_max);
            new_Str(&V_RX_max);
            new_Str(&V_GY_max);
            new_Str(&V_BZ_max);
            new_Str(&WX_);
            new_Str(&WY_);
            new_Str(&WZ_);
            new_Str(&W_x_1);
            new_Str(&W_x_2);
            #ifdef DEBUG
            // printf("string calloc sussess\n");
            #endif
            while(fgets(buf, sizeof(buf), f) != NULL) {
                sscanf(buf, "%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%s", Data, M_Time, id, point, Station, Operator, Voltage, Itime, AR_T_1, AR_T_2, LCM_X, LCM_Y, LCM_Z, RX, RY, RZ, GX, GY, GZ, BX, BY, BZ, WX, WY, WZ, CG, R_x, R_y, G_x, G_y, B_x, B_y, W_x, W_y, RX_max, GY_max, BZ_max, V_RX_max, V_GY_max, V_BZ_max, WX_, WY_, WZ_, W_x_1, W_x_2);
                #ifdef DEBUG
                // printf("parser ok...\n");
                printf("%s-%s %s\n", id, point, LCM_Y);
                #endif
                // create new measure point in each cond
                Opt_data opt_data_point = NULL;
                new_Opt_data(&opt_data_point);
                strncpy(opt_data_point->id, id, 100);
                strncpy(opt_data_point->point, point, 100);
                #ifdef DEBUG
                printf("opt id-point: %s-%s\n", opt_data_point->id, opt_data_point->point);
                #endif
                // find if LC in the list, otherwise create one
                Node tmp_has_opt_cond_node = NULL;
                Node tmp_cond_node = all_cond->head;
                for (uint32_t i = 0; i < all_cond->length; i++) {
                    #ifdef DEBUG
                    printf("Find if there is panel-point in condition %s", ((Cond)(tmp_cond_node->value))->desc);
                    #endif
                    tmp_has_opt_cond_node = List_find(((Cond)(tmp_cond_node->value))->data, opt_data_point, Cond_comp_has_point);
                    if (tmp_has_opt_cond_node) {
                        break;
                    }
                    tmp_cond_node = tmp_cond_node->next;
                }
                #ifdef DEBUG
                if (tmp_has_opt_cond_node == NULL) {
                    printf("something wrong\n");
                    exit(EXIT_FAILURE);
                } else {
                    printf("%p\n", tmp_has_opt_cond_node);
                    printf("cond: %s-%s\n", ((Opt_data)(tmp_has_opt_cond_node->value))->id, ((Opt_data)(tmp_has_opt_cond_node->value))->point);
                }
                #endif 
                del_Opt_data(opt_data_point);
                Data_XY_add(((Opt_data)(tmp_has_opt_cond_node->value))->VT,atof(Voltage), atof(LCM_Y));
            }
        }
        break;
    case RT_file:
        {
            Str data = NULL, time = NULL, id = NULL, point = NULL, station = NULL, operator = NULL, cell_pos = NULL, Target_Vpk = NULL, Initial_Vpk = NULL, OD_Rise = NULL, OD_fall = NULL, Normalized_V = NULL, Specific_target = NULL, Photo = NULL, Sensor = NULL, temp_Sensor = NULL, tempC = NULL, product = NULL, Ton_1090_mean = NULL, Ton_1090_stdev = NULL, Toff_1090_mean = NULL, Toff_1090_stdev = NULL, Ton_0595_mean = NULL, Ton_0595_stdev = NULL, Toff_0595_mean = NULL, Toff_0595_stdev = NULL;
            
            new_Str(&data);
            new_Str(&time);
            new_Str(&id);
            new_Str(&point);
            new_Str(&station);
            new_Str(&operator);
            new_Str(&cell_pos);
            new_Str(&Target_Vpk);
            new_Str(&Initial_Vpk);
            new_Str(&OD_Rise);
            new_Str(&OD_fall);
            new_Str(&Normalized_V);
            new_Str(&Specific_target);
            new_Str(&Photo);
            new_Str(&Sensor);
            new_Str(&temp_Sensor);
            new_Str(&tempC);
            new_Str(&product);
            new_Str(&Ton_1090_mean);
            new_Str(&Ton_1090_stdev);
            new_Str(&Toff_1090_mean);
            new_Str(&Toff_1090_stdev);
            new_Str(&Ton_0595_mean);
            new_Str(&Ton_0595_stdev);
            new_Str(&Toff_0595_mean);
            new_Str(&Toff_0595_stdev);
            
            while(fgets(buf, sizeof(buf), f) != NULL) {
                sscanf(buf, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", data, time, id, point, station, operator, cell_pos, Target_Vpk, Initial_Vpk, OD_Rise, OD_fall, Normalized_V, Specific_target, Photo, Sensor, temp_Sensor, tempC, product, Ton_1090_mean, Ton_1090_stdev, Toff_1090_mean, Toff_1090_stdev, Ton_0595_mean, Ton_0595_stdev, Toff_0595_mean, Toff_0595_stdev);
                // create new measure point in each cond
                Opt_data opt_data_point = NULL;
                new_Opt_data(&opt_data_point);
                strncpy(opt_data_point->id, id, 100);
                strncpy(opt_data_point->point, point, 100);
                // find if LC in the list, otherwise create one
                Node tmp_has_opt_cond_node = NULL;
                Node tmp_cond_node = all_cond->head;
                for (uint32_t i = 0; i < all_cond->length; i++) {
                    tmp_has_opt_cond_node = List_find(((Cond)(tmp_cond_node->value))->data, opt_data_point, Cond_comp_has_point);
                    if (tmp_has_opt_cond_node) {
                        break;
                    }
                }
                #ifdef DEBUG
                if (tmp_has_opt_cond_node == NULL) {
                    printf("something wrong\n");
                } else {
                    printf("%p\n", tmp_has_opt_cond_node);
                    printf("cond: %s-%s\n", ((Opt_data)(tmp_has_opt_cond_node->value))->id, ((Opt_data)(tmp_has_opt_cond_node->value))->point);
                }
                #endif 
                del_Opt_data(opt_data_point);
                Data_XY_add(((Opt_data)(tmp_has_opt_cond_node->value))->VRT,atof(Target_Vpk), atof(Ton_1090_mean)+atof(Toff_1090_mean));
                Data_XY_add(((Opt_data)(tmp_has_opt_cond_node->value))->VTon, atof(Target_Vpk), atof(Ton_1090_mean));
                Data_XY_add(((Opt_data)(tmp_has_opt_cond_node->value))->VToff, atof(Target_Vpk), atof(Toff_1090_mean));
            }
        }
        break;
    default:
        printf("There is no such input file type\n");
        break;
    }
    fclose(f);
    return 0;
}
