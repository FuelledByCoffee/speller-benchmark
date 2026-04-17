## Benchmark utility for the cs50 speller problem

This was originally written in C by Azaan. It has since been ported and updated.

### Usage
---------------
Create a folder in your speller directory for the benchmark files. Build the project and install the benchmark program.

Run `./bench` from your speller directory after installing.

Optional flags:
- `-1`, `--single-thread` : disable multithreading
- `-s`, `--staff-speller[=path]` : include the staff speller in the benchmark; optional path overrides the default `speller50`
- `-y`, `--your-speller[=path]` : override the your speller executable path; defaults to `speller`

### Building
-----------
```sh
make
```


Project layout
--------------

    ├─ cmake/           CMake utils
    └─ src/             application source code


Dependencies
------------
- cmake
- A C++ compiler
