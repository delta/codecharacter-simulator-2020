# CodeCharacter Simulator 2020
Simulator for Pragyan CodeCharacter 2020

## Setup Instructions

1. Download [Boost 1.71](https://www.boost.org/users/history/version_1_71_0.html)

2. `cd` to the Boost directory and build the system library by running `./bootstrap.sh --prefix=. --with-libraries=system && ./b2 cxxflags=-std=c++14 install`

3. `git clone --recursive https://github.com/delta/codecharacter-simulator-2020.git`

4. Download & Install [CMake](https://cmake.org/install/) (>= v3.15.3)

5. Download & Install [Protobuf](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md)

6. Install Clang++ (>=8.0.1)

7. `mkdir build && cd build`

8. `export LD_LIBRARY_PATH=<repo_install_location>/lib`

9. `cmake .. -DCMAKE_INSTALL_PREFIX=<repo_install_location> -DBOOST_ROOT=<boost_download_location> -DCMAKE_CXX_COMPILER=clang++`

10. `make install`


To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests

## Building Documentations

1. Run `doxygen && cd docs/ && make html && cd ../` at the root level of the project.
2. The documentation files are built under `docs/build/html`
