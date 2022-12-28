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

#include <glib-object.h>
#include <gst/gst.h>
#include <scope.h>

#include <cassert>
#include <string>

namespace fcarouge::sample {
namespace {

//! @brief Signal callback to link raw video pad to sink.
void link_pad_callback(GstElement *source, GstPad *pad, GstElement *sink);

//! @brief Bus watch callback to quit execution context on media stop.
auto quit_callback(auto bus, auto message, auto loop) -> gboolean;

//! @brief Pipeline cleanup scope deleter.
void gst_pipeline_destroy(GstElement *pipeline);

//! @brief Application example demonstrating usage of the Kalman element.
//!
//! @details Demonstrates a GStreamer application. Sets up a media pipeline.
//! Plays a file from a URI. Converts the media to raw video. Filters the frames
//! with a Kalman filter. Plays the video filtered video out. Also sets up a bus
//! watch and pad linking callbacks in support of the pipeline.
[[maybe_unused]] const auto application{[] {
  // Initialize the GStreamer library to enable framework usage, or terminate.
  gst_init(nullptr, nullptr);

  // Create a top-level bin media pipeline to manage media streaming.
  const sr::unique_resource pipeline{gst_pipeline_new(nullptr),
                                     gst_pipeline_destroy};
  assert(pipeline.get() && "Failed to create the pipeline.");

  // Create the execution context main loop where computation takes place.
  const sr::unique_resource loop{g_main_loop_new(nullptr, 0),
                                 g_main_loop_unref};
  assert(loop.get() && "Failed to create the event loop.");

  // Create a decoder bin element from URI to source the media.
  auto *const decoder{
      gst_element_factory_make_full("uridecodebin", "uri",
                                    "file:///home/dev/cpp/gstkalman/gstkalman/"
                                    "sample/data/roundhay_garden.mp4",
                                    nullptr)};
  assert(decoder && "Failed to create the decoder from URI element.");

  // Create an element converting formats for compatibility.
  auto *const converter{gst_element_factory_make("videoconvert", nullptr)};
  assert(converter && "Failed to create the video conversion element.");

  // Create the Kalman filter element under demonstration.
  auto *const kalman{
      gst_element_factory_make_full("kalman", "p", 100., "r", 100., nullptr)};
  assert(kalman && "Failed to create the kalman filter element.");

  // Create an element to play the media out.
  auto *const sink{gst_element_factory_make("autovideosink", nullptr)};
  assert(sink && "Failed to create the video sink element.");

  // Add each element to the bin.
  for (auto &&element : {decoder, converter, kalman, sink}) {
    assert(gst_bin_add(GST_BIN(pipeline.get()), element));
  }

  // Chain the elements of the bin together, in order.
  assert(gst_element_link_many(converter, kalman, sink, nullptr));

  // Attach a signal callback linking the pad to sink on pad addition.
  assert(g_signal_connect(decoder, "pad-added", G_CALLBACK(link_pad_callback),
                          converter));

  // Obtain the bus of the pipeline to instrument it.
  const sr::unique_resource bus{gst_element_get_bus(pipeline.get()),
                                gst_object_unref};
  assert(bus.get() && "Failed to obtain the pipeline bus.");

  // Watch the bus with a callback stopping the execution on error or media end.
  const sr::unique_resource watch_id{
      gst_bus_add_watch(bus.get(), quit_callback, loop.get()), g_source_remove};
  assert(watch_id.get() != 0 && "Failed to add the bus watch callback.");

  // Prepare the pipeline to run.
  const GstStateChangeReturn state_changed{
      gst_element_set_state(pipeline.get(), GST_STATE_PLAYING)};
  assert(state_changed != GST_STATE_CHANGE_FAILURE &&
         "Failed to play the pipeline.");

  // Run! The loop will return on error or end of media.
  g_main_loop_run(loop.get());

  return 0;
}()};

void link_pad_callback(GstElement *source, GstPad *pad, GstElement *sink) {
  static_cast<void>(source);

  // Obtain the media capabilities of the pad.
  const sr::unique_resource pad_capabilities{gst_pad_get_current_caps(pad),
                                             gst_caps_unref};
  assert(pad_capabilities.get() && "Failed to obtain the pad's capabilities.");

  // Obtain the capabilities data.
  const auto *capabilities_data{
      gst_caps_get_structure(pad_capabilities.get(), 0)};
  assert(capabilities_data && "Failed to obtain the pad's capabilities data.");

  // View the name of the capabilities.
  const std::string_view capabilities_name{
      gst_structure_get_name(capabilities_data)};
  assert(!capabilities_name.empty() && "Failed to obtain the pad's type.");

  // Ignore all pads but the one offering raw video support.
  if (!capabilities_name.starts_with("video/x-raw")) {
    return;
  }

  // Obtain the existing pad of the sink element.
  const sr::unique_resource sink_pad{gst_element_get_static_pad(sink, "sink"),
                                     gst_object_unref};
  assert(sink_pad.get() && "Failed to obtain the sink pad.");

  // Link the source and sink pads together.
  const GstPadLinkReturn link_status{gst_pad_link(pad, sink_pad.get())};
  assert(link_status == GST_PAD_LINK_OK && "Failed to link the sink pad.");
}

auto quit_callback(auto bus, auto message, auto loop) -> gboolean {
  static_cast<void>(bus);

  // Stop the execution context if the media ended or errored.
  if (const GstMessageType message_type{GST_MESSAGE_TYPE(message)};
      message_type == GST_MESSAGE_EOS || message_type == GST_MESSAGE_ERROR) {
    g_main_loop_quit(static_cast<GMainLoop *>(loop));
  }

  return true;
}

void gst_pipeline_destroy(GstElement *pipeline) {
  // Clean stop the pipeline before destrution.
  const GstStateChangeReturn state_changed{
      gst_element_set_state(pipeline, GST_STATE_NULL)};
  assert(state_changed != GST_STATE_CHANGE_FAILURE &&
         "Failed to clear the pipeline.");
  gst_object_unref(pipeline);
};

} // namespace
} // namespace fcarouge::sample
