#include "stdafx.h"
#define BOOST_EXTENSION_SYSTEM_DECL BOOST_EXTENSION_EXPORT_DECL
#define BOOST_EXTENSION_ALGLOOPDEFAULTIMPL_DECL BOOST_EXTENSION_EXPORT_DECL

//#include <System/SystemDefaultImplementation.h>
#include <System/AlgLoopSolverFactory.h>
//#include <System/EventHandling.h>
//#include <System/AlgLoopDefaultImplementation.h>
#include <LibrariesConfig.h>

using boost::extensions::factory;

BOOST_EXTENSION_TYPE_MAP_FUNCTION {

   types.get<std::map<std::string, factory<IAlgLoopSolverFactory,IGlobalSettings& > > >()
    ["AlgLoopSolverFactory"].set<AlgLoopSolverFactory>();

}
