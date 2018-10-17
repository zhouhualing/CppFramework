# Unit tests for the Serialization module (export/import)

#INCLUDES=-I ../../../../../../common/CppUnitLite/ -I ..
INCLUDE= -I . -I /localhome/andy/develop/xerces-c/include -I ../../../../../../common/lib -I ../../../include
LIBS=-L /localhome/andy/develop/xerces-c/lib -lxerces-c -lstdc++ -L ../../../../../../common/lib/sqlite/Release -lsqlite3

CPPFLAGS=-g -Wall -Werror -DUSE_MOCK $(INCLUDE)
DEP_COMMAND=makedepend

# Comment in next two lines to enable profiling
#CPPFLAGS += -pg
#LDFLAGS += -pg

# Re-enable when time permits
# -Weffc++ 

#	TclExporter.cpp TclWithXmlExporter.cpp 
SRCS = AbstractRecursiveExporter.cpp AbstractRecursiveFileExporter.cpp \
	StringExporter.cpp \
	AbstractTclExporter.cpp TclExporter.cpp TclWithXmlExporter.cpp \
	XmlExporter.cpp XmlSpecificExporter.cpp XmlGenericExporter.cpp \
	Importable.cpp ImportableFactory.cpp \
	XmlSpecificImporter.cpp XmlSpecificSaxHandlers.cpp \
	ImexSaver.cpp ImexLoader.cpp \
	unitTest/MockScriptable.cpp unitTest/MockProvider.cpp unitTest/xmlTest.cpp
OBJS := $(patsubst %.cpp,%.o, $(SRCS))
OBJ_DIR = .

.cpp.o :
	${CC} $(CPPFLAGS) -o $@ -c $<

xmlTest: ${OBJS}
	g++ -o $@ ${OBJS} ${LDFLAGS} $(LIBS)

clean:
	rm ${OBJS}

dep:
	${DEP_COMMAND} -p${OBJ_DIR}/ -f- -- ${CFLAGS} ${INCLUDE} ${SRCS} > Makefile.dep

include Makefile.dep
