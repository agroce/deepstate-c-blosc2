CBLOSC2=/home/user/c-blosc2/install
CBLOSC=/home/user/c-blosc/install

FILES=testcblosc2 testcblosc2_san testcblosc2_swarm testcblosc2_AFL testcblosc2_swarm_AFL testcblosc2_LF testcblosc2_swarm_LF testcblosc testcblosc_san testcblosc_swarm testcblosc_AFL testcblosc_swarm_AFL testcblosc_LF testcblosc_swarm_LF testcblosc 

all: $(FILES)

clean:
	rm -rf $(FILES)

testcblosc2: TestCBlosc2.cpp
	$(CXX) -o testcblosc2 TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2.a -ldeepstate -lpthread

testcblosc2_san: TestCBlosc2.cpp
	clang++ -o testcblosc2_san TestCBlosc2.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_san.a -ldeepstate -lpthread -fsanitize=address,undefined,integer

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

testcblosc: TestCBlosc.cpp
	$(CXX) -o testcblosc TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc.a -ldeepstate -lpthread

testcblosc_san: TestCBlosc.cpp
	clang++ -o testcblosc_san TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_san.a -ldeepstate -lpthread -fsanitize=address,undefined,integer

testcblosc_swarm: TestCBlosc.cpp
	$(CXX) -o testcblosc_swarm TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc_AFL: TestCBlosc.cpp
	afl-clang++ -o testcblosc_AFL TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_AFL.a -ldeepstate_AFL -lpthread

testcblosc_swarm_AFL: TestCBlosc.cpp
	afl-clang++ -o testcblosc_swarm_AFL TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_AFL.a -ldeepstate_AFL -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc_LF: TestCBlosc.cpp
	clang++ -o testcblosc_LF TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer

testcblosc_swarm_LF: TestCBlosc.cpp
	clang++ -o testcblosc_swarm_LF TestCBlosc.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer -DDEEPSTATE_PURE_SWARM
