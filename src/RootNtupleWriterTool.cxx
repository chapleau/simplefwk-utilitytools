///////////////////////// -*- C++ -*- /////////////////////////////
// RootNtupleWriterTool.cxx 
// Implementation file for class RootNtupleWriterTool
/////////////////////////////////////////////////////////////////// 
  
// local includes
#include "UtilityTools/RootNtupleWriterTool.h"
#include "Services/IncidentService.h"
#include "UtilityTools/type.hpp"
// STL includes

// FrameWork includes
#include <TFile.h>
#include "TTree.h"
#include "TClass.h"
#include <typeinfo>
#include "TDataType.h"
#include "TString.h"
#include "TROOT.h"


//from TTree.cxx
static char DataTypeToChar(EDataType datatype)
{
    // Return the leaflist 'char' for a given datatype.
    
    switch(datatype) {
        case kChar_t:     return 'B';
        case kUChar_t:    return 'b';
        case kBool_t:     return 'O';
        case kShort_t:    return 'S';
        case kUShort_t:   return 's';
        case kCounter:
        case kInt_t:      return 'I';
        case kUInt_t:     return 'i';
        case kDouble_t:
        case kDouble32_t: return 'D';
        case kFloat_t:
        case kFloat16_t:  return 'F';
        case kLong_t:     return 0; // unsupported
        case kULong_t:    return 0; // unsupported?
        case kchar:       return 0; // unsupported
        case kLong64_t:   return 'L';
        case kULong64_t:  return 'l';
            
        case kCharStar:   return 'C';
        case kBits:       return 0; //unsupported
            
        case kOther_t:
        case kNoType_t:
        default:
            return 0;
    }
    return 0;
}

std::unordered_multimap<std::size_t, std::string> RootNtupleWriterTool::m_loaded_types;
std::unordered_map<std::string, std::set<std::string> > RootNtupleWriterTool::m_files_associated_trees;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
RootNtupleWriterTool::RootNtupleWriterTool(const std::string& name,
                                           const std::string& file_name,
                                           const std::string& tree_name,
                                           bool single) :
   Messaging  ( name ),
   m_ttree_name(tree_name), m_file_name(file_name), m_ttree(0), m_file(0), m_lock_tree(false)
{
  Register(single);
}

RootNtupleWriterTool::RootNtupleWriterTool(const std::string& name,
                                           const std::string& file_name,
                                           const std::string& tree_name, TLogLevel lvl,
                                           bool single) :
   Messaging  ( name, lvl ),
   m_ttree_name(tree_name), m_file_name(file_name), m_ttree(0), m_file(0), m_lock_tree(false)
{
   Register(single);
}

// Destructor
///////////////
RootNtupleWriterTool::~RootNtupleWriterTool()
{}


void RootNtupleWriterTool::Register(bool single) {

   // Get handle on IncidentSvc
  IncidentService * inc_svc = IncidentService::getInstance();
  if (!inc_svc) {LOG("Coulnd't get IncidentService", logERROR); return; }
  
  // Register listener for BeginRun
  inc_svc->addListener(this, "EndEvent",0,single);
  inc_svc->addListener(this, "BeginEvent",0,single);
  
  //no need for these guys to be executed more than once..
  inc_svc->addListener(this, "EndRun",0, true);
  inc_svc->addListener(this, "BeginRun",99, true); //first one!
  

}

void RootNtupleWriterTool::cleanup() {
   // Get handle on IncidentSvc
  IncidentService * inc_svc = IncidentService::getInstance();
  if (!inc_svc) {LOG("Coulnd't get IncidentService", logWARNING);}
  
  if (inc_svc) {
       //de-registering ourself from the incident service
       inc_svc->removeListener(this, "EndEvent");
       inc_svc->removeListener(this, "BeginEvent");
      
       //singleShot
       //inc_svc->removeListener(this, "EndRun");
       //inc_svc->removeListener(this, "BeginRun");
  }

  for (coll_store_t::iterator it = m_collections.begin(); it != m_collections.end(); ++it) {
      if ((*it).second) delete (*it).second; 
      (*it).second = 0;
  }

  m_collections.clear();

}

// Initialization
////////////////////////////
int RootNtupleWriterTool::initialize()
{

  //pre-load std::vector<> dictionnary
  //only if we haven't done so yet
  if (m_files_associated_trees.empty()) {
    gROOT->ProcessLine("#include <string>");
    gROOT->ProcessLine("#include <vector>");
  }
  
  m_lock_tree = false;
    
  LOG ("Initializing " << name() << "... TTree = "<<m_ttree_name, logINFO);
  if (m_ttree_name == "") { LOG("TTree name unset", logERROR); return 0; }
  if (m_file_name == "") { LOG("File name unset", logERROR); return 0; }
  
  auto file_found = m_files_associated_trees.find(m_file_name);
  
  if (file_found == m_files_associated_trees.end()) {
    
    LOG("New file to open.. "<<m_file_name,logDEBUG);
    
    m_file = new TFile(m_file_name.c_str(), "RECREATE");
      if (m_file->IsZombie()) {
         LOG("Error opening file", logERROR);
         return 0;
      }
  }
  
  LOG("Root path: "<< gDirectory->GetPath(), logDEBUG);
  
  auto & set_of_trees = m_files_associated_trees[m_file_name];
  
  auto tree_found = set_of_trees.find(m_ttree_name);
  
  if (tree_found != set_of_trees.end()) { LOG("Prob, TTree with name ["<<m_ttree_name<<"] already created..", logERROR); return 0; }
  
  // ? set working dir
  
  m_ttree = new TTree(m_ttree_name.c_str(), m_ttree_name.c_str());
  set_of_trees.emplace(m_ttree_name);

  // ? cd to previous working dir..

  m_loaded_types.clear();
  
  return 1;
  
}


// we shall stop filling the TTree
/////////////////
void RootNtupleWriterTool::stop() {

     // Get handle on IncidentSvc
  IncidentService * inc_svc = IncidentService::getInstance();
  if (!inc_svc) {LOG("Coulnd't get IncidentService", logERROR); return; }
  
  //deregister ourselves
  inc_svc->removeListener(this, "EndEvent");
  inc_svc->removeListener(this, "BeginEvent");

}

/////////////////////////// cleanup
int RootNtupleWriterTool::finalize()
{

  LOG("Finalizing " << name(), logINFO);
  
  
  auto file_found = m_files_associated_trees.find(m_file_name);
  if (file_found == m_files_associated_trees.end()) {
     //nothing to be done, file was written out and closed by some other owner
     cleanup();
     return 1;
  }
  
  if (m_file) {
    LOG("Writing, Closing file "<<m_file_name, logDEBUG);
  
    m_file->Write();
    //m_file->Close();
    delete m_file; //Close() is called in destructor
    
    //remove from list so that we don't double-write/close/delete
    m_files_associated_trees.erase(file_found);
  }
  
  //delete m_file will delete all associated TObject in same TDirectory
  
  //delete m_ttree;
  
  
  //perform some clean-up
  cleanup();
  
  return 1;
} 
////////////////////////////////////////////////////////////////////////////////
int RootNtupleWriterTool::registerBranch(std::string branch_name, IObjectHolder* obj) {
    
    if ( !m_ttree) LOG("No TTree..", logWARNING);
   
    
    if (m_lock_tree ) {
        LOG("TTree is locked...cannot add any more branches", logERROR);
        return 0;
    }
    
    
    if (  m_collections.find(branch_name) != m_collections.end() ) {
        LOG("Branch ["<<branch_name<<"] already registered !", logERROR);
        return 0;
    }
    
    //not much guarantee about std::type_info::name though..
    //if two names for same type, type will be loaded twice..ok
    //if two types for same name, only the first one will be loaded
    //use hash_code instead..not much guarantee either actually, (collisions)
    //std says same value if two type_info are equal (not necessarily a if-and-only-if).
    //so use a multi_map !

    //could we store pointers to type_info instead ? ... depends on the implementation of typeid (?)
    
    std::string typeid_name = obj->getTypeInfo().name();
    //c++11
    std::size_t typeid_hash  = obj->getTypeInfo().hash_code();
    
        
    LOG("Branch ["<<branch_name<<"] is of type (from typeid): "<<typeid_name<< " ("<<demangle(typeid_name.c_str())<<") with hash_code: "<<typeid_hash, logDEBUG);
    
    EDataType type_t = TDataType::GetType( obj->getTypeInfo() );
    if ( !( type_t == kOther_t || type_t == kNoType_t) ) {
        LOG("  Dealing with fundamental type: "<<TDataType::GetTypeName(type_t), logDEBUG);
        TString varname; varname.Form("%s/%c",branch_name.c_str(),DataTypeToChar(type_t));
        LOG("  "<<varname.Data(), logDEBUG);
        //taking ownership of the ObjectHolder
        m_collections[branch_name] = obj;
        
        std::vector<std::pair<void*, std::string> > ptrs;
        obj->getRawPointers(ptrs);
        if (ptrs.size() != 1 ) { LOG("ptrs size not 1 !?", logERROR); return 0;}
        if (m_ttree) {
           if ( !(m_ttree->Branch(branch_name.c_str(),ptrs[0].first,varname.Data())) ) {LOG("Prob with branch", logWARNING);}
           else {LOG("  ..OK", logDEBUG);}
        }
 
    }
    else {
        //Load dictionnaries
        int count_cl = m_loaded_types.count(typeid_hash);
        if (count_cl > 0) {
            
            auto range = m_loaded_types.equal_range(typeid_hash);
            auto it_range = range.first;
            for ( ; it_range != range.second; ++it_range) {
                const std::string & cl_name = it_range->second;
                //check if typeids compare equal
                //class should already be loaded, should be quick..
                TClass * cl = TClass::GetClass(cl_name.c_str(), false, true); 
                if (!cl) { // bad
                    LOG("class "<<cl_name<<" not found..?", logERROR);
                    continue;
                }
                
                if ( cl->GetTypeInfo() && *(cl->GetTypeInfo()) == obj->getTypeInfo()) //found it !
                    break;
            }
            
            if ( it_range != range.second ) {
                //we found a positive match..
                const std::string& class_name =  it_range->second;
                LOG("  Type Already loaded... "<<class_name, logDEBUG);
                obj->setClassName(class_name);
            }
            else {
                LOG("Same hash, but no positive match..",logDEBUG);
                count_cl = 0; //so that it's picked up below
            }
        }
        
        if (count_cl == 0){ //count_cl == 0, or no positive match
            
            TClass * cl_typeid = TClass::GetClass(obj->getTypeInfo());
            if (!cl_typeid) {
                  LOG("  Class isn't pre-loaded.. ", logINFO);
                  std::string demangled_name = demangle(typeid_name.c_str());
                  cl_typeid = TClass::GetClass(demangled_name.c_str());
                
                
                  LOG(demangled_name<<": "<< cl_typeid <<" -  "<<cl_typeid->GetTypeInfo(), logINFO);
                  if (!cl_typeid || !(cl_typeid->GetTypeInfo()) || *(cl_typeid->GetTypeInfo()) != obj->getTypeInfo() )
                     {LOG("  No ROOT dictionnary available for "<<demangled_name, logERROR); return 0;}
                
            }
            m_loaded_types.emplace(typeid_hash, cl_typeid->GetName());
            LOG("  Dictionnary found: "<<cl_typeid->GetName(), logDEBUG);
            obj->setClassName(cl_typeid->GetName());
            
        } //not loaded
        
        
        //taking ownership of the ObjectHolder
        m_collections[branch_name] = obj;
        
        std::vector<std::pair<void*, std::string> > ptrs;
        obj->getRawPointers(ptrs);
        if (m_ttree) {
          for ( std::size_t i = 0; i < ptrs.size(); ++i)
            m_ttree->Bronch((branch_name+ptrs[i].second).c_str(),obj->getClassName().c_str(), ptrs[i].first);
        }
        
    } //if not a primitive/fundamental type
    
  
  
  return 1;
}

////////////////////////////////////////////////////////////////////////////////  
int RootNtupleWriterTool::pushBack(std::string branch_name, const boost::any& data) {
    
    if (  m_collections.find(branch_name) == m_collections.end() ) {
        LOG("Branch ["<<branch_name<<"] not registered !", logERROR);
        return 0;
    }
    
    IObjectHolder* &obj = m_collections[branch_name];
    if (!obj) { LOG("Null pointer for ObjectHolder for Branch ["<<branch_name<<"]", logERROR); return 0; }
    
    if ( !(obj->push_back(data)) ) { LOG("Bad cast in push_back for Branch ["<<branch_name<<"]", logERROR); return 0; }
  
  return 1;
}


/////////////////////////////

IObjectHolder* RootNtupleWriterTool::getObjectHolder(std::string branch_name) {
    
    if (  m_collections.find(branch_name) == m_collections.end() ) {
        LOG("Branch ["<<branch_name<<"] not registered !", logERROR);
        return 0;
    }
    
    
    return m_collections[branch_name];

}


////////////////////////////////////////////////////////////////////////////////
// Handle incidents
////////////////////////////////////////////////////////////////////////////////
void RootNtupleWriterTool::handle(const Incident& inc)
{


  if (inc.svcType()=="EndEvent") {
     LOG("in EndEvent", logDEBUG);
      
     //see if branches are non-empty
     bool isempty = true;
     for (coll_store_t::iterator it = m_collections.begin(); it != m_collections.end(); ++it) {
          if ((*it).second) {
              if ((*it).second->isempty() == false) { isempty = false; break; }
          }
     }
 
     //fill the TTree
      if ( !isempty && m_ttree )
          m_ttree->Fill();
      
  } else if (inc.svcType() == "BeginEvent") {
      LOG("in BeginEvent", logDEBUG);

      //lock the TTree, cannot add any more branches at this stage.
      m_lock_tree = true; 
      
      //clear the content of the various ObjectHolder's
      for (coll_store_t::iterator it = m_collections.begin(); it != m_collections.end(); ++it) {
          if ((*it).second) {
              if ( !((*it).second->clear()) ) { LOG("Couldn't clear object holder for branch "<<(*it).first , logWARNING); }
          } else { LOG("Null object holder for branch "<<(*it).first, logWARNING); }
      } //for loop
          
  } else if (inc.svcType() == "BeginRun") {
  
     if ( !(this->initialize())) { LOG("Couldn't initialize properly..", logERROR); }
     
  } else if (inc.svcType() == "EndRun") {
  
     if ( !(this->finalize())) { LOG("Couldn't finalize properly..", logERROR); }
     
  }  //else if
  
  

} //void





