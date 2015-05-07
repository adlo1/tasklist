#include <gtk/gtk.h>
#include <libwnck/libwnck.h>
#include "tasklist.h"

enum {
	BUTTON_CLICK_ACTION_SIGNAL,
	LAST_SIGNAL
};

static void my_tasklist_class_init (MyTasklistClass *klass);
static void my_tasklist_init (MyTasklist *tasklist);

static guint button_click_action_signals[LAST_SIGNAL]={0};

GType my_tasklist_get_type (void)
{
	static GType my_tasklist_type = 0;
	
	if (!my_tasklist_type)
	{
		const GTypeInfo my_tasklist_info =
		{
			sizeof(MyTasklistClass),
			NULL, /*base_init*/
			NULL, /*base_finalize*/
			(GClassInitFunc) my_tasklist_class_init,
			NULL, /*class_finalize*/
			NULL,
			sizeof(MyTasklist),
			0,
			(GInstanceInitFunc) my_tasklist_init,
		};
		my_tasklist_type = g_type_register_static (GTK_TYPE_EVENT_BOX, "MyTasklist", &my_tasklist_info, 0);
	}
	return my_tasklist_type;
}

static void my_tasklist_class_init (MyTasklistClass *klass)

{
	button_click_action_signals [BUTTON_CLICK_ACTION_SIGNAL] = 
		g_signal_new ("button-click-action",
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_FIRST|G_SIGNAL_ACTION,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
	
}

static void my_tasklist_init (MyTasklist *tasklist)
{
	
	
	tasklist->table = gtk_table_new (3, 3, TRUE);
	gtk_container_add (GTK_CONTAINER(tasklist), tasklist->table);
	
	
	gtk_widget_show (tasklist->table);
	tasklist->screen = wnck_screen_get_default();
	wnck_screen_force_update (tasklist->screen);
	WnckWindow *window = 0;		//Dummy WnckWindow because the function requires me to pass a WnckWindow
	my_tasklist_on_window_opened (tasklist->screen, window, tasklist);
	g_signal_connect (tasklist->screen, "window-opened",
                G_CALLBACK (my_tasklist_on_window_opened), tasklist); 
                
    g_signal_connect (tasklist->screen, "window-closed",
                G_CALLBACK (my_tasklist_on_window_opened), tasklist); 
                
   g_signal_connect (tasklist->screen, "active-workspace-changed",
               G_CALLBACK (my_tasklist_on_window_opened), tasklist);

}

GtkWidget* my_tasklist_new (void)
{
	return GTK_WIDGET(g_object_new (my_tasklist_get_type (), NULL));
}



static void my_tasklist_update_windows (MyTasklist *tasklist, gboolean new_window)
{

           
			

	gtk_widget_destroy (tasklist->table);
	tasklist->table=gtk_table_new (3,3,TRUE);
	
	GList *window_l;
	
	
	//Table attachment values
	
	guint left_attach =0;	
	guint right_attach=1;		
	guint top_attach=0;		
	guint bottom_attach=1;
	
 

  for (window_l = wnck_screen_get_windows (tasklist->screen); window_l != NULL; window_l = window_l->next)
    {
      tasklist->window = WNCK_WINDOW (window_l->data);
      
      //Determine if window is on the active workspace
      
      if(wnck_window_is_on_workspace(tasklist->window,wnck_screen_get_active_workspace(tasklist->screen)))
	{
		const GtkTargetEntry targets [] = {"application/x-wnck-window-id",0,0};
		GtkWidget *label = gtk_label_new_with_mnemonic(wnck_window_get_name (tasklist->window));
		tasklist->button = gtk_button_new();
		gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
		gtk_container_add (GTK_CONTAINER(tasklist->button),label);
		gtk_widget_set_size_request (tasklist->button, 200, 40);
      
      
    
		gulong xid = wnck_window_get_xid (tasklist->window);
        
		if (!(wnck_window_is_skip_tasklist(tasklist->window)))
       
		{
        
			gtk_table_attach_defaults (GTK_TABLE(tasklist->table), tasklist->button, left_attach, 
					right_attach, top_attach, bottom_attach);
	  
	  
	  
			g_signal_connect (tasklist->button, "clicked", my_tasklist_button_clicked, tasklist->window);
			g_signal_connect (GTK_BUTTON(tasklist->button), "clicked", 
				my_tasklist_button_emit_signal, tasklist);
    
			if (right_attach % 3 == 0)
			{
				top_attach++;
				bottom_attach++;
				left_attach=0;
				right_attach=1;
			}
			else
			{
				left_attach++;
				right_attach++;
			}
	
			gtk_drag_source_set (tasklist->button,GDK_BUTTON1_MASK,targets,1,GDK_ACTION_MOVE);
	
			g_signal_connect (tasklist->button, "drag-data-get",
					G_CALLBACK (my_tasklist_drag_data_get_handl), xid);
            
			g_signal_handlers_disconnect_matched (tasklist->window, G_SIGNAL_MATCH_FUNC, 
					NULL, NULL, NULL, my_tasklist_on_name_changed, NULL);
  

	
			g_signal_connect (tasklist->window, "name-changed",
					G_CALLBACK (my_tasklist_on_name_changed), label);
    
    
    
			//If the window is new, connect to its workspace-changed signal
                
			if (new_window)
                
			{                                           
				g_signal_handlers_disconnect_by_func (tasklist->window, 
					my_tasklist_window_workspace_changed, tasklist);
                
				g_signal_connect (tasklist->window, "workspace-changed",
					G_CALLBACK (my_tasklist_window_workspace_changed), tasklist);                
			}
                
    
		}
	}
}

gtk_container_add(GTK_CONTAINER(tasklist), tasklist->table);
gtk_widget_show_all (tasklist->table);
}

static void my_tasklist_on_name_changed (WnckWindow *window, GtkWidget *label) 
{
	gtk_label_set_text (GTK_LABEL(label), wnck_window_get_name (window));
}

static void my_tasklist_on_window_opened (WnckScreen *screen, WnckWindow *window, MyTasklist *tasklist)
{
	my_tasklist_update_windows (tasklist, TRUE);
}

static void my_tasklist_window_workspace_changed (WnckWindow *window, MyTasklist *tasklist)
{
	my_tasklist_update_windows (tasklist, FALSE);
}

static void my_tasklist_button_clicked (GtkButton *button, WnckWindow *window)
	
{
	wnck_window_activate (window, gtk_get_current_event_time());
	
}

static void my_tasklist_button_emit_signal (GtkButton *button, MyTasklist *tasklist)
{
	g_signal_emit_by_name (tasklist, "button-click-action");
	
}

static void
my_tasklist_drag_data_get_handl
(GtkWidget *widget, GdkDragContext *context, GtkSelectionData *selection_data,
        guint target_type, guint time, gulong xid)
{
	g_assert (selection_data != NULL);
	 
	
	gtk_selection_data_set
                (
                        selection_data,         /* Allocated GdkSelectionData object */
                        gtk_selection_data_get_target(selection_data), /* target type */
                        8,                 /* number of bits per 'unit' */
                        (guchar*)&xid,/* pointer to data to be sent */
                        sizeof (gulong)   /* length of data in units */
                );
                
               
     
        
}
