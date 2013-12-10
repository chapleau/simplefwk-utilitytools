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

ROOT has its own I/O system that allows to persistify objects (in the Object-Oriented sense) on disk. It provides a data structure called a tree (or a [TTree][]) designed to reduce disk space and enhance data access speed. A tree is a collection of branches representing variables whose values are to be stored multiple times. Branches can then be read back indivdually, as needed, on an entry-by-entry basis. 

###Writing to a TTree

``RootNtupleWriterTool`` is the helper tool to be used to seamlessly output ntuple-like data to a ROOT file. 
The simplest constructor is as follow:

```c++
RootNtupleWriterTool::RootNtupleWriterTool(const std::string& name,       //name of the tool
                                           const std::string& file_name,  //ROOT file name
                                           const std::string& tree_name)  //name of TTree
```

A tool that receives a pointer to an initialized ``RootNtupleWriterTool`` only need to use two methods: one to register branches, and one to fill up branches [With the exception of the event-wise steering, which will be discussed below].

####Registering branches
A branch is created in the TTree through the following method:

```c++
int RootNtupleWriterTool::registerBranch(std::string branch_name, IObjectHolder* obj)
````

Here, the first argument is self-explanatory while the second is a pointer to a newly created (in-place) object (inheriting from the ``IObjectHolder`` interface) that implements data management functions specific to the type of object to be stored. This extra level of abstraction allows for a generic design where the details of the data to be stored are completely decoupled from the writer tool itself. Concrete ``IObjectHolder`` classes are available in the ``simplefwk-utilitytoolsinterfaces`` package for primitive data types and for STL containers (``std::vector`` notably).

Since ROOT can only (de)serialize data into a file if it knows about it (through what is called a dictionnary), every request for branch creation perform type safety checks to assess the compatibility of the branch data type with ROOT's I/O system.

####Filling up branches

###Reading from a TTree



[ROOT]: http://root.cern.ch
[TTree]: http://root.cern.ch/root/html/TTree.html
[swig]: http://swig.org
[macports]: http://www.macports.org/
