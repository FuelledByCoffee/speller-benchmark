## Benchmark utility for the cs50 speller problem

This was originally written in C by Azaan. It has since been ported and updated.

### Usage
---------------
Create a folder in your speller directory for the benchmark files. Build the project and install the benchmark program.

Run `./bench` from your speller directory after installing.

### Building
-----------
```sh
cmake -B build
cmake --build build -t install
```


Project layout
--------------

    ├─ cmake/           CMake utils
    └─ src/             application source code


Dependencies
------------
- cmake
- A C++ compiler
