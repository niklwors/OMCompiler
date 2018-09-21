#pragma once
/** @addtogroup simcorefactoriesPolicies
 *
 *  @{
 */

#include <SimCoreFactory/ObjectFactory.h>

shared_ptr<INonLinSolverSettings> createNewtonSettings();
 shared_ptr<INonLinSolverSettings> createKinsolSettings();
 shared_ptr<INonLinearAlgLoopSolver> createNewtonSolver(shared_ptr<INonLinSolverSettings> solver_settings,shared_ptr<INonLinearAlgLoop> algLoop);
 shared_ptr<INonLinearAlgLoopSolver> createKinsolSolver(shared_ptr<INonLinSolverSettings> solver_settings,shared_ptr<INonLinearAlgLoop> algLoop);
template <class CreationPolicy>
class StaticNonLinSolverOMCFactory : virtual public ObjectFactory<CreationPolicy>
{

public:
    StaticNonLinSolverOMCFactory(PATH library_path,PATH modelicasystem_path,PATH config_path)
    :ObjectFactory<CreationPolicy>(library_path,modelicasystem_path,config_path)
      {
      };
    virtual ~StaticNonLinSolverOMCFactory(){};

   virtual shared_ptr<INonLinSolverSettings> createNonLinSolverSettings(string nonlin_solver)
   {
      string nonlin_solver_key;
      #ifndef ENABLE_OMSI
      if(nonlin_solver.compare("newton")==0)
      {
        shared_ptr<INonLinSolverSettings> settings = createNewtonSettings();
        return settings;
      }

      #ifdef ENABLE_SUNDIALS_STATIC
      if(nonlin_solver.compare("kinsol")==0)
      {
          shared_ptr<INonLinSolverSettings> settings = createKinsolSettings();
          return settings;
      }
      #endif //ENABLE_SUNDIALS_STATIC
      throw ModelicaSimulationError(MODEL_FACTORY,"Selected nonlin solver is not available");
      //return NonLinSolverOMCFactory<CreationPolicy>::createNonLinSolverSettings(nonlin_solver);
       #else
      throw ModelicaSimulationError(MODEL_FACTORY,"osi for nonlinear solver is not yet supported");
      #endif //ENABLE_OMSI
   }
   virtual shared_ptr<INonLinearAlgLoopSolver> createNonLinSolver(string solver_name, shared_ptr<INonLinSolverSettings> solver_settings,shared_ptr<INonLinearAlgLoop> algLoop = shared_ptr<INonLinearAlgLoop>())
   {
      #ifndef ENABLE_OMSI
      if(solver_name.compare("newton")==0)
      {
        shared_ptr<INonLinearAlgLoopSolver> newton = createNewtonSolver(solver_settings,algLoop);
        return newton;
      }

      #ifdef ENABLE_SUNDIALS_STATIC
      if(solver_name.compare("kinsol")==0)
      {
        shared_ptr<INonLinearAlgLoopSolver> kinsol = createKinsolSolver(solver_settings,algLoop);
        return kinsol;
      }
      #endif //ENABLE_SUNDIALS_STATIC
      throw ModelicaSimulationError(MODEL_FACTORY,"Selected nonlin solver is not available");
      #else
      throw ModelicaSimulationError(MODEL_FACTORY,"osi for nonlinear solver is not yet supported");
      #endif //ENABLE_OMSI
      //return NonLinSolverOMCFactory<CreationPolicy>::createNonLinSolver(algLoop, solver_name, solver_settings);
    }
};
/** @} */ // end of simcorefactoriesPolicies
