rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../sim-install -DCMAKE_CXX_COMPILER=clang++     
make -j install
