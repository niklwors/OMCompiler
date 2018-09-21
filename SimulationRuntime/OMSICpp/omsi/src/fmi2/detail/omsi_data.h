#pragma once

//omsi header
#include <omsi.h>
#include <omsi_callbacks.h>
#include "fmi2Functions.h"
#include <string>

using std::string;
omsi_t* instantiate_omsi(omsi_string                    instanceName,
						 omsu_type                      fmuType,
						 omsi_string                    fmuGUID,
						 omsi_string                    fmuResourceLocation,
						 const omsi_callback_functions* functions,
						 omsi_bool                      visible,
						 omsi_bool                      loggingOn);

omsi_int initialize_omsi(omsi_t* omsu, const omsi_callback_functions*  functions, omsi_string instanceName);

void free_omsi(omsi_t* omsu);



string get_init_file_name(omsi_string instanceName, omsi_string fmuResourceLocation);