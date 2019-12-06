cd /home/user

git clone https://github.com/Blosc/c-blosc2.git

cd c-blosc2
mkdir build
cd build
CC=afl-clang CXX=afl-clang++ cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc2/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc2/install/lib/libblosc2.a /home/user/c-blosc2/install/lib/libblosc2_AFL.a

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ CFLAGS=-fsanitize=fuzzer-no-link CXXFLAGS=-fsanitize=fuzzer-no-link cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc2/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc2/install/lib/libblosc2.a /home/user/c-blosc2/install/lib/libblosc2_LF.a 

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ CFLAGS=-fsanitize=address,undefined,integer CXXFLAGS=-fsanitize=address,undefined,integer cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc2/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc2/install/lib/libblosc2.a /home/user/c-blosc2/install/lib/libblosc2_san.a 

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc2/install ..
cmake --build .
cmake --build . --target install

cd /home/user

git clone https://github.com/Blosc/c-blosc.git

cd c-blosc
mkdir build
cd build
CC=afl-clang CXX=afl-clang++ cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc/install/lib/libblosc.a /home/user/c-blosc/install/lib/libblosc_AFL.a

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ CFLAGS=-fsanitize=fuzzer-no-link CXXFLAGS=-fsanitize=fuzzer-no-link cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc/install/lib/libblosc.a /home/user/c-blosc/install/lib/libblosc_LF.a 

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ CFLAGS=-fsanitize=address,undefined,integer CXXFLAGS=-fsanitize=address,undefined,integer cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc/install ..
cmake --build .
cmake --build . --target install
mv /home/user/c-blosc/install/lib/libblosc.a /home/user/c-blosc/install/lib/libblosc_san.a 

cd ..
rm -rf build

mkdir build
cd build
CC=clang CXX=clang++ cmake -DCMAKE_INSTALL_PREFIX=/home/user/c-blosc/install ..
cmake --build .
cmake --build . --target install

cd /home/user/deepstate-c-blosc2
