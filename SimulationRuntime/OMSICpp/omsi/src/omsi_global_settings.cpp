/** @addtogroup coreSimulationSettings
 *
 *  @{
 */

#include <Core/ModelicaDefine.h>
#include <Core/Modelica.h>
//#define BOOST_EXTENSION_GLOBALSETTINGS_DECL BOOST_EXTENSION_EXPORT_DECL
#include <omsi_global_settings.h>

OMSIGlobalSettings::OMSIGlobalSettings()
  : _startTime(0.0)
  , _endTime(5.0)
  , _hOutput(0.001)
  , _emitResults(EMIT_ALL)
  , _infoOutput(true)
  , _selected_solver("Euler")
  , _selected_lin_solver("linearSolver")
  , _selected_nonlin_solver("Newton")
  , _resultsfile_name("results.csv")
  , _endless_sim(false)
  , _nonLinSolverContinueOnError(false)
  , _outputPointType(OPT_ALL)
  , _alarm_time(0)
  , _outputFormat(MAT)
  ,_init_file_path("")
{
}

OMSIGlobalSettings::~OMSIGlobalSettings()
{
}

///< Start time of integration (default: 0.0)
double OMSIGlobalSettings::getStartTime()
{
  return _startTime;
}

void OMSIGlobalSettings::setStartTime(double time)
{
  _startTime = time;
}

///< End time of integraiton (default: 1.0)
double OMSIGlobalSettings::getEndTime()
{
  return _endTime;
}

void OMSIGlobalSettings::setEndTime(double time)
{
  _endTime = time;
}

///< Output step size (default: 20 ms)
double OMSIGlobalSettings::gethOutput()
{
  return _hOutput;
}

void OMSIGlobalSettings::sethOutput(double h)
{
  _hOutput = h;
}

bool OMSIGlobalSettings::useEndlessSim()
{
  return _endless_sim ;
}

void OMSIGlobalSettings::useEndlessSim(bool endles)
{
  _endless_sim = endles;
}

OutputPointType OMSIGlobalSettings::getOutputPointType()
{
  return _outputPointType;
}

void OMSIGlobalSettings::setOutputPointType(OutputPointType type)
{
  _outputPointType = type;
}

LogSettings OMSIGlobalSettings::getLogSettings()
{
  return _log_settings;
}

void OMSIGlobalSettings::setLogSettings(LogSettings set)
{
  _log_settings = set;
}

///< Write out results (default: EMIT_ALL)
EmitResults OMSIGlobalSettings::getEmitResults()
{
  return _emitResults;
}

void OMSIGlobalSettings::setEmitResults(EmitResults emitResults)
{
  _emitResults = emitResults;
}

void OMSIGlobalSettings::setResultsFileName(string name)
{
  _resultsfile_name = name;
}

string  OMSIGlobalSettings::getResultsFileName()
{
  return _resultsfile_name;
}

///< Write out statistical simulation infos, e.g. number of steps (at the end of simulation); [false,true]; default: true)
bool OMSIGlobalSettings::getInfoOutput()
{
  return _infoOutput;
}

void OMSIGlobalSettings::setInfoOutput(bool output)
{
  _infoOutput = output;
}

string OMSIGlobalSettings::getOutputPath()
{
  return _output_path ;
}
 string OMSIGlobalSettings::getInitfilePath()
 {
        return _init_file_path;
 }
 void OMSIGlobalSettings::setInitfilePath(string path)
 {
        _init_file_path = path;
 }
void OMSIGlobalSettings::setOutputPath(string path)
{
  _output_path = path;
}

string OMSIGlobalSettings::getSelectedSolver()
{
  return _selected_solver;
}

void OMSIGlobalSettings::setSelectedSolver(string solver)
{
  _selected_solver = solver;
}

string   OMSIGlobalSettings::getSelectedLinSolver()
{
  return _selected_lin_solver;
}

void OMSIGlobalSettings::setSelectedLinSolver(string solver)
{
  _selected_lin_solver = solver;
}

string OMSIGlobalSettings::getSelectedNonLinSolver()
{
  return _selected_nonlin_solver;
}

void OMSIGlobalSettings::setSelectedNonLinSolver(string solver)
{
  _selected_nonlin_solver = solver;
}

/**
initializes settings object by an xml file
*/
void OMSIGlobalSettings::load(std::string xml_file)
{
}

void OMSIGlobalSettings::setRuntimeLibrarypath(string path)
{
  _runtimeLibraryPath = path;
}

string OMSIGlobalSettings::getRuntimeLibrarypath()
{
  return _runtimeLibraryPath;
}

void OMSIGlobalSettings::setAlarmTime(unsigned int t)
{
  _alarm_time = t;
}

unsigned int OMSIGlobalSettings::getAlarmTime()
{
  return _alarm_time;
}

void OMSIGlobalSettings::setNonLinearSolverContinueOnError(bool value)
{
  _nonLinSolverContinueOnError = value;
}

bool OMSIGlobalSettings::getNonLinearSolverContinueOnError()
{
  return _nonLinSolverContinueOnError;
}

void OMSIGlobalSettings::setSolverThreads(int val)
{
  _solverThreads = val;
}

int OMSIGlobalSettings::getSolverThreads()
{
  return _solverThreads;
}

 OutputFormat OMSIGlobalSettings::getOutputFormat()
 {
     return _outputFormat;
 }
  void OMSIGlobalSettings::setOutputFormat(OutputFormat outputFormat)
  {
      _outputFormat = outputFormat;
  }
/** @} */ // end of coreSimulationSettings
