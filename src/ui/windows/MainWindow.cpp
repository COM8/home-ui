#include "MainWindow.hpp"
#include "ui/utils/UiUtils.hpp"
#include <memory>
#include <gtkmm/enums.h>
#include <spdlog/spdlog.h>

namespace ui::windows {
MainWindow::MainWindow() {
    prep_window();
}

void MainWindow::prep_window() {
    set_title("Home UI");
    set_default_size(800, 480);
    signal_key_press_event().connect(sigc::mem_fun(this, &MainWindow::on_key_pressed));
    signal_window_state_event().connect(sigc::mem_fun(this, &MainWindow::on_window_state_changed));
    fullscreen();

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
    Gtk::Box* leftBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    mainBox->add(*leftBox);
    leftBox->set_homogeneous(false);
    leftBox->set_vexpand(true);
    leftBox->add(actions);
    Gtk::Box* leftBottomBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_VERTICAL);
    leftBox->pack_end(*leftBottomBox);
    leftBottomBox->add(deviceStatus);
    leftBottomBox->set_valign(Gtk::Align::ALIGN_END);
    deviceStatus.set_valign(Gtk::Align::ALIGN_END);

    // Quick actions:
    leftBottomBox->add(quickActionsBox);
    quickActionsBox.set_halign(Gtk::Align::ALIGN_CENTER);
    quickActionsBox.set_valign(Gtk::Align::ALIGN_END);
    quickActionsBox.set_margin_bottom(10);
    quickActionsBox.add(fullscreenBtn);
    fullscreenBtn.set_image_from_icon_name("view-fullscreen", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
    fullscreenBtn.set_tooltip_text("Toggle fullscreen");
    fullscreenBtn.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_toggle_full_screen_clicked));
    quickActionsBox.add(cursorBtn);
    cursorBtn.set_image_from_icon_name("input-mouse", Gtk::BuiltinIconSize::ICON_SIZE_DIALOG);
    cursorBtn.set_tooltip_text("Toggle cursor visibility");
    cursorBtn.set_margin_start(10);
    cursorBtn.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_toggle_cursor_clicked));

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

void MainWindow::hide_cursor() {
    get_window()->set_cursor(Gdk::Cursor::create(Gdk::CursorType::BLANK_CURSOR));
    cursorHidden = true;
    SPDLOG_INFO("Cursor hidden.");
}

void MainWindow::show_cursor() {
    get_window()->set_cursor();
    cursorHidden = false;
    SPDLOG_INFO("Cursor shown.");
}

//-----------------------------Events:-----------------------------
void MainWindow::on_inspector_clicked() {
    viewMoreBtn->get_popover()->popdown();
    gtk_window_set_interactive_debugging(true);
}

void MainWindow::on_full_screen_clicked() { fullscreen(); }

void MainWindow::on_toggle_full_screen_clicked() {
    if (inFullScreen) {
        unfullscreen();
        inFullScreen = false;
    } else {
        fullscreen();
        inFullScreen = true;
    }
};

void MainWindow::on_toggle_cursor_clicked() {
    if (cursorHidden) {
        show_cursor();
    } else {
        hide_cursor();
    }
};

bool MainWindow::on_key_pressed(GdkEventKey* event) {
    if (event->keyval == GDK_KEY_Escape && inFullScreen) {
        unfullscreen();
        maximize();
        show_cursor();
        return true;
    }
    if (event->keyval == GDK_KEY_F11) {
        if (inFullScreen) {
            unfullscreen();
            maximize();
            show_cursor();
        } else {
            fullscreen();
            hide_cursor();
        }
        return true;
    }
    if (event->keyval == GDK_KEY_F12) {
        if (cursorHidden) {
            show_cursor();
        } else {
            hide_cursor();
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
