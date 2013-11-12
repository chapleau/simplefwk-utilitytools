%module RootNtupleTools
%{
#include "UtilityTools/RootNtupleWriterTool.h"
#include "UtilityTools/RootNtupleReaderTool.h"
%}

%include "std_string.i"

enum TLogLevel {logERROR, logWARNING, logINFO, logDEBUG, logVERBOSE};

class IRootNtupleWriterTool {

private :

   IRootNtupleWriterTool();
   ~IRootNtupleWriterTool();

};

class RootNtupleWriterTool : virtual public IRootNtupleWriterTool {


public:

     /// Constructor with parameters:
  RootNtupleWriterTool(const std::string&,const std::string&,const std::string&,bool single = false );
  RootNtupleWriterTool(const std::string&, const std::string&,const std::string&, TLogLevel,bool single = false);
        
  ~RootNtupleWriterTool();
};


%include "std_vector.i"

namespace std {
       %template(FloatVector) vector<float>;
       %template(StringVector) vector<string>;
}


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


};


%template(GetBranchEntry_Int) RootNtupleReaderTool::GetBranchEntry<int>;
%template(GetBranchEntry_FloatVector) RootNtupleReaderTool::GetBranchEntry<std::vector<float> >;
%template(GetBranchEntry_StringVector) RootNtupleReaderTool::GetBranchEntry<std::vector<std::string> >;

%include "cpointer.i"
%pointer_functions(int, intp);

