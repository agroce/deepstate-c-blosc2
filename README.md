The test harness + Makefile will allow you to test [CBlosc-2](https://github.com/Blosc/c-blosc2) using

- DeepState's built-in brute force fuzzer
- AFL
- libFuzzer
- Eclipser (seems Eclipser/QEMU causes some spurious fails that don't reproduce, but still generates interesting tests)

It also makes use of [swarm testing](https://agroce.github.io/issta12.pdf).

The Makefile expects that:

- you installed everything in `install` under the cblosc dir
- for AFL, you built c-blosc2 using `CC=afl-clang CXX=afl-clang++ cmake -DCMAKE_INSTALL_PREFIX=../install ..`
- for libFuzzer, you built c-blosc2 using `CC=clang CXX=clang++ CFLAGS=-fsanitize=fuzzer-no-link CXXFLAGS=-fsanitize=fuzzer-no-link cmake -DCMAKE_INSTALL_PREFIX=../install ..`

and copied the resulting library `.a` files to appropriately named files (see the Makefile)

See https://github.com/trailofbits/deepstate for more on DeepState.

You probably need to edit the Makefile, unless you just use the DeepState docker image and check out c-blosc2 to the obvious location.
