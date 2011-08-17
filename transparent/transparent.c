/* Create a top level window with a transparent background */

#include <gtk/gtk.h>

#define BACKGROUND_ALPHA 0

static gboolean
draw_transparent_back_cb (GtkWidget *widget,
			  cairo_t   *cr,
			  gpointer   user_data)
{
	GtkStyleContext	*context;
	GdkRGBA		 acolor;
	int		 width;
	int		 height;

	context = gtk_widget_get_style_context (widget);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	gtk_window_get_size (GTK_WINDOW (widget), &width, &height);

	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
	cairo_paint (cr);

	gtk_style_context_get_background_color (context, GTK_STATE_NORMAL, &acolor);
	acolor.alpha = BACKGROUND_ALPHA;
	gdk_cairo_set_source_rgba (cr, &acolor);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill(cr);

	return FALSE;
}

static void
set_visual (GtkWidget *widget)
{
	GdkScreen *screen;
	GdkVisual *visual;

	screen = gtk_widget_get_screen (widget);
	visual = gdk_screen_get_rgba_visual (screen);
	if (visual == NULL)
		visual = gdk_screen_get_system_visual (screen);

	gtk_widget_set_visual (widget, visual);
}

static void
screen_change_cb (GtkWidget *widget,
		  GdkScreen *previous_screen,
		  gpointer   user_data)
{
	set_visual (widget);
}

int
main (int argc, char *argv[])
{
	GtkWidget *window;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(window, TRUE);

	set_visual (window);

	g_signal_connect (G_OBJECT (window), "draw",
			  G_CALLBACK (draw_transparent_back_cb), NULL);
	g_signal_connect (G_OBJECT (window), "screen_changed",
			  G_CALLBACK (screen_change_cb), NULL);

	g_signal_connect (G_OBJECT (window), "destroy",
			  G_CALLBACK (gtk_main_quit), NULL);

	gtk_widget_show (window);

	gtk_main ();

	return 0;
}
