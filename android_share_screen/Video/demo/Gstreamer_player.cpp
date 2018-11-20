/* 

   CFLAGS=`pkg-config --cflags gtk+-2.0 gstreamer-0.10`  
   LIBS=`pkg-config --libs gtk+-2.0 gstreamer-0.10`  

all: player  

player:   
gcc -o MediaPlayer main.c main.h $(LIBS) $(CFLAGS) -l gstinterfaces-0.10  

clean:  
rm *.o player 

gcc -o MediaPlayer Gstreamer_player.cpp `pkg-config --cflags gtk+-2.0 gstreamer-0.10` `pkg-config --libs gtk+-2.0 gstreamer-0.10` -lgstinterfaces-0.10
 *main.c  
 *Simple media player based on Gstreamer and GTK 
 */  
#include <gst/gst.h>  
#include <gdk/gdkx.h>  
#include <gtk/gtk.h>  
//#include <gst/interfaces/xoverlay.h>  
#include <string.h>  

typedef enum {  
	STATE_STOP,  
	STATE_PLAY,  
	STATE_PAUSE  
} PlayerState;  

void gui_status_update(PlayerState state);  
void gui_update_time(const gchar *time, const gint64 position, const gint64 length);  
// void gui_update_metadata(const gchar *title, const gchar *artist);  

gboolean load_file(const gchar *uri);  
void seek_to(gdouble percentage); 

static GtkWidget *main_window;  
static GtkWidget *play_button;  
static GtkWidget *pause_button;  
static GtkWidget *stop_button;  
static GtkWidget *status_label;  
static GtkWidget *time_label;  
static GtkWidget *seek_scale;  
static GtkWidget *video_output;  
static gpointer window;  

static GstElement *play = NULL;  
static GstElement *bin;  

static guint timeout_source = 0;  
static char *current_filename = NULL;  
gboolean no_seek = FALSE;  
// 打开文件  
static void file_open(GtkAction *action)  
{  
	GtkWidget *file_chooser = gtk_file_chooser_dialog_new(  
			"Open File", GTK_WINDOW(main_window),  
			GTK_FILE_CHOOSER_ACTION_OPEN,  
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,  
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,  
			NULL);  

	if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT) {  
		char *filename;  
		filename = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(file_chooser));  
		// g_signal_emit_by_name(G_OBJECT(stop_button), "clicked");  
		if (current_filename) g_free(current_filename);  
		current_filename = filename;  
		if (load_file(filename))  
			gtk_widget_set_sensitive(GTK_WIDGET(play_button), TRUE);  
	}  

	gtk_widget_destroy(file_chooser);  
}  
// 退出  
static void file_quit(GtkAction *action)  
{  
	gtk_main_quit();  
}  
// 关于  
static void help_about(GtkAction *action)  
{  
	GtkWidget *about_dialog = gtk_about_dialog_new();  
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about_dialog), "MediaPlayer");  
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "0.0.0");  
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "Copyright @ 2011, Figo");  

	gtk_dialog_run(GTK_DIALOG(about_dialog));  
	gtk_widget_destroy(about_dialog);  
}  

static GtkActionEntry mainwindow_action_entries[] = {  
	{ "FileMenu", "NULL", "文件" },  
	{  
		"OpenFile",  
		GTK_STOCK_OPEN,  
		"打开(O)",  
		"<control>O",  
		"Open a file for playback",  
		G_CALLBACK(file_open)  
	},  
	{  
		"QuitPlayer",  
		GTK_STOCK_QUIT,  
		"退出(Q)",  
		"<control>Q",  
		"Quit the media player",  
		G_CALLBACK(file_quit)  
	},  

	{ "HelpMenu", "NULL", "帮助" },  
	{  
		"HelpAbout",  
		GTK_STOCK_ABOUT,  
		"关于",  
		"",  
		"About the media player",  
		G_CALLBACK(help_about)  
	}  
};  

static gboolean update_time_callback(GstElement *pipeline)  
{  
	GstFormat fmt = GST_FORMAT_TIME;  
	gint64 position;  
	gint64 length;  
	gchar time_buffer[25];  

	if (gst_element_query_position(pipeline, &fmt, &position)  
			&& gst_element_query_duration(pipeline, &fmt, &length)) {
		g_snprintf(time_buffer, 24, "%u:%02u:%02u", GST_TIME_ARGS(position));  
		gui_update_time(time_buffer, position, length);  
	}  

	return TRUE;  
}

gboolean play_file()  
{  
	if (play) {  
		/* Start playing */  
		gst_element_set_state(play, GST_STATE_PLAYING);  
		gui_status_update(STATE_PLAY);  
		/* Connect a callback to trigger every 200 milliseconds to 
		 * update the GUI with the playback progress. We remember 
		 * the ID of this source so that we can remove it when we stop 
		 * playing */  
		timeout_source = g_timeout_add(200, (GSourceFunc)update_time_callback, play);  
		return TRUE;  
	}  

	return FALSE;  
}

static void play_clicked(GtkWidget *widget, gpointer data)  
{  
	if (current_filename) {  
		if (play_file()) {  
			gtk_widget_set_sensitive(GTK_WIDGET(stop_button), TRUE);  
			gtk_widget_set_sensitive(GTK_WIDGET(pause_button), TRUE);  
			// g_print("Play was pressed\n");    
			gui_status_update(STATE_PLAY);  
			g_print("Play\n");    
		}  
		else {  
			g_print("Failed to play\n");  
		}  
	}  
}  

static void pause_clicked(GtkWidget *widget, gpointer data)  
{           
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))  
	{  
		gst_element_set_state(play,GST_STATE_PAUSED);  
		GstStateChangeReturn statechange;  
		statechange = gst_element_set_state(play, GST_STATE_PAUSED);  
		gui_status_update(STATE_PAUSE);  
		g_print("Pause\n");  
		// g_print("Pause was pressed\n");  
	}  
	else  
	{  
		// g_print("Pause was pressed\n");  
		gst_element_set_state(play,GST_STATE_PLAYING);  
		gui_status_update(STATE_PLAY);  
		g_print("Resume to play\n");  
	}                
}  

static void stop_clicked(GtkWidget *widget, gpointer data)  
{        
	if (timeout_source)  
		g_source_remove(timeout_source);  
	timeout_source = 0;  

	// g_print("Stop was pressed\n");    
	gst_element_set_state(play,GST_STATE_NULL);  
	// gst_object_unref(GST_OBJECT(play));//不用销毁管道  
	gui_status_update(STATE_STOP);  
	g_print("Deleting pipeline\n");  
	g_print("Stop\n");      
}  

/* Handler for user moving seek bar */  
static void seek_value_changed(GtkRange *range, gpointer data)  
{  
	if (no_seek)   
		return;  

	gdouble val = gtk_range_get_value(range);  
	seek_to(val);  
}  

GtkWidget *build_gui()  
{  
	GtkWidget *main_vbox;  
	GtkWidget *status_hbox;  
	GtkWidget *controls_hbox;  
	GtkWidget *saturation_controls_hbox;  

	GtkActionGroup *actiongroup;  
	GtkUIManager *ui_manager;  

	actiongroup = gtk_action_group_new("MainwindowActiongroup");  
	gtk_action_group_add_actions(actiongroup,  
			mainwindow_action_entries,  
			G_N_ELEMENTS(mainwindow_action_entries),  
			NULL);  

	ui_manager = gtk_ui_manager_new();  
	gtk_ui_manager_insert_action_group(ui_manager, actiongroup, 0);  
	gtk_ui_manager_add_ui_from_string(  
			ui_manager,  
			"<ui>"  
			"    <menubar name='MainMenu'>"  
			"        <menu action='FileMenu'>"  
			"            <menuitem action='OpenFile'/>"  
			"            <separator name='fsep1'/>"  
			"            <menuitem action='QuitPlayer'/>"  
			"        </menu>"  
			"        <menu action='HelpMenu'>"  
			"            <menuitem action='HelpAbout'/>"  
			"        </menu>"         
			"    </menubar>"  
			"</ui>",  
			-1,  
			NULL);  


	// 创建主 GtkVBOx. 其他所有都在它里面  
	// 0：各个构件高度可能不同，6：构件之间的间距为6 像素  
	main_vbox = gtk_vbox_new(0, 0);  

	// 添加菜单栏  
	gtk_box_pack_start(  
			GTK_BOX(main_vbox),  
			gtk_ui_manager_get_widget(ui_manager, "/ui/MainMenu"),  
			FALSE, FALSE, 0);  

	/* 
	// 视频显示区域 
	video_output = gtk_drawing_area_new (); 
	gtk_box_pack_start (GTK_BOX (main_vbox), video_output, TRUE, TRUE, 0); 
	//gtk_widget_set_size_request (video_output, 0x200, 0x100); 
	gtk_widget_set_size_request (video_output, 672, 378); 
	gtk_widget_show (video_output); 
	*/     
	// 滑动条控制  
	seek_scale = gtk_hscale_new_with_range(0, 100, 1);  
	gtk_scale_set_draw_value(GTK_SCALE(seek_scale), FALSE);  
	gtk_range_set_update_policy(GTK_RANGE(seek_scale), GTK_UPDATE_DISCONTINUOUS);  
	g_signal_connect(G_OBJECT(seek_scale), "value-changed", G_CALLBACK(seek_value_changed), NULL);  
	gtk_box_pack_start(GTK_BOX(main_vbox), seek_scale, FALSE, FALSE, 0);  

	// controls_hbox  
	controls_hbox = gtk_hbox_new(TRUE, 6);  
	gtk_box_pack_start_defaults(GTK_BOX(main_vbox), controls_hbox);  

	// 播放按钮  
	play_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);  
	// 设置“敏感”属性，FALSE 表示为灰色，不响应鼠标键盘事件  
	gtk_widget_set_sensitive(play_button, FALSE);  
	g_signal_connect(G_OBJECT(play_button), "clicked", G_CALLBACK(play_clicked), NULL);  
	gtk_box_pack_start_defaults(GTK_BOX(controls_hbox), play_button);  

	// 暂停按钮,为使按下时停留在按下状态，使用GtkToggleButton  
	pause_button = gtk_toggle_button_new_with_label(GTK_STOCK_MEDIA_PAUSE);  
	// 将按钮设置为固化按钮  
	gtk_button_set_use_stock(GTK_BUTTON(pause_button), TRUE);  
	gtk_widget_set_sensitive(pause_button, FALSE);  
	g_signal_connect(G_OBJECT(pause_button), "clicked", G_CALLBACK(pause_clicked), NULL);  
	gtk_box_pack_start_defaults(GTK_BOX(controls_hbox), pause_button);  

	// 停止按钮  
	stop_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_STOP);  
	gtk_widget_set_sensitive(stop_button, FALSE);  
	g_signal_connect(G_OBJECT(stop_button), "clicked", G_CALLBACK(stop_clicked), NULL);  
	gtk_box_pack_start_defaults(GTK_BOX(controls_hbox), stop_button);       

	// status_hbox  
	status_hbox = gtk_hbox_new(TRUE, 0);  
	gtk_box_pack_start(GTK_BOX(main_vbox), status_hbox, FALSE, FALSE, 0);  
	// 状态标签  
	status_label = gtk_label_new("<b>已停止</b>");  
	gtk_label_set_use_markup(GTK_LABEL(status_label), TRUE);  
	gtk_misc_set_alignment(GTK_MISC(status_label), 0.0, 0.5);  
	gtk_box_pack_start(GTK_BOX(status_hbox), status_label, TRUE, TRUE, 0);  
	// 时间标签     
	time_label = gtk_label_new("00:00:00");  
	gtk_misc_set_alignment(GTK_MISC(time_label), 0.5, 1.0);  
	gtk_box_pack_start(GTK_BOX(status_hbox), time_label, TRUE, TRUE, 0);  

	return main_vbox;  
}  

/* 
   void stop_playback() 
   { 
   if (timeout_source) 
   g_source_remove(timeout_source); 
   timeout_source = 0; 

   if (play) { 
   gst_element_set_state(play, GST_STATE_NULL); 
   gst_object_unref(GST_OBJECT(play)); 
   play = NULL; 
   } 
   gui_status_update(STATE_STOP); 
   }*/  

// destory main window  
static void destroy(GtkWidget *widget, gpointer data)  
{  
	gtk_main_quit();  
}  
// 更新播放时间  
void gui_update_time(const gchar *time, const gint64 position, const gint64 length)  
{  
	gtk_label_set_text(GTK_LABEL(time_label), time);  
	if (length > 0) {  
		no_seek = TRUE;  
		gtk_range_set_value(GTK_RANGE(seek_scale), ((gdouble)position / (gdouble)length) * 100.0);  
		no_seek = FALSE;  
	}  
}  
// 更新播放状态  
void gui_status_update(PlayerState state)  
{  
	switch (state) {  
		case STATE_STOP:  
			gtk_widget_set_sensitive(GTK_WIDGET(stop_button), FALSE);  
			gtk_widget_set_sensitive(GTK_WIDGET(pause_button), FALSE);             
			gtk_label_set_markup(GTK_LABEL(status_label), "<b>已停止</b>");  
			gtk_range_set_value(GTK_RANGE(seek_scale), 0.0);        
			gtk_label_set_text(GTK_LABEL(time_label), "00:00:00");  
			break;  
		case STATE_PLAY:  
			gtk_widget_set_sensitive(GTK_WIDGET(stop_button), TRUE);  
			gtk_widget_set_sensitive(GTK_WIDGET(pause_button), TRUE);              
			gtk_label_set_markup(GTK_LABEL(status_label), "<b>播放中</b>");  
			break;  
		case STATE_PAUSE:             
			gtk_label_set_markup(GTK_LABEL(status_label), "<b>已暂停</b>");  
			break;  
		default:  
			break;  
	}  
}  

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data)  
{  
	switch (GST_MESSAGE_TYPE(message)) {  
		case GST_MESSAGE_ERROR:  
			{  
				GError *err;  
				gchar *debug;  

				gst_message_parse_error(message, &err, &debug);  
				g_print("Error: %s\n", err->message);  
				g_error_free(err);  
				g_free(debug);  

				gtk_main_quit();  
				break;  
			}  
		case GST_MESSAGE_EOS:  
			g_print("End of stream\n");  
			// stop_playback();  
			// stop_clicked(GtkWidget *widget, gpointer data);  
			if (timeout_source)  
				g_source_remove(timeout_source);  
			timeout_source = 0;  
			gst_element_set_state(play, GST_STATE_NULL);     
			gui_status_update(STATE_STOP);  
			break;          
		default:  
			break;  
	}  

	return TRUE;  
}  

static gboolean build_gstreamer_pipeline(const gchar *uri)  
{  
	// 创建一个playbin 元素   
	play = gst_pipeline_new ("Mediaplayer");  
	bin  = gst_element_factory_make ("playbin", "bin");  

	gst_bin_add (GST_BIN (play), bin);  
	{  
		GstBus *bus;  
		bus = gst_pipeline_get_bus (GST_PIPELINE (play));  
		gst_bus_add_watch (bus, bus_callback, NULL);  
		gst_object_unref (bus);  
	}  

	g_object_set (G_OBJECT (bin), "uri", uri, NULL);  

	return TRUE;  
}  
// load file to play  
gboolean load_file(const gchar *uri)  
{  
	if (build_gstreamer_pipeline(uri))  
		return TRUE;  
	return FALSE;  
}

/* Attempt to seek to the given percentage through the file */  
void seek_to(gdouble percentage)  
{  
	GstFormat fmt = GST_FORMAT_TIME;  
	gint64 length;  

	/* If it seems safe to attempt a seek... */  
	if (play && gst_element_query_duration(play, &fmt, &length)) {  
		/* ...calculate where to seek to */  
		gint64 target = ((gdouble)length * (percentage / 100.0));  

		/* ...and attempt the seek */  
		if (!gst_element_seek(play, 1.0, GST_FORMAT_TIME,  
					GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,  
					target, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))  
			g_print("Failed to seek to desired position\n");  
	}  
}  

int main(int argc, char *argv[])  
{  
	// 初始化 GTK+  
	gtk_init(&argc, &argv);  
	gst_init(&argc, &argv);  
	// 创建窗口  
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);  
	// 设置窗口标题  
	gtk_window_set_title(GTK_WINDOW(main_window), "MediaPlayer");  
	// 主窗口销毁句柄  
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(destroy), NULL);  
	// 创建主窗口GUI  
	gtk_container_add(GTK_CONTAINER(main_window), build_gui());  
	// 显示  
	gtk_widget_show_all(GTK_WIDGET(main_window));     
	// 开始主循环  
	gtk_main();  

	return 0;  
} 
