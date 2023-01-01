# A GStreamer Kalman Filter Video Plugin in C++

```
Factory Details:
  Rank                     none (0)
  Long-name                Kalman Filter
  Klass                    Filter/Effect/Video
  Description              A video Kalman filter plugin.
  Author                   Francois Carouge <francois.carouge@gmail.com>

Plugin Details:
  Name                     kalman
  Description              A video Kalman filter plugin.
  Filename                 /mnt/f/Drive/Projects/cpp/gstkalman/build/source/libgstkalman.so
  Version                  0.1.0
  License                  Unlicense
  Source module            kalman
  Binary package           kalman
  Origin URL               https://github.com/FrancoisCarouge/GstKalman

GObject
 +----GInitiallyUnowned
       +----GstObject
             +----GstElement
                   +----GstBaseTransform
                         +----GstKalman

Pad Templates:
  SINK template: 'sink'
    Availability: Always
    Capabilities:
      video/x-raw
  
  SRC template: 'src'
    Availability: Always
    Capabilities:
      video/x-raw

Element has no clocking capabilities.
Element has no URI handling capabilities.

Pads:
  SINK: 'sink'
    Pad Template: 'sink'
  SRC: 'src'
    Pad Template: 'src'

Element Properties:
  name                : The name of the object
                        flags: readable, writable, 0x2000
                        String. Default: "kalman0"
  p                   : Initialize estimate uncertainty.
                        flags: readable, writable
                        Float. Range:               0 -    3.402823e+38 Default:               0 
  parent              : The parent of the object
                        flags: readable, writable, 0x2000
                        Object of type "GstObject"
  qos                 : Handle Quality-of-Service events
                        flags: readable, writable
                        Boolean. Default: false
  r                   : Initialize output uncertainty.
                        flags: readable, writable
                        Float. Range:               0 -    3.402823e+38 Default:               0 
```

- [A GStreamer Kalman Filter Video Plugin in C++](#a-gstreamer-kalman-filter-video-plugin-in-c)
- [Examples](#examples)
- [Installation](#installation)
- [Reference](#reference)
- [Resources](#resources)
- [Continuous Integration \& Deployment Actions](#continuous-integration--deployment-actions)
- [Third Party Acknowledgement](#third-party-acknowledgement)
- [Sponsors](#sponsors)
  - [Corporate Sponsor](#corporate-sponsor)
  - [Named Sponsors](#named-sponsors)
- [License](#license)

# Examples

```shell
gst-launch-1.0 uridecodebin uri="file:///path/to/roundhay_garden.mp4" ! videoconvert ! kalman p=100 r=100 ! autovideosink
```

# Installation

# Reference

# Resources

# Continuous Integration & Deployment Actions

# Third Party Acknowledgement

# Sponsors

Become a sponsor today! Support this project with coffee and infrastructure!

[![Sponsor](https://img.shields.io/badge/Support-Sponsor-brightgreen)](http://paypal.me/francoiscarouge)

## Corporate Sponsor

*You company logo and link here!*

## Named Sponsors

*Your name and link here!*

Thanks everyone!

# License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

GStreamer Kalman Filter Video Plugin in C++ is public domain:

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

For more information, please refer to <https://unlicense.org>