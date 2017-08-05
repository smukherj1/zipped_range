
CXX = clang++
BOOST_DIR = /mnt/d/Libs/boost_1_64_0
BOOST_LIBDIR = $(BOOST_DIR)/stage/lib/gcc-5.3
# Limiting to C++11 in order to ensure MSVC 2013 can compile
# this code
CFLAGS = -Wall -std=c++11 -O2 -I$(BOOST_DIR) -I.
CFLAGS += -Wno-missing-braces
LDFLAGS = -L$(BOOST_LIBDIR) $(BOOST_LIBDIR)/libboost_unit_test_framework.a
TARGET = test

test: test.cpp zipped_range.hpp zipped_range_detail.hpp
	@echo Compiling...
	$(CXX) $(CFLAGS) test.cpp -o $@ $(LDFLAGS)
	@echo Running $(TARGET)...
	@./$@

clean:
	-rm $(TARGET) *.o