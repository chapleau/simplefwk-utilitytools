%module RootNtupleTools
%{
#include "UtilityTools/RootNtupleWriterTool.h"
#include "UtilityTools/RootNtupleReaderTool.h"
%}

%include "exception.i"

%exception {
    try {
        $action
    }
    catch (const std::exception & e)
    {
        SWIG_exception(SWIG_RuntimeError, (std::string("C++ std::exception in $decl: ") + e.what()).c_str());
    }
    catch (...)
    {
        SWIG_exception(SWIG_UnknownError, "C++ anonymous exception");
    }
}


%include "std_string.i"


%include "std_vector.i"

namespace std {
       %template(IntVector) vector<int>;
       %template(UIntVector) vector<unsigned int>;
       %template(FloatVector) vector<float>;
       %template(DoubleVector) vector<double>;
       %template(StringVector) vector<string>;
       %template(ULongIntVector) vector<unsigned long long int>;
}



enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logVERBOSE};


class IRootNtupleWriterTool {

private :

   IRootNtupleWriterTool();
   ~IRootNtupleWriterTool();

};



namespace boost {


class any {
  public:

  any(const int *);
  any(const unsigned int *);
  any(const unsigned long long int *);
  any(const std::string *);
  any(const double *);
  any(const float *);

};

}


class RootNtupleWriterTool : virtual public IRootNtupleWriterTool {


public:

     /// Constructor with parameters:
  RootNtupleWriterTool(const std::string&,const std::string&,const std::string&,bool single = false );
  RootNtupleWriterTool(const std::string&, const std::string&,const std::string&, TLogLevel,bool single = false);
  
  int registerBranch(std::string, IObjectHolder*);
  
  int pushBack(std::string, const boost::any&);

  void stop();
        
  ~RootNtupleWriterTool();
};




class RootNtupleReaderTool {


   public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RootNtupleReaderTool(const std::string&,const std::string&,const std::string& );
  RootNtupleReaderTool(const std::string&, const std::string&,const std::string&, TLogLevel);
  

  /// Destructor: 
  ~RootNtupleReaderTool();

  template <class T>
  const T* GetBranchEntry(const std::string&, unsigned long);

  template <class T>
  const T* GetBranchEntryWithIndex(const std::string&, unsigned long, unsigned long);

  int BuildIndex(const std::string&, const std::string&);

};


%template(GetBranchEntry_Int) RootNtupleReaderTool::GetBranchEntry<int>;
%template(GetBranchEntry_Float) RootNtupleReaderTool::GetBranchEntry<float>;
%template(GetBranchEntry_UInt) RootNtupleReaderTool::GetBranchEntry<unsigned int>;
%template(GetBranchEntry_ULongInt) RootNtupleReaderTool::GetBranchEntry<unsigned long long int>;
%template(GetBranchEntry_String) RootNtupleReaderTool::GetBranchEntry<std::string>;

%template(GetBranchEntry_FloatVector) RootNtupleReaderTool::GetBranchEntry<std::vector<float> >;
%template(GetBranchEntry_IntVector) RootNtupleReaderTool::GetBranchEntry<std::vector<int> >;
%template(GetBranchEntry_DoubleVector) RootNtupleReaderTool::GetBranchEntry<std::vector<double> >;
%template(GetBranchEntry_StringVector) RootNtupleReaderTool::GetBranchEntry<std::vector<std::string> >;
%template(GetBranchEntry_UIntVector) RootNtupleReaderTool::GetBranchEntry<std::vector<unsigned int> >;
%template(GetBranchEntry_ULongIntVector) RootNtupleReaderTool::GetBranchEntry<std::vector<unsigned long long int> >;

%template(GetBranchEntryWithIndex_Int) RootNtupleReaderTool::GetBranchEntryWithIndex<int>;
%template(GetBranchEntryWithIndex_String) RootNtupleReaderTool::GetBranchEntryWithIndex<std::string>;
%template(GetBranchEntryWithIndex_Float) RootNtupleReaderTool::GetBranchEntryWithIndex<float>;
%template(GetBranchEntryWithIndex_FloatVector) RootNtupleReaderTool::GetBranchEntryWithIndex<std::vector<float> >;
%template(GetBranchEntryWithIndex_IntVector) RootNtupleReaderTool::GetBranchEntryWithIndex<std::vector<int> >;
%template(GetBranchEntryWithIndex_ULongIntVector) RootNtupleReaderTool::GetBranchEntryWithIndex<std::vector<unsigned long long int> >;
%template(GetBranchEntryWithIndex_ULongInt) RootNtupleReaderTool::GetBranchEntryWithIndex<unsigned long long int>;

%include "cpointer.i"
%pointer_functions(int, intp);
%pointer_functions(float, floatp);
%pointer_functions(unsigned int, uintp);
%pointer_functions(unsigned long long int, ulongintp);
%pointer_functions(std::string, stringp);

%pointer_class(int, int_p);
%pointer_class(float, float_p);
%pointer_class(unsigned int, uint_p);
%pointer_class(unsigned long long int, ulongint_p);
%pointer_class(std::string, string_p);



