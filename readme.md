#Simple framework - Utility tools

``simplefwk-utilitytools`` contains two helper tools that abstract low-level details of ROOT's I/O infrastructure. [ROOT][] is a C++ library developed and used primarily by the High Energy Physics community. It is versatile enough to be useful in many applications dealing with large amounts of data. 

##Installation

A makefile is provided for unix-like operating systems (tested on Mac OS X 10.7.5). Requirements: 
* C++11 compliant c++ compiler (e.g. g++ 4.8)
* Python 2.x (e.g. version 2.7.1)
* [swig][] 2.x 
* ROOT 5.x 

(All the above can be obtained through [macports][] for Max OS X systems.)

* ``simplefwk-services`` package
* ``simplefwk-utilitytoolsinterfaces`` package

##Usage
###Data storage in ROOT

ROOT has its own I/O system that allows to persistify objects (in the Object-Oriented sense) on disk. It provides a data structure called a _tree_ (or a [TTree][]) designed to reduce disk space and enhance data access speed when storing large quantities of same-class objects. A ROOT tree may have many entries and consists in a collection of branches representing objects to be stored once per entry. Branches can then be read back indivdually, as needed, on an entry-by-entry basis. 

###Writing to a TTree

``RootNtupleWriterTool`` is the helper tool to be used to seamlessly output ntuple-like data to a ROOT file (using the TTree data structure). 
The simplest constructor is as follow:

````c++
RootNtupleWriterTool::RootNtupleWriterTool(const std::string& name,       //name of the tool
                                           const std::string& file_name,  //ROOT file name
                                           const std::string& tree_name)  //name of TTree
````

A tool that receives a pointer to an initialized ``RootNtupleWriterTool`` only need to use two methods: one to register branches, and one to fill up branches [With the exception of the event-wise steering, which will be discussed below].

####Registering branches
A branch is created in the TTree through the following method:

````c++
int RootNtupleWriterTool::registerBranch(std::string branch_name, IObjectHolder* obj)
````

Here, the first argument is self-explanatory (a string representing the name of the branch to be created) while the second is a pointer to a newly created object (inheriting from the ``IObjectHolder`` interface) that implements data/memory management functions specific to the class associated with the branch. This extra level of abstraction allows for a generic design where the details of the data to be stored are completely decoupled from the writer tool itself. Concrete ``IObjectHolder`` classes are available in the ``simplefwk-utilitytoolsinterfaces`` package for primitive data types and for STL containers (``std::vector`` notably). The writer tool takes ownership of the ``IObjectHolder`` object.

A single ``IObjectHolder`` can be used to register _child_ branches. It can, for instance, provide a unique suffix (for each child branch) that will be appended to the ``branch_name`` string by the writer tool. Different member variables within a class can then be stored in independent branches.   

Since ROOT can only (de)serialize data into a file if it knows about it (through what is called a _dictionnary_), every request for branch creation performs type safety checks to assess the compatibility of the branch data type with ROOT's I/O system.

####Filling up branches

Once a branch has been registered, it can be filled with the following method:

````c++
int RootNtupleWriterTool::pushBack(std::string branch_name, const boost::any& data)
````

Here, ``boost::any`` is used for data type abstraction. Safety checks are perform to make sure the data being passed can be handled by the branch's ``IObjectHolder``. An unregistered branch triggers an error.

####Steering

The event steering is provided by the Incident Service (see ``simplefwk-services``). An example, in Python, would look like the following:

````python
root_svc = RootNtupleWriterTool("RootTool", "tree.root", "ttree")
mytool = ToolThatOutputsData(root_svc)

inc_svc = IncidentService.getInstance()

inc_svc.fireIncident(Incident("BeginRun"))

for row in myfile:

  inc_svc.fireIncident(Incident("BeginEvent"))
  
  mytool.execute(row)
  
  inc_svc.fireIncident(Incident("EndEvent"))
  
inc_svc.fireIncident(Incident("EndRun"))  
````

Here, the tool named _mytool_ can register branches in its constructor or more conveniently when the **BeginRun** incident is fired (by listening for this particular incident). 

The **BeginEvent** incident is used by the ROOT writer tool to clear the content of the various ``IObjectHolder``s in preparation of ``RootNtupleWriterTool::pushBack`` calls by _mytool_. When the **EndEvent** incident is fired, the TTree is filled with the data stored in the ``IObjectHolder``s for the current event.

During the **EndRun** incident, the TTree is written on disk to the file specified by the user (here, tree.root). 

###Reading from a TTree



[ROOT]: http://root.cern.ch
[TTree]: http://root.cern.ch/root/html/TTree.html
[swig]: http://swig.org
[macports]: http://www.macports.org/
