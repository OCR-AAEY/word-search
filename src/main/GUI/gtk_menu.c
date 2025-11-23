#include <gtk/gtk.h>

void image_loading(GtkImage *image, gchar *path)
{

    g_print("yeah");
    gtk_image_set_from_file(image, path);

    if (!image)
    {
        g_print("Failed to load image.\n");
    }
}

void open_file(GtkWidget *widget, gpointer label, GtkImage *image)
{
    GtkWidget *dialog;
    gchar *filename;

    // Create a file chooser dialog

    dialog = gtk_file_chooser_dialog_new(
        "Open File", GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open",
        GTK_RESPONSE_ACCEPT, NULL);

    // Show the dialog and get the selected file path
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_label_set_text(GTK_LABEL(label),
                           filename); // Update the label with the filename
        g_print("%s\n", filename);
        image_loading(image, filename); // Load the image
        g_free(filename);
    }

    // Destroy the dialog after usage
    gtk_widget_destroy(dialog);
}

void on_solve_clicked(GtkWidget *widget, gpointer data, GtkImage *image)
{
    g_print("SOLVE clicked\n");
    open_file(widget, data, image);
}

void on_steps_clicked(GtkWidget *widget, gpointer data)
{
    g_print("Steps clicked\n");
}

void on_exit_clicked(GtkWidget *widget, gpointer data)
{

    g_print("Exiting application...\n");
    gtk_main_quit();
}
void buttons_creation(GtkWidget **solve_b, GtkWidget **step_b,
                      GtkWidget **exit_b)
{
    *solve_b = gtk_button_new_with_label("SOLVE");
    *step_b = gtk_button_new_with_label("How it works");
    *exit_b = gtk_button_new_with_label("EXIT");
}
void buttons_func(GtkWidget **solve_b, GtkWidget **step_b, GtkWidget **exit_b,
                  GtkImage *image, GtkWidget **window)
{
    g_signal_connect(*solve_b, "clicked", G_CALLBACK(on_solve_clicked), image);
    g_signal_connect(*step_b, "clicked", G_CALLBACK(on_steps_clicked), NULL);
    g_signal_connect(*exit_b, "clicked", G_CALLBACK(on_exit_clicked), NULL);
    gtk_widget_show_all(*window);
}
void window_creation(GtkWidget **window)
{
    *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(*window), "Word Search Solver");
    gtk_window_set_default_size(GTK_WINDOW(*window), 400, 300);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *box;
    GtkImage *image; // Image widget
    GtkWidget *button_box;
    GtkWidget *solve_b, *step_b, *exit_b;
    //    GtkWidget *img_cont;
    // Initialize GTK:
    gtk_init(&argc, &argv);

    window_creation(&window);
    // Connect the "destroy" signal to quit GTK
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a main box to hold the image and buttons
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL,
                      10); // 10px spacing between elements
    gtk_container_add(GTK_CONTAINER(window), box);

    image = GTK_IMAGE(gtk_image_new()); // Initialize the image widget

    image_loading(image, "assets/logo/image.png");

    // Add the image above the buttons
    gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(image), FALSE, FALSE,
                       10); // 10px spacing

    // Create a box to hold the buttons (vertically)
    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,
                             10); // 10px spacing between buttons
    buttons_creation(&solve_b, &step_b, &exit_b);
    buttons_func(&solve_b, &step_b, &exit_b, image, &window);

    // Add buttons to the button box

    gtk_box_pack_start(GTK_BOX(button_box), solve_b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), step_b, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), exit_b, FALSE, FALSE, 0);
    // Center the button box within the window
    gtk_widget_set_valign(
        button_box,
        GTK_ALIGN_CENTER); // Align the button box vertically in the center

    // Add the button box to the main box
    gtk_box_pack_end(GTK_BOX(box), button_box, TRUE, TRUE,
                     10); // Add at the bottom

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
