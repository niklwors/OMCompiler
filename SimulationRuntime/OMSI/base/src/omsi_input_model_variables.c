/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */
#include <omsi_global.h>
#include <omsi_input_model_variables.h>


#define UNUSED(x) (void)(x)     /* ToDo: delete later */



omsi_status omsi_allocate_model_variables(omsi_t*                           omsu,
                                          const omsi_callback_functions*    functions) {

    /* Variables */
    omsi_unsigned_int n_bools, n_ints, n_reals, n_strings;


    /* set global function pointer */
    global_callback = (omsi_callback_functions*) functions;

    /* Log function call */
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Instantiate: Allocates memory for model_variables");

    /*Todo: Allocate memory for all string model variables*/

    /*Allocate memory for all boolean model variables*/
    n_bools = omsu->model_data->n_bool_vars + omsu->model_data->n_bool_parameters;
    if (n_bools > 0) {
        omsu->sim_data->model_vars_and_params->bools = (omsi_bool*)alignedMalloc(sizeof(omsi_bool) * n_bools, 64);
        omsu->sim_data->pre_vars->bools = (omsi_bool*)alignedMalloc(sizeof(omsi_bool) * n_bools, 64);
        omsu->sim_data->model_vars_and_params->n_bools = n_bools;
        omsu->sim_data->pre_vars->n_bools = n_bools;
    }
    else {
        omsu->sim_data->model_vars_and_params->bools = NULL;
        omsu->sim_data->pre_vars->bools = NULL;
        omsu->sim_data->model_vars_and_params->n_bools = 0;
        omsu->sim_data->pre_vars->n_bools = 0;
    }

    /* Allocate memory for all integer model variables */
    n_ints = omsu->model_data->n_int_vars+ omsu->model_data->n_int_parameters;
    if (n_ints > 0) {
        omsu->sim_data->model_vars_and_params->ints = (omsi_int*)alignedMalloc(sizeof(omsi_int) * n_ints, 64);
        omsu->sim_data->pre_vars->ints =  (omsi_int*)alignedMalloc(sizeof(omsi_int) * n_ints, 64);
        omsu->sim_data->model_vars_and_params->n_ints = n_ints;
        omsu->sim_data->pre_vars->n_ints = n_ints;
    }
    else {
        omsu->sim_data->model_vars_and_params->ints = NULL;
        omsu->sim_data->pre_vars->ints = NULL;
        omsu->sim_data->model_vars_and_params->n_ints = 0;
        omsu->sim_data->pre_vars->n_ints = 0;
    }

    /* Allocate memory for all real model variables */
    n_reals =  omsu->model_data->n_states + omsu->model_data->n_derivatives + omsu->model_data->n_real_vars + omsu->model_data->n_real_parameters;
    if (n_reals > 0) {
        omsu->sim_data->model_vars_and_params->reals = (omsi_real*)alignedMalloc(sizeof(omsi_real) * n_reals, 64);
        omsu->sim_data->pre_vars->reals = (omsi_real*)alignedMalloc(sizeof(omsi_real) * n_reals, 64);
        omsu->sim_data->model_vars_and_params->n_reals = n_reals;
        omsu->sim_data->pre_vars->n_reals = n_reals;
    }
    else {
        omsu->sim_data->model_vars_and_params->reals = NULL;
        omsu->sim_data->model_vars_and_params->n_reals = 0;
        omsu->sim_data->pre_vars->reals = NULL;
        omsu->sim_data->pre_vars->n_reals = 0;
    }

    /* ToDo: Allocate memory for all string variables */
    n_strings = omsu->model_data->n_string_vars+ omsu->model_data->n_string_parameters;
    if (n_strings > 0) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: String variables / parameters not supported yet!");
        return omsi_error;
    }
    else {
        omsu->sim_data->model_vars_and_params->strings = NULL;
        omsu->sim_data->pre_vars->strings = NULL;
        omsu->sim_data->model_vars_and_params->strings = 0;
        omsu->sim_data->pre_vars->strings = 0;
    }

    return omsi_ok;
}

omsi_status omsi_initialize_model_variables(omsi_t*                         omsu,
                                            const omsi_callback_functions*  functions,
                                            omsi_string                     instanceName) {

    /* Variables */
    omsi_unsigned_int n;
    omsi_unsigned_int state, derstate;
    omsi_unsigned_int real_algebraic, real_parameter, real_alias;
    omsi_unsigned_int int_algebraic, int_algebraic_2;
    omsi_unsigned_int int_parameter, int_parameter_2, int_alias;
    omsi_unsigned_int bool_parameter, bool_parameter_2;
    omsi_unsigned_int bool_algebraic, bool_algebraic_2;

    UNUSED(functions);
    UNUSED(instanceName);

    if(!model_variables_allocated(omsu, "fmi2Instantiate"))
        return omsi_error;

    if (!omsu->sim_data->model_vars_and_params->reals && omsu->sim_data->model_vars_and_params->n_reals > 0) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Real variables are not yet allocated.");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params->ints && omsu->sim_data->model_vars_and_params->n_ints > 0) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate: Int variables are not yet allocated.");
        return omsi_error;
    }
    if (!omsu->sim_data->model_vars_and_params->bools  && omsu->sim_data->model_vars_and_params->n_bools > 0) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2Instantiate:  Bool variables are not yet allocated.");
        return omsi_error;
    }

    /*Initialize state variables from init xml file values*/
    n = omsu->model_data->n_states;
    for (state = 0; state < n; ++state) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[state].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[state] = attr->start;
    }

    /*Initialize derivatives variables from init xml file values*/
    n = n + omsu->model_data->n_derivatives;
    for (derstate = state; derstate < n; ++derstate) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[derstate].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[derstate] = attr->start;
    }

    /*Initialize real algebraic variables from init xml file values*/
    n = n + omsu->model_data->n_real_vars;
    for (real_algebraic = derstate; real_algebraic < n; ++real_algebraic) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[real_algebraic].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[real_algebraic] = attr->start;
    }

    /*Initialize real parameter variables from init xml file values*/
    n = n + omsu->model_data->n_real_parameters;
    for (real_parameter = real_algebraic; real_parameter < n; ++real_parameter) {
        real_var_attribute_t* attr = (real_var_attribute_t*)(omsu->model_data->model_vars_info[real_parameter].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->reals[real_parameter] = attr->start;
    }

    /*real alias variables are not extra included in real vars memory,therefore they are skipped*/
    real_alias = omsu->model_data->n_real_aliases;
    n = n + real_alias;

    /*Initialize int algebraic variables from init xml file values*/
    n = n + omsu->model_data->n_int_vars;
    for (int_algebraic = real_parameter+ real_alias, int_algebraic_2=0; int_algebraic < n; ++int_algebraic, ++int_algebraic_2) {
        int_var_attribute_t* attr = (int_var_attribute_t*)(omsu->model_data->model_vars_info[int_algebraic].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->ints[int_algebraic_2] = attr->start;
    }

    /*Initialize int parameter from init xml file values*/
    n = n + omsu->model_data->n_int_parameters;
    for (int_parameter = int_algebraic, int_parameter_2 = int_algebraic_2; int_parameter < n; ++int_parameter, ++int_parameter_2) {
        int_var_attribute_t* attr = (int_var_attribute_t*)(omsu->model_data->model_vars_info[int_parameter].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->ints[int_parameter_2] = attr->start;
    }

    /*int alias variables are not extra included in int vars memory,therefore they are skipped*/
    int_alias = omsu->model_data->n_int_aliases;
    n = n + int_alias;

    /*Initialize bool algebraic variables from init xml file values*/
    n = n + omsu->model_data->n_bool_vars;
    for (bool_algebraic = int_parameter+ int_alias, bool_algebraic_2 = 0; bool_algebraic < n; ++bool_algebraic, ++bool_algebraic_2) {
        bool_var_attribute_t* attr = (bool_var_attribute_t*)(omsu->model_data->model_vars_info[bool_algebraic].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->bools[bool_algebraic_2] = attr->start;
    }

    /*Initialize bool parameter  from init xml file values*/
    n = n + omsu->model_data->n_bool_parameters;
    for (bool_parameter = bool_algebraic, bool_parameter_2 = bool_algebraic_2; bool_parameter < n; ++bool_parameter, ++bool_parameter_2) {
        bool_var_attribute_t* attr = (bool_var_attribute_t*)(omsu->model_data->model_vars_info[bool_parameter].modelica_attributes);
        if (!attr) {
            filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                    "fmi2Instantiate:  could not read start value attribute.");
            return omsi_error;
        }
        omsu->sim_data->model_vars_and_params->bools[bool_parameter_2] = attr->start;
    }

    /*Todo: Initialize string algebraic variables  from init xml file values*/

    return omsi_ok;
}

omsi_status omsi_free_model_variables(sim_data_t* sim_data) {

    /* Check input */
    if (!sim_data) {
            return omsi_warning;
        }

    if (sim_data->model_vars_and_params) {
        if (sim_data->model_vars_and_params->bools) {
            alignedFree(sim_data->model_vars_and_params->bools);
        }
        if (sim_data->model_vars_and_params->ints) {
            alignedFree(sim_data->model_vars_and_params->ints);
        }
        if (sim_data->model_vars_and_params->reals) {
                alignedFree(sim_data->model_vars_and_params->reals);
        }

        global_callback->freeMemory(sim_data->model_vars_and_params);
    }

    if (sim_data->pre_vars) {
        if (sim_data->pre_vars->bools) {
            alignedFree(sim_data->pre_vars->bools);
        }
        if (sim_data->pre_vars->ints) {
            alignedFree(sim_data->pre_vars->ints);
        }
        if (sim_data->pre_vars->reals) {
        alignedFree(sim_data->pre_vars->reals);
        }

        global_callback->freeMemory(sim_data->pre_vars);
    }

    return omsi_ok;
}


void* alignedMalloc(size_t required_bytes,
                    size_t alignment)        /* ToDo: change size_t to some omsi type */
{
    void *p1;
    void **p2;

    omsi_int offset = alignment - 1 + sizeof(void*);
    p1 = global_callback->allocateMemory(1,required_bytes + offset);
    p2 = (void**)(((size_t)(p1)+offset)&~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void alignedFree(void* p)
{
    void* p1 = ((void**)p)[-1];         /* get the pointer to the buffer we allocated */
    global_callback->freeMemory(p1);
}


/*
 * ============================================================================
 * Getters
 * ============================================================================
 */

omsi_status omsi_get_boolean(omsi_t*                    omsu,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             omsi_bool                  value[]){

    /* Variables */
    omsi_unsigned_int i;
    omsi_unsigned_int n_prev_model_vars;
    omsi_int index;

    if (!model_variables_allocated(omsu, "fmi2GetBoolean")) {
        return omsi_error;
    }

    if (nvr > 0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetBoolean: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr > 0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetBoolean: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    /* Get bool values */
    for (i = 0; i < nvr; i++){
        /* Check for negated alias */
        n_prev_model_vars = omsu->model_data->n_states +omsu->model_data->n_derivatives + omsu->model_data->n_real_vars + omsu->model_data->n_real_parameters + omsu->model_data->n_real_aliases
                          + omsu->model_data->n_int_vars + omsu->model_data->n_int_parameters + omsu->model_data->n_int_aliases;
        index = omsi_get_negated_index(&omsu->model_data->model_vars_info[vr[i]+n_prev_model_vars], vr[i]);

        if (index < 0) {
            if (omsi_vr_out_of_range(omsu, "fmi2GetBoolean", -index, omsu->sim_data->model_vars_and_params->n_bools)) {
                return omsi_error;
            }
            value[i] =getBoolean(omsu, -index);
        } else {
            if (omsi_vr_out_of_range(omsu, "fmi2GetBoolean", index, omsu->sim_data->model_vars_and_params->n_bools)) {
                return omsi_error;
            }
            value[i] =getBoolean(omsu, index);
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2GetBoolean: #b%u# = %s", vr[i], value[i] ? "true" : "false");
    }
    return omsi_ok;
}

omsi_status omsi_get_integer(omsi_t*                     omsu,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             omsi_int                   value[]){

    /* Variables */
    omsi_unsigned_int i;
    omsi_unsigned_int n_prev_model_vars;
    omsi_int index;

    if (!model_variables_allocated(omsu, "fmi2GetInteger")) {
        return omsi_error;
    }

    if (nvr > 0 &&  vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetInteger: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr > 0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetInteger: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    /* Get integers */
    for (i = 0; i < nvr; i++) {
        /* Check for negated alias */
        n_prev_model_vars = omsu->model_data->n_states +omsu->model_data->n_derivatives + omsu->model_data->n_real_vars + omsu->model_data->n_real_parameters + omsu->model_data->n_real_aliases;
        index = omsi_get_negated_index(&omsu->model_data->model_vars_info[vr[i]+n_prev_model_vars], vr[i]);

        if (index < 0) {
            if (omsi_vr_out_of_range(omsu, "fmi2GetInteger", -index, omsu->sim_data->model_vars_and_params->n_ints)) {
                return omsi_error;
            }
            value[i] =getInteger(omsu, -index);
        } else {
            if (omsi_vr_out_of_range(omsu, "fmi2GetInteger", index, omsu->sim_data->model_vars_and_params->n_ints)) {
                return omsi_error;
            }
            value[i] =getInteger(omsu, index);
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2GetInteger: #i%u# = %d", vr[i], value[i]);
    }
    return omsi_ok;
}

omsi_status omsi_get_real(omsi_t*                    omsu,
                          const omsi_unsigned_int   vr[],
                          omsi_unsigned_int         nvr,
                          omsi_real                 value[]){

    /* Variables */
    omsi_unsigned_int i;
    omsi_int index;

    if (!model_variables_allocated(omsu, "fmi2GetReal")) {
        return omsi_error;
    }

    if (nvr > 0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetReal: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr > 0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetReal: Invalid argument value[] = NULL.");
      return omsi_error;
    }

    /* Get reals */
    for (i = 0; i < nvr; i++) {
        /* Check for negated alias */
        index = omsi_get_negated_index(&omsu->model_data->model_vars_info[vr[i]], vr[i]);

        if (index < 0) {
            if (omsi_vr_out_of_range(omsu, "fmi2GetReal", -index, omsu->sim_data->model_vars_and_params->n_reals)) {
                return omsi_error;
            }
            value[i] =getReal(omsu, -index);
        } else {
            if (omsi_vr_out_of_range(omsu, "fmi2GetReal", index, omsu->sim_data->model_vars_and_params->n_reals)) {
                return omsi_error;
            }
            value[i] =getReal(omsu, index);
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2GetReal: vr = %i, value = %f", vr[i], value[i]);
    }
    return omsi_ok;
}

omsi_status omsi_get_string(omsi_t*                  omsu,
                            const omsi_unsigned_int vr[],
                            omsi_unsigned_int       nvr,
                            omsi_string             value[]){

    /* Variables */
    omsi_unsigned_int i;
    omsi_unsigned_int n_prev_model_vars;
    omsi_int index;

    if (!model_variables_allocated(omsu, "fmi2GetString")) {
        return omsi_error;
    }

    if (nvr>0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetString: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr>0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2GetString: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    for (i = 0; i < nvr; i++) {
        /* Check for negated alias */
        n_prev_model_vars = omsu->model_data->n_states +omsu->model_data->n_derivatives + omsu->model_data->n_real_vars + omsu->model_data->n_real_parameters + omsu->model_data->n_real_aliases
                          + omsu->model_data->n_int_vars + omsu->model_data->n_int_parameters + omsu->model_data->n_int_aliases
                          + omsu->model_data->n_bool_vars + omsu->model_data->n_bool_parameters + omsu->model_data->n_bool_aliases;
        index = omsi_get_negated_index(&omsu->model_data->model_vars_info[vr[i]+n_prev_model_vars], vr[i]);

        if (index < 0) {
            if (omsi_vr_out_of_range(omsu, "fmi2GetString", -index, omsu->sim_data->model_vars_and_params->n_strings)) {
                return omsi_error;
            }
            value[i] =getString(omsu, -index);
        } else {
            if (omsi_vr_out_of_range(omsu, "fmi2GetString", index, omsu->sim_data->model_vars_and_params->n_strings)) {
                return omsi_error;
            }
            value[i] =getString(omsu, index);
        }
        if (omsi_vr_out_of_range(omsu, "fmi2GetString", vr[i], omsu->sim_data->model_vars_and_params->n_strings)) {
            return omsi_error;
        }
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2GetString: #s%u# = '%s'", vr[i], value[i]);
    }
    return omsi_ok;
}

/*
 * ============================================================================
 * Setters
 * ============================================================================
 */

/* ToDo: Include code for negated aliases */
omsi_status omsi_set_boolean(omsi_t*                    omsu,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             const omsi_bool            value[]) {

    /* Variables */
    omsi_unsigned_int i;

    if (!model_variables_allocated(omsu, "fmi2SetBoolean"))
        return omsi_error;

    if (nvr>0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetBoolean: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr>0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetBoolean: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2SetBoolean: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (omsi_vr_out_of_range(omsu, "fmi2SetBoolean", vr[i], omsu->sim_data->model_vars_and_params->n_bools))
            return omsi_error;
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetBoolean: #b%d# = %s", vr[i], value[i] ? "true" : "false");

        setBoolean(omsu, vr[i], value[i]);
    }

    return omsi_ok;
}

omsi_status omsi_set_integer(omsi_t*                    omsu,
                             const omsi_unsigned_int    vr[],
                             omsi_unsigned_int          nvr,
                             const omsi_int             value[]) {

    /* Variables */
    omsi_unsigned_int i;

    if (!model_variables_allocated(omsu, "fmi2SetInteger"))
        return omsi_error;

    if (nvr>0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetInteger: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr>0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetInteger: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2SetInteger: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (omsi_vr_out_of_range(omsu, "fmi2SetInteger", vr[i], omsu->sim_data->model_vars_and_params->n_ints))
            return omsi_error;
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetInteger: #i%d# = %d", vr[i], value[i]);
        setInteger(omsu, vr[i], value[i]);
    }

    return omsi_ok;
}

omsi_status omsi_set_real(omsi_t*                   omsu,
                          const omsi_unsigned_int   vr[],
                          omsi_unsigned_int         nvr,
                          const omsi_real           value[]) {

    /* Variables */
    omsi_unsigned_int i;

    if (!model_variables_allocated(omsu, "fmi2SetReal"))
        return omsi_error;

    if (nvr>0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetReal: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr>0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetReal: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2SetReal: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (omsi_vr_out_of_range(omsu, "fmi2SetReal", vr[i], omsu->sim_data->model_vars_and_params->n_reals))
            return omsi_error;
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetReal: #r%d# = %.16g", vr[i], value[i]);
        setReal(omsu, vr[i], value[i]);
    }

    return omsi_ok;
}

omsi_status omsi_set_string(omsi_t*                  omsu,
                            const omsi_unsigned_int vr[],
                            omsi_unsigned_int                  nvr,
                            const omsi_string       value[]) {

    /* Variables */
    omsi_unsigned_int i;

    if (!model_variables_allocated(omsu, "fmi2SetString"))
        return omsi_error;

    if (nvr>0 && vr==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetString: Invalid argument vr[] = NULL.");
        return omsi_error;
    }
    if (nvr>0 && value==NULL) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2SetString: Invalid argument value[] = NULL.");
        return omsi_error;
    }

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
        "fmi2SetString: nvr = %d", nvr);

    for (i = 0; i < nvr; i++) {
        if (omsi_vr_out_of_range(omsu, "fmi2SetString", vr[i], omsu->sim_data->model_vars_and_params->n_strings))
            return omsi_error;
        filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2SetString: #s%d# = '%s'", vr[i], value[i]);

        setString(omsu, vr[i], value[i]);
    }

    return omsi_ok;
}


omsi_bool model_variables_allocated(omsi_t*     omsu,
                                    omsi_string functionName) {

    UNUSED(omsu);
    UNUSED(functionName);
#if 0
    /* Check inputs */
    if (!omsu->model_data) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "%s: No model data available.", functionName);
        return omsi_false;
    }
    if (!omsu->model_data->model_vars_info) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "%s: No model vars info available.", functionName);
        return omsi_false;
    }
    if (!omsu->sim_data->model_vars_and_params) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
            "%s: No model vars and parameter structure is not yet allocated.", functionName);
        return omsi_false;
    }
#endif
    return omsi_true;
}



/*
 * ============================================================================
 * Helper functions for getters and setters
 * ============================================================================
 */

/* What happens for alias variables for getters and setters? */

/*
 * Gets real number of struct OSU with value reference vr.
 */
omsi_real getReal (omsi_t*                  osu_data,
                   const omsi_unsigned_int  vr) {

    omsi_real output = osu_data->sim_data->model_vars_and_params->reals[vr];
    return output;
}


/*
 * Sets real number of struct OSU for index reference vr with value
*/
omsi_status setReal(omsi_t*                 osu_data,
                    const omsi_unsigned_int vr,
                    const omsi_real         value) {

    osu_data->sim_data->model_vars_and_params->reals[vr] = value;
    return omsi_ok;
}


/*
 * Gets integer number of struct OSU with value reference vr
*/
omsi_int getInteger (omsi_t*                    osu_data,
                     const omsi_unsigned_int    vr) {

    /* Variables */
    omsi_int output;

    /*index = vr - osu_data->sim_data->model_vars_and_params->n_reals;*/
    output = osu_data->sim_data->model_vars_and_params->ints[vr];
   return output;
}


/*
 * Sets integer number of struct OSU for index reference vr with value
 */
omsi_status setInteger(omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr,
                       const omsi_int           value) {

    /* index = vr - osu_data->sim_data->model_vars_and_params->n_reals; */
    osu_data->sim_data->model_vars_and_params->ints[vr] = value;
    return omsi_ok;
}


/*
 * gets boolean variable of struct OSU with value reference vr
 */
omsi_bool getBoolean (omsi_t*                  osu_data,
                      const omsi_unsigned_int   vr) {
    /* Variables */
    omsi_bool output;

   /*index = vr - osu_data->sim_data->model_vars_and_params->n_reals
               - osu_data->sim_data->model_vars_and_params->n_ints; */
    output = osu_data->sim_data->model_vars_and_params->bools[vr];
    return output;
}


/*
 * sets boolean variable of struct OSU for index reference vr with value
 */
omsi_status setBoolean(omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr,
                       const omsi_bool          value) {

    /* index = vr - osu_data->sim_data->model_vars_and_params->n_reals
               - osu_data->sim_data->model_vars_and_params->n_ints; */
    osu_data->sim_data->model_vars_and_params->bools[vr] = value;
    return omsi_ok;
}

/*
 * gets string of struct OSU with value reference vr
*/
omsi_string getString (omsi_t*                  osu_data,
                       const omsi_unsigned_int  vr) {

    /* Variables */
    omsi_string output;

    output = osu_data->sim_data->model_vars_and_params->strings[vr];
    return output;
}


/*
 * sets string of struct OSU for index reference vr with value
 */
omsi_status setString(omsi_t*                  osu_data,
                      const omsi_unsigned_int   vr,
                      const omsi_string         value) {

    osu_data->sim_data->model_vars_and_params->strings[vr] = value;
    return omsi_error;
}

