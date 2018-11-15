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

/** \file omsi.h
 */

/** \defgroup OMSI OpenModelica Simulation Interface
 *
 * Long description of OMSI group.
 */



/** @addtogroup OMSIBase OMSI Base Library
  * \ingroup OMSI
  *
  * Detailed OMSIBase description here!
  *  @{ */



#ifndef _OMSI_H
#define _OMSI_H

#include <limits.h>
#include <float.h>
#include <omsi_solver.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * type definitions of variables
 */
#ifdef OSI_FMI2_WRAPPER_H
#include <fmi2Functions.h>     /* ToDo: delete, is duplicate but silences warnings in my eclipse :-P */

typedef fmi2ValueReference  omsi_unsigned_int;
typedef fmi2Real            omsi_real;
typedef fmi2Integer         omsi_int;
typedef fmi2Integer         omsi_long;
typedef fmi2Boolean         omsi_bool;
#define omsi_true  fmi2True
#define omsi_false fmi2False
#ifndef true
#define true fmi2True
#endif
#ifndef false
#define false fmi2False
#endif
typedef fmi2Char            omsi_char;
typedef fmi2String          omsi_string;
#else
typedef unsigned int        omsi_unsigned_int;
typedef double              omsi_real;
typedef int                 omsi_int;
typedef long                omsi_long;
typedef int                 omsi_bool;
#define omsi_true  1
#define omsi_false 0
typedef char                omsi_char;
typedef const omsi_char*    omsi_string;
#endif

#define OMSI_TYPES_DEFINED

/* macros */
#define OMSI_DBL_MAX DBL_MAX
#define OMSI_INT_MAX INT_MAX

/* used for filtered_log */
#define NUMBER_OF_CATEGORIES 12
typedef enum {
    log_events,
    log_singulalinearsystems,
    log_nonlinearsystems,
    log_linearsystems,
    log_dynamicstateselection,
    log_statuswarning,
    log_statusdiscard,
    log_statuserror,
    log_statusfatal,
    log_statuspending,
    log_all,
    log_fmi2_call
} log_categories;


static const omsi_string log_categories_names[NUMBER_OF_CATEGORIES] = {
    "logEvents",
    "logSingularLinearSystems",
    "logNonlinearSystems",
    "logLinearSystems",
    "logDynamicStateSelection",
    "logStatusWarning",
    "logStatusDiscard",
    "logStatusError",
    "logStatusFatal",
    "logStatusPending",
    "logAll",
    "logFmi2Call"
};


/* Model FMU/ OSU states */
typedef enum {
  modelInstantiated       = 1<<0, /* ME and CS */
  modelInitializationMode = 1<<1, /* ME and CS */
  modelContinuousTimeMode = 1<<2, /* ME only */
  modelEventMode          = 1<<3, /* ME only */
  modelSlaveInitialized   = 1<<4, /* CS only */
  modelTerminated         = 1<<5, /* ME and CS */
  modelError              = 1<<6  /* ME and CS */
} ModelState;

/* Event informations */
typedef struct {
   omsi_bool newDiscreteStatesNeeded;
   omsi_bool terminateSimulation;
   omsi_bool nominalsOfContinuousStatesChanged;
   omsi_bool valuesOfContinuousStatesChanged;
   omsi_bool nextEventTimeDefined;
   omsi_real nextEventTime;
} omsi_event_info;


/* forward some structs */
struct omsi_function_t;


/*
 * variable basic data type
 */
typedef enum {
    OMSI_TYPE_UNKNOWN,
    OMSI_TYPE_REAL,
    OMSI_TYPE_INTEGER,
    OMSI_TYPE_BOOLEAN,
    OMSI_TYPE_STRING
}omsi_data_type;

static const omsi_string omsiDataTypesNames[] = {
    "Unknown",
    "Real",
    "Integer",
    "Boolean",
    "String"};


/* ToDo: is this the right location for these definitions? */
typedef enum {
    omsi_model_exchange,
    omsi_co_simulation      /* not supported yet */
}omsu_type;


/*
 * OpenModelic Simulation Unit (OMSU) solving mode
 */
typedef enum {
    omsi_instantiated_mode,
    omsi_initialization_mode,
    omsi_continuousTime_mode,
    omsi_event_mode,
    omsi_none_mode
} omsi_solving_mode_t;


/*
 * Actual status of OpenModelica Simulation Unit (OMSU)
 */
typedef enum {
    omsi_ok,
    omsi_warning,
    omsi_discard,
    omsi_error,
    omsi_fatal,
    omsi_pending
}omsi_status;


/*
 * ============================================================================
 * Definitions for simulation data
 * ============================================================================
 */
typedef struct omsi_index_type {
  omsi_data_type        type;    /* data type*/
  omsi_unsigned_int     index;   /* index in sim_data->model_vars_and_params->[datatype]
                                  * where [datatype]=reals|ints|bools depending on type */
} omsi_index_type;


/*
 * Struct of arrays containing actual values for Variables, Parameters,
 * Aliases and so on for associating time value.
 * Also containing number of containing reals, ints and bools.
 */
typedef struct omsi_values {
    omsi_real*      reals;      /* array of omsi_real */
    omsi_int*       ints;       /* array of omsi_int */
    omsi_bool*      bools;      /* array of omsi_bool */
    omsi_string*    strings;    /* array of omsi_string */
    void*           externs;    /* array of pointer to extern objects */
    omsi_real       time_value; /* current system time */

    omsi_unsigned_int   n_reals;    /* length of array reals */
    omsi_unsigned_int   n_ints;     /* length of array ints */
    omsi_unsigned_int   n_bools;    /* length of array bools */
    omsi_unsigned_int   n_strings;  /* length of array strings */
    omsi_unsigned_int   n_externs;  /* length of array externs */
} omsi_values;


/** \brief General algebraic system.
 *
 *  Struct containing information for one linear or non-linear algebraic system
 *  and solver_data to solve the described equation system.
 *
 *  Linear case: A*x=b
 *
 *  Non-linear case: f(x)=0
 */
typedef struct omsi_algebraic_system_t {
    omsi_unsigned_int id;               /**< Unique identification for algebraic system. */

    omsi_unsigned_int n_iteration_vars; /**< Number of iteration variables. */

    omsi_unsigned_int n_conditions;     /**< Number of zerocrossing conditions. */
    omsi_int* zerocrossing_indices;     /**< Array of zerocrossing indices of size `n_conditions`. */

    omsi_bool isLinear;                 /**< Describes if algebraic system is linear.
                                          *    * `isLinear=true`: algebraic system is linear
                                          *    * `isLinear=false`: algebraic system is linear. */
    struct omsi_function_t* jacobian;   /**< Pointer to `omsi_function_t` to describe
                                          *    * linear case: jacobian describes matrix A
                                          *    * non-linear case: jacobian describes f' */

    struct omsi_function_t* functions;  /**< Pointer to omsi_function for residual function. */
    solver_data* solver_data;           /**< Pointer to solver instance. */
}omsi_algebraic_system_t;


/** \brief Structure containing all data to evaluate equations or system of equations.
 *
 * For example the initialization system would be one `omsi_function_t` containing all
 * potential linear and non-linear loops, which include in turn `omsi_function_t` structs
 * for residual functions and Jacobi matrix.
 *
 */
typedef struct omsi_function_t {
    omsi_unsigned_int           n_algebraic_system; /**< Number of algebraic systems. */
    omsi_algebraic_system_t*    algebraic_system_t; /**< Array of algebraic systems. */

    omsi_values* function_vars;                     /**< Pointer to variables and parameters.
                                                     *   Either local copy of needed variables for evaluation or
                                                     *   pointer to next higher struct with `omsi_values` array. */

    omsi_values* local_vars;                        /**< Pointer to local variables like seed variables and
                                                     *   and dummy derivative variables. */

    /** evaluate function
     * \param [in,out] this_function                Pointer to this `omsi_fucntion_t` struct.
     * \param [in]     read_only_vars_and_params    Pointer to read only struct with variables and parameters.
     *                                              from next higher struct with `omsi_values` array.
     * \param [in]     data                         Pointer to optional data for evaluated function. Can be `NULL`.
     *
     * Points to function in generated code.
     */
    omsi_status (*evaluate) (struct omsi_function_t*    this_function,
                             const omsi_values*         read_only_vars_and_params,
                             void*                      data);
    omsi_index_type* input_vars_indices;    /* index to next higher omsi_values pointer */
    omsi_index_type* output_vars_indices;   /* e.g to sim_data_t->model_vars_and_params */

    omsi_unsigned_int n_input_vars;         /* number of input variables */
    omsi_unsigned_int n_inner_vars;         /* number of inner variables */
    omsi_unsigned_int n_output_vars;        /* number of output variables */
} omsi_function_t;


/** \brief Structure containing all dynamic simulation data.
 *  Detailed Description
 */
typedef struct sim_data_t{
    omsi_function_t* initialization;    /**< Pointer to omsi_function_t struct
                                         * necessary for initialization. */
    omsi_function_t* simulation;        /**< Pointer to omsi_function_t struct
                                         * necessary for simulation. */

    omsi_values* model_vars_and_params; /**< Pointer to struct of arrays containing
                                         * values for all variables, parameters and so on. */

    omsi_values* pre_vars;              /**< Pointer to all pre variables. */
    omsi_index_type* pre_vars_mapping;  /**< Mapping pre_vars to corresponding
                                         * variables in model_vars_and_params. */

    omsi_bool* zerocrossings_vars;      /**< Conditions of zerocrossing functions. */
    omsi_bool* pre_zerocrossings_vars;  /**< Pre conditions of zerocrossing functions. */

    /* start indices to model_vars_and_params */
    omsi_unsigned_int inputs_real_index;    /*start index of input real variables */
    omsi_unsigned_int inputs_int_index;     /*start index of input integer variables */
    omsi_unsigned_int inputs_bool_index;    /*start index of input boolean variables */
    omsi_unsigned_int outputs_real_index;   /*start index of output real variables */
    omsi_unsigned_int outputs_int_index;    /*start index of output integer variables */
    omsi_unsigned_int outputs_bool_index;   /*start index of output boolean variables */

} sim_data_t;


/*
 * ============================================================================
 * Definitions for model informations
 * ============================================================================
 */
/*
 *   additional file information for debugging
 */
typedef struct file_info {
    omsi_string filename;       /* filename where variable is defined */
    omsi_int    lineStart;      /* number of line where definition of variable starts */
    omsi_int    colStart;       /* number of columns where definition of variable starts */
    omsi_int    lineEnd;        /* number of line where definition of variable ends */
    omsi_int    colEnd;         /* number of columns where definition of variable ends */
    omsi_bool   fileWritable;   /* =true if file writable, else =false */
} file_info;


/*
 *   additional equation information for debugging
 */
typedef struct equation_info_t{
    omsi_int        id;                 /* unique equation reference from  info.json */
    omsi_int        profileBlockIndex;
    omsi_int        parent;
    omsi_int        numVar;             /* number of defining variables */
    omsi_string*    variables;          /* array of unknown variables */
    file_info       info;               /* file informations for  equation */
} equation_info_t;


/*
 *  Modelica attributes for real variables
 */
typedef struct real_var_attribute_t {
    omsi_string unit;          /* default = "" */
    omsi_string displayUnit;   /* default = "" */
    omsi_real   min;           /* default = -Inf */
    omsi_real   max;           /* default = +Inf */
    omsi_bool   fixed;         /* depends on the type */
    omsi_real   nominal;       /* default = 1.0 */
    omsi_real   start;         /* default  = 0.0 */
} real_var_attribute_t;


/*
 *  Modelica attributes for integer variables
 */
typedef struct int_var_attribute_t {
    omsi_int  min;      /* = -Inf */
    omsi_int  max;      /* = +Inf */
    omsi_bool fixed;    /* depends on the type */
    omsi_int  start;    /* = 0 */
} int_var_attribute_t;


/*
 *  Modelica attributes for boolean variables
 */
typedef struct bool_var_attribute_t {
    omsi_bool fixed;    /* depends on the type */
    omsi_bool start;    /* = false */
} bool_var_attribute_t;


/*
 * Modelica attributes for string variables
 */
typedef struct string_var_attribute_t {
    omsi_char * start;  /* = "" */
} string_var_attribute_t;


/*
 * Informations for single variable or parameter
 */
typedef struct model_variable_info_t {
    omsi_int        id;                     /* unique value reference from *_init.xml */
    omsi_string     name;                   /* name of variable|parameter|alias */
    omsi_string     comment;                /* variable description  or modelica comment*/
    omsi_index_type type_index;             /* tuple of data_type and index in sim_data->model_vars_and_params->..., if isAlias=true then index from alias variable */
    void*           modelica_attributes;    /* pointer to modelica attributes  ( real_var_attribute | int_var_attribute | bool_var_attribute | string_var_attribute ) */
    omsi_bool       isAlias;                /* true if alias, else false */
    omsi_int        negate;                 /* if negated -1 else 1 */
    omsi_int        aliasID;                /* pointer to alias if >= 0 */
    file_info       info;                   /* file informations for variable|parameter|alias */
} model_variable_info_t;


/** \brief Structure containing all static simulation informations.
 *
 *  Used for debugging features.
 */
typedef struct model_data_t {
    omsi_string         modelGUID;              /**< Model GUID. */
    omsi_unsigned_int   n_states;               /**< Number of continuous states. */
    omsi_unsigned_int   n_derivatives;          /**< Number of derivatives. */
    omsi_unsigned_int   n_real_vars;            /**< Number of real algebraic variables. */
    omsi_unsigned_int   n_real_parameters;      /**< Number of real parameters. */
    omsi_unsigned_int   n_real_aliases;         /**< Number of real alias variables. */
    omsi_unsigned_int   n_int_vars;             /**< Number of integer algebraic variables. */
    omsi_unsigned_int   n_int_parameters;       /**< Number of integer parameters. */
    omsi_unsigned_int   n_int_aliases;          /**< Number of integer alias variables. */
    omsi_unsigned_int   n_bool_vars;            /**< Number of boolean algebraic variables. */
    omsi_unsigned_int   n_bool_parameters;      /**< Number of boolean parameters. */
    omsi_unsigned_int   n_bool_aliases;         /**< Number of boolean alias variables. */
    omsi_unsigned_int   n_string_vars;          /**< Number of string algebraic variables. */
    omsi_unsigned_int   n_string_parameters;    /**< Number of string parameters. */
    omsi_unsigned_int   n_string_aliases;       /**< Number of string alias variables. */
    omsi_unsigned_int   n_zerocrossings;        /**< Number of zero crossings. */
    omsi_unsigned_int   n_equations;            /**< Number of all equations. */
    omsi_unsigned_int   n_init_equations;       /**< Number of initial equations. */
    omsi_unsigned_int   n_regular_equations;    /**< Number of regular equations. */
    omsi_unsigned_int   n_alias_equations;      /**< Number of alias equations. */

    model_variable_info_t*  model_vars_info;  /**< Array of variable informations for all N variables and parameters,
                                                 * N = `n_states` + `n_derivatives` `n_$all_vars` + `n_$all_parameters`,  $all={real,int,bool}. */
    equation_info_t*        equation_info;    /**< Array of equation informations for all equations. */
} model_data_t;


/*
 * ============================================================================
 * Definitions for experiment informations
 * ============================================================================
 */

/** \brief Structure containing experiment informations.
 *
 *  E.g. containing time intervall for simulation and chosen ODE/DAE solver.
 */
typedef struct omsi_experiment_t {
    omsi_real           start_time;     /**< Start time of experment, default=0. */
    omsi_real           stop_time;      /**< End time of experiment, defalut=`start_time`+1 */
    omsi_real           step_size;      /**< Step size for solvers, default (`stop_time`-`start_time`)/500 */
    omsi_unsigned_int   num_outputs;    /**< Number of outputs of model. */
    omsi_real           tolerance;      /**< Tolerance for solver, default=1e-5. */
    omsi_string         solver_name;    /**< Name of used solver, default="dassl". */
} omsi_experiment_t;


/*
 * ============================================================================
 * Structure containing simulation, experiment and model informations
 * ============================================================================
 */

/** \brief Structure containing simulation, experiment and model informations.
 *
 */
typedef struct omsi_t {
    sim_data_t*         sim_data;   /**< Containing data for simulation. */
    omsi_experiment_t*  experiment; /**< Containing infos for experiment. */
    model_data_t*       model_data; /**< Containing additional model infos. */

    omsi_bool           logCategories[NUMBER_OF_CATEGORIES];    /* Containing information for filtered logger */
    omsi_bool           loggingOn;                              /* Set logging on or off*/
} omsi_t;


/*
 * ============================================================================
 * Function prototypes
 * ============================================================================
 */
omsi_int omsi_initiatiate_osu(omsi_t** omsi);
omsi_int omsi_initialize_model(omsi_t** omsi);
omsi_int omsi_initialize_solver(omsi_t** omsi);
omsi_int omsi_intialize_simulation(omsi_t** omsi);



#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif

/** @} */
