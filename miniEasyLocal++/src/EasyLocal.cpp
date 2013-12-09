/** 
    @file EasyLocal.cpp
    @brief Implementation of the template-free part.

    This file contains part of the implementation of the 
    EasyLocal++ framework.
    
    It must be compiled together with any project that uses the 
    framework classes or, at least, it must be linked with
    the static library that contain the already compiled fragment 
    of the framework.
    
    @author Andrea Schaerf (schaerf@uniud.it), 
            Luca Di Gaspero (digasper@dimi.uniud.it)
    @version 0.2
    @date 18 Jan 2002
    @note This version works both with MS Visual C++ and the GNU C++ 
          compiler. Yet, it is extensively tested only with the GNU compiler.
*/
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <FlexLexer.h>

#include "EasyLocal.h"

namespace easylocal {
  
  /** 
      Uniform random generator, it picks an integer value in the range
      [i, j].

      @param i lower bound of the interval
      @param j upper bound of the interval
      @return an integer in the range [i, j]
  */
  int Random(int i, int j)
  { 
    static bool init = false;
    if (init == false)  // if the random source has not been 
      {                 // initialized yet
	srand(time(0)); // let's initialize it
	init = true;
      }
    return (rand() % (j - i + 1)) + i; // return the randomly picked value
  }

  /**
     Sets the number of experiments the solver shall perform.

     @param t the number of trials
  */
  void AbstractTester::SetSolverTrials(unsigned int t)
  { trials = t; }

  /** 
      Sets the name of the file to be used as log for the 
      experiments.

      @param s the name of the file
  */
  void AbstractTester::SetLogFile(std::string s)
  { logstream = new std::ofstream(s.c_str()); }

  /** 
      Sets the prefix for the output file. Therefore, if during a
      batch of experiments is required to provide also the output, it will
      be written on files whose name starts with the given prefix.

      @param s the prefix to be used.
  */
  void AbstractTester::SetOutputPrefix(std::string s)
  { output_file_prefix = s; }
	
  /** 
      Sets the prefix for the plot file. If plotting is required, the tester
      will write it on a set of files whose name starts with the given prefix.

      @param s the prefix to be used 
  */
  void AbstractTester::SetPlotPrefix(std::string s)
  { plot_file_prefix = s; } 
	
  //
  // Actual Runner Parameters Classes related methods
  //
  
  /** 
      Constructs a ParameterData object with name n, type t and value v.
      
      @param n the parameter name
      @param t the parameter type
      @param v the parameter value
  */
  ParameterData::ParameterData(std::string n, std::string t, ValueType v)
    : name(n), type(t), value(v)
  {}
  
  /** 
      Returns the name of the parameter.
      
      @return the name of the parameter.
  */
  std::string ParameterData::Name() const
  { return name; }

  /** 
      Returns the type of the parameter.
      
      @return the type of the parameter.
  */
  std::string ParameterData::Type() const
  { return type; }
	
  /** 
      Returns the value of the parameter.
      
      @return the value of the parameter.
  */
  ValueType ParameterData::Value() const
  { return value; }
	
  
  /** 
      Adds a parameter of a given type to a parameter box. 
      
      @param name the name of the parameter
      @param type the type of the parameter
      @param value the value of the parameter
   */  
  void ParameterBox::Put(std::string name, std::string type, ValueType value) 
  { parameters.push_back(ParameterData(name,type,value)); }
	
  /** 
      Adds a parameter of type unsigned long to a parameter box. 

      @param name the name of the parameter
      @param value the value of the parameter
   */
  void ParameterBox::Put(std::string name, unsigned long value)
  { 
    ValueType tmp;
    tmp.natural = value;
    parameters.push_back(ParameterData(name,"ulong",tmp)); 
  }

  /** 
      Adds a parameter of type unsigned int to a parameter box. 

      @param name the name of the parameter
      @param value the value of the parameter
   */
  void ParameterBox::Put(std::string name, unsigned int value)
  { 
    ValueType tmp;
    tmp.short_natural = value;
    parameters.push_back(ParameterData(name,"uint",tmp)); 
  }
	
  /** 
      Adds a parameter of type double to a parameter box. 

      @param name the name of the parameter
      @param value the value of the parameter
   */
  void ParameterBox::Put(std::string name, double value)  
  { 
    ValueType tmp;
    tmp.real = value;
    parameters.push_back(ParameterData(name,"double",tmp)); 
  }
	
  /**  
       Retrieves the value of the given parameter and checks whether
       it has the right type.
       
       @param name the name of the parameter to be retrieved
       @param type the type of the parameter to be retrieved
       @param value the value of the retrieved parameter 
  */
  void ParameterBox::Get(std::string name, std::string type, ValueType& value) const
  {
    bool found = false;
    for (std::list<ParameterData>::const_iterator i = parameters.begin(); i != parameters.end(); i++)
      if (i->Name() == name) {
	found = true;
	assert(type == i->Type());
	value = i->Value();
	break;
      }
    assert(found);
  }
	
  /**  
       Retrieves the value of a unsigned long parameter.
      
       @param name the name of the parameter to be retrieved
       @param value the value of the parameter
  */
  void ParameterBox::Get(std::string name, unsigned long& value) const
  {
    bool found = false;
    for (std::list<ParameterData>::const_iterator i = parameters.begin(); i != parameters.end(); i++)
      if (i->Name() == name) {
	found = true;
	assert("ulong" == i->Type());
	value = i->Value().natural;
	break;
      }
    assert(found);
  }
	
  /** 
      Retrieves the value of a unsigned int parameter.
      
      @param name the name of the parameter to be retrieved
      @param value the value of the parameter
  */
  void ParameterBox::Get(std::string name, unsigned int& value) const
  {
    bool found = false;
    for (std::list<ParameterData>::const_iterator i = parameters.begin(); i != parameters.end(); i++)
      if (i->Name() == name) {
	found = true;
	assert("uint" == i->Type());
	value = i->Value().short_natural;
	break;
      }
    assert(found);
  }
	
  /**  
      Retrieves the value of a double parameter.
      
      @param name the name of the parameter to be retrieved
      @param value the value of the parameter
  */
  void ParameterBox::Get(std::string name, double& value) const
  {
    bool found = false;
    for (std::list<ParameterData>::const_iterator i = parameters.begin(); i != parameters.end(); i++)
      if (i->Name() == name) {
	found = true;
	assert("double" == i->Type());
	value = i->Value().real;
	break;
      }
    assert(found);
  }
	
  /** 
      Deletes all the parameters stored in the parameter box. 
  */
  void ParameterBox::Clear()
  { parameters.clear(); }		
}; // end of namespace easylocal
