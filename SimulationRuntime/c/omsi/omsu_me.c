/**
 *  \file omsi_me.cpp
 *  \brief Brief
 */

//C Simulation kernel includes


//OpenModelica Simulation Interface
#include <omsi.h>
#include "osu_me.h"
#include "simulation/solver/stateset.h"
#include "simulation_data.h"
#include "simulation/solver/stateset.h"
#include "simulation/solver/model_help.h"
#if !defined(OMC_NUM_NONLINEAR_SYSTEMS) || OMC_NUM_NONLINEAR_SYSTEMS>0
#include "simulation/solver/nonlinearSystem.h"
#endif
#if !defined(OMC_NUM_LINEAR_SYSTEMS) || OMC_NUM_LINEAR_SYSTEMS>0
#include "simulation/solver/linearSystem.h"
#endif
#if !defined(OMC_NUM_MIXED_SYSTEMS) || OMC_NUM_MIXED_SYSTEMS>0
#include "simulation/solver/mixedSystem.h"
#endif
#include "simulation/solver/delay.h"
#include "simulation/simulation_info_json.h"
#include "simulation/simulation_input_xml.h"

#ifdef __cplusplus
extern "C" {
#endif

fmi2Status fmi2EventUpdate(fmi2Component c, fmi2EventInfo* eventInfo)
{
  int i;
  osu_t* OSU = (osu_t *)c;
  threadData_t *threadData = OSU->threadData;

  if (nullPointer(OSU, "fmi2EventUpdate", "eventInfo", eventInfo))
    return fmi2Error;
  eventInfo->valuesOfContinuousStatesChanged = fmi2False;
  FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EventUpdate: Start Event Update! Next Sample Event %u", eventInfo->nextEventTime)

  /* try */
  MMC_TRY_INTERNAL(simulationJumpBuffer)

#if !defined(OMC_NO_STATESELECTION)
    if (stateSelection(OSU->old_data, OSU->threadData, 1, 1))
    {
      FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EventUpdate: Need to iterate state values changed!")
      /* if new set is calculated reinit the solver */
      eventInfo->valuesOfContinuousStatesChanged = fmi2True;
    }
#endif
    storePreValues(OSU->old_data);

    /* activate sample event */
    for(i=0; i<OSU->old_data->modelData->nSamples; ++i)
    {
      if(OSU->old_data->simulationInfo->nextSampleTimes[i] <= OSU->old_data->localData[0]->timeValue)
      {
        OSU->old_data->simulationInfo->samples[i] = 1;
        infoStreamPrint(LOG_EVENTS, 0, "[%ld] sample(%g, %g)", OSU->old_data->modelData->samplesInfo[i].index, OSU->old_data->modelData->samplesInfo[i].start, OSU->old_data->modelData->samplesInfo[i].interval);
      }
    }

    OSU->old_data->callback->functionDAE(OSU->old_data, OSU->threadData);

    /* deactivate sample events */
    for(i=0; i<OSU->old_data->modelData->nSamples; ++i)
    {
      if(OSU->old_data->simulationInfo->samples[i])
      {
        OSU->old_data->simulationInfo->samples[i] = 0;
        OSU->old_data->simulationInfo->nextSampleTimes[i] += OSU->old_data->modelData->samplesInfo[i].interval;
      }
    }

    for(i=0; i<OSU->old_data->modelData->nSamples; ++i)
      if((i == 0) || (OSU->old_data->simulationInfo->nextSampleTimes[i] < OSU->old_data->simulationInfo->nextSampleEvent))
        OSU->old_data->simulationInfo->nextSampleEvent = OSU->old_data->simulationInfo->nextSampleTimes[i];

    if(OSU->old_data->callback->checkForDiscreteChanges(OSU->old_data, OSU->threadData) || OSU->old_data->simulationInfo->needToIterate || checkRelations(OSU->old_data) || eventInfo->valuesOfContinuousStatesChanged)
    {
      FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EventUpdate: Need to iterate(discrete changes)!")
      eventInfo->newDiscreteStatesNeeded  = fmi2True;
      eventInfo->nominalsOfContinuousStatesChanged = fmi2False;
      eventInfo->valuesOfContinuousStatesChanged  = fmi2True;
      eventInfo->terminateSimulation = fmi2False;
    }
    else
    {
      eventInfo->newDiscreteStatesNeeded  = fmi2False;
      eventInfo->nominalsOfContinuousStatesChanged = fmi2False;
      eventInfo->terminateSimulation = fmi2False;
    }
    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EventUpdate: newDiscreteStatesNeeded %s",eventInfo->newDiscreteStatesNeeded?"true":"false");

    /* due to an event overwrite old values */
    overwriteOldSimulationData(OSU->old_data);

    /* TODO: check the event iteration for relation
     * in fmi2 import and export. This is an workaround,
     * since the iteration seem not starting.
     */
    storePreValues(OSU->old_data);
    updateRelationsPre(OSU->old_data);

    //Get Next Event Time
    double nextSampleEvent=0;
    nextSampleEvent = getNextSampleTimeFMU(OSU->old_data);
    if (nextSampleEvent == -1)
    {
      eventInfo->nextEventTimeDefined = fmi2False;
    }
    else
    {
      eventInfo->nextEventTimeDefined = fmi2True;
      eventInfo->nextEventTime = nextSampleEvent;
    }
    FILTERED_LOG(OSU, fmi2OK, LOG_FMI2_CALL, "fmi2EventUpdate: Checked for Sample Events! Next Sample Event %u",eventInfo->nextEventTime)

    return fmi2OK;

  /* catch */
  MMC_CATCH_INTERNAL(simulationJumpBuffer)

  FILTERED_LOG(OSU, fmi2Error, LOG_FMI2_CALL, "fmi2EventUpdate: terminated by an assertion.")
  OSU->_need_update = 1;
  return fmi2Error;
}

//unsupported functions
fmi2Status omsi_free_fmu_state(fmi2Component c, fmi2FMUstate* FMUstate) {
	return unsupportedFunction(c, "fmi2FreeFMUstate", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}

fmi2Status omsi_serialized_fmu_state_size(fmi2Component c, fmi2FMUstate FMUstate, size_t* size) {
	return unsupportedFunction(c, "fmi2SerializedFMUstateSize", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}

fmi2Status omsi_serialize_fmu_state(fmi2Component c, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size) {
	return unsupportedFunction(c, "fmi2SerializeFMUstate", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}

fmi2Status  omsi_de_serialize_fmu_state(fmi2Component c, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate) {
	return unsupportedFunction(c, "fmi2DeSerializeFMUstate", modelInstantiated|modelInitializationMode|modelEventMode|modelContinuousTimeMode|modelTerminated|modelError);
}


























#ifdef __cplusplus
}
#endif
