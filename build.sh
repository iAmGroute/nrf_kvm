#!/bin/bash

mkdir -p build/
rm -rf build/*

# Assembly Pre

clang-9 -cc1                                                                          \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0                                  \
    -I "." -isystem "../nrfx_deps/output"                                             \
    -D "__ARM_ARCH_6M__"                                                              \
    -D "__nRF_FAMILY"                                                                 \
    -D "__SEGGER_LINKER"                                                              \
    -D "__SES_ARM"                                                                    \
    -D "__SES_VERSION=45203"                                                          \
    -D "__SIZEOF_WCHAR_T=4"                                                           \
    -D "ARM_MATH_CM0"                                                                 \
    -D "FLASH_PLACEMENT=1"                                                            \
    -D "NRF51"                                                                        \
    -D "NRF51822"                                                                     \
    -D "NRF51822_XXAA"                                                                \
    -D "__HEAP_SIZE__=0"                                                              \
    -D "NO_SYSTEM_INIT"                                                               \
    -E "lib/Assembly/ses_startup_thumb.s"                                             \
    -o "build/ses_startup_thumb_PP.s"


clang-9 -cc1                                                                          \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0                                  \
    -I "." -isystem "../nrfx_deps/output"                                             \
    -D "__ARM_ARCH_6M__"                                                              \
    -D "__nRF_FAMILY"                                                                 \
    -D "__SEGGER_LINKER"                                                              \
    -D "__SES_ARM"                                                                    \
    -D "__SES_VERSION=45203"                                                          \
    -D "__SIZEOF_WCHAR_T=4"                                                           \
    -D "ARM_MATH_CM0"                                                                 \
    -D "FLASH_PLACEMENT=1"                                                            \
    -D "NRF51"                                                                        \
    -D "NRF51822"                                                                     \
    -D "NRF51822_XXAA"                                                                \
    -D "__HEAP_SIZE__=0"                                                              \
    -D "NO_SYSTEM_INIT"                                                               \
    -E "lib/Assembly/NRF51822/ses_startup_nrf_common.s"                               \
    -o "build/ses_startup_nrf_common_PP.s"


clang-9 -cc1                                                                          \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0                                  \
    -I "." -isystem "../nrfx_deps/output"                                             \
    -D "__ARM_ARCH_6M__"                                                              \
    -D "__nRF_FAMILY"                                                                 \
    -D "__SEGGER_LINKER"                                                              \
    -D "__SES_ARM"                                                                    \
    -D "__SES_VERSION=45203"                                                          \
    -D "__SIZEOF_WCHAR_T=4"                                                           \
    -D "ARM_MATH_CM0"                                                                 \
    -D "FLASH_PLACEMENT=1"                                                            \
    -D "NRF51"                                                                        \
    -D "NRF51822"                                                                     \
    -D "NRF51822_XXAA"                                                                \
    -D "__HEAP_SIZE__=0"                                                              \
    -D "NO_SYSTEM_INIT"                                                               \
    -E "lib/Assembly/NRF51822/ses_startup_nrf51.s"                                    \
    -o "build/ses_startup_nrf51_PP.s"

# Assembly

clang-9 -cc1as                                                                        \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0 -mrelocation-model "static"      \
    -I "."                                                                            \
    "build/ses_startup_thumb_PP.s"                                                    \
    -filetype "obj" -o build/ses_startup_thumb.o

clang-9 -cc1as                                                                        \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0 -mrelocation-model "static"      \
    -I "."                                                                            \
    "build/ses_startup_nrf_common_PP.s"                                               \
    -filetype "obj" -o build/ses_startup_nrf_common.o

clang-9 -cc1as                                                                        \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0 -mrelocation-model "static"      \
    -I "."                                                                            \
    "build/ses_startup_nrf51_PP.s"                                                    \
    -filetype "obj" -o build/ses_startup_nrf51.o

# C

clang-9 -cc1                                                                          \
    -triple thumbv6m-none-eabi -target-cpu cortex-m0 -mrelocation-model "static"      \
    -x c++ -std=c++11                                                                 \
    -I "." -isystem "../nrfx_deps/output"                                             \
    -D "__ARM_ARCH_6M__"                                                              \
    -D "__nRF_FAMILY"                                                                 \
    -D "__SEGGER_LINKER"                                                              \
    -D "__SES_ARM"                                                                    \
    -D "__SES_VERSION=45203"                                                          \
    -D "__SIZEOF_WCHAR_T=4"                                                           \
    -D "ARM_MATH_CM0"                                                                 \
    -D "FLASH_PLACEMENT=1"                                                            \
    -D "NRF51"                                                                        \
    -D "NRF51822"                                                                     \
    -D "NRF51822_XXAA"                                                                \
    -D "__HEAP_SIZE__=0"                                                              \
    -D "NO_SYSTEM_INIT"                                                               \
    -Wall -O3 "nrf_kvm/BuildMe.cpp"                                                   \
    -emit-obj -o "build/BuildMe.o"

# build.ind

echo "\"build/ses_startup_thumb.o\""      >> "build/build.ind"
echo "\"build/ses_startup_nrf_common.o\"" >> "build/build.ind"
echo "\"build/ses_startup_nrf51.o\""      >> "build/build.ind"
echo "\"build/BuildMe.o\""                >> "build/build.ind"

# Linker

/usr/share/segger_embedded_studio_for_arm_4.52c/bin/segger-ld                         \
    --add-region FLASH1=0x00040000@0x00000000 --add-region RAM1=0x00004000@0x20000000 \
    -mcpu=cortex-m0                                                                   \
    --entry=Reset_Handler                                                             \
    -D"__STACKSIZE__=1024" -D"__STACKSIZE_PROCESS__=0" -D"__HEAPSIZE__=0"             \
    --list-all-undefineds --fatal-warnings --verbose                                  \
    -T"./lib/Assembly/NRF51822/ses_linker.icf"                                        \
    -Map "build/build.map" -o "build/build.elf"                                       \
    "@build/build.ind"




#     -D__SIZEOF_WCHAR_T=4 -D__HEAP_SIZE__=0 -D__SES_VERSION=45203 -D__SEGGER_LINKER -DNRF51822_XXAA -D__nRF_FAMILY -DARM_MATH_CM0 -DFLASH_PLACEMENT=1


# /usr/share/segger_embedded_studio_for_arm_4.52c/gcc/arm-none-eabi/bin/ld -X --omagic -eReset_Handler --defsym=__vfprintf=__vfprintf_int_nwp --defsym=__vfscanf=__vfscanf_int --fatal-warnings -EL --gc-sections "-T build/build.ld" -Map "build/build.map" -u_vectors -o "build/build.elf" --emit-relocs --start-group "@build/build.ind" --end-group

# mkld -memory-map-file "./memorymap_xxAA.xml" -section-placement-file "lib/Assembly/NRF51822/flash_placement.xml" -check-segment-overflow -symbols "__STACKSIZE__=1024;__STACKSIZE_PROCESS__=0;__HEAPSIZE__=0" "build/build.ld"

# /usr/share/segger_embedded_studio_for_arm_4.52c/bin/segger-ld --add-region FLASH1=0x00040000@0x00000000 --add-region RAM1=0x00004000@0x20000000 --defsym=__STACKSIZE__=1024 --defsym=__STACKSIZE_PROCESS__=0 --defsym=__HEAPSIZE__=0 --full-section-headers --entry=Reset_Handler --defsym=__vfprintf=__vfprintf_int_nwp --defsym=__vfscanf=__vfscanf_int --silent --list-all-undefineds --fatal-warnings -EL -mcpu=cortex-m0 --gc-sections "-T./lib/Assembly/NRF51822/xxAA.icf" -Map "build/build.map" -u_vectors -o "build/build.elf" --emit-relocs --start-group "@build/build.ind" --end-group