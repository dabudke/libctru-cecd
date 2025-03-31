cmake_minimum_required(VERSION 3.13)

project(libctru_cecd)

add_library(ctru-cecd
  source/3ds/services/cec/cecCec.c
  source/3ds/services/cec/cecCecApi.c
  source/3ds/services/cec/cecCecSys.c
)

target_include_directories(ctru-cecd PRIVATE include)
