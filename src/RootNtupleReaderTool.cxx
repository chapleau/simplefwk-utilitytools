// local includes
#include "UtilityTools/RootNtupleReaderTool.h"
#include "UtilityTools/type.hpp"
// STL includes

// FrameWork includes
#include <TFile.h>
#include <TTree.h>
#include <TClass.h>
#include <typeinfo>
#include <TDataType.h>
#include <TBranchElement.h>
#include <TLeaf.h>
//#include "TString.h"
#include <TROOT.h>

std::unordered_map<std::string, std::pair<TFile*,std::set<std::string> > > RootNtupleReaderTool::m_files_associated_trees;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
RootNtupleReaderTool::RootNtupleReaderTool(const std::string& name,
                                           const std::string& file_name,
                                           const std::string& tree_name) :
   Messaging  ( name ),
   m_ttree_name(tree_name), m_file_name(file_name), m_ttree(0)
{
  Register();
}

RootNtupleReaderTool::RootNtupleReaderTool(const std::string& name,
                                           const std::string& file_name,
                                           const std::string& tree_name, TLogLevel lvl) :
   Messaging  ( name, lvl ),
   m_ttree_name(tree_name), m_file_name(file_name), m_ttree(0)
{
   Register();
}

// Destructor
///////////////
RootNtupleReaderTool::~RootNtupleReaderTool()
{
  LOG("Finalizing " << name()<<" in Destructor", logDEBUG);
  
  auto file_found = m_files_associated_trees.find(m_file_name);
  if (file_found == m_files_associated_trees.end()) {
     //nothing to be done, file was closed by some other owner
     return;
  }
  
  TFile * f = m_files_associated_trees[m_file_name].first;
  
  if (f) {
    LOG("Closing file "<<m_file_name, logDEBUG);
  
    //m_file->Close();
    delete f; //Close() is called in destructor
    
    //remove from list so that we don't double-write/close/delete
    m_files_associated_trees.erase(file_found);
  }
  


}

void RootNtupleReaderTool::Register() {
    
  LOG ("Initializing " << name() << "... TTree = "<<m_ttree_name, logINFO);
  if (m_ttree_name == "") { LOG("TTree name unset", logERROR); return; }
  if (m_file_name == "") { LOG("File name unset", logERROR); return; }
  
  //pre-load std::vector<> dictionnary
  //only if we haven't done so yet
  if (!m_loaded_builtin_dicts) {
    gROOT->ProcessLine("#include <string>");
    gROOT->ProcessLine("#include <vector>");
    m_loaded_builtin_dicts = true;
  }

  
  auto file_found = m_files_associated_trees.find(m_file_name);
  
  if (file_found == m_files_associated_trees.end()) {
    
    LOG("New file to open.. "<<m_file_name,logDEBUG);
    
    TFile *f = new TFile(m_file_name.c_str(), "READ");
      if (!f || f->IsZombie()) {
         LOG("Error opening file", logERROR);
         return;
      }
    auto &v = m_files_associated_trees[m_file_name];
    v.first = f;
      
  }
  
  LOG("Root path: "<< gDirectory->GetPath(), logDEBUG);
  
  auto & set_of_trees = m_files_associated_trees[m_file_name].second;
  
  auto tree_found = set_of_trees.find(m_ttree_name);
  
  //not necessary to enforce this but might avoid some duplications..
  if (tree_found != set_of_trees.end()) { LOG("Prob, TTree with name ["<<m_ttree_name<<"] already loaded..", logERROR); return; }
  
  
  m_ttree = 0;
  m_files_associated_trees[m_file_name].first->GetObject(m_ttree_name.c_str(), m_ttree);
  
  if (!m_ttree) { LOG("Cannot find TTree with name ["<<m_ttree_name<<"] in file "<<m_file_name, logERROR); return; }
  
  set_of_trees.emplace(m_ttree_name);

}


template <class T>
const T* RootNtupleReaderTool::GetBranchEntry(const std::string& branch_name, unsigned long ientry) {

 if (!m_ttree) return (T*)0;
 
 LOG("Trying to retrieve branch ["<<m_ttree_name<<"/"<<branch_name<<"] at entry "<<ientry, logDEBUG);
 
 if (m_branch_coll.find(branch_name) == m_branch_coll.end()) {
 
    LOG("New branch to collect ["<<m_ttree_name<<"/"<<branch_name<<"]", logDEBUG);
    
    TBranch* & br = m_branch_coll[branch_name];
    br = m_ttree->GetBranch(branch_name.c_str());
    if (!br) { LOG("Couldn't retrieve branch! ",logERROR); return (T*)0; }
    //let the branch deal with the ownership of the data
    br->SetAddress(0);
    
 }//collecting branch info
 
 
 TBranch* & br = m_branch_coll.at(branch_name);
 
 //just to be on the safe side
 if (!br) return (T*)0;

 //first make sure requested type matches expected one

 TClass * exp_cl = 0;
 EDataType exp_typ;
 br->GetExpectedType(exp_cl, exp_typ);

 if ( br->IsA() == TBranchElement::Class() && exp_cl) {
    //dealing with object/class
    //check if T correspond to expected class
    
    if ( exp_cl->GetTypeInfo() && *(exp_cl->GetTypeInfo()) == typeid(T)) {
        //get entry
        m_status[branch_name] = br->GetEntry(ientry);
        return (m_status[branch_name]>0) ? (const T*)dynamic_cast<TBranchElement*>(br)->GetObject() : 0; //this will likely perfrom a reinterpret_cast...ughh
    }
    else {LOG("Wrong type requested (class) "<<exp_cl->GetTypeInfo(), logERROR);}
 }
 else {
    //dealing with fundamental type
    EDataType d_type = TDataType::GetType(typeid(T));
    if ( d_type == exp_typ) {
       //get entry
       m_status[branch_name] = br->GetEntry(ientry);
       TLeaf * leaf = br->GetLeaf(branch_name.c_str());
       if (!leaf) { LOG("Empty branch...?", logERROR); return 0; }
       
       return (m_status[branch_name]>0) ? static_cast<const T*>(leaf->GetValuePointer()) : 0;
    }
    else {LOG("Wrong type requested (type) "<<d_type<<" "<<exp_typ, logERROR);}
 }

 return (T*)0;
}

template const int* RootNtupleReaderTool::GetBranchEntry(const std::string&, unsigned long);
template const std::vector<float>* RootNtupleReaderTool::GetBranchEntry(const std::string&, unsigned long);
template const std::vector<double>* RootNtupleReaderTool::GetBranchEntry(const std::string&, unsigned long);
template const std::vector<std::string>* RootNtupleReaderTool::GetBranchEntry(const std::string&, unsigned long);










