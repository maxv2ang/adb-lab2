# Lab2 Storage and Buffer Manager

## 1. directory structure

```shell
├── CMakeLists.txt
├── data-5w-50w-zipf.txt        // trace file
├── data.dbf
├── main.cpp
└── src
    ├── CMakeLists.txt
    ├── bmgr.cpp
    ├── dsmgr.cpp
    └── include
        ├── bmgr.hpp            // buffer manager
        ├── common.hpp
        └── dsmgr.hpp           // disk and storage manager
```

## 2. compile

```shell
➜  adb_lab2 mkdir build
➜  adb_lab2 cd build
➜  build cmake ..
➜  build make
```

## 3. run

```shell
➜  build ./test
```
