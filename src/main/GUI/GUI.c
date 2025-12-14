#include "grid_rebuild/grid_rebuild.h"
#include "image_loader/image_loading.h"
#include "location/letters_extraction.h"
#include "ocr/neural_network.h"
#include "pretreatment/pretreatment.h"
#include "pretreatment/visualization.h"
#include "solver/grid.h"
#include "utils/utils.h"
#include "wordlist_rebuild/wordlist_rebuild.h"
#include <gtk/gtk.h>
#include <string.h>

#define MODEL "real"

/* ----------GLOBALS------------ */
const char *step_image_paths[8] = {
    "assets/logo/image.png",       ROTATED_FILENAME,
    POSTTREATMENT_FILENAME,        HOUGHLINES_VISUALIZATION_FILENAME,
    WORDS_BOUNDING_BOXES_FILENAME, LETTERS_BOUNDING_BOXES_FILENAME,
    "extracted/grid/(0_0).png",    "extracted/solved.png"};
GtkImage *step_images[8];
GtkButton *step_load[8], *step_next_btn[8], *step_back_btn[8];
GtkButton *solve_load_btn[2], *solve_next_btn[2], *solve_back_btn[2];
/* ---------- STRUCTS ---------- */
typedef struct
{
    GtkImage *current_image;
    GtkButton *next_btn;
} LoadNextData;

/* ---------- CALLBACKS ---------- */
char **wordlist_to_wordarray(Wordlist *wordlist)
{
    char **words = malloc((wordlist->count + 1) * sizeof(char *));
    words[wordlist->count] = NULL;
    for (size_t i = 0; i < (size_t)(wordlist->count); i++)
    {
        words[i] = malloc((wordlist->lengths[i] + 1) * sizeof(char));
        memcpy(words[i], wordlist->words[i], wordlist->lengths[i]);
        words[wordlist->lengths[i]] = '\0';
    }

    wordlist_free(wordlist);

    return words;
}

static void exit_app(GtkButton *b, gpointer win)
{
    gtk_widget_destroy(GTK_WIDGET(win));
}

static void go_to_screen(GtkButton *b, gpointer stack_name)
{
    GtkStack *stack = GTK_STACK(g_object_get_data(G_OBJECT(b), "stack"));
    gtk_stack_set_visible_child_name(stack, (const char *)stack_name);
}

static void reload_image(GtkImage *image, const char *path)
{
    gtk_image_clear(image);
    gtk_image_set_from_file(image, path);
}

static void load_action(GtkButton *button, gpointer user_data)
{
    LoadNextData *data = (LoadNextData *)user_data;
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Load Image", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    // image filtering
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/jpg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    // file explorer
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        // solver launch
        Point **points;

        size_t h_points;
        size_t w_points;
        Grid *grid = grid_rebuild_from_folder_with_model(
            "./extracted/grid", "./assets/ocr/model/" MODEL ".nn");
        Wordlist *wordlist = wordlist_rebuild_from_folder(
            "./extracted/words", "./assets/ocr/model/" MODEL ".nn");
        size_t nb_words = wordlist->count;
        char **words = wordlist_to_wordarray(wordlist);
        int e = locate_and_extract_letters_png((const char *)filename, &points,
                                               &h_points, &w_points);
        if (e)
        {
            g_print("invalid file");
        }
        gtk_image_set_from_file(data->current_image, filename);

        ImageData *img = load_image(filename);

        Matrix *m, *tmp;
        m = image_to_grayscale(img);
        free_image(img);

        tmp = adaptative_gaussian_thresholding(m, 1.0f, 11, 10, 5);
        mat_free(m);
        m = tmp;

        mat_inplace_to_one_hot(m);

        mat_inplace_toggle(m);

        tmp = morph_transform(m, 2, Closing);
        mat_free(m);
        m = tmp;

        tmp = morph_transform(m, 2, Opening);
        mat_free(m);
        m = tmp;

        tmp = mat_strip_margins(m);
        mat_free(m);
        m = tmp;

        tmp = mat_scale_to_28(m, 0.0f);
        mat_free(m);
        m = tmp;

        mat_inplace_vertical_flatten(m);

        Neural_Network *net = net_load_from_file("assets/ocr/model/grid.nn");
        char res = net_decode_letter(net, m, NULL);

        mat_free(m);
        net_free(net);
        g_print("The character is : %c\n", res);
        int **structure = grid_solve(grid, words, nb_words);
        highlight_words((const char *)filename, structure, points, nb_words);
        free_points(points, h_points);
        gtk_widget_set_sensitive(GTK_WIDGET(solve_next_btn[0]), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(step_next_btn[0]), TRUE);
        /* Reload all step images from disk */
        for (int i = 1; i < 8; i++)
        {
            reload_image(step_images[i], step_image_paths[i]);
            if (step_next_btn[i])
                gtk_widget_set_sensitive(GTK_WIDGET(step_next_btn[i]), TRUE);
        }

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void next_action(GtkButton *button, gpointer user_data)
{
    // next screen
    GtkStack *stack = GTK_STACK(user_data);
    const char *next_screen =
        (const char *)g_object_get_data(G_OBJECT(button), "next_screen");
    gtk_stack_set_visible_child_name(stack, next_screen);
}

static void save_image_action(GtkButton *button, gpointer user_data)
{
    // result saving
    GtkImage *image = GTK_IMAGE(user_data);
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
    if (!pixbuf)
    {
        g_print("No image loaded to save.\n");
        return;
    }

    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Image", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                   TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "image.png");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gdk_pixbuf_save(pixbuf, filename, "png", NULL, NULL);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

/* ---------- MENU SCREEN ---------- */
GtkWidget *create_menu_screen(GtkStack *stack, GtkWidget **steps_btn,
                              GtkWidget **solve_btn)
{
    GtkWidget *overlay = gtk_overlay_new();

    GtkWidget *bg_image = gtk_image_new_from_file("assets/logo/image.png");
    gtk_widget_set_hexpand(bg_image, TRUE);
    gtk_widget_set_vexpand(bg_image, TRUE);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), bg_image);

    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_halign(menu_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(menu_box, GTK_ALIGN_CENTER);

    GtkWidget *menu_label = gtk_label_new("Word Search Solver");

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        provider,

        "label { background-color: #D3D3D3 ; color: black ; padding: 8px; "
        "border-radius: 4px; }",
        -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(menu_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    *solve_btn = gtk_button_new_with_label("Solve");
    *steps_btn = gtk_button_new_with_label("Steps");
    GtkWidget *exit_btn = gtk_button_new_with_label("Exit");

    gtk_widget_set_size_request(*solve_btn, 120, 30);
    gtk_widget_set_size_request(*steps_btn, 120, 30);
    gtk_widget_set_size_request(exit_btn, 120, 30);

    gtk_box_pack_start(GTK_BOX(menu_box), menu_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(menu_box), *solve_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(menu_box), *steps_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(menu_box), exit_btn, FALSE, FALSE, 5);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), menu_box);
    gtk_stack_add_named(stack, overlay, "menu");

    g_object_set_data(G_OBJECT(*solve_btn), "stack", stack);
    g_object_set_data(G_OBJECT(*steps_btn), "stack", stack);
    g_signal_connect(*solve_btn, "clicked", G_CALLBACK(go_to_screen),
                     (gpointer) "solve_load");
    g_signal_connect(*steps_btn, "clicked", G_CALLBACK(go_to_screen),
                     (gpointer) "step1_loading");
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(exit_app),
                     gtk_widget_get_toplevel(GTK_WIDGET(overlay)));

    return overlay;
}

/* ---------- STEP SCREEN ---------- */
GtkWidget *create_step_screen(GtkStack *stack, const char *label_text,
                              const char *screen_name,
                              const char *next_screen_name,
                              const char *back_screen_name, gboolean has_load,
                              const char *image_path, GtkImage **out_image,
                              GtkButton **out_load_btn,
                              GtkButton **out_next_btn,
                              GtkButton **out_back_btn, int step_index)
{

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkWidget *image = gtk_image_new_from_file(image_path); // unique image

    GtkWidget *label = gtk_label_new(label_text);

    GtkWidget *next_btn = NULL;
    if (next_screen_name && next_screen_name[0] != '\0')
    {
        next_btn = gtk_button_new_with_label("Next");
        gtk_widget_set_size_request(next_btn, 120, 30);
        gtk_widget_set_sensitive(next_btn, has_load ? FALSE : TRUE);
    }

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 30);

    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 5);
    if (step_index == 6)
    {
        GtkWidget *text = gtk_label_new("The character is :");
        gtk_box_pack_start(GTK_BOX(box), text, FALSE, FALSE, 5);
    }

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 5);

    GtkWidget *load_btn = NULL;
    if (has_load)
    {
        load_btn = gtk_button_new_with_label("Load");
        gtk_widget_set_size_request(load_btn, 120, 30);
        gtk_box_pack_start(GTK_BOX(box), load_btn, FALSE, FALSE, 5);

        LoadNextData *data = g_new0(LoadNextData, 1);
        data->current_image = GTK_IMAGE(image);
        data->next_btn = GTK_BUTTON(next_btn);
        g_signal_connect(load_btn, "clicked", G_CALLBACK(load_action), data);
    }

    if (next_btn)
        gtk_box_pack_start(GTK_BOX(box), next_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), back_btn, FALSE, FALSE, 5);

    gtk_stack_add_named(stack, box, screen_name);

    if (next_btn)
    {
        g_object_set_data(G_OBJECT(next_btn), "next_screen",
                          (gpointer)next_screen_name);
        g_signal_connect(next_btn, "clicked", G_CALLBACK(next_action), stack);
    }

    g_object_set_data(G_OBJECT(back_btn), "stack", stack);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(go_to_screen),
                     (gpointer)back_screen_name);

    if (out_image)
        *out_image = GTK_IMAGE(image);
    if (out_load_btn)
        *out_load_btn = GTK_BUTTON(load_btn);
    if (out_next_btn)
        *out_next_btn = GTK_BUTTON(next_btn);
    if (out_back_btn)
        *out_back_btn = GTK_BUTTON(back_btn);

    return box;
}

/* ---------- SOLVE SCREEN ---------- */
GtkWidget *create_solve_screen(GtkStack *stack, const char *screen_name,
                               const char *next_screen_name,
                               const char *back_screen_name,
                               const char *image_path, GtkImage **out_image,
                               GtkButton **out_load_btn,
                               GtkButton **out_next_btn,
                               GtkButton **out_back_btn)
{

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkWidget *image = gtk_image_new_from_file(image_path);
    GtkWidget *load_btn = gtk_button_new_with_label("Load");
    GtkWidget *next_btn = gtk_button_new_with_label("Next");
    GtkWidget *back_btn = gtk_button_new_with_label("Back");

    gtk_widget_set_size_request(load_btn, 120, 30);
    gtk_widget_set_size_request(next_btn, 120, 30);
    gtk_widget_set_size_request(back_btn, 120, 30);
    gtk_widget_set_sensitive(next_btn, FALSE);

    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), load_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), next_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), back_btn, FALSE, FALSE, 5);

    gtk_stack_add_named(stack, box, screen_name);

    LoadNextData *data = g_new0(LoadNextData, 1);
    data->current_image = GTK_IMAGE(image);
    data->next_btn = GTK_BUTTON(next_btn);
    g_signal_connect(load_btn, "clicked", G_CALLBACK(load_action), data);

    g_object_set_data(G_OBJECT(next_btn), "next_screen",
                      (gpointer)next_screen_name);
    g_signal_connect(next_btn, "clicked", G_CALLBACK(next_action), stack);

    g_object_set_data(G_OBJECT(back_btn), "stack", stack);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(go_to_screen),
                     (gpointer)back_screen_name);

    if (out_image)
        *out_image = GTK_IMAGE(image);
    if (out_load_btn)
        *out_load_btn = GTK_BUTTON(load_btn);
    if (out_next_btn)
        *out_next_btn = GTK_BUTTON(next_btn);
    if (out_back_btn)
        *out_back_btn = GTK_BUTTON(back_btn);

    return box;
}

/* ---------- MAIN ---------- */
int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Word Search Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget *stack_widget = gtk_stack_new();
    GtkStack *stack = GTK_STACK(stack_widget);
    gtk_stack_set_transition_type(stack,
                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(stack, 300);
    gtk_container_add(GTK_CONTAINER(window), stack_widget);

    /* ---------- MENU ---------- */
    GtkWidget *steps_btn, *solve_btn;
    create_menu_screen(stack, &steps_btn, &solve_btn);

    /* ---------- STEPS ---------- */
    const char *step_screens[8] = {"step1_loading",
                                   "step2_rotation",
                                   "step3_preatreatement",
                                   "step4_grid_detection",
                                   "step5_word_detection",
                                   "step6_letter_detection",
                                   "step7_character_recognition",
                                   "step8_solved"};
    const char *step_next[8] = {
        "step2_rotation",         "step3_preatreatement",
        "step4_grid_detection",   "step5_word_detection",
        "step6_letter_detection", "step7_character_recognition",
        "step8_solved",           ""};
    const char *step_back[8] = {"menu",
                                "step1_loading",
                                "step2_rotation",
                                "step3_preatreatement",
                                "step4_grid_detection",
                                "step5_word_detection",
                                "step6_letter_detection",
                                "step7_character_recognition"};
    /*const char *step_image_paths[10] = {
        "assets/logo/image.png",
        "assets/"ROTATED_FILENAME,
        "assets/"POSTTREATMENT_FILENAME ,
        "assets/"HOUGHLINES_VISUALIZATION_FILENAME ,
        "assets/"WORDS_BOUNDING_BOXES_FILENAME  ,
        "assets/"LETTERS_BOUNDING_BOXES_FILENAME ,
        "assets/steps/7.png",
        "assets/steps/8.png",
        "assets/steps/9.png",
        "assets/solved.png"};
        */
    // GtkImage *step_images[10];
    // GtkButton *step_load[10], *step_next_btn[10], *step_back_btn[10];

    for (int i = 0; i < 8; i++)
    {
        create_step_screen(stack, step_screens[i], step_screens[i],
                           step_next[i], step_back[i], i == 0,
                           step_image_paths[i], &step_images[i], &step_load[i],
                           &step_next_btn[i], &step_back_btn[i], i);

        // Step 8: Save & Quit
        if (i == 7)
        {
            GtkWidget *save_btn = gtk_button_new_with_label("Save");
            GtkWidget *quit_btn = gtk_button_new_with_label("Quit");
            gtk_widget_set_size_request(save_btn, 120, 30);
            gtk_widget_set_size_request(quit_btn, 120, 30);
            GtkWidget *parent_box =
                gtk_widget_get_parent(GTK_WIDGET(step_images[i]));
            gtk_box_pack_start(GTK_BOX(parent_box), save_btn, FALSE, FALSE, 5);
            gtk_box_pack_start(GTK_BOX(parent_box), quit_btn, FALSE, FALSE, 5);
            g_signal_connect(save_btn, "clicked", G_CALLBACK(save_image_action),
                             step_images[i]);
            g_signal_connect(quit_btn, "clicked", G_CALLBACK(exit_app), window);
        }
    }

    /* ---------- SOLVE ---------- */
    GtkImage *solve_images[2];

    create_solve_screen(stack, "solve_load", "solve_save", "menu",
                        "assets/solved.png", &solve_images[0],
                        &solve_load_btn[0], &solve_next_btn[0],
                        &solve_back_btn[0]);

    GtkWidget *solve_save_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_halign(solve_save_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(solve_save_box, GTK_ALIGN_CENTER);
    GtkWidget *solve_save_image = gtk_image_new_from_file("assets/solved.png");
    GtkWidget *save_btn = gtk_button_new_with_label("Save");
    GtkWidget *quit_btn = gtk_button_new_with_label("Quit");
    gtk_widget_set_size_request(save_btn, 120, 30);
    gtk_widget_set_size_request(quit_btn, 120, 30);
    gtk_box_pack_start(GTK_BOX(solve_save_box), solve_save_image, FALSE, FALSE,
                       5);
    gtk_box_pack_start(GTK_BOX(solve_save_box), save_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(solve_save_box), quit_btn, FALSE, FALSE, 5);
    gtk_stack_add_named(stack, solve_save_box, "solve_save");
    g_signal_connect(save_btn, "clicked", G_CALLBACK(save_image_action),
                     solve_save_image);
    g_signal_connect(quit_btn, "clicked", G_CALLBACK(exit_app), window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_stack_set_visible_child_name(stack, "menu");
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
