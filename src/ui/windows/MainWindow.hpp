#pragma once

#include "ui/widgets/ActionsWidget.hpp"
#include "ui/widgets/MvgWidget.hpp"
#include "ui/widgets/WeatherWidget.hpp"
#include <gtkmm.h>

namespace ui::windows {
class MainWindow : public Gtk::Window {
 private:
    bool inFullScreen{false};
    Gtk::Stack* stack{nullptr};
    Gtk::MenuButton* viewMoreBtn{nullptr};
    widgets::MvgWidget mvg{};
    widgets::WeatherWidget weather{};
    widgets::ActionsWidget actions{};

 public:
    MainWindow();
    MainWindow(MainWindow&&) = default;
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(MainWindow&&) = default;
    MainWindow& operator=(const MainWindow&) = delete;
    ~MainWindow() override = default;

 private:
    void prep_window();
    void prep_overview_stack_page(Gtk::Stack* stack);
    static void prep_setting_stack_page(Gtk::Stack* stack);

    //-----------------------------Events:-----------------------------
    void on_inspector_clicked();
    void on_full_screen_clicked();
    bool on_window_state_changed(GdkEventWindowState* state);
    bool on_key_pressed(GdkEventKey* event);
};
}  // namespace ui::windows
