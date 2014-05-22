CC=g++
CFLAGS=-std=c++11 -fPIC -Wall -I. -I../simplefwk-services -I../simplefwk-utilitytoolsinterfaces -c
LDFLAGS=-std=c++11 -shared 
LDFLAGSEXTRA=-install_name @rpath/UtilityTools/lib/
OBJDIR=build
LIBDIR=lib

all: $(OBJDIR) $(LIBDIR) python/_RootNtupleTools.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(LIBDIR):
	mkdir $(LIBDIR) 

python/_RootNtupleTools.so: lib/libRootNtupleTools.so python/RootNtupleTools_wrap.o
	$(CC) $(LDFLAGS) python/RootNtupleTools_wrap.o `python2.7-config --ldflags` -o python/_RootNtupleTools.so -Wl,-rpath,'$$ORIGIN/../lib' -Llib/ -lRootNtupleTools 

python/RootNtupleTools_wrap.o: python/RootNtupleTools_wrap.cxx
	$(CC) $(CFLAGS) python/RootNtupleTools_wrap.cxx `python2.7-config --cflags` -o python/RootNtupleTools_wrap.o

python/RootNtupleTools_wrap.cxx: python/RootNtupleTools.i
	swig -Wall -c++ -python python/RootNtupleTools.i

lib/libRootNtupleTools.so: build/RootNtupleWriterTool.o build/RootNtupleReaderTool.o build/type.o build/CommonRootBase.o
	$(CC) $(LDFLAGS) build/RootNtupleWriterTool.o build/RootNtupleReaderTool.o build/type.o build/CommonRootBase.o `root-config --libs` -o lib/libRootNtupleTools.so -Wl,-rpath,'$$ORIGIN/../../simplefwk-services/lib'  -L../simplefwk-services/lib -lServices 

build/RootNtupleWriterTool.o: src/RootNtupleWriterTool.cxx
	$(CC) $(CFLAGS) src/RootNtupleWriterTool.cxx `root-config --cflags` -o build/RootNtupleWriterTool.o

build/RootNtupleReaderTool.o: src/RootNtupleReaderTool.cxx
	$(CC) $(CFLAGS) src/RootNtupleReaderTool.cxx `root-config --cflags` -o build/RootNtupleReaderTool.o

build/type.o: src/type.cpp UtilityTools/type.hpp
	$(CC) $(CFLAGS) src/type.cpp -o build/type.o 

build/CommonRootBase.o: src/CommonRootBase.cxx UtilityTools/CommonRootBase.h
	$(CC) $(CFLAGS) src/CommonRootBase.cxx -o build/CommonRootBase.o

clean:
	rm -r build/
	rm -r lib/
	rm python/RootNtupleTools_wrap*
	rm python/RootNtupleTools.py*
	rm python/_RootNtupleTools.so
