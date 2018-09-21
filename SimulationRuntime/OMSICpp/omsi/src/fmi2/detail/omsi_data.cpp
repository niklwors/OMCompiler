
//Cpp Simulation kernel includes
#include <Core/ModelicaDefine.h>
#include <Core/Modelica.h>
#include "omsi_data.h"
#include <omsi_input_xml.h>
#include <omsi_input_json.h>
#include <omsi_input_model_variables.h>
#include <omsi_input_sim_data.h>


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

omsi_t* instantiate_omsi(omsi_string                    instanceName,
						 omsu_type                      fmuType,
	                     omsi_string                    fmuGUID,
	                     omsi_string                    fmuResourceLocation,
						 const omsi_callback_functions* functions,
						 omsi_bool                      visible,
						 omsi_bool                      loggingOn)
{
	/* allocate memory for Openmodelica Simulation Unit */
	omsi_t* omsu = (omsi_t*)functions->allocateMemory(1, sizeof(omsi_t));
	if (!omsu) {
		functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Not enough memory.");
		return NULL;
	}
	string initXMLFilename = get_init_file_name(instanceName, fmuResourceLocation);
	if (omsu_process_input_xml(omsu, initXMLFilename.c_str() , fmuGUID, instanceName, functions)) {     // ToDo: needs some information beforehand
		functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Could not process %s.", initXMLFilename.c_str());
		return NULL;
	}

	if ((omsu_allocate_sim_data(omsu, functions, instanceName))) {     // ToDo: needs some information beforehand
		functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Could not process %s.", initXMLFilename.c_str());
		return NULL;
	}

	if ((omsi_allocate_model_variables(omsu, functions))){     // ToDo: needs some information beforehand
		functions->logger(functions->componentEnvironment, instanceName, omsi_error, "error", "fmi2Instantiate: Could not process %s.", initXMLFilename.c_str());
		return NULL;
	}

	return omsu;
}


omsi_int initialize_omsi(omsi_t* omsu, const omsi_callback_functions*  functions, omsi_string instanceName)
{
	//initialize start values for real,int and bool variables
	omsi_int status = omsi_initialize_model_variables(omsu,functions, instanceName);
	return status;
}
void free_omsi(omsi_t* omsu)
{

	omsi_free_model_variables(omsu);
}

string get_init_file_name(omsi_string instanceName, omsi_string fmuResourceLocation)
{

	fs::path model_name_path(string(instanceName) + ("_init.xml"));
	fs::path init_file_path = fs::path(string(fmuResourceLocation));
	init_file_path /= model_name_path;
	return init_file_path.string();


}
