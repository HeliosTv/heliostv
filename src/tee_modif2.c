/*
 * Dynamic pipelines example, uridecodebin with sinks added and removed
 *
 * Copyright (c) 2014 Sebastian Dröge <sebastian@centricular.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <string.h>
#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *src, *filter, *tee;
static gboolean linked = FALSE;
static GList *sinks;



/***********************************************************************************/
/************************************ Debug Dot ************************************/
void GeneratePipelineDot(GstElement *Pipeline)
{
   guint Step = 0;
   if(Pipeline == NULL)
      return;
   gchar *DotName;
   char *sz_ElementName = gst_element_get_name(Pipeline);
   DotName = g_strdup_printf ("%s_%i", sz_ElementName, Step);
   Step++;
   GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (Pipeline),
         GST_DEBUG_GRAPH_SHOW_ALL, DotName);
   g_free (DotName);
   g_free (sz_ElementName);
   return ;
}
/***********************************************************************************/

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}



typedef struct
{
  GstPad *teepad;
  GstElement *queue;
  GstElement *filter;
  GstElement *demux;
  GstElement *parse;
  GstElement *dec;
  GstElement *conv;
  GstElement *scale;
  GstElement *sink;
  gboolean removing;
} Sink;

/*static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err = NULL;
      gchar *name, *debug = NULL;

      name = gst_object_get_path_string (message->src);
      gst_message_parse_error (message, &err, &debug);

      g_printerr ("ERROR: from element %s: %s\n", name, err->message);
      if (debug != NULL)
        g_printerr ("Additional debug info:\n%s\n", debug);

      g_error_free (err);
      g_free (debug);
      g_free (name);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_WARNING:{
      GError *err = NULL;
      gchar *name, *debug = NULL;

      name = gst_object_get_path_string (message->src);
      gst_message_parse_warning (message, &err, &debug);

      g_printerr ("ERROR: from element %s: %s\n", name, err->message);
      if (debug != NULL)
        g_printerr ("Additional debug info:\n%s\n", debug);

      g_error_free (err);
      g_free (debug);
      g_free (name);
      break;
    }
    case GST_MESSAGE_EOS:
      g_print ("Got EOS\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}*/

static GstPadProbeReturn
unlink_cb (GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
  Sink *sink = user_data;
  GstPad *sinkpad;

  if (!g_atomic_int_compare_and_exchange (&sink->removing, FALSE, TRUE))
    return GST_PAD_PROBE_OK;

  sinkpad = gst_element_get_static_pad (sink->filter, "sink");
  gst_pad_unlink (sink->teepad, sinkpad);
  gst_object_unref (sinkpad);

  gst_bin_remove (GST_BIN (pipeline), sink->filter);
  gst_bin_remove (GST_BIN (pipeline), sink->demux);
  gst_bin_remove (GST_BIN (pipeline), sink->parse);
  gst_bin_remove (GST_BIN (pipeline), sink->dec);
  gst_bin_remove (GST_BIN (pipeline), sink->conv);
  gst_bin_remove (GST_BIN (pipeline), sink->scale);
  gst_bin_remove (GST_BIN (pipeline), sink->sink);

  gst_element_set_state (sink->filter, GST_STATE_NULL);
  gst_element_set_state (sink->demux, GST_STATE_NULL);
  gst_element_set_state (sink->parse, GST_STATE_NULL);
  gst_element_set_state (sink->dec, GST_STATE_NULL);
  gst_element_set_state (sink->conv, GST_STATE_NULL);
  gst_element_set_state (sink->scale, GST_STATE_NULL);
  gst_element_set_state (sink->sink, GST_STATE_NULL);

  gst_object_unref (sink->filter);
  gst_object_unref (sink->demux);
  gst_object_unref (sink->parse);
  gst_object_unref (sink->dec);
  gst_object_unref (sink->conv);
  gst_object_unref (sink->scale);
  gst_object_unref (sink->sink);

  gst_element_release_request_pad (tee, sink->teepad);
  gst_object_unref (sink->teepad);

  g_print ("removed\n");

  return GST_PAD_PROBE_REMOVE;
}

static gboolean tick_cb (gpointer data)
{

  GeneratePipelineDot(pipeline); //Debug
  if (!sinks || g_random_int () % 2 == 0) {
    Sink *sink = g_new0 (Sink, 1);
    GstPad *sinkpad;
    GstPadTemplate *templ;

    templ = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee), "src_%u");

    g_print ("add\n");

    sink->teepad = gst_element_request_pad (tee, templ, NULL, NULL);

    sink->filter = gst_element_factory_make ("mpegtspidfilter", NULL);
    sink->demux = gst_element_factory_make ("tsdemux", NULL);
    sink->parse = gst_element_factory_make ("mpegvideoparse", NULL);
    sink->dec = gst_element_factory_make ("avdec_mpeg2video", NULL);
    sink->conv = gst_element_factory_make ("videoconvert", NULL);
    sink->scale = gst_element_factory_make ("videoscale", NULL);
    sink->sink = gst_element_factory_make ("autovideosink", NULL);
    sink->removing = FALSE;

    g_object_set (G_OBJECT (sink->filter), "pids", "120:130:131:133:1537:0:100", NULL);

    gst_bin_add_many (GST_BIN (pipeline), gst_object_ref (sink->filter), gst_object_ref (sink->demux), gst_object_ref (sink->parse), gst_object_ref (sink->dec), gst_object_ref (sink->conv), gst_object_ref (sink->scale), gst_object_ref (sink->sink), NULL);
    gst_element_link (sink->filter, sink->demux);
    gst_element_link_many (sink->parse, sink->dec, sink->conv, sink->scale, sink->sink, NULL);

    g_signal_connect (sink->demux, "pad-added", G_CALLBACK (on_pad_added), sink->parse);

    gst_element_sync_state_with_parent (sink->filter);
    gst_element_sync_state_with_parent (sink->demux);
    gst_element_sync_state_with_parent (sink->parse);
    gst_element_sync_state_with_parent (sink->dec);
    gst_element_sync_state_with_parent (sink->conv);
    gst_element_sync_state_with_parent (sink->scale);
    gst_element_sync_state_with_parent (sink->sink);

    sinkpad = gst_element_get_static_pad (sink->filter, "sink");
    gst_pad_link (sink->teepad, sinkpad);
    gst_object_unref (sinkpad);

    g_print ("added\n");

    sinks = g_list_append (sinks, sink);
  } else {
    Sink *sink;

    g_print ("remove\n");

    sink = sinks->data;
    sinks = g_list_delete_link (sinks, sinks);
    gst_pad_add_probe (sink->teepad, GST_PAD_PROBE_TYPE_IDLE, unlink_cb, sink,
        (GDestroyNotify) g_free);
  }

  return TRUE;
}

static void pad_added_cb (GstElement * element, GstPad * pad, gpointer user_data)
{
  g_print("pad_added\n");

  GstElement *temptee = (GstElement *) user_data;

  GstStructure *s;
  const gchar *name;

  if (linked)
    return;

  GeneratePipelineDot(pipeline); //Debug

  GstPad *sinkpad, *teepad;
  GstElement *queue, *sink;
  GstPadTemplate *templ;

  sinkpad = gst_element_get_static_pad (temptee, "sink");
  if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK) {
    g_printerr ("Failed to link tee with queue\n");
    gst_object_unref (sinkpad);
    g_main_loop_quit (loop);
    return;
  }
  gst_object_unref (sinkpad);

  templ =
      gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (tee),
      "src_%u");
  teepad = gst_element_request_pad (tee, templ, NULL, NULL);
  queue = gst_element_factory_make ("queue2", NULL);
  sink = gst_element_factory_make ("fakesink", NULL);

  g_object_set (sink, "sync", TRUE, NULL);

  gst_bin_add_many (GST_BIN (pipeline), queue, sink, NULL);
  gst_element_link_many (queue, sink, NULL);

  sinkpad = gst_element_get_static_pad (queue, "sink");
  gst_pad_link (teepad, sinkpad);
  gst_object_unref (sinkpad);

  GeneratePipelineDot(pipeline); //Debug

  g_timeout_add_seconds (3, tick_cb, NULL);
  linked = TRUE;

  GeneratePipelineDot(pipeline); //Debug
}

int
main (int argc, char **argv)
{
  if (argc != 1) {
    g_error ("Usage: no args");
    return 0;
  }

  gst_init (&argc, &argv);

  pipeline = gst_pipeline_new (NULL);
  src = gst_element_factory_make ("dvbsrc", NULL);
  //src = gst_element_factory_make ("dvbbasebin", NULL);
  filter = gst_element_factory_make ("queue", NULL);
  tee = gst_element_factory_make ("tee", NULL);

  g_object_set (G_OBJECT (src), "frequency", 562000000, NULL);
  g_object_set (G_OBJECT (src), "inversion", 1, NULL);
  g_object_set (G_OBJECT (src), "code-rate-lp", 1, NULL);
  g_object_set (G_OBJECT (src), "bandwidth", 0, NULL);
  g_object_set (G_OBJECT (src), "modulation", 1, NULL);
  g_object_set (G_OBJECT (src), "trans_mode", 1, NULL);
  g_object_set (G_OBJECT (src), "guard", 1, NULL);
  g_object_set (G_OBJECT (src), "hierarchy", 1, NULL);
  //g_object_set (G_OBJECT (src), "program-numbers", "1537", NULL);
  //g_object_set (G_OBJECT (filter), "pids", "120:130:131:133:1537:0:100", NULL);
  //g_object_set (G_OBJECT (filter), "pids", "220:230:232:233:1538:0:210", NULL);

  gst_bin_add_many (GST_BIN (pipeline), src, filter, tee, NULL);
  gst_element_link (src, filter);

  GeneratePipelineDot(pipeline); //Debug

  pad_added_cb (filter, gst_element_get_static_pad (filter, "src"), tee);

  loop = g_main_loop_new (NULL, FALSE);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);

  gst_object_unref (pipeline);

  return 0;
}

