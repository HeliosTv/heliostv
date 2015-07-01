#include <include/heliostv.h>
#include <vector>
#include <cstdlib>
#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gio/gio.h>

/***********************************************************************************/
/************************** class channel list singleton ***************************/
class channel_list
{
private:
   std::vector<int> channel_number;

   channel_list () : channel_number({})
      {
      }

   //token_list* operator= (const token_list*){}

public:
   static channel_list* Instantiate()
      {
        static channel_list *list = NULL;
        if(list == NULL)
        {
            list = new channel_list();
        }
        return list;
      }

   bool is_present(int data)
      { 
        for (int i = 0; i<channel_number.size() ;i++) 
        {
          if (data == channel_number[i])
          {
            return TRUE;
          }
        }
        return FALSE;
      }

   void add (int data) 
      { 
        channel_number.push_back(data);
      }

   void remove (int data) 
      { 
        for (int i = 0; i<channel_number.size() ;i++) 
        {
          if (data == channel_number[i])
          {
            channel_number.erase(channel_number.begin()+i);
          }
        }
      }

   int get_size () 
      { 
	int size = channel_number.size();
	return size;
      }

};
/***********************************************************************************/



/***********************************************************************************/
/******************************* Def Struct Pipeline *******************************/
typedef struct {
     const char *channel, *pids;
     int frequency;
     GstElement *pipeline, *dvbsrc, *tee, *multisocketsink[6];
} _CustomData;
/***********************************************************************************/



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



/***********************************************************************************/
/*********************************** Dynamic pad ***********************************/
static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}
/***********************************************************************************/



/***********************************************************************************/
/********************************* Select Channel **********************************/
int channel (const char *channels, _CustomData *data)
{
	  data->pids = "0";
   if (!strcmp(channels,"TF1") || !strcmp(channels,"1"))
          {
          data->frequency=562000000;
	  data->pids = "0,100,120,130,131,133,1537";
          }
    else if (!strcmp(channels,"France2") || !strcmp(channels,"2"))
          {
          data->frequency=586000000;
	  data->pids = "0,110,120,130,131,132,257";
          }
    else if (!strcmp(channels,"France3") || !strcmp(channels,"3"))
          {
          data->frequency=586000000;
	  data->pids = "0,210,220,230,231,273";
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
          data->frequency=562000000;
	  data->pids = "0,700,720,730,731,732,733,1543";
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
          data->frequency=562000000;
	  data->pids = "0,600,620,630,631,633,1542";
          }
    else if (!strcmp(channels,"NT1") || !strcmp(channels,"11"))
          {
          data->channel="1027";
          data->frequency=546000000;
          }
    else if (!strcmp(channels,"NRJ12") || !strcmp(channels,"12"))
          {
          data->frequency=562000000;
	  data->pids = "0,210,220,230,232,233,1538";
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
          data->frequency=770000000;
          }
    else if (!strcmp(channels,"NUMERO23") || !strcmp(channels,"23"))
          {
          data->channel="2818";
          data->frequency=770000000;
          }
    else if (!strcmp(channels,"RMC") || !strcmp(channels,"24"))
          {
          data->channel="2819";
          data->frequency=770000000;
          }
    else if (!strcmp(channels,"CHERIE25") || !strcmp(channels,"25"))
          {
          data->channel="2563";
          data->frequency=642000000;
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
          data->frequency=562000000;
	  data->pids = "0,300,320,330,1539";
          }
    else if (!strcmp(channels,"TF1HD") || !strcmp(channels,"51"))
          {
          data->frequency=530000000;
	  data->pids = "0,110,120,130,131,132,1281";
          }
    else if (!strcmp(channels,"France2HD") || !strcmp(channels,"52"))
          {
          data->frequency=530000000;
	  data->pids = "0,210,220,230,231,232,1282";
          }
    else if (!strcmp(channels,"M6HD") || !strcmp(channels,"56"))
          {
          data->frequency=530000000;
	  data->pids = "0,310,320,330,331,332,1283";
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
  g_print ("mpegtspidfilter\n");

  GMainLoop *loop;

  GError **err;

  g_print ("fd = %d\n",fd_socket);

  int i = 0;
  bool end = false;

  channel_list *list;
  list = channel_list::Instantiate();

  GSocket *socket = g_socket_new_from_fd ((gint)fd_socket, err);

  static _CustomData *data = NULL;

  if (data == NULL)
    { 
    GstPadTemplate *tee_src_pad_template;
    GstPad *tee_channel_pad, *mpegtspidfilter_channel_pad;

    data = (_CustomData*)malloc(sizeof(_CustomData));

    int Test=0;

    /* Initialisation */
    loop = g_main_loop_new (NULL, FALSE);

    /* set the channel */
    Test=channel(chaine, data);
    if (Test==-1)
      return -1;

    /* Create gstreamer elements */
    data->pipeline = gst_pipeline_new ("mpegtsplayer");
    data->dvbsrc = gst_element_factory_make ("dvbsrc", "dvbsrc");
    data->tee = gst_element_factory_make ("tee", "tee");

 
    /* check creation */
    if (!data->pipeline || !data->dvbsrc || !data->tee)
    {
      g_print ("One element could not be created-> Exiting->\n");
      return -1;
    }

    /* Set up the pipeline */
    g_print ("Elements are created\n");

    /* set the properties of dvbsrc */
    g_object_set (G_OBJECT (data->dvbsrc), "frequency", data->frequency, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "inversion", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "code-rate-lp", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "bandwidth", 0, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "modulation", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "trans_mode", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "guard", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "hierarchy", 1, NULL);


    /* we add all elements into the pipeline */
    gst_bin_add_many (GST_BIN (data->pipeline), data->dvbsrc, data->tee, NULL);

    GeneratePipelineDot(data->pipeline); //Debug

    g_print ("Added all the Elements into the pipeline\n");

    /* we link the elements together */
    gst_element_link (data->dvbsrc, data->tee);
  }

  if (!list->is_present(atoi(chaine)))
  {
    int Test;
    Test=channel(chaine, data);
    if (Test==-1)
      return -1;

    while (i<=6 && !end)
    {
      GstElement *mpegtspidfilter[6];
      GstPadTemplate *tee_src_pad_template[6];
      GstPad *tee_channel_pad[6], *mpegtspidfilter_channel_pad[6];

      g_print ("size : %d\n", list->get_size());

      if (list->get_size() == i)
      {
	list->add(atoi(chaine));

	/* Create gstreamer elements */
	char name_mpegtspidfilter[20], name_multisocketsink[20];
	sprintf(name_mpegtspidfilter, "mpegtspidfilter_%d", i);
	sprintf(name_multisocketsink, "multisocketsink_%d", i);
	mpegtspidfilter[i] = gst_element_factory_make ("mpegtspidfilter", name_mpegtspidfilter);
	data->multisocketsink[i] = gst_element_factory_make ("multisocketsink", name_multisocketsink);

 
	/* check creation */
	if (!mpegtspidfilter[i] || !data->multisocketsink[i])
	{
	  g_print ("One element could not be created-> Exiting->\n");
	  return -1;
	}

	/* Set up the pipeline */
	g_print ("Elements are created\n");

	/* set the properties of mpegtspidfilter */
	g_object_set (G_OBJECT (mpegtspidfilter[i]), "pids", data->pids, NULL);
	g_print ("pids : %s\n", data->pids);

	/* set the properties of multisocketsink */
	g_object_set (G_OBJECT (data->multisocketsink[i]),
	     "unit-format", GST_FORMAT_TIME,
	     "units-max", (gint64) 7 * GST_SECOND,
	     "units-soft-max", (gint64) 3 * GST_SECOND,
	     "recover-policy", 3 /* keyframe */ ,
	     "timeout", (guint64) 10 * GST_SECOND,
	     "sync-method", 1 /* next-keyframe */ ,
	     NULL); 

	/* we add all elements into the pipeline */
	gst_bin_add_many (GST_BIN (data->pipeline), mpegtspidfilter[i], data->multisocketsink[i], NULL);

	g_print ("Added all the Elements into the pipeline\n");

	/* we link the elements together */
	gst_element_link (mpegtspidfilter[i], data->multisocketsink[i]);

	/* Manually link the Tee, which has "Request" pads */
	/*tee_src_pad_template[i] = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (data->tee), "src_%u");
	tee_channel_pad[i] = gst_element_request_pad (data->tee, tee_src_pad_template[i], "src_%u", NULL);
	mpegtspidfilter_channel_pad[i] = gst_element_get_static_pad (mpegtspidfilter[i], "sink");
	gst_pad_link (tee_channel_pad[i], mpegtspidfilter_channel_pad[i]);
	gst_object_unref (mpegtspidfilter_channel_pad[i]);
*/
 	g_signal_connect (data->tee, "pad-added", G_CALLBACK(on_pad_added), mpegtspidfilter[i]);

	tee_src_pad_template[i] = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (data->tee), "src_%u");
	tee_channel_pad[i] = gst_element_request_pad (data->tee, tee_src_pad_template[i], "src_%u", NULL);

	g_print ("Linked all the Elements together\n");

	end = true;
      }
      i++;
    }
  }
  else
  {
    g_signal_emit_by_name (data->multisocketsink[0], "add", socket);
  }


  /* Set the pipeline to "playing" state*/
  g_print ("Playing the video\n");
  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);

  GeneratePipelineDot(data->pipeline); //Debug

  g_print ("i = %d\n", i);
  g_signal_emit_by_name (data->multisocketsink[i-1], "add", socket);

  /* Iterate */
  g_print ("Running...\n");

  GeneratePipelineDot(data->pipeline); //Debug

  /*if ( no more client )
    free(data);*/

  #if 0 
  g_main_loop_run (loop);


  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (data->pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (data->pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
  #endif

  return 0;
} 
/***********************************************************************************/

