CC=g++-mp-4.8
CFLAGS=-std=c++11 -fPIC -Wall -I. -I../Services -I../UtilityToolsInterfaces -c
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
	$(CC) $(LDFLAGS) python/RootNtupleTools_wrap.o -lpython2.7 -Llib/ -lRootNtupleTools -Xlinker -rpath -Xlinker `dirname \`pwd\`` -o python/_RootNtupleTools.so

python/RootNtupleTools_wrap.o: python/RootNtupleTools_wrap.cxx
	$(CC) $(CFLAGS) python/RootNtupleTools_wrap.cxx -I/usr/include/python2.7/ -o python/RootNtupleTools_wrap.o

python/RootNtupleTools_wrap.cxx: python/RootNtupleTools.i
	swig -Wall -c++ -python python/RootNtupleTools.i

lib/libRootNtupleTools.so: build/RootNtupleWriterTool.o build/RootNtupleReaderTool.o build/type.o build/CommonRootBase.o
	$(CC) $(LDFLAGS) $(LDFLAGSEXTRA)libRootNtupleTools.so build/RootNtupleWriterTool.o build/RootNtupleReaderTool.o build/type.o build/CommonRootBase.o `root-config --libs` -L../Services/lib -lServices -Xlinker -rpath -Xlinker `dirname \`pwd\`` -o lib/libRootNtupleTools.so

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
