#[[ __          _      __  __          _   _
| |/ /    /\   | |    |  \/  |   /\   | \ | |
| ' /    /  \  | |    | \  / |  /  \  |  \| |
|  <    / /\ \ | |    | |\/| | / /\ \ | . ` |
| . \  / ____ \| |____| |  | |/ ____ \| |\  |
|_|\_\/_/    \_\______|_|  |_/_/    \_\_| \_|

GStreamer Kalman Filter Video Plugin
Version 0.1.0
https://github.com/FrancoisCarouge/GstKalman

SPDX-License-Identifier: Unlicense

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org> ]]

add_library(gstkalman_gstreamer INTERFACE)

find_path(
  GSTREAMER_INCLUDE
  NAMES "gst/gst.h"
  PATH_SUFFIXES "gstreamer-1.0")

find_library(GSTREAMER_LIBRARY NAMES "gstreamer-1.0")
find_library(GSTBASE_LIBRARY NAMES "gstbase-1.0")

find_package_handle_standard_args(GStreamer DEFAULT_MSG GSTREAMER_LIBRARY
                                  GSTREAMER_INCLUDE)

target_include_directories(gstkalman_gstreamer INTERFACE ${GSTREAMER_INCLUDE})
target_link_libraries(gstkalman_gstreamer INTERFACE ${GSTREAMER_LIBRARY})
target_link_libraries(gstkalman_gstreamer INTERFACE ${GSTBASE_LIBRARY})
