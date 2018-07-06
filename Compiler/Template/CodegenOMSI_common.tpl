/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

 package CodegenOMSI_common
" file:        CodegenOMSIC.tpl
  package:     CodegenOMSIC
  description: Code generation using Susan templates for
               OpenModelica Simulation Inferface (OMSI) for C and C++
"

import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenUtilSimulation;
import CodegenFMU;
import CodegenOMSIC_Equations;
import CodegenCFunctions;
import CodegenAdevs;


/* public */
template generateEquationsCode (SimCode simCode, String FileNamePrefix)
"Entrypoint to generate all Code for linear systems.
 Code is generated directly into files"
::=
  match simCode
  case SIMCODE(omsiData=omsiData as SOME(OMSI_DATA(simulation=simulation as OMSI_FUNCTION(__)))) then

    // generate file for algebraic systems in simulation problem
    let content = generateOmsiFunctionCode(simulation, FileNamePrefix)
    let () = textFile(content, FileNamePrefix+"_equations_sim.c")

    // generate file for initialization problem
    // ToDo: add
  <<>>
end generateEquationsCode;


template generateOmsiFunctionCode(OMSIFunction omsiFunction, String FileNamePrefix)
"Generates file for all equations, containing equation evaluations for all systems"
::=
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""

  let _ = generateOmsiFunctionCode_inner(omsiFunction, FileNamePrefix, &evaluationCode, &functionCall)

  <<
  /* All Equations Code */
  #include "<%FileNamePrefix%>_blablabla.h"

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation */


  /* Evaluation functions for each equation */
  <%evaluationCode%>


  /* Equations evaluation */
  omsi_status <%FileNamePrefix%>_allEqns(omsi_function_t* simulation, omsi_values* model_vars_and_params){

    <%functionCall%>

    return omsi_ok;
  }

  #if defined(__cplusplus)
  }
  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateOmsiFunctionCode;


template generateOmsiFunctionCode_inner(OMSIFunction omsiFunction, String FileNamePrefix, Text &evaluationCode, Text &functionCall)
""
::=
  match omsiFunction
  case OMSI_FUNCTION(__) then

    let __ = equations |> eqsystem => (
      match eqsystem
      case SES_SIMPLE_ASSIGN(__)
      case SES_RESIDUAL(__) then
        // TODO write code
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix) +"\n"
        let &evaluationCode += CodegenOMSIC_Equations.equationFunction(eqsystem, contextOMSI, FileNamePrefix) +"\n"

        <<>>
      case SES_ALGEBRAIC_SYSTEM(__) then
        // write own file for each algebraic system
        let &functionCall += CodegenOMSIC_Equations.equationCall(eqsystem, FileNamePrefix) +"\n"
        let content = generateOmsiAlgSystemCode(eqsystem, FileNamePrefix)
        let () = textFile(content, FileNamePrefix+"_algSyst_"+ index + "_sim.c")
        <<>>
      else
        // NOT IMPLEMENTED YET
        // ToDo: add Error
        <<>>
      end match
    )

  <<>>
end generateOmsiFunctionCode_inner;


template generateOmsiAlgSystemCode (SimEqSystem equationSystem, String FileNamePrefix)
""
::=
  let &evaluationCode = buffer ""
  let &functionCall = buffer ""
  let matrixString = ""

  match equationSystem
  case SES_ALGEBRAIC_SYSTEM(matrix = matrix as SOME(JAC_MATRIX(__))) then
    let _ = generateOmsiFunctionCode_inner(residual, FileNamePrefix, &evaluationCode, &functionCall)
    let matrixString = CodegenOMSIC_Equations.generateMatrixInitialization(matrix)

  <<
  /* Algebraic system code */
  #include "<%FileNamePrefix%>_blablabla.h"

  #if defined(__cplusplus)
  extern "C" {
  #endif

  /* Instantiation */
  <%matrixString%>

  /* Evaluation functions for each equation */
  <%evaluationCode%>


  /* Equations evaluation */
  omsi_status <%FileNamePrefix%>_eqFunction_<%index%>_(omsi_function_t* simulation, omsi_values* model_vars_and_params){

    <%functionCall%>

    return omsi_ok;
  }

  #if defined(__cplusplus)
  }
  #endif
  <%\n%>
  >>
  /* leave a newline at the end of file to get rid of the warning */
end generateOmsiAlgSystemCode;















//======================================================================












template insertCopyrightOpenModelica()
::=
  <<
  /*
  * This file is part of OpenModelica.
  *
  * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
  * c/o Linköpings universitet, Department of Computer and Information Science,
  * SE-58183 Linköping, Sweden.
  *
  * All rights reserved.
  *
  * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
  * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
  * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
  * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
  * ACCORDING TO RECIPIENTS CHOICE.
  *
  * The OpenModelica software and the Open Source Modelica
  * Consortium (OSMC) Public License (OSMC-PL) are obtained
  * from OSMC, either from the above address,
  * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
  * http://www.openmodelica.org, and in the OpenModelica distribution.
  * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
  *
  * This program is distributed WITHOUT ANY WARRANTY; without
  * even the implied warranty of  MERCHANTABILITY or FITNESS
  * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
  * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
  *
  * See the full OSMC Public License conditions for more details.
  *
  */
  >>
end insertCopyrightOpenModelica;


annotation(__OpenModelica_Interface="backend");
end CodegenOMSI_common;