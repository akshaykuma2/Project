#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <pthread.h>
#include<stdlib.h>


static void* mypipeline(void* args);
GstBus *bus;
GstMessage *msg;
GstElement *pipeline;
pthread_t pipeline_creation;  // This thread will create pipeline and handle
pthread_t control_thread;     // This thread to handle user inputs while vido is playing
static int loop;





// Thread to handle the inputs given by user while video is playing
static void* keyboard(void* args)
{
	char key;
	while(1)
	{
		scanf("%c",&key);
		if (key=='e')
		{
			exit(0);
		}
		if(key=='p')
		{
			GstState state;							//Knowing the state of the of video
			gst_element_get_state(pipeline,&state,NULL,GST_CLOCK_TIME_NONE);
			if (state==GST_STATE_PLAYING)					// video is playing setting it to pause
			{
				gst_element_set_state(pipeline,GST_STATE_PAUSED);
				printf("video is paused\n");
		
			}
			else if(state==GST_STATE_PAUSED)				// if video is paused setting it to play
			{
				gst_element_set_state(pipeline,GST_STATE_PLAYING); 
				printf("video is playing\n");
			
			}
    		}	
    		else if(key=='l')							// To loop the video 
    		{
    			printf("Entered loop mode\n");
			printf("To exit loop mode again press l\n");   	
    		if(loop ==1)
    		{
    			printf("Exiting loop mood\n");
    			loop=0;
    		}
    		else if(loop==0)
    		{
    		//printf("Video playing in loop\n");
    			loop=1;
    		}
    		}	
    		else if(key== 'n') 							//To take new input from the user
    		{
        	   
			char url[2064];
			g_print("Enter link:");
			scanf("%s",url);
			/*unrefernce the exesting pipeline and calling the the pipeline thread*/
			gst_element_set_state(pipeline, GST_STATE_NULL);
			gst_object_unref(pipeline);
        		pthread_create(&pipeline_creation, NULL, mypipeline, url) ;
   			pthread_join(pipeline_creation, NULL);
			
    	   
    
   		}
  	}
  return NULL;
 }
 
 
// thread for pipeline cretaion


static void* mypipeline(void* args)
{
	
        char* path=(char*)args;	//Type casting the arguments
	
	char input[2048];
	
	sprintf(input,"playbin uri=%s",path);   //Copying the path into input 	
	do
	{
		pipeline=gst_parse_launch(input,NULL);		//Creating pipeline
		if(!pipeline){					//Checking weather pipeline created or not
			printf("Failed to create pipeline\n");
			break;
		}
		
		gst_element_set_state(pipeline,GST_STATE_PLAYING);   //Setting pipeline to play
		//printf("i called by thread\n");
		
		pthread_create(&control_thread, NULL, keyboard, pipeline);  	//creating control thread
	
	
	
	
		bus = gst_element_get_bus(pipeline);
	        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
                gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
		printf("pipeline closed\n");
        }while(loop==1);
    	if (msg != NULL) {
    	GError *err;
    	gchar *debug_info;
    	switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
        GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
        debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should n
        ot reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }
    gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	printf("pipeline closed\n");
	
	pthread_join(control_thread,NULL);
	pthread_cancel(control_thread);
        pthread_exit(NULL);
        printf("hi\n");
 
 }
 
 
 
 
 
/*Main thread*/
int main(int argc,char *argv[])  //Main thread
{


gst_init(&argc, &argv);

  char url[1024];
	g_print("Enter link:");
	scanf("%s",url);           //asking user for url needs to play
	
  
  printf("Here are the list of controls\n");
	printf("Press p for play or pause the video\n");
	printf("Press n to play a new video\n");
	printf("Press l to play the in video loop\n");
	printf("Again press l to terminate the loop\n");
	printf("Press e to exit the video\n");

    /* Create a thread for pipeline creation */
   
    pthread_create(&pipeline_creation, NULL, mypipeline, url);
    pthread_join(pipeline_creation,NULL);
    
    

   




gst_message_unref (msg);
gst_object_unref (bus);
gst_element_set_state(pipeline, GST_STATE_NULL);
gst_object_unref(pipeline);
return 0;
}
