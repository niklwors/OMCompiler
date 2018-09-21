/**
 *  \file omsi_factory.cpp
 *  \brief Brief
 */
//Cpp Simulation kernel includes
#include <Core/ModelicaDefine.h>
#include <Core/Modelica.h>
#include <omsi_global_settings.h>
#include <omsi_factory.h>
#include <Core/System/SimObjects.h>
#include <Core/SimController/ISimController.h>


shared_ptr<IMixedSystem>  createOSUSystem(shared_ptr<OMSIGlobalSettings> globalSettings,string instanceName,omsi_t* omsu)
 {

		   //instantiate osu system
          shared_ptr<IMixedSystem> osu  = createOSU(dynamic_pointer_cast<IGlobalSettings>(globalSettings), omsu);

          return osu;


 }



