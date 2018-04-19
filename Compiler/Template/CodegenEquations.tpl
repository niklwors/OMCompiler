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

package CodegenEquations
" file:        CodegenEquations.tpl
  package:     CodegenEquations
  description: Code generation using Susan templates for 
               OpenModelica Simulation Inferface (OMSI) equation related templates
"



import interface SimCodeTV;
import interface SimCodeBackendTV;
import CodegenUtil;
import CodegenUtilSimulation.*;

template equationFunctionPrototypes(SimEqSystem eq, String modelNamePrefixStr)
 "Generates prototype for an equation function"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  <<
  void <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(*Data_Struct_something data, *Data_Struct_something threadData);<%\n%>
  >>
end equationFunctionPrototypes;


template equationFunction(SimEqSystem eq, String modelNamePrefixStr)
 "Generates C-function for an equation evaluation"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  let equationInfos = dumpEqs(fill(eq,1))
  let equationCode =""
  <<
  /*
  <%equationInfos%>
  */
  void <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(*Data_Struct_something data, *Data_Struct_something threadData){
    const int equationIndexes[2] = {1,<%ix%>};
    <%equationCode%>
    /*
     *Hier muss erst festgelegt werden, wie die Datenstrukturen aufgeteilt werden sollen.
     */
  }
  >>
end equationFunction;


template equationCall(SimEqSystem eq, String modelNamePrefixStr)
 "Generates call function for evaluating functions"
::=
  let ix = CodegenUtilSimulation.equationIndex(eq)
  <<
  <%CodegenUtil.symbolName(modelNamePrefixStr,"eqFunction")%>_<%ix%>(data, threadData);
  >>
end equationCall;


template generateEquationFiles(list<SimEqSystem> allEquations, String fileNamePrefix)
"Generates content of fileNamePrefix_eqns.c"
::=
  let eqFuncs = ""
            let _ = allEquations |> eqn => (
            let &eqFuncs += equationFunction(eqn, fileNamePrefix) + "\n\n"
            <<>>
            )
  let eqCalls = ""
  let _ =  allEquations |> eqn => (
            let &eqCalls += equationCall(eqn, fileNamePrefix) + "\n"
            <<>>
            )

  <<
  #include "<%fileNamePrefix%>_eqns.h"
  
  /* Equation functions */
  <%eqFuncs%>
  /* Equations evaluation */
  int evalEquations(){

    <%eqCalls%>

    return 0;
  }
  >>
end generateEquationFiles;


template generateEquationFilesHeader(list<SimEqSystem> allEquations, String fileNamePrefix)
"Generates content of header file fileNamePrefix_eqns.h"
::=
  let eqFuncsPrototypes = ""
            let _ = allEquations |> eqn => (
            let &eqFuncsPrototypes += equationFunctionPrototypes(eqn, fileNamePrefix)
            <<>>
            )

  <<
  #ifndef <%fileNamePrefix%>_eqns
  #define <%fileNamePrefix%>_eqns

  /* Equation functions prototypes */
  #if defined(__cplusplus)
  extern "C" {
  #endif

  int evalEquations(Blablabla);
  <%eqFuncsPrototypes%>

  #if defined(__cplusplus)
  }
  #endif

  #endif
  >>
end generateEquationFilesHeader;


annotation(__OpenModelica_Interface="backend");
end CodegenEquations;