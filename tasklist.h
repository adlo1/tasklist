#ifndef MY_TASKLIST_H
#define MY_TASKLIST_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

G_BEGIN_DECLS

#define MY_TASKLIST_TYPE (my_tasklist_get_type())
#define MY_TASKLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TASKLIST_TYPE, MyTasklist))
#define MY_TASKLIST_CLASS (klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TASKLIST_TYPE, MyTasklistClass))
#define IS_MY_TASKLIST (obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TASKLIST_TYPE))
#define IS_MY_TASKLIST_CLASS(klass), (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TASKLIST_TYPE))

typedef struct _MyTasklist MyTasklist;
typedef struct _MyTasklistClass MyTasklistClass;

struct _MyTasklist
{
	GtkEventBox event_box;
	
	GtkWidget *table;
	GtkWidget *button;
	WnckWindow *window;
	WnckScreen *screen;
	void (*func) ();
	gboolean my_tasklist_button_click_action_is_set;
	const GtkTargetEntry target;
	
	
};

struct _MyTasklistClass
{
	GtkEventBoxClass parent_class;
};

GType my_tasklist_get_type (void);
GtkWidget* my_tasklist_new (void);
static void my_tasklist_update_windows (MyTasklist *tasklist, gboolean new_window);
static void my_tasklist_on_window_opened (WnckScreen *screen, WnckWindow *window, MyTasklist *tasklist);
static void my_tasklist_on_name_changed (WnckWindow *window, GtkWidget *label);
static void my_tasklist_window_workspace_changed (WnckWindow *window, MyTasklist *tasklist);
static void my_tasklist_button_clicked (GtkButton *button, WnckWindow *window);
static void my_tasklist_drag_data_get_handl
(GtkWidget *widget, GdkDragContext *context, GtkSelectionData *selection_data,
        guint target_type, guint time, gulong xid);
        
static void my_tasklist_set_button_click_action (MyTasklist *tasklist, void (*func) ());

static void my_tasklist_button_emit_signal (GtkButton *button, MyTasklist *tasklist);
G_END_DECLS

#endif /*__MY_TASKLIST_H__*/
