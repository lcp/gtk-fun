/* gcc -g -Wall `pkg-config --cflags --libs gtk+-x11-2.0` grabkey.c -o grabkey */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#define GRAB_KEY "x"
#define GRAB_MOD Mod4Mask

typedef struct {
	KeyCode  keycode;
	guint    modmask;
} EventData;

static gboolean
process_keycode (XEvent    *event,
                 EventData *edata)
{
	Display *display;

	display = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());

	g_print ("KeyPress: %s\n", (event->type == KeyPress) ? "True" : "False");
	g_print ("KeyRelease: %s\n", (event->type == KeyRelease) ? "True" : "False");
	g_print ("Got state: %d keycode: %d\n", event->xkey.state, event->xkey.keycode);
	if (   (event->type == KeyPress)
	    && (event->xkey.keycode == edata->keycode)
	    && (event->xkey.state & edata->modmask)) {
		g_print ("Got (%d) + %s\n", GRAB_MOD, GRAB_KEY);

		XAllowEvents (display, AsyncKeyboard, event->xkey.time);

		return TRUE;
	}

	XAllowEvents (display, ReplayKeyboard, event->xkey.time);
	gdk_flush ();

	return FALSE;
}

static GdkFilterReturn
my_filter_func (GdkXEvent *xevent,
                GdkEvent  *event,
                gpointer   data)
{
	EventData *edata = (EventData *)data;

	g_return_val_if_fail (edata != NULL, GDK_FILTER_CONTINUE);

	if (process_keycode (xevent, edata))
		return GDK_FILTER_REMOVE;

	return GDK_FILTER_CONTINUE;
}

static gboolean
try_grab_key (KeyCode  keycode,
              guint    modmask)
{
	gdk_error_trap_push ();

	XGrabKey (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()),
	          keycode, AnyModifier,
	          gdk_x11_get_default_root_xwindow(),
	          TRUE, GrabModeAsync, GrabModeSync);

	gdk_flush ();
	if (gdk_error_trap_pop ()) {
		g_print ("Failed to grab key %s\n", GRAB_KEY);
		return FALSE;
	}

	return TRUE;
}

int
main (int argc, char *argv[])
{
	Display    *display;
	EventData  *data;
	KeyCode     keycode;
	guint       modmask;

	gtk_init (&argc, &argv);

	display = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());
	keycode = XKeysymToKeycode (display, gdk_keyval_from_name (GRAB_KEY));
	modmask = GRAB_MOD; 

	if (!try_grab_key (keycode, modmask))
		return 1;

	data = g_new0 (EventData, 1);
	data->keycode = keycode;
	data->modmask = modmask;

	gdk_window_add_filter (NULL, my_filter_func, data);

	gtk_main ();

	XUngrabKey (display, keycode, modmask,
	            gdk_x11_get_default_root_xwindow());

	g_free (data);

	return 0;
}
