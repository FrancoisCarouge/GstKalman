/*  __          _      __  __          _   _
| |/ /    /\   | |    |  \/  |   /\   | \ | |
| ' /    /  \  | |    | \  / |  /  \  |  \| |
|  <    / /\ \ | |    | |\/| | / /\ \ | . ` |
| . \  / ____ \| |____| |  | |/ ____ \| |\  |
|_|\_\/_/    \_\______|_|  |_/_/    \_\_| \_|

GStreamer Kalman Filter Video Plugin in C++
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

For more information, please refer to <https://unlicense.org> */

//! @file
//! @brief The GStreamer Kalman filter video plugin element implementation.

#include <string_view>

#include <glib-object.h>
#include <gst/base/gstbasetransform.h>
#include <gst/gst.h>

namespace {

constexpr std::string_view name{"kalman"};
constexpr std::string_view classification{"Filter/Effect/Video"};
constexpr std::string_view long_name{"Kalman Filter"};
constexpr std::string_view description{"A video Kalman filter plugin."};
constexpr std::string_view version{"0.1.0"};
constexpr std::string_view license{"Unlicense"};
constexpr std::string_view author{
    "Francois Carouge <francois.carouge@gmail.com>"};
constexpr std::string_view origin{
    "https://github.com/FrancoisCarouge/GstKalman"};
constexpr std::string_view capabilities{"video/x-raw"};

//! @brief The GStreamer Kalman filter video plugin element datastructure.
//!
//! @details A GObject, GLib, GStreamer compatible element datastructure. The
//! name
//! `_GstKalman` conforms to the GObject framework expectations.
struct _GstKalman {
  GstBaseTransform element;
};

// Declares the GstKalman element, a final class, part of the GStreamer module,
// derived from the base transform element, and defines type support.
G_DECLARE_FINAL_TYPE(GstKalman, gst_kalman, GST, KALMAN, GstBaseTransform)
G_DEFINE_TYPE(GstKalman, gst_kalman, GST_TYPE_BASE_TRANSFORM);

auto initialize(GstPlugin *plugin) -> gboolean;
auto gst_kalman_transform_in_place(GstBaseTransform *base, GstBuffer *output)
    -> GstFlowReturn;

// Defines and exports the entry point and metadata of the plugin.
#define PACKAGE name.data()
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, kalman,
                  description.data(), initialize, version.data(),
                  license.data(), name.data(), origin.data())
GST_ELEMENT_REGISTER_DEFINE(kalman, name.data(), GST_RANK_NONE,
                            gst_kalman_get_type());

//! @brief Registers the plugin element.
auto initialize(GstPlugin *plugin) -> gboolean {
  return GST_ELEMENT_REGISTER(kalman, plugin);
}

//! @brief Defines the element details.
void gst_kalman_class_init(GstKalmanClass *klass) {
  auto *gstelement_class{GST_ELEMENT_CLASS(klass)};

  gst_element_class_set_details_simple(gstelement_class, long_name.data(),
                                       classification.data(),
                                       description.data(), author.data());

  GstStaticPadTemplate sink_template{"sink", GstPadDirection::GST_PAD_SINK,
                                     GstPadPresence::GST_PAD_ALWAYS,
                                     GST_STATIC_CAPS(capabilities.data())};
  gst_element_class_add_pad_template(
      gstelement_class, gst_static_pad_template_get(&sink_template));

  GstStaticPadTemplate source_template{"src", GstPadDirection::GST_PAD_SRC,
                                       GstPadPresence::GST_PAD_ALWAYS,
                                       GST_STATIC_CAPS(capabilities.data())};
  gst_element_class_add_pad_template(
      gstelement_class, gst_static_pad_template_get(&source_template));

  GST_BASE_TRANSFORM_CLASS(klass)->transform_ip =
      GST_DEBUG_FUNCPTR(gst_kalman_transform_in_place);
}

//! @brief Instantiates the element.
void gst_kalman_init(GstKalman *element) { static_cast<void>(element); }

//! @brief Processes the data buffer in-place.
auto gst_kalman_transform_in_place(GstBaseTransform *element_base,
                                   GstBuffer *output) -> GstFlowReturn {

  if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_TIMESTAMP(output))) {
    gst_object_sync_values(GST_OBJECT(element_base),
                           GST_BUFFER_TIMESTAMP(output));
  }

  return GST_FLOW_OK;
}

} // namespace
