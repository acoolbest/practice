#include <gtk/gtk.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*
   功能说明：此程序为登录器，密码位数设置为6位。当密码输入正确时，点击sure会显示密码。
   当错误时，密码输入行会清空。只要密码不正确或少于6位时，密码行处于加密模式显示。
   */
/* 功能:      设置控件字体大小
 * widget:    需要改变字体的控件
 * size:      字体大小
 * is_button: TRUE代表控件为按钮，FALSE为其它控件
 */
static void set_widget_font_size(GtkWidget *widget, int size, gboolean is_button)
{
	GtkWidget *labelChild;  
	PangoFontDescription *font;  
	gint fontSize = size;  

	font = pango_font_description_from_string("Sans");          //"Sans"字体名   
	pango_font_description_set_size(font, fontSize*PANGO_SCALE);//设置字体大小   

	if(is_button){
		labelChild = gtk_bin_get_child(GTK_BIN(widget));//取出GtkButton里的label  
	}else{
		labelChild = widget;
	}

	//设置label的字体，这样这个GtkButton上面显示的字体就变了
	gtk_widget_modify_font(GTK_WIDGET(labelChild), font);
	pango_font_description_free(font);
}
/* 功能：  设置背景图
 * widget: 主窗口
 * w, h:   图片的大小
 * path：  图片路径
 */
static void chang_background(GtkWidget *widget, int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget, TRUE);//允许窗口可以绘图
	gtk_widget_realize(widget); 

	/* 更改背景图时，图片会重叠
	 * 这时要手动调用下面的函数，让窗口绘图区域失效，产生窗口重绘制事件（即 expose 事件）。
	 */
	gtk_widget_queue_draw(widget);

	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL);// 创建图片资源对象
	// w, h是指定图片的宽度和高度
	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h, GDK_INTERP_BILINEAR);


	GdkPixmap *pixmap = NULL;

	/* 创建pixmap图像; 
	 * NULL：不需要蒙版; 
	 * 123： 0~255，透明到不透明
	 */
	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
	// 通过pixmap给widget设置一张背景图，最后一个参数必须为: FASLE
	gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);

	// 释放资源
	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}


// 按Enter，获取行编辑的内容
//鼠标点击行编辑，按回车相当于sure。
void enter_callback( GtkWidget *widget, gpointer entry ) 
{ 
	const gchar *entry_text; 
	// 获得文本内容
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry)); 
	//g_print("Entry contents: %s\n", entry_text); 
	if(0==strcmp(entry_text,"123456"))
	{
		gtk_entry_set_visibility(GTK_ENTRY(entry), TRUE);// 密码模式
		g_print("TRUE");
	}
	else
	{
		gtk_entry_set_text(GTK_ENTRY(entry),"");
	}
}
void deal_pressed(GtkButton *button,gpointer entry)
{ 
	gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);// 密码模式
	char buf[12]="";
	char buf1[6]="";
	int num=0;
	const gchar *text=NULL;
	text=gtk_entry_get_text(GTK_ENTRY(entry));//获得行编辑器里面的内容
	const gchar *but=gtk_button_get_label(button);//获取按键内容
	sprintf(buf,"%s",text);
	if(0==strcmp(but,"sure"))
	{
		if(0==strcmp(buf,"123456"))
		{
			gtk_entry_set_visibility(GTK_ENTRY(entry), TRUE);// 密码模式
			g_print("TRUE");
		}
		else
		{
			buf[0]='\0';//消除一个字符
		}
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
int main( int argc, char *argv[] ) 
{ 
	gtk_init (&argc, &argv); 
	// 初始化
	int i=0;
	char buf[10]="";//此处说明：如果数组不定义大小，在linux上能运行，但是在arm板无法运行。

	GtkWidget *button=NULL;
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
	// 创建窗口
	gtk_window_set_title(GTK_WINDOW(window), "login device"); 
	// 设置标题
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //点击‘x’退出程序

	gtk_widget_set_size_request(window, 800, 480);//设置窗口大小
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);// 固定窗口大小
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);//窗口在显示器的位置
	chang_background(window, 800, 480, "Desktop.jpg");// 设置窗口背景图
	//固定布局
	GtkWidget *table = gtk_table_new(7,8,TRUE);//设置总行列数

	//创建一个password  
	GtkWidget *password = gtk_label_new("password:"); 
	//设置password的标签
	set_widget_font_size(password, 30, FALSE);// 设置字体大小

	GtkWidget *entry = gtk_entry_new();  // 创建行编辑 
	set_widget_font_size(entry, 20, FALSE);// 设置字体大小

	gtk_entry_set_max_length(GTK_ENTRY(entry), 6);     // 设置行编辑显示最大字符的长度

	gtk_table_attach_defaults(GTK_TABLE(table),password,1,3,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),entry,3,7,1,2);
	gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);// 密码模式 


	//创建"<-"和"sure"
	GtkWidget *button1 = gtk_button_new_with_label("<-");
	set_widget_font_size(button1, 40, TRUE);// 设置字体大小
	gtk_table_attach_defaults(GTK_TABLE(table),button1,6,7,3,4);


	GtkWidget *button2 = gtk_button_new_with_label("sure");
	set_widget_font_size(button2, 30, TRUE);// 设置字体大小
	gtk_table_attach_defaults(GTK_TABLE(table),button2,6,7,4,5);
	g_signal_connect(button1,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理
	g_signal_connect(button2,"clicked",G_CALLBACK(deal_pressed),entry);//按键处理

	GtkWidget * but2 = gtk_button_new_with_label("X");//此三行作用：图标右上角的X，觉得不美观可删除。
	gtk_table_attach_defaults(GTK_TABLE(table),but2,7,8,0,1);
	g_signal_connect(but2,"clicked",G_CALLBACK(gtk_main_quit),NULL);//点击X时，程序退出

	for(i=0;i<10;i++)
	{
		sprintf(buf,"%d",i);//将int型转为char型
		button = gtk_button_new_with_label(buf); 
		set_widget_font_size(button, 20, TRUE);// 设置字体大小
		// 将按钮组装到一个固定容器的窗口中，设置位置。
		if(i<5) 
		{
			gtk_table_attach_defaults(GTK_TABLE(table),button,i+1,i+2,3,4);
		}
		else
		{ 
			gtk_table_attach_defaults(GTK_TABLE(table),button,i-4,i-3,4,5); 
		} 
		//数字键及获取输入框内容  
		g_signal_connect(button,"pressed",G_CALLBACK(deal_pressed),entry);//按键处理,使用此信号pressed，回车按下时不会获取内容。
	}
	g_signal_connect(entry, "activate", G_CALLBACK(enter_callback), entry);

	gtk_container_add(GTK_CONTAINER (window), table); // 固定放进窗口
	gtk_widget_show_all(window); // 显示窗口所有控件
	gtk_main(); // 主事件循环
	return 0; 
} 
