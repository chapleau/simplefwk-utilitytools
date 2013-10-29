%module RootNtupleWriterTool
%{
#include "UtilityTools/RootNtupleWriterTool.h"
%}

%include "std_string.i"

enum TLogLevel  {logERROR, logWARNING, logINFO, logDEBUG, logVERBOSE};

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