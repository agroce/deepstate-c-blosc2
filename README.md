The test harness + Makefile will allow you to test
[CBlosc-2](https://github.com/Blosc/c-blosc2) and the original
[CBlosc](https://github.com/Blosc/c-blosc) using

- DeepState's built-in brute force fuzzer
- AFL
- libFuzzer
- Eclipser (seems Eclipser/QEMU causes some spurious fails that don't reproduce, but still generates interesting tests)

It also can make use of [swarm testing](https://agroce.github.io/issta12.pdf).

The Makefile expects that:

- you installed everything in `install` under the cblosc[2] dir
- for AFL, you built c-blosc2 using `CC=afl-clang CXX=afl-clang++ cmake -DCMAKE_INSTALL_PREFIX=../install ..`
- for libFuzzer, you built c-blosc2 using `CC=clang CXX=clang++ CFLAGS=-fsanitize=fuzzer-no-link CXXFLAGS=-fsanitize=fuzzer-no-link cmake -DCMAKE_INSTALL_PREFIX=../install ..`

and copied the resulting library `.a` files to appropriately named
files (see the Makefile).  Running `source getbloscs.sh` should, in
the DeepState docker environment, do all these things for you.  This
will take a while, due to compilation of some compressors being very
slow with sanitizers.

Tests are binary compatible between the c-blosc versions, which means
original c-blosc tests read some pointless bytes.

You can get started fuzzing with just a few commands:

```
$ git clone https://github.com/trailofbits/deepstate.git
$ cd deepstate
$ docker build -t deepstate . -f docker/Dockerfile
$ docker run -it deepstate bash
```

In the docker do:

```
$ git clone https://github.com/agroce/deepstate-c-blosc2.git
$ cd deepstate-c-blosc2
$ source getbloscs.sh
$ make
$ ./testcblosc2 --fuzz --timeout 100
```

See https://github.com/trailofbits/deepstate for more on DeepState.
