# CodeCharacter Simulator 2020

Simulator for Pragyan CodeCharacter 2020

## Setup Instructions

1. Download & Install [CMake](https://cmake.org/install/) (>= v3.15.3)

2. Download & Install [Protobuf](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md)

3. Install Clang++ (>=9.0.1) (Can install using `sudo apt-get install clang`)

4. Download [Boost 1.71](https://www.boost.org/users/history/version_1_71_0.html) (preferably in `/usr/local/lib/`)

5. `cd` to the Boost directory and build the system library by running `./bootstrap.sh --prefix=. --with-libraries=system && ./b2 cxxflags=-std=c++14 install`

6. `cd` to your projects directory and run `git clone --recursive https://github.com/delta/codecharacter-simulator-2020.git` && `git config core.hooksPath .githooks/`

7. run `pip install cmake-format`

8. Install Clang-format using `sudo apt-get install clang-format`

9. `mkdir build && cd build`

10. `export LD_LIBRARY_PATH=<repo_install_location>/lib`

11. `cmake .. -DCMAKE_INSTALL_PREFIX=<repo_install_location> -DBOOST_ROOT=<boost_download_location> -DCMAKE_CXX_COMPILER=clang++`

12. `make install`

To run the simulator, `<your_install_location>/bin/simulator`

To run the unit tests, `<your_install_location>/bin/test`

Pass `-DBUILD_PROJECT=<project_name>` to cmake to build only a specific module. Passing `no_tests` as the project name builds everything but the unit tests

## Setting up Doxygen, Sphinx and building Documentation.

1. sudo apt update  
   sudo apt install -y doxygen python3-pip  
   sudo python3 -m pip install sphinx_rtd_theme breathe

2. Run `doxygen && cd docs/ && make html && cd ../` at the root level of the project.
3. The documentation files are built under `docs/build/html`
