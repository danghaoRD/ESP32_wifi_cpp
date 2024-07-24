# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/MY PC/esp/v5.2/esp-idf/components/bootloader/subproject"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/tmp"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/src/bootloader-stamp"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/src"
  "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "F:/code/ESP/Wifi_Cpp/wifi_cpp_ex1/ESP32_wifi_cpp/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
