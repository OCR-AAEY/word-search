#include <cairo.h>
#include <gtk/gtk.h>

// Callback function for drawing
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    // Set the drawing color (RGB format, values between 0 and 1)
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0); // Blue color

    // Draw a rectangle (x, y, width, height)
    cairo_rectangle(cr, 50, 50, 300, 300);
    cairo_fill(cr); // Fill the rectangle with the color

    // Draw a red line
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Red color
    cairo_move_to(cr, 50, 50);               // Start at (50, 50)
    cairo_line_to(cr, 350, 350);             // End at (350, 350)
    cairo_stroke(cr);                        // Draw the line

    return FALSE; // Returning FALSE allows GTK to handle further drawing
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *drawing_area;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a new top-level window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Cairo with GTK in C");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    // Connect the "destroy" event to close the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a drawing area widget
    drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    // Connect the "draw" signal of the drawing area to the on_draw callback
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    // Show all widgets in the window
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
