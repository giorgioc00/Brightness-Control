#include <gtk/gtk.h>
#include <libayatana-appindicator/app-indicator.h>
#include <cstdlib>
#include <cstring>

// Constants
namespace Config {
    constexpr const char* APP_ID = "brightness-control";
    constexpr const char* APP_NAME = "Brightness Control";
    constexpr const char* ICON_NAME = "video-display";
    constexpr const int DISPLAY_DEFAULT = 1;
    constexpr const int DISPLAY_MAX = 16;
    constexpr const int VCP_CODE = 10;
    constexpr const int BRIGHTNESS_MIN = 0;
    constexpr const int BRIGHTNESS_MAX = 20;
    constexpr const int BRIGHTNESS_DEFAULT = 10;
    constexpr const int WINDOW_WIDTH = 350;
    constexpr const int WINDOW_HEIGHT = 120;
    constexpr const int BORDER_WIDTH = 15;
}

// Application state
struct AppState {
    GtkWidget* window = nullptr;
    GtkWidget* scale = nullptr;
    GtkWidget* level_label = nullptr;
    AppIndicator* indicator = nullptr;
    int current_brightness = Config::BRIGHTNESS_DEFAULT;
    int display_num = Config::DISPLAY_DEFAULT;
};

static AppState app;

// Utility functions
void execute_ddcutil(int brightness_level) {
    int brightness_value = brightness_level * 5; // Convert 0-20 to 0-100
    char command[256];
    snprintf(command, sizeof(command),
             "ddcutil setvcp %d %d --display %d --noverify",
             Config::VCP_CODE, brightness_value, app.display_num);
    system(command);
}

void update_tray_label() {
    if (app.indicator) {
        char label[16];
        snprintf(label, sizeof(label), "%d", app.current_brightness);
        app_indicator_set_label(app.indicator, label, "10");
    }
}

void update_brightness_level(int value) {
    if (value < Config::BRIGHTNESS_MIN || value > Config::BRIGHTNESS_MAX) {
        return;
    }

    app.current_brightness = value;
    execute_ddcutil(value);
    update_tray_label();

    // Update window label
    if (app.level_label) {
        char text[32];
        snprintf(text, sizeof(text), "Level: %d", value);
        gtk_label_set_text(GTK_LABEL(app.level_label), text);
    }

    // Update scale
    if (app.scale) {
        gtk_range_set_value(GTK_RANGE(app.scale), value);
    }
}

// Window management
void show_window() {
    if (app.window) {
        gtk_window_deiconify(GTK_WINDOW(app.window));
        gtk_window_present(GTK_WINDOW(app.window));
    }
}

void hide_window() {
    if (app.window) {
        gtk_widget_hide(app.window);
    }
}

void quit_application() {
    gtk_main_quit();
}

// Signal handlers
void on_brightness_changed(GtkWidget* widget, gpointer data) {
    double value = gtk_range_get_value(GTK_RANGE(app.scale));
    update_brightness_level(static_cast<int>(value));
}

void on_display_changed(GtkWidget* widget, gpointer data) {
    app.display_num = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

void on_quick_set_selected(GtkWidget* widget, gpointer data) {
    int level = GPOINTER_TO_INT(data);
    update_brightness_level(level);
}

void on_show_window_clicked(GtkWidget* widget, gpointer data) {
    show_window();
}

void on_hide_window_clicked(GtkWidget* widget, gpointer data) {
    hide_window();
}

void on_quit_clicked(GtkWidget* widget, gpointer data) {
    quit_application();
}

gboolean on_window_close(GtkWidget* widget, GdkEvent* event, gpointer data) {
    hide_window();
    return TRUE;
}

// UI Builders
GtkWidget* create_brightness_submenu() {
    GtkWidget* submenu = gtk_menu_new();

    for (int i = Config::BRIGHTNESS_MIN; i <= Config::BRIGHTNESS_MAX; i++) {
        char label[16];
        snprintf(label, sizeof(label), "%d", i);

        GtkWidget* item = gtk_menu_item_new_with_label(label);
        g_signal_connect(item, "activate", G_CALLBACK(on_quick_set_selected),
                        GINT_TO_POINTER(i));
        gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
    }

    return submenu;
}

GtkWidget* create_tray_menu() {
    GtkWidget* menu = gtk_menu_new();

    // Show window item
    GtkWidget* show_item = gtk_menu_item_new_with_label("Show Brightness Control");
    g_signal_connect(show_item, "activate", G_CALLBACK(on_show_window_clicked), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), show_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    // Quick set submenu
    GtkWidget* quick_set_menu = gtk_menu_item_new_with_label("Quick Set");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quick_set_menu);

    GtkWidget* quick_set_submenu = create_brightness_submenu();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(quick_set_menu), quick_set_submenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    // Quit item
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit_clicked), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);

    gtk_widget_show_all(menu);
    return menu;
}

void create_tray_indicator() {
    app.indicator = app_indicator_new(
        Config::APP_ID,
        Config::ICON_NAME,
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS
    );

    if (!app.indicator) {
        return;
    }

    app_indicator_set_status(app.indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_icon_full(app.indicator, Config::ICON_NAME, Config::APP_NAME);

    GtkWidget* menu = create_tray_menu();
    app_indicator_set_menu(app.indicator, GTK_MENU(menu));

    update_tray_label();
}

GtkWidget* create_main_window() {
    // Create window
    app.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app.window), Config::APP_NAME);
    gtk_window_set_default_size(GTK_WINDOW(app.window),
                                Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(app.window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(app.window), Config::ICON_NAME);
    gtk_container_set_border_width(GTK_CONTAINER(app.window), Config::BORDER_WIDTH);

    // Main container
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(app.window), vbox);

    // Title
    GtkWidget* title_label = gtk_label_new(nullptr);
    gtk_label_set_markup(GTK_LABEL(title_label), "<big><b>Monitor Brightness</b></big>");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 5);

    // Display selector
    GtkWidget* display_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* display_label = gtk_label_new("Display:");
    gtk_box_pack_start(GTK_BOX(display_box), display_label, FALSE, FALSE, 0);

    GtkWidget* display_spin = gtk_spin_button_new_with_range(1, Config::DISPLAY_MAX, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(display_spin), app.display_num);
    gtk_box_pack_start(GTK_BOX(display_box), display_spin, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), display_box, FALSE, FALSE, 0);

    // Level display
    app.level_label = gtk_label_new("Level: 10");
    gtk_box_pack_start(GTK_BOX(vbox), app.level_label, FALSE, FALSE, 0);

    // Brightness slider
    GtkAdjustment* adj = GTK_ADJUSTMENT(gtk_adjustment_new(
        Config::BRIGHTNESS_DEFAULT,
        Config::BRIGHTNESS_MIN,
        Config::BRIGHTNESS_MAX,
        1, 1, 0
    ));

    app.scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj);
    gtk_scale_set_digits(GTK_SCALE(app.scale), 0);
    gtk_scale_set_value_pos(GTK_SCALE(app.scale), GTK_POS_RIGHT);
    gtk_widget_set_margin_top(app.scale, 10);
    gtk_widget_set_margin_bottom(app.scale, 10);

    // Add marks for each level
    for (int i = Config::BRIGHTNESS_MIN; i <= Config::BRIGHTNESS_MAX; i++) {
        gtk_scale_add_mark(GTK_SCALE(app.scale), i, GTK_POS_BOTTOM, nullptr);
    }

    gtk_box_pack_start(GTK_BOX(vbox), app.scale, TRUE, TRUE, 0);

    // Hide to tray button
    GtkWidget* hide_button = gtk_button_new_with_label("Hide to Tray");
    gtk_box_pack_start(GTK_BOX(vbox), hide_button, FALSE, FALSE, 5);

    // Connect signals
    g_signal_connect(app.scale, "value-changed", G_CALLBACK(on_brightness_changed), nullptr);
    g_signal_connect(display_spin, "value-changed", G_CALLBACK(on_display_changed), nullptr);
    g_signal_connect(hide_button, "clicked", G_CALLBACK(on_hide_window_clicked), nullptr);
    g_signal_connect(app.window, "delete-event", G_CALLBACK(on_window_close), nullptr);

    return app.window;
}

int main(int argc, char* argv[]) {
    GOptionEntry entries[] = {
        { "display", 'd', 0, G_OPTION_ARG_INT, &app.display_num,
          "ddcutil display number to control (default: 1)", "N" },
        { nullptr }
    };

    GError* error = nullptr;
    if (!gtk_init_with_args(&argc, &argv, nullptr, entries, nullptr, &error)) {
        g_printerr("%s\n", error ? error->message : "Failed to initialize GTK");
        g_clear_error(&error);
        return 1;
    }

    if (app.display_num < 1 || app.display_num > Config::DISPLAY_MAX) {
        g_printerr("Display number must be between 1 and %d\n", Config::DISPLAY_MAX);
        return 1;
    }

    create_main_window();
    create_tray_indicator();

    // Ensure window is visible and bring to front
    gtk_widget_show_all(app.window);
    gtk_window_present(GTK_WINDOW(app.window));

    gtk_main();

    return 0;
}
