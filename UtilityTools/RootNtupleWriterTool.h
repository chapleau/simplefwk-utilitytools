///////////////////////// -*- C++ -*- /////////////////////////////
// RootNtupleWriterTool.h 
// Header file for class RootNtupleWriterTool
/////////////////////////////////////////////////////////////////// 
#ifndef UTILITYTOOLS_ROOTNTUPLEWRITERTOOL_H
#define UTILITYTOOLS_ROOTNTUPLEWRITERTOOL_H 1

// STL includes
#include <string>
#include <unordered_map>
#include <set>

// RootNtupleWriterTool includes
#include "UtilityToolsInterfaces/IRootNtupleWriterTool.h"
#include "Services/IIncidentListener.h"
#include "Services/log_levels.h"
#include "Services/Messaging.h"

// Forward declaration

class TTree;
class TFile;

class RootNtupleWriterTool
  : virtual public IRootNtupleWriterTool,
    virtual public IIncidentListener, public Messaging
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RootNtupleWriterTool(const std::string&,const std::string&,const std::string& );
  RootNtupleWriterTool(const std::string&, const std::string&,const std::string&, TLogLevel);
  

  /// Destructor: 
  virtual ~RootNtupleWriterTool(); 

  // Athena algtool's Hooks
  virtual int  initialize();
  virtual int  finalize();

  virtual int registerBranch(std::string, IObjectHolder*);

  virtual int pushBack(std::string, const boost::any&);
  
  virtual void stop();
  
  virtual IObjectHolder* getObjectHolder(std::string);

  //IIncidentListener impl
  virtual void handle(const Incident&);


 protected:
 
 
  std::string m_ttree_name;
  std::string m_file_name;

  typedef std::unordered_map<std::string,IObjectHolder*> coll_store_t;
  coll_store_t m_collections;

  
  TTree * m_ttree;
  TFile * m_file;
  
  static std::unordered_map<std::string, std::set<std::string> > m_files_associated_trees;
  
    
  void Register();
    
  //share among instances of the tool
  static std::unordered_map<std::size_t, std::string> m_loaded_types;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  RootNtupleWriterTool();
    
  bool m_lock_tree;

  // Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> 
