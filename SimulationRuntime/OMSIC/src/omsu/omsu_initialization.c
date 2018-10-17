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

/*
 * This file defines functions for the FMI used via the OpenModelica Simulation
 * Interface (OMSI). These functions are used for instantiation and initialization
 * of the FMU.
 */

#define DEBUG omsi_false
#define DEBUG_PRINT(function) if (DEBUG) {                                     \
    printf("\nDEBUG PRINT\n");                                                 \
    printf("=====================================================\n");         \
    fflush(stdout);                                                            \
    function; fflush(stdout);                                                  \
    }                                                                          \

#include <omsu_initialization.h>


osu_t* omsic_instantiate(omsi_string                            instanceName,
                         omsu_type                              fmuType,
                         omsi_string                            fmuGUID,
                         omsi_string                            fmuResourceLocation,
                         const omsi_callback_functions*         functions,
                         omsi_bool                              __attribute__((unused)) visible,
                         omsi_bool                              loggingOn)
{
    /* Variables */
    osu_t *OSU;

    /* set global callback functions */
    global_callback = (omsi_callback_functions*) functions;
    global_instance_name = instanceName;
    global_callback->componentEnvironment = OSU;

    filtered_base_logger(NULL, log_statuserror, omsi_error,
            "Logger test: %s", "helloWorldsgsdgsdgsdgsdgsdg");

    /* allocate memory for Openmodelica Simulation Unit */
    OSU = functions->allocateMemory(1, sizeof(osu_t));
    if (!OSU) {
        filtered_base_logger(NULL, log_statuserror, omsi_error,
                "fmi2Instantiate: Not enough memory.");
        return NULL;
    }

    /* set general OSU data */
    OSU->GUID = (omsi_char*) functions->allocateMemory(1, strlen(fmuGUID) + 1);
    if (!OSU->GUID) {
        filtered_base_logger(NULL, log_statuserror, omsi_error,
                        "fmi2Instantiate: Not enough memory.");
        return NULL;
    }

    strcpy(OSU->GUID, fmuGUID);
    OSU->instanceName = strdup(instanceName);
    OSU->type = fmuType;
    OSU->fmiCallbackFunctions = functions;
    OSU->vrStates = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));
    OSU->vrStatesDerivatives = (omsi_unsigned_int *) functions->allocateMemory(1, sizeof(omsi_unsigned_int));
    OSU->osu_functions = (omsi_template_callback_functions_t*) functions->allocateMemory(1, sizeof(omsi_template_callback_functions_t));

    if (!OSU->osu_functions || !OSU->instanceName || !OSU->vrStates || !OSU->vrStatesDerivatives) {
        filtered_base_logger(NULL, log_statuserror, omsi_error,
                "fmi2Instantiate: Not enough memory.");
        return NULL;
    }

    /* Set template function pointers */
    filtered_base_logger(NULL, log_all, omsi_ok,
            "fmi2Instantiate: Set callback functions from generated C-Code");
    initialize_start_function(OSU->osu_functions);      /* ToDo: At the moment only for static compilation */


    /* Call OMSIBase function for initialization of osu_data */
    OSU->osu_data = omsi_instantiate(instanceName, fmuType, fmuGUID, fmuResourceLocation, functions, OSU->osu_functions, visible, loggingOn);

    /* Set pointer to logCategories and loggingOn */
    OSU->logCategories = OSU->osu_data->logCategories;
    OSU->loggingOn = &OSU->osu_data->loggingOn;

    /* Set state and log informations */
    OSU->state = modelInstantiated;
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Instantiate: GUID=%s", fmuGUID);


    DEBUG_PRINT(omsu_print_osu(OSU))

    return OSU;
}


/*
 * Informs the OpenModelica Simulation Unit to enter the initialization mode.
 */
omsi_status omsi_enter_initialization_mode(osu_t* OSU) {

    if (invalidState(OSU, "fmi2EnterInitializationMode", modelInstantiated, ~0)) {
        filtered_base_logger(global_logCategories, log_statuserror, omsi_error,
                "fmi2EnterInitializationMode: Call was not allowed.");
        return omsi_error;
    }

    /* Log function call */
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2EnterInitializationMode: Successful.");

    OSU->state = modelInitializationMode;

    return omsi_ok;
}

/*
 * Informs the OpenNodelica Simulation Unit to exit initialization mode.
 */
omsi_status omsi_exit_initialization_mode(osu_t* OSU) {

    if (invalidState(OSU, "fmi2ExitInitializationMode", modelInitializationMode, ~0))
        return omsi_error;
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2ExitInitializationMode: ....");

    /* ToDo: free OSU->omsi_data->initialization */
    /* ToDo: allocate OSU->omsi_data->simulation here to save some memory? */

    OSU->state = modelEventMode;
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2ExitInitializationMode: Success.");

    return omsi_ok;
}

/*
 * Setup experiment data for the Openmodelica Simulation Unit.
 * Gets called from function fmi2SetupExperiment.
 */
omsi_status omsi_setup_experiment(osu_t*     OSU,
                                  omsi_bool  toleranceDefined,
                                  omsi_real  tolerance,
                                  omsi_real  startTime,
                                  omsi_bool  stopTimeDefined,
                                  omsi_real  stopTime) {

    if (invalidState(OSU, "fmi2SetupExperiment", modelInstantiated, ~0))
        return omsi_error;

    if (toleranceDefined) {
        OSU->osu_data->experiment->tolerance = tolerance;
    }
    else {
        OSU->osu_data->experiment->tolerance = 1e-5;    /* default tolerance */
    }
    OSU->osu_data->experiment->start_time = startTime;
    if (stopTimeDefined) {
        OSU->osu_data->experiment->stop_time = stopTime;
    }
    else {
        OSU->osu_data->experiment->stop_time = startTime+1;     /* default stop time */
    }
    OSU->osu_data->experiment->step_size = (OSU->osu_data->experiment->stop_time
                                            - OSU->osu_data->experiment->start_time) / 500;

    /* Log function call */
    filtered_base_logger(global_logCategories, log_fmi2_call, omsi_ok,
            "fmi2SetupExperiment: toleranceDefined=%s, tolerance=%g, startTime=%g, stopTimeDefined=%s, stopTime=%g",
            toleranceDefined ? "true" : "false", OSU->osu_data->experiment->tolerance, startTime,
            stopTimeDefined ? "true" : "false", OSU->osu_data->experiment->stop_time);

    return omsi_ok;
}

/*
 * Frees all allocated memory for the Openmodelica Simulation Unit.
 * Does nothing if a null pointer is provided.
 * Gets called from function fmi2FreeInstance.
 */
void omsi_free_instance(osu_t* OSU) {

    /* Variables */
    omsi_int meStates;
    omsi_int csStates;

    if (OSU==NULL) {
        return;
    }

    meStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;
    csStates = modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError;

    if (invalidState(OSU, "fmi2FreeInstance", meStates, csStates)) {
        return;
    }
    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2FreeInstance");

#if 0
    /* free OSU data */
    omsu_free_osu_data(OSU->osu_data);
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_data);
    /* ToDo: free everything inside osu_functions */
    OSU->fmiCallbackFunctions->freeMemory(OSU->osu_functions);

    /* free instanceName & GUID */
    if (OSU->instanceName) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->instanceName);
    if (OSU->GUID) OSU->fmiCallbackFunctions->freeMemory((void*)OSU->GUID);

    /* free callback functions */
    OSU->fmiCallbackFunctions->freeMemory(OSU);
#endif
}

/*
 * Resets the Openmodelica Simulation Unit.
 * Gets called from function fmi2Reset.
 */
omsi_status omsi_reset(osu_t* OSU) {

    if (invalidState(OSU, "fmi2Reset", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError, ~0))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Reset");

    if (OSU->state & modelTerminated) {
      /* initialize OSU */
      /* omsic_model_setup_data(OSU);*/            /* ToDo: implement */
    }
    /* reset the values to start */
    /*setDefaultStartValues(OSU);*/     /* ToDo: implement */

    OSU->state = modelInstantiated;
    return omsi_ok;
}

/*
 * Informs that the simulation run is terminated.
 * Gets called from function fmi2Terminate.
 */
omsi_status omsi_terminate(osu_t* OSU) {

    if (invalidState(OSU, "fmi2Terminate", modelEventMode|modelContinuousTimeMode, ~0))
        return omsi_error;

    filtered_base_logger(global_logCategories, log_all, omsi_ok,
            "fmi2Terminate");

    OSU->state = modelTerminated;
    return omsi_ok;
}
