#include "UiContext.hpp"
#include <adwaita.h>

namespace ui {
int UiContext::run(int argc, char** argv) {
    // Initialize Adwaita:
    adw_init();

    // Create the main GTK application:
    app = Gtk::Application::create("org.home_ui");

    // Add icon paths:
    Gtk::IconTheme::get_for_display(Gdk::Display::get_default())->add_resource_path("/ui/icons/scalable/action");

    app->signal_startup().connect([&] {
        add_main_window();
    });

    // The app will return once execution finished:
    return app->run(argc, argv);
}

void UiContext::add_main_window() {
    if (!mainWindow) {
        mainWindow = std::make_unique<windows::MainWindow>();
    }
    app->add_window(*mainWindow);
    mainWindow->show();
}
}  // namespace ui
