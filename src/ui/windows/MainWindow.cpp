#include "MainWindow.hpp"
#include "ui/utils/UiUtils.hpp"
#include <memory>

namespace ui::windows {
MainWindow::MainWindow() {
    prep_window();
}

void MainWindow::prep_window() {
    set_title("Home UI");
    set_default_size(800, 550);
    signal_key_press_event().connect(sigc::mem_fun(this, &MainWindow::on_key_pressed));
    signal_window_state_event().connect(sigc::mem_fun(this, &MainWindow::on_window_state_changed));
    // fullscreen();

    // Content:
    stack = Gtk::make_managed<Gtk::Stack>();
    prep_overview_stack_page(stack);
    prep_setting_stack_page(stack);
    add(*stack);

    // Header bar:
    Gtk::HeaderBar* headerBar = Gtk::make_managed<Gtk::HeaderBar>();
    viewMoreBtn = Gtk::make_managed<Gtk::MenuButton>();
    viewMoreBtn->set_image_from_icon_name("open-menu");
    Gtk::PopoverMenu* viewMorePopover = Gtk::make_managed<Gtk::PopoverMenu>();
    Gtk::Stack* viewMoreMenuStack = Gtk::make_managed<Gtk::Stack>();
    Gtk::Box* viewMoreMenuBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    Gtk::Button* fullscreenBtn = Gtk::make_managed<Gtk::Button>("Fullscreen");
    fullscreenBtn->signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_full_screen_clicked));
    viewMoreMenuBox->add(*fullscreenBtn);
    Gtk::Button* inspectorBtn = Gtk::make_managed<Gtk::Button>("Inspector");
    inspectorBtn->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_inspector_clicked));
    viewMoreMenuBox->add(*inspectorBtn);
    Gtk::Button* aboutBtn = Gtk::make_managed<Gtk::Button>("About");
    viewMoreMenuBox->add(*aboutBtn);
    viewMoreMenuStack->add(*viewMoreMenuBox, "main");
    viewMoreMenuStack->show_all();
    viewMorePopover->add(*viewMoreMenuStack);
    viewMoreBtn->set_popover(*viewMorePopover);
    headerBar->pack_end(*viewMoreBtn);

    Gtk::StackSwitcher* stackSwitcher = Gtk::make_managed<Gtk::StackSwitcher>();
    stackSwitcher->set_stack(*stack);
    headerBar->set_custom_title(*stackSwitcher);
    set_titlebar(*headerBar);

    show_all();
}

void MainWindow::prep_overview_stack_page(Gtk::Stack* stack) {
    Gtk::Box* mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    mainBox->set_halign(Gtk::Align::ALIGN_FILL);
    mainBox->set_valign(Gtk::Align::ALIGN_FILL);
    mainBox->set_vexpand(true);
    mainBox->set_homogeneous(true);

    // Actions:
    mainBox->add(actions);

    Gtk::Box* rightBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    rightBox->set_homogeneous(true);
    mainBox->add(*rightBox);
    // MVG:
    rightBox->add(mvg);

    // Weather:
    rightBox->add(weather);

    stack->add(*mainBox, "overview", "Overview");
}

void MainWindow::prep_setting_stack_page(Gtk::Stack* stack) {
    stack->add(settings, "settings", "Settings");
}

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_clicked() {
    viewMoreBtn->get_popover()->popdown();
    gtk_window_set_interactive_debugging(true);
}

void MainWindow::on_full_screen_clicked() {
    fullscreen();
}

bool MainWindow::on_key_pressed(GdkEventKey* event) {
    if (event->keyval == GDK_KEY_Escape && inFullScreen) {
        unfullscreen();
        maximize();
        return true;
    }
    if (event->keyval == GDK_KEY_F11) {
        if (inFullScreen) {
            unfullscreen();
            maximize();
        } else {
            fullscreen();
        }
        return true;
    }
    return false;
}

bool MainWindow::on_window_state_changed(GdkEventWindowState* state) {
    inFullScreen = state->new_window_state & GDK_WINDOW_STATE_FULLSCREEN;
    return false;
}
}  // namespace ui::windows
