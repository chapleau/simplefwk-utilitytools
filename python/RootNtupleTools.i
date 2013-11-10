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
  RootNtupleWriterTool(const std::string&,const std::string&,const std::string& );
  RootNtupleWriterTool(const std::string&, const std::string&,const std::string&, TLogLevel);
        
  ~RootNtupleWriterTool();
};

%include "std_vector.i"

namespace std {
       %template(FloatVector) vector<float>;
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

%include "cpointer.i"
%pointer_functions(int, intp);
