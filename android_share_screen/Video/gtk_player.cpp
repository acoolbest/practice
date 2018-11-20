// g++ -o gtk_player gtk_player.cpp `pkg-config --libs gtk+-2.0 libvlc` `pkg-config --cflags gtk+-2.0 libvlc`

/* License WTFPL http://sam.zoy.org/wtfpl/ */
/* Written by Vincent Schüßler */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h> /* the key value defines can be found here */
#include <vlc/vlc.h>
#include <iostream>

using namespace std;

#define BORDER_WIDTH 6
//#define FULL_SCREEN

void destroy(GtkWidget *widget, gpointer data);
void player_widget_on_realize(GtkWidget *widget, gpointer data);
void on_open(GtkWidget *widget, gpointer data);
void open_media(const char* uri);
void play(void);
void pause_player(void);
void on_playpause(GtkWidget *widget, gpointer data);
void on_stop(GtkWidget *widget, gpointer data);

libvlc_media_player_t *media_player;
libvlc_instance_t *vlc_inst;
GtkWidget *playpause_button;

GtkWidget *main_window;
GtkWidget *urledit_window;



void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void player_widget_on_realize(GtkWidget *widget, gpointer data) {
    libvlc_media_player_set_xwindow(media_player, GDK_WINDOW_XID(gtk_widget_get_window(widget)));
}

void on_open(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Choose Media", (GtkWindow *)data, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *uri;
        uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
		printf("%s\n", uri);
        open_media(uri);
        g_free(uri);
    }
    gtk_widget_destroy(dialog);
}

void open_media(const char* uri) {
    libvlc_media_t *media;
    media = libvlc_media_new_location(vlc_inst, uri);
    libvlc_media_player_set_media(media_player, media);
    play();
    libvlc_media_release(media);
}

void on_playpause(GtkWidget *widget, gpointer data) {
    if(libvlc_media_player_is_playing(media_player) == 1) {
        pause_player();
    }
    else {
        play();
    }
}

void on_stop(GtkWidget *widget, gpointer data) {
    pause_player();
    libvlc_media_player_stop(media_player);
}

void play(void) {
    libvlc_media_player_play(media_player);
    gtk_button_set_label(GTK_BUTTON(playpause_button), GTK_STOCK_MEDIA_PAUSE);
}

void pause_player(void) {
    libvlc_media_player_pause(media_player);
    gtk_button_set_label(GTK_BUTTON(playpause_button), GTK_STOCK_MEDIA_PLAY);
}

#ifdef FULL_SCREEN
void on_key_esc(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
        case GDK_Escape: 
            gtk_main_quit();
            break;
        default:
            break;
    }
}
#endif

void deal_pressed(GtkButton *button,gpointer entry)
{ 
	//gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);// 密码模式
	char buf[255]="";
	char buf1[255]="";
	int num=0;
	const gchar *text=NULL;
	text=gtk_entry_get_text(GTK_ENTRY(entry));//获得行编辑器里面的内容
	const gchar *but=gtk_button_get_label(button);//获取按键内容
	sprintf(buf,"%s",text);
	if(0==strcmp(but,"sure"))
	{
		g_print("TRUE: %s\n", buf);
		gtk_widget_hide_all(urledit_window);
		open_media(buf);
		#if 0
		if(0==strcmp(buf,"123456"))
		{
			gtk_entry_set_visibility(GTK_ENTRY(entry), TRUE);// 密码模式
			g_print("TRUE");
		}
		else
		{
			buf[0]='\0';//消除一个字符
		}
		#endif
	}
	else if(0==strcmp(but,"<-"))
	{
		num=strlen(buf);
		//printf("%d\n",num);//得到当前输入的长度
		buf[num-1]='\0';//消除一个字符
	}
	else
		sprintf(buf1,"%s",but);
	gtk_entry_set_text(GTK_ENTRY(entry), strcat(buf,buf1));  // 设置内容
	g_print("%s\n",buf);//正常显示链接后内容
}

// 按Enter，获取行编辑的内容
//鼠标点击行编辑，按回车相当于sure。
void enter_callback( GtkWidget *widget, gpointer entry ) 
{ 
	const gchar *entry_text; 
	// 获得文本内容
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry)); 
	g_print("Entry contents: %s\n", entry_text);
	gtk_widget_hide_all(urledit_window);
	open_media(entry_text);
	#if 0
	if(0==strcmp(entry_text,"123456"))
	{
		gtk_entry_set_visibility(GTK_ENTRY(entry), TRUE);// 密码模式
		g_print("TRUE");
	}
	else
	{
		gtk_entry_set_text(GTK_ENTRY(entry),"");
	}
	#endif
}
void edit_url(GtkWidget *widget, gpointer data)
{
	//gtk_widget_hide_all(main_window);
	gtk_widget_show_all(urledit_window);	
}

GtkWidget *create_window(char *background = NULL)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);    
	gtk_widget_set_usize(window, 300, 100);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	//g_signal_connect (G_OBJECT(window), "destroy",
                       // G_CALLBACK(gtk_main_quit), NULL);
    if(background != NULL)
    {
		gtk_widget_set_app_paintable(window, TRUE);
		gtk_widget_realize(window);
		GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file (background, NULL);
		GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple (src_pixbuf, 
						300, 100, GDK_INTERP_BILINEAR);
		GdkPixmap *pixmap = NULL;
		gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 256);
		gdk_window_set_back_pixmap(window->window, pixmap, FALSE);		
		
		g_object_unref (src_pixbuf);
		g_object_unref (dst_pixbuf);
		g_object_unref (pixmap);
	}
	return window;

}

GtkWidget *design_urledit_window()
{
	GtkWidget *window = create_window();
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
	
	GtkWidget *entry = gtk_entry_new();  // 创建行编辑 
	//set_widget_font_size(entry, 10, FALSE);// 设置字体大小
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);     // 设置行编辑显示最大字符的长度
	gtk_entry_set_text(GTK_ENTRY(entry), "rtmp://192.168.0.163:1935/hls/test2");
	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
	
	
	//创建"<-"和"sure"
	GtkWidget *button1 = gtk_button_new_with_label("<-");
	//set_widget_font_size(button1, 40, TRUE);// 设置字体大小
	//gtk_box_pack_start(GTK_BOX(vbox), button1, FALSE, FALSE, 0);
	//gtk_table_attach_defaults(GTK_TABLE(table),button1,6,7,3,4);


	GtkWidget *button2 = gtk_button_new_with_label("sure");
	//set_widget_font_size(button2, 30, TRUE);// 设置字体大小
	//gtk_box_pack_start(GTK_BOX(vbox), button2, FALSE, FALSE, 0);
	//gtk_table_attach_defaults(GTK_TABLE(table),button2,6,7,4,5);
	
	GtkWidget *hbuttonbox = gtk_hbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), BORDER_WIDTH);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_START);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), button1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), button2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, FALSE, 0);
	
	g_signal_connect(button1,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理
	g_signal_connect(button2,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理
	g_signal_connect(entry, "activate", G_CALLBACK(enter_callback), entry);	
	return window;
}

int main( int argc, char *argv[] ) {
    GtkWidget *window,
              *vbox,
              *menubar,
              *menu,
              *fileitem,
              *filemenu_openitem,
              *player_widget,
              *hbuttonbox,
              *stop_button;
    
    gtk_init (&argc, &argv);
	//main_window = design_main_window();
	urledit_window = design_urledit_window();
	
	#ifdef FULL_SCREEN
	// Get the Screen Resolution
	GdkScreen* screen;
    gint width, height;
    screen = gdk_screen_get_default();
    width = gdk_screen_get_width(screen);
    height = gdk_screen_get_height(screen);
    printf("screen width: %d, height: %d\n", width, height);
	#endif
	
    // setup window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    #ifdef FULL_SCREEN
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE); /* hide the title bar and the boder */
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "GTK+ libVLC Demo");
	#else
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_window_set_title(GTK_WINDOW(window), "GTK+ libVLC Demo");
	#endif
	
    //setup vbox
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //setup menu
	
	menubar = gtk_menu_bar_new();
    menu = gtk_menu_new();
    fileitem = gtk_menu_item_new_with_label ("File");
    filemenu_openitem = gtk_menu_item_new_with_label("Open");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), filemenu_openitem);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileitem), menu);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), fileitem);
	
    g_signal_connect(filemenu_openitem, "activate", G_CALLBACK(on_open), window);
	
	GtkWidget *menu2 = gtk_menu_new();
    GtkWidget *url_item = gtk_menu_item_new_with_label ("URL");
    GtkWidget *edit_url_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu2), edit_url_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(url_item), menu2);
    gtk_menu_bar_append(GTK_MENU_BAR(menubar), url_item);
	
    g_signal_connect(edit_url_item, "activate", G_CALLBACK(edit_url), NULL);
	
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	//-----------
	#if 0
	GtkWidget *entry = gtk_entry_new();  // 创建行编辑 
	//set_widget_font_size(entry, 10, FALSE);// 设置字体大小
	gtk_entry_set_max_length(GTK_ENTRY(entry), 255);     // 设置行编辑显示最大字符的长度
	gtk_entry_set_text(GTK_ENTRY(entry), "rtmp://192.168.0.163:1935/hls/test2");
	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
	
	
	//创建"<-"和"sure"
	GtkWidget *button1 = gtk_button_new_with_label("<-");
	//set_widget_font_size(button1, 40, TRUE);// 设置字体大小
	//gtk_box_pack_start(GTK_BOX(vbox), button1, FALSE, FALSE, 0);
	//gtk_table_attach_defaults(GTK_TABLE(table),button1,6,7,3,4);


	GtkWidget *button2 = gtk_button_new_with_label(GTK_STOCK_OK);
	//set_widget_font_size(button2, 30, TRUE);// 设置字体大小
	//gtk_box_pack_start(GTK_BOX(vbox), button2, FALSE, FALSE, 0);
	//gtk_table_attach_defaults(GTK_TABLE(table),button2,6,7,4,5);
	
	GtkWidget *vbuttonbox = gtk_vbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(vbuttonbox), BORDER_WIDTH);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(vbuttonbox), GTK_BUTTONBOX_START);
    gtk_box_pack_start(GTK_BOX(vbuttonbox), button1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbuttonbox), button2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), vbuttonbox, FALSE, FALSE, 0);
	
	g_signal_connect(button1,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理
	g_signal_connect(button2,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理
	g_signal_connect(entry, "activate", G_CALLBACK(enter_callback), entry);
	#endif
	//-----------
	
    //setup player widget
    player_widget = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), player_widget, TRUE, TRUE, 0);

    //setup controls
    //playpause_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
    playpause_button = gtk_button_new_with_label(GTK_STOCK_MEDIA_PLAY);
    gtk_button_set_use_stock(GTK_BUTTON(playpause_button), TRUE);
    stop_button = gtk_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
    g_signal_connect(playpause_button, "clicked", G_CALLBACK(on_playpause), NULL);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop), NULL);
    hbuttonbox = gtk_hbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), BORDER_WIDTH);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_START);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), playpause_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbuttonbox), stop_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, FALSE, 0);

	

    //setup vlc
    vlc_inst = libvlc_new(0, NULL);
    media_player = libvlc_media_player_new(vlc_inst);
    g_signal_connect(G_OBJECT(player_widget), "realize", G_CALLBACK(player_widget_on_realize), NULL);

    gtk_widget_show_all(window);
	
	#ifdef FULL_SCREEN
	/* add key event for quit */
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_esc), NULL);
	#endif
	
    gtk_main ();

    libvlc_media_player_release(media_player);
    libvlc_release(vlc_inst);
    return 0;
}
