#CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT)  -I../base -I../base/xml -Wall -D_GNU_SOURCE -D_REENRANT `mysql_config --cflags` `xml2-config --cflags` 
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -L../base -L../base/xml -lzebra -lencdec -llzma -lpthread -lrt -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx 
LIB_ZEBRA = ../base/libzebra.a

ALL_TARGETS = xmlpg codepg charbaseViewer dbhelp messagehelp

all: $(ALL_TARGETS)

$(OBJS) : %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

include .xmlpg.d
.xmlpg.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
xmlpg: xmlpg.o $(LIB_ZEBRA)
	$(CXX) xmlpg.o -o $@ $(CXXLFLAGS)


include .codepg.d
.codepg.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
codepg: codepg.o $(LIB_ZEBRA)
	$(CXX) codepg.o -o $@ $(CXXLFLAGS)

include .charbaseViewer.d
.charbaseViewer.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
charbaseViewer: charbaseViewer.o $(LIB_ZEBRA)
	$(CXX) charbaseViewer.o -o $@ $(CXXLFLAGS)

include .dbhelp.d
.dbhelp.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
dbhelp: dbhelp.o $(LIB_ZEBRA)
	$(CXX) dbhelp.o -o $@ $(CXXLFLAGS)

include .messagehelp.d
.messagehelp.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
messagehelp: messagehelp.o $(LIB_ZEBRA)
	$(CXX) messagehelp.o -o $@ $(CXXLFLAGS)

clean:
	@rm -rf *.o $(ALL_TARGETS) core *~
