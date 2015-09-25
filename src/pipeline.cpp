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
   std::vector<const char*> channel_number;

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

   bool is_present(const char* data)
      {
        for (int i = 0; i<channel_number.size() ;i++)
        {
          if (!strcmp(data, channel_number[i]))
          {
            return TRUE;
          }
        }
        return FALSE;
      }

   void add (const char* data)
      {
        channel_number.push_back(data);
      }

   void remove (const char* data)
      {
        for (int i = 0; i<channel_number.size() ;i++)
        {
          if (!strcmp(data, channel_number[i]))
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
     GstElement *pipeline, *dvbsrc, *tee, *mpegtspidfilter[6], *queue[6], *multisocketsink[6];
     GstPadTemplate *tee_src_pad_template;
     GstPad *tee_pad[6];
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
int info_parse (char *info, char *chaine, int frequency, char *pids)
{
  char *temp;
  temp = strtok (info, ":");

  while(temp != NULL)
  {
    strcat (chaine, temp);
    temp = strtok (NULL, ":");

    frequency = atoi(temp);
    temp = strtok (NULL, ":");

    strcpy (pids, temp);
    temp = strtok (NULL, ":");
  }
  return frequency;
}
/***********************************************************************************/



/************************************* pipeline ************************************/
int pipeline (char *info, int fd_socket)
{
  GError **err;

  g_print ("fd : %d\n",fd_socket);

  int i = 0, j = 0;
  bool end = false;
  char name_mpegtspidfilter[30] = "", name_multisocketsink[30] = "";


  int frequency;
  char chaine[30] = "", pids[30] = "";

  g_print ("info : %s\n", info);

  if (info != NULL)
  {
    g_print ("parsing : \n");
    frequency = info_parse (info, chaine, frequency, pids);
  }
  else
    g_print ("cannot parse : NULL\n");

  strcat(name_mpegtspidfilter, "mpegtspidfilter_");
  strcat(name_mpegtspidfilter, chaine);
  strcat(name_multisocketsink, "multisocketsink_");
  strcat(name_multisocketsink, chaine);

  channel_list *list;
  list = channel_list::Instantiate();

  GSocket *socket = g_socket_new_from_fd ((gint)fd_socket, err);

  static _CustomData *data = NULL;

  if (data == NULL)
    {
    data = (_CustomData*)malloc(sizeof(_CustomData));

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
    g_object_set (G_OBJECT (data->dvbsrc), "frequency", frequency, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "inversion", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "code-rate-lp", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "bandwidth", 0, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "modulation", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "trans_mode", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "guard", 1, NULL);
    g_object_set (G_OBJECT (data->dvbsrc), "hierarchy", 1, NULL);

    /* we add all elements into the pipeline */
    gst_bin_add_many (GST_BIN (data->pipeline), data->dvbsrc, data->tee, NULL);

    g_print ("Added all the Elements into the pipeline\n");

    gst_element_link (data->dvbsrc, data->tee);

    data->tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (data->tee), "src_%u");

    for(j=0;j<6;j++)
    data->tee_pad[j] = gst_element_request_pad (data->tee, data->tee_src_pad_template, NULL, NULL);
  }

  /* Create filtering part */
  if (!list->is_present(chaine))
  {
    while (i<=6 && !end)
    {
      gulong id;

      if (list->get_size() == i)
      {
	char *current;
	current = strdup(chaine);
	list->add(current);

        //data->tee_pad[i] = gst_element_request_pad (data->tee, data->tee_src_pad_template, NULL, NULL);

	/* Create gstreamer elements */
	data->mpegtspidfilter[i] = gst_element_factory_make ("mpegtspidfilter", name_mpegtspidfilter);
	data->multisocketsink[i] = gst_element_factory_make ("multisocketsink", name_multisocketsink);

	/* set the properties of mpegtspidfilter */
	g_object_set (G_OBJECT (data->mpegtspidfilter[i]), "pids", pids, NULL);
	g_print ("pids : %s\n", pids);

	/* set the properties of multisocketsink */
	g_object_set (G_OBJECT (data->multisocketsink[i]),
	     "unit-format", GST_FORMAT_TIME,
	     "units-max", (gint64) 7 * GST_SECOND,
	     "units-soft-max", (gint64) 3 * GST_SECOND,
	     "recover-policy", 3,
	     "timeout", (guint64) 10 * GST_SECOND,
 	     "sync-method", 1,
	     "sync", FALSE,
	     "async", FALSE,
	     NULL);

	/* we add all elements into the pipeline */
	gst_bin_add_many (GST_BIN (data->pipeline), (GstElement *) gst_object_ref (data->mpegtspidfilter[i]), (GstElement *) gst_object_ref (data->multisocketsink[i]), NULL);

	gst_element_link_many (data->mpegtspidfilter[i], data->multisocketsink[i], NULL);

	gst_element_sync_state_with_parent (data->mpegtspidfilter[i]);
	gst_element_sync_state_with_parent (data->multisocketsink[i]);

        GeneratePipelineDot(data->pipeline); //Debug

	GstPad *sinkpad = gst_element_get_static_pad (data->mpegtspidfilter[i], "sink");
	gst_pad_link (data->tee_pad[i], sinkpad);
	gst_object_unref (sinkpad);

	g_print ("Create a pipeline with %s\n", gst_element_get_name(data->mpegtspidfilter[i]));

	end = true;
      }
      i++;
    }
  }
  else
    g_print ("channel already asked\n");

  /* Set the pipeline to "playing" state*/
  g_print ("Playing the video\n");

  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Running...\n");

 for (i=0;i<list->get_size();i++)
  {
    if(!strcmp(name_mpegtspidfilter, gst_element_get_name(data->mpegtspidfilter[i])))
    {
      g_signal_emit_by_name (data->multisocketsink[i], "add", socket);
      g_print ("add socket to %s\n", gst_element_get_name(data->multisocketsink[i]));
    }
  }

  sleep(3);

  GeneratePipelineDot(data->pipeline); //Debug

  /*if ( no more client )
    free(data);*/

  #if 0
  g_main_loop_run (loop);


  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (data->pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
  #endif

  return 0;
}
/***********************************************************************************/
