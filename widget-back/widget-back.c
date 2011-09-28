#include <gtk/gtk.h>

#define BACKGROUND_ALPHA 0.75

static void
color_reverse (GdkRGBA *a)
{
        gdouble red;
        gdouble green;
        gdouble blue;
        gdouble h;
        gdouble s;
        gdouble v;

        red = a->red;
        green = a->green;
        blue = a->blue;

        gtk_rgb_to_hsv (red, green, blue, &h, &s, &v);

        v = 0.5 + (0.5 - v);
        if (v > 1.0)
                v = 1.0;
        else if (v < 0.0)
                v = 0.0;

        gtk_hsv_to_rgb (h, s, v, &red, &green, &blue);

        a->red = red;
        a->green = green;
        a->blue = blue;
}

static void
draw_rounded_rectangle (cairo_t *cr,
			gdouble  aspect,
			gdouble  x,
			gdouble  y,
			gdouble  corner_radius,
			gdouble  width,
			gdouble  height)
{
	gdouble radius = corner_radius / aspect;

	cairo_move_to (cr, x + radius, y);

	cairo_line_to (cr,
		       x + width - radius,
		       y);
	cairo_arc (cr,
		   x + width - radius,
		   y + radius,
		   radius,
		   -90.0f * G_PI / 180.0f,
		   0.0f * G_PI / 180.0f);
	cairo_line_to (cr,
		       x + width,
		       y + height - radius);
	cairo_arc (cr,
		   x + width - radius,
		   y + height - radius,
		   radius,
		   0.0f * G_PI / 180.0f,
		   90.0f * G_PI / 180.0f);
	cairo_line_to (cr,
		       x + radius,
		       y + height);
	cairo_arc (cr,
		   x + radius,
		   y + height - radius,
		   radius,
		   90.0f * G_PI / 180.0f,
		   180.0f * G_PI / 180.0f);
	cairo_line_to (cr,
		       x,
		       y + radius);
	cairo_arc (cr,
		   x + radius,
		   y + radius,
		   radius,
		   180.0f * G_PI / 180.0f,
		   270.0f * G_PI / 180.0f);
	cairo_close_path (cr);
}

static gboolean
draw_box_back (GtkWidget *widget,
	       cairo_t   *cr,
	       gpointer   user_data)
{
	GtkStyleContext	*context;
	GdkRGBA		 acolor;
	int		 width;
	int		 height;

	context = gtk_widget_get_style_context (widget);
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0);
	cairo_paint (cr);

	draw_rounded_rectangle (cr, 1.0, 0.0, 0.0, height/10, width-1, height-1);
	gtk_style_context_get_background_color (context, GTK_STATE_NORMAL, &acolor);
	color_reverse (&acolor);
	acolor.alpha = BACKGROUND_ALPHA;
	gdk_cairo_set_source_rgba (cr, &acolor);
	cairo_fill(cr);

	return FALSE;
}

int
main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *eventbox;
	GtkWidget *icon;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

	icon = gtk_image_new_from_file ("bt-blocked.svg");
	eventbox = gtk_event_box_new ();
	gtk_widget_set_app_paintable(eventbox, TRUE);
	g_signal_connect (G_OBJECT (eventbox), "draw", G_CALLBACK (draw_box_back), NULL);
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (eventbox), TRUE);

	gtk_container_add (GTK_CONTAINER (eventbox), icon);
	gtk_container_add (GTK_CONTAINER (window), eventbox);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
