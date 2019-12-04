CBLOSC2=/home/user/c-blosc2/install

all: testcblosc2 testcblosc2_swarm testcblosc2_AFL testcblosc2_swarm_AFL testcblosc2_LF testcblosc2_swarm_LF

clean:
	rm -rf testcblosc2 testcblosc2_swarm testcblosc2_AFL testcblosc2_swarm_AFL testcblosc2_LF testcblosc2_swarm_LF

testcblosc2: TestCBlosc2.cpp
	$(CXX) -o testcblosc2 TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2.a -ldeepstate -lpthread

testcblosc2_swarm: TestCBlosc2.cpp
	$(CXX) -o testcblosc2_swarm TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc2_AFL: TestCBlosc2.cpp
	afl-clang++ -o testcblosc2_AFL TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_AFL.a -ldeepstate_AFL -lpthread

testcblosc2_swarm_AFL: TestCBlosc2.cpp
	afl-clang++ -o testcblosc2_swarm_AFL TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_AFL.a -ldeepstate_AFL -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc2_LF: TestCBlosc2.cpp
	clang++ -o testcblosc2_LF TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer

testcblosc2_swarm_LF: TestCBlosc2.cpp
	clang++ -o testcblosc2_swarm_LF TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer -DDEEPSTATE_PURE_SWARM
