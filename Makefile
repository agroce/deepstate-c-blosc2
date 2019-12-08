CBLOSC2=/home/user/c-blosc2/install
CBLOSC=/home/user/c-blosc/install

FILES=testcblosc2 testcblosc2_san testcblosc2_swarm testcblosc2_AFL testcblosc2_swarm_AFL testcblosc2_LF testcblosc2_swarm_LF testcblosc testcblosc_swarm testcblosc_AFL testcblosc_swarm_AFL testcblosc_LF testcblosc_swarm_LF testcblosc 

all: $(FILES)

clean:
	rm -rf $(FILES)

testcblosc2: TestCBloscs.cpp
	$(CXX) -o testcblosc2 TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2.a -ldeepstate -lpthread

testcblosc2_san: TestCBloscs.cpp
	clang++ -o testcblosc2_san TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_san.a -ldeepstate -lpthread -fsanitize=address,undefined,integer

testcblosc2_swarm: TestCBloscs.cpp
	$(CXX) -o testcblosc2_swarm TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc2_AFL: TestCBloscs.cpp
	afl-clang++ -o testcblosc2_AFL TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_AFL.a -ldeepstate_AFL -lpthread

testcblosc2_swarm_AFL: TestCBloscs.cpp
	afl-clang++ -o testcblosc2_swarm_AFL TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_AFL.a -ldeepstate_AFL -lpthread -DDEEPSTATE_PURE_SWARM

testcblosc2_LF: TestCBloscs.cpp
	clang++ -o testcblosc2_LF TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer

testcblosc2_swarm_LF: TestCBloscs.cpp
	clang++ -o testcblosc2_swarm_LF TestCBloscs.cpp -I$(CBLOSC2)/include $(CBLOSC2)/lib/libblosc2_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer -DDEEPSTATE_PURE_SWARM

testcblosc: TestCBloscs.cpp
	$(CXX) -o testcblosc TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc.a -ldeepstate -lpthread -DTEST_ORIGINAL_CBLOSC

# FAILS RIGHT NOW DUE TO TEST NOT WORKING WITH UBSAN
#testcblosc_san: TestCBloscs.cpp
#	clang++ -o testcblosc_san TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_san.a -ldeepstate -lpthread -fsanitize=address,undefined,integer

testcblosc_swarm: TestCBloscs.cpp
	$(CXX) -o testcblosc_swarm TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM -DTEST_ORIGINAL_CBLOSC

testcblosc_AFL: TestCBloscs.cpp
	afl-clang++ -o testcblosc_AFL TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_AFL.a -ldeepstate_AFL -lpthread -DTEST_ORIGINAL_CBLOSC

testcblosc_swarm_AFL: TestCBloscs.cpp
	afl-clang++ -o testcblosc_swarm_AFL TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_AFL.a -ldeepstate_AFL -lpthread -DDEEPSTATE_PURE_SWARM -DTEST_ORIGINAL_CBLOSC

testcblosc_LF: TestCBloscs.cpp
	clang++ -o testcblosc_LF TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer -DTEST_ORIGINAL_CBLOSC

testcblosc_swarm_LF: TestCBloscs.cpp
	clang++ -o testcblosc_swarm_LF TestCBloscs.cpp -I$(CBLOSC)/include $(CBLOSC)/lib/libblosc_LF.a -ldeepstate_LF -lpthread -fsanitize=fuzzer -DDEEPSTATE_PURE_SWARM -DTEST_ORIGINAL_CBLOSC

tests: $(FILES)
	./testcblosc2 --input_test_files_dir reduced_LF_novalue_corpus_testcblosc2 --no_fork
	./testcblosc2 --input_test_files_dir reduced_LF_value_corpus_testcblosc2 --no_fork
	./testcblosc --input_test_files_dir reduced_LF_novalue_corpus_testcblosc --no_fork
	./testcblosc --input_test_files_dir reduced_LF_value_corpus_testcblosc --no_fork
