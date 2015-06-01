#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <include/heliostv.h>
#include <gio/gio.h>


/***********************************************************************************/
/******************************* Def Struct Pipeline *******************************/
typedef struct {
     const char *channel, *host;
     int frequency, port;
     GstElement *pipeline, *dvbbasebin, *demuxer, *typefind, *multisocketsink;
} _CustomData;
/***********************************************************************************/




/***********************************************************************************/
/*********************************** Dynamic pad ***********************************/
static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("------> Dynamic pad created, linking decoder/converter\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}
/***********************************************************************************/




/***********************************************************************************/
/********************************* Select Channel **********************************/
int channel (const char *channels, _CustomData *data)
{
   if (!strcmp(channels,"TF1") || !strcmp(channels,"1"))
          {
          data->channel="1537";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"France2") || !strcmp(channels,"2"))
          {
          data->channel="257";
          data->frequency=586000000;
          }
    else if (!strcmp(channels,"France3") || !strcmp(channels,"3"))
          {
          data->channel="273";
          data->frequency=586000000;
          }
    else if (!strcmp(channels,"CANAL+") || !strcmp(channels,"4"))
          {
          data->channel="769";
          data->frequency=482000000;
          }
    else if (!strcmp(channels,"France5") || !strcmp(channels,"5"))
          {
          data->channel="260";
          data->frequency=586000000;
          }
    else if (!strcmp(channels,"M6") || !strcmp(channels,"6"))
          {
          data->channel="1025";
          data->frequency=546000000;
          }
    else if (!strcmp(channels,"ARTE") || !strcmp(channels,"7"))
          {
          data->channel="1543";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"D8") || !strcmp(channels,"8"))
          {
          data->channel="513";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"W9") || !strcmp(channels,"9"))
          {
          data->channel="1026";
          data->frequency=546000000;
          }
    else if (!strcmp(channels,"TMC") || !strcmp(channels,"10"))
          {
          data->channel="1542";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"NT1") || !strcmp(channels,"11"))
          {
          data->channel="1027";
          data->frequency=546000000;
          }
    else if (!strcmp(channels,"NRJ12") || !strcmp(channels,"12"))
          {
          data->channel="1538";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"LCP") || !strcmp(channels,"13"))
          {
          data->channel="262";
          data->frequency=586000000;
          }
    else if (!strcmp(channels,"France4") || !strcmp(channels,"14"))
          {
          data->channel="519";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"BFMTV") || !strcmp(channels,"15"))
          {
          data->channel="515";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"ITELE") || !strcmp(channels,"16"))
          {
          data->channel="516";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"D17") || !strcmp(channels,"17"))
          {
          data->channel="517";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"Gulli") || !strcmp(channels,"18"))
          {
          data->channel="518";
          data->frequency=506000000;
          }
    else if (!strcmp(channels,"FranceO") || !strcmp(channels,"19"))
          {
          data->channel="261";
          data->frequency=586000000;
          }
    else if (!strcmp(channels,"HD1") || !strcmp(channels,"20"))
          {
          data->channel="2561";
          data->frequency=642000000;
          }
    else if (!strcmp(channels,"LEQUIPE21") || !strcmp(channels,"21"))
          {
          data->channel="2562";
          data->frequency=642000000;
          }
    else if (!strcmp(channels,"6TER") || !strcmp(channels,"22"))
        {
          data->channel="2817";
          data->frequency=642000000;
          }
    else if (!strcmp(channels,"NUMERO23") || !strcmp(channels,"23"))
          {
          data->channel="2818";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"RMC") || !strcmp(channels,"24"))
          {
          data->channel="2819";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"CHERIE25") || !strcmp(channels,"25"))
          {
          data->channel="2563";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"IDF1") || !strcmp(channels,"32"))
          {
          data->channel="2051";
          data->frequency=570000000;
          }
    else if (!strcmp(channels,"France24") || !strcmp(channels,"33"))
          {
          data->channel="2052";
          data->frequency=570000000;
          }
    else if (!strcmp(channels,"BFMBUSINESS") || !strcmp(channels,"34"))
          {
          data->channel="2053";
          data->frequency=570000000;
          }
    else if (!strcmp(channels,"PARISPREMIERE") || !strcmp(channels,"41"))
          {
          data->channel="1028";
          data->frequency=546000000;
          }
    else if (!strcmp(channels,"CANAL+SPORT") || !strcmp(channels,"42"))
          {
          data->channel="771";
          data->frequency=482000000;
          }
    else if (!strcmp(channels,"CANAL+CINEMA") || !strcmp(channels,"43"))
          {
          data->channel="770";
          data->frequency=482000000;
          }
    else if (!strcmp(channels,"PLANETE+") || !strcmp(channels,"45"))
          {
          data->channel="772";
          data->frequency=482000000;
          }
    else if (!strcmp(channels,"LCI") || !strcmp(channels,"48"))
          {
          data->channel="1539";
          data->frequency=562000000;
          }
    else if (!strcmp(channels,"TF1HD") || !strcmp(channels,"51"))
          {
          data->channel="1281";
          data->frequency=530000000;
          }
    else if (!strcmp(channels,"France2HD") || !strcmp(channels,"52"))
          {
          data->channel="1282";
          data->frequency=530000000;
          }
    else if (!strcmp(channels,"M6HD") || !strcmp(channels,"56"))
          {
          data->channel="1283";
          data->frequency=530000000;
          }
    else if (!strcmp(channels,"ARTEHD") || !strcmp(channels,"57"))
          {
          data->channel="1031";
          data->frequency=546000000;
          }
    else {
          g_print("Channel doesn't exist\nChannels available :\n\t1 - TF1\t\t20 - HD1 (Privee)\n\t2 - France2\t21 - LEQUIPE21 (Privee)\n\t3 - France3\t22 - 6TER (Privee)\n\t4 - CANAL+\t23 - NUMERO23 (Privee)\n\t5 - France5\t24 - RMC (Privee)\n\t6 - M6\t\t25 - CHERIE25 (Privee)\n\t7 - ARTE\t32 - IDF1 (Pas de flux ?)\n\t8 - D8\t\t33 - France24 (Pas de flux ?)\n\t9 - W9\t\t34 - BFMBUSINESS (Pas de flux ?)\n\t10 - TMC\t41 - PARISPREMIERE (Privee)\n\t11 - NT1\t42 - CANAL+SPORT (Privee)\n\t12 - NRJ12\t43 - CANAL+CINEMA (Privee)\n\t13 - LCP\t45 - PLANETE+ (Privee)\n\t14 - France4\t48 - LCI (Privee)\n\t15 - BFMTV\t51 - TF1HD\n\t16 - ITELE\t52 - France2HD\n\t17 - D17\t56 - M6HD\n\t18 - Gulli\t57 - ARTEHD\n\t19 - FranceO\n");
          return -1;
    }
}
/***********************************************************************************/




/************************************* pipeline ************************************/
int pipeline (const char *chaine, const char *host, int port, int fd_socket)
{
  GMainLoop *loop;

  GstBus *bus;
  guint bus_watch_id;
  GError **err;

  g_print ("fd = %d\n",fd_socket);

  GSocket *socket = g_socket_new_from_fd ((gint)fd_socket, err);

  _CustomData data;

  int Test=0;

  data.host = host;
  data.port = port;


  /* Initialisation */
  loop = g_main_loop_new (NULL, FALSE);


  /* set the channel */
  Test=channel(chaine, &data);
  if (Test==-1)
    return -1;


  /* Create gstreamer elements */
  data.pipeline = gst_pipeline_new ("mpegtsplayer");
  data.dvbbasebin = gst_element_factory_make ("dvbbasebin", "dvbbasebin");
  data.typefind = gst_element_factory_make ("typefind", "typefind");
  data.multisocketsink = gst_element_factory_make ("multisocketsink", "tcp-sink");

 
  /* check audio/video */
  if (!data.pipeline || !data.dvbbasebin || !data.typefind || !data.multisocketsink)
  {
    g_print ("One element could not be created. Exiting.\n");
    return -1;
  }

  /* Set up the pipeline */
  g_print ("Elements are created\n");

  /* set the properties of dvbbasebin */
  g_object_set (G_OBJECT (data.dvbbasebin), "frequency", data.frequency, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "inversion", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "code-rate-lp", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "bandwidth", 0, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "modulation", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "trans_mode", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "guard", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "hierarchy", 1, NULL);
  g_object_set (G_OBJECT (data.dvbbasebin), "program-numbers", data.channel, NULL);

  /* set the properties of queueaudio */
  //g_object_set (G_OBJECT (data.tcpserversink), "host", data.host, NULL);
  //g_object_set (G_OBJECT (data.tcpserversink), "port", data.port, NULL);
  g_object_set (G_OBJECT (data.multisocketsink),
      "unit-format", GST_FORMAT_TIME,
      "units-max", (gint64) 7 * GST_SECOND,
      "units-soft-max", (gint64) 3 * GST_SECOND,
      "recover-policy", 3 /* keyframe */ ,
      "timeout", (guint64) 10 * GST_SECOND,
      "sync-method", 1 /* next-keyframe */ ,
      NULL); 


  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (data.pipeline), data.dvbbasebin, data.typefind, data.multisocketsink, NULL);

  g_print ("Added all the Elements into the pipeline\n");

  /* we link the elements together */
  gst_element_link_many (data.dvbbasebin, data.typefind, data.multisocketsink, NULL);

  g_print ("Linked all the Elements together\n");

  /* Set the pipeline to "playing" state*/
  g_print ("Playing the video\n");
  gst_element_set_state (data.pipeline, GST_STATE_PLAYING);

  g_signal_emit_by_name (data.multisocketsink, "add", socket);

  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);


  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (data.pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (data.pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
} 
/***********************************************************************************/

