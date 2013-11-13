#ifndef UTILITYTOOLS_ROOTNTUPLEWREADERTOOL_H
#define UTILITYTOOLS_ROOTNTUPLEWREADERTOOL_H 1

// STL includes
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <utility>

#include "Services/log_levels.h"
#include "Services/Messaging.h"

#include "UtilityTools/CommonRootBase.h"

// Forward declaration

class TTree;
class TFile;
class TBranch;

class RootNtupleReaderTool
  : public Messaging, public CommonRootBase
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RootNtupleReaderTool(const std::string&,const std::string&,const std::string& );
  RootNtupleReaderTool(const std::string&, const std::string&,const std::string&, TLogLevel);
  

  /// Destructor: 
  virtual ~RootNtupleReaderTool();

  template <class T>
  const T* GetBranchEntry(const std::string&, unsigned long);

 protected:
 
  void Register();
  
  std::string m_ttree_name;
  std::string m_file_name;
  
  TTree * m_ttree;
  //TFile * m_file;
  
  static std::unordered_map<std::string, std::pair<TFile*, std::set<std::string> > > m_files_associated_trees;
  
  std::unordered_map<std::string, TBranch * > m_branch_coll;
  std::unordered_map<std::string, int > m_status;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  RootNtupleReaderTool();
    
  

  // Containers
  

}; 


#endif