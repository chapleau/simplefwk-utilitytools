CC=g++-mp-4.8
CFLAGS=-std=c++11 -fPIC -Wall -I. -I../Services -I../UtilityToolsInterfaces -c
LDFLAGS=-std=c++11 -shared 
LDFLAGSEXTRA=-install_name @rpath/UtilityTools/lib/
OBJDIR=build
LIBDIR=lib

all: $(OBJDIR) $(LIBDIR) python/_RootNtupleWriterTool.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(LIBDIR):
	mkdir $(LIBDIR) 

python/_RootNtupleWriterTool.so: lib/libRootNtupleWriterTool.so python/RootNtupleWriterTool_wrap.o
	$(CC) $(LDFLAGS) python/RootNtupleWriterTool_wrap.o -lpython2.7 -Llib/ -lRootNtupleWriterTool -Xlinker -rpath -Xlinker `dirname \`pwd\`` -o python/_RootNtupleWriterTool.so

python/RootNtupleWriterTool_wrap.o: python/RootNtupleWriterTool_wrap.cxx
	$(CC) $(CFLAGS) python/RootNtupleWriterTool_wrap.cxx -I/usr/include/python2.7/ -o python/RootNtupleWriterTool_wrap.o

python/RootNtupleWriterTool_wrap.cxx:
	swig -Wall -c++ -python python/RootNtupleWriterTool.i

lib/libRootNtupleWriterTool.so: build/RootNtupleWriterTool.o build/type.o
	$(CC) $(LDFLAGS) $(LDFLAGSEXTRA)libRootNtupleWriterTool.so build/RootNtupleWriterTool.o build/type.o `root-config --libs` -L../Services/lib -lServices -Xlinker -rpath -Xlinker `dirname \`pwd\`` -o lib/libRootNtupleWriterTool.so

build/RootNtupleWriterTool.o: src/RootNtupleWriterTool.cxx
	$(CC) $(CFLAGS) src/RootNtupleWriterTool.cxx `root-config --cflags` -o build/RootNtupleWriterTool.o

build/type.o: src/type.cpp UtilityTools/type.hpp
	$(CC) $(CFLAGS) src/type.cpp -o build/type.o 


clean:
	rm -r build/
	rm -r lib/
	rm python/RootNtupleWriterTool_wrap*
	rm python/RootNtupleWriterTool.py*
	rm python/_RootNtupleWriterTool.so
