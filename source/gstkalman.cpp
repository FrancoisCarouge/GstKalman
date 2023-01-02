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

#include <fcarouge/kalman.hpp>
#include <glib-object.h>
#include <gst/base/gstbasetransform.h>
#include <gst/gst.h>
#include <scope.h>

#include <algorithm>
#include <execution>
#include <limits>
#include <span>
#include <string_view>
#include <vector>

namespace {

// A single precision, no input, constant system dynamic model Kalman filter.
using kalman = fcarouge::kalman<float, float>;

//! @brief The GStreamer Kalman filter video plugin element datastructure.
//!
//! @details A GObject, GLib, GStreamer compatible element datastructure. The
//! name `_GstKalman` conforms to the GObject framework naming expectations.
struct _GstKalman {
  //! @brief The transform base class providing default support.
  GstBaseTransform element;

  //! @brief A Kalman filter for each pixel of the frame.
  std::vector<kalman> filters;

  //! @brief The filter's initialization estimate uncertainty characteristics.
  float p{1.F};

  //! @brief The filter's initialization output uncertainty characteristics.
  float r{0.F};
};

//! @brief The GStreamer Kalman filter element properties.
//!
//! @todo Understand why GLib must have a zero-th property.
enum property : guint { _, p, r };

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

// Declares the GstKalman element, a final class, part of the GStreamer module,
// derived from the base transform element, and defines type support.
G_DECLARE_FINAL_TYPE(GstKalman, gst_kalman, GST, KALMAN, GstBaseTransform)
G_DEFINE_TYPE(GstKalman, gst_kalman, GST_TYPE_BASE_TRANSFORM);

auto initialize(GstPlugin *plugin) -> gboolean;
void gst_kalman_set_property(GObject *object, guint prop_id,
                             const GValue *value, GParamSpec *pspec);
void gst_kalman_get_property(GObject *object, guint prop_id, GValue *value,
                             GParamSpec *pspec);
auto gst_kalman_transform_in_place(GstBaseTransform *base, GstBuffer *output)
    -> GstFlowReturn;

// Defines and exports the entry point and metadata of the plugin.
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
//!
//! @details Sets up the element metadata, static sink and source, in-place
//! compute, and properties.
void gst_kalman_class_init(GstKalmanClass *klass) {
  auto *object_klass{G_OBJECT_CLASS(klass)};
  object_klass->set_property = gst_kalman_set_property;
  object_klass->get_property = gst_kalman_get_property;

  constexpr GParamFlags described_readwrite{
      static_cast<GParamFlags>(static_cast<unsigned>(G_PARAM_READWRITE) |
                               static_cast<unsigned>(G_PARAM_STATIC_STRINGS))};
  constexpr auto max_float{std::numeric_limits<float>::max()};
  g_object_class_install_property(
      object_klass, property::p,
      g_param_spec_float("p", "P", "Initialize estimate uncertainty.", 0.,
                         max_float, 1., described_readwrite));
  g_object_class_install_property(
      object_klass, property::r,
      g_param_spec_float("r", "R", "Initialize output uncertainty.", 0.,
                         max_float, 0., described_readwrite));

  auto *gstelement_klass{GST_ELEMENT_CLASS(klass)};
  gst_element_class_set_static_metadata(gstelement_klass, long_name.data(),
                                        classification.data(),
                                        description.data(), author.data());

  GstStaticPadTemplate sink_template{"sink", GstPadDirection::GST_PAD_SINK,
                                     GstPadPresence::GST_PAD_ALWAYS,
                                     GST_STATIC_CAPS(capabilities.data())};
  gst_element_class_add_pad_template(
      gstelement_klass, gst_static_pad_template_get(&sink_template));

  GstStaticPadTemplate source_template{"src", GstPadDirection::GST_PAD_SRC,
                                       GstPadPresence::GST_PAD_ALWAYS,
                                       GST_STATIC_CAPS(capabilities.data())};
  gst_element_class_add_pad_template(
      gstelement_klass, gst_static_pad_template_get(&source_template));

  GST_BASE_TRANSFORM_CLASS(klass)->transform_ip =
      GST_DEBUG_FUNCPTR(gst_kalman_transform_in_place);
}

//! @brief Update the element instance data on property change.
void gst_kalman_set_property(GObject *object, guint prop_id,
                             const GValue *value, GParamSpec *pspec) {
  static_cast<void>(pspec);

  switch (auto *element{GST_KALMAN(object)}; prop_id) {
  case property::p:
    element->p = g_value_get_float(value);
    break;
  case property::r:
    element->r = g_value_get_float(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

//! @brief Provides the element instance data on property request.
//!
//! @todo Implement and test.
void gst_kalman_get_property(GObject *object, guint prop_id, GValue *value,
                             GParamSpec *pspec) {
  static_cast<void>(object);
  static_cast<void>(prop_id);
  static_cast<void>(value);
  static_cast<void>(pspec);
}

//! @brief Instantiates the element.
//!
//! @details Nothing to do for now.
void gst_kalman_init(GstKalman *element) { static_cast<void>(element); }

//! @brief Processes the data buffer in-place.
//!
//! @details Filters the frame pixels. Reset and reinitialize the filters on
//! resolution change.
auto gst_kalman_transform_in_place(GstBaseTransform *element_base,
                                   GstBuffer *output) -> GstFlowReturn {

  if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_TIMESTAMP(output))) {
    gst_object_sync_values(GST_OBJECT(element_base),
                           GST_BUFFER_TIMESTAMP(output));
  }

  const sr::unique_resource source_capabilities{
      gst_pad_get_current_caps(element_base->srcpad), gst_caps_unref};
  g_return_val_if_fail(source_capabilities.get(), GST_FLOW_ERROR);

  const auto *source_data{gst_caps_get_structure(source_capabilities.get(), 0)};
  g_return_val_if_fail(source_data, GST_FLOW_ERROR);

  gint width{0};
  g_return_val_if_fail(gst_structure_get_int(source_data, "width", &width),
                       GST_FLOW_ERROR);

  gint height{0};
  g_return_val_if_fail(gst_structure_get_int(source_data, "height", &height),
                       GST_FLOW_ERROR);

  auto *element{GST_KALMAN(element_base)};
  auto &filters{element->filters};
  const std::size_t resolution{static_cast<std::size_t>(width * height)};

  GstMapInfo map;
  const sr::unique_resource mapping{
      gst_buffer_map(output, &map, GST_MAP_READWRITE),
      [&map, &output](gboolean status) {
        //! @todo Should the buffer be unmapped even on failed status?
        static_cast<void>(status);
        gst_buffer_unmap(output, &map);
      }};

  //! @todo Support additional formats, hardwares, contexts, and strategies.
  const std::span pixels{map.data, resolution};

  //! @todo Resolution is a poor indicator of the need to re-initialized the
  //! filters.
  //! @todo Should this check be performed as an event, signal, or capabilities
  //! change instead of part of the transform?
  if (resolution != filters.size()) {
    //! @todo Support in-place non-default construction to avoid the secondary
    //! initialization loop?
    filters = std::vector<kalman>{resolution};
    std::transform(std::execution::par_unseq, std::begin(pixels),
                   std::end(pixels), std::begin(filters), std::begin(pixels),
                   [element](const auto &pixel, auto &filter) {
                     filter.x(static_cast<typename kalman::state>(pixel));
                     filter.p(element->p);
                     filter.r(element->r);
                     return pixel;
                   });
  } else {
    //! @todo Use the timestamp of the frame if available?
    std::transform(std::execution::par_unseq, std::begin(pixels),
                   std::end(pixels), std::begin(filters), std::begin(pixels),
                   []<typename Pixel>(const Pixel &pixel, auto &filter) {
                     filter.update(static_cast<typename kalman::output>(pixel));
                     using limit = std::numeric_limits<Pixel>;
                     return std::clamp<Pixel>(filter.x(), limit::min(),
                                              limit::max());
                   });
  }

  return GST_FLOW_OK;
}

} // namespace
