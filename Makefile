.PHONY: all

all: checksize

checksize: checksize.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) checksize.cpp -o checksize

dist-clean:
	@$(RM) checksize
