
#pragma once
#include "stdafx.h"
#if defined(__vxworks)


#elif defined(SIMSTER_BUILD)

#include "Hybrj.h"
#include "HybrjSettings.h"


/*Simster factory*/
extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export_hybrj(boost::extensions::factory_map & fm)
{
    fm.get<IAlgLoopSolver,int,IAlgLoop*, INonLinSolverSettings*>()[1].set<Hybrj>();
    fm.get<INonLinSolverSettings,int >()[2].set<HybrjSettings>();
}

#elif defined(OMC_BUILD)

#include "Hybrj.h"
#include "HybrjSettings.h"


using boost::extensions::factory;

BOOST_EXTENSION_TYPE_MAP_FUNCTION {
  types.get<std::map<std::string, factory<IAlgLoopSolver,IAlgLoop*, INonLinSolverSettings*> > >()
    ["hybrj"].set<Hybrj>();
  types.get<std::map<std::string, factory<INonLinSolverSettings> > >()
    ["hybrjSettings"].set<HybrjSettings>();
 }


#else
error "operating system not supported"
#endif



