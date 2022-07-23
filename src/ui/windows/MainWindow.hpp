#pragma once

#include "ui/widgets/ActionsWidget.hpp"
#include "ui/widgets/DeviceStatusWidget.hpp"
#include "ui/widgets/MvgWidget.hpp"
#include "ui/widgets/SettingsWidget.hpp"
#include "ui/widgets/WeatherWidget.hpp"
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <gtkmm/scalebutton.h>

namespace ui::windows {
class MainWindow : public Gtk::Window {
 private:
    bool cursorHidden{false};
    Gtk::Stack* stack{nullptr};
    Gtk::MenuButton* viewMoreBtn{nullptr};
    widgets::MvgWidget mvg{};
    widgets::WeatherWidget weather{};
    widgets::ActionsWidget actions{};
    widgets::DeviceStatusWidget deviceStatus{};
    widgets::SettingsWidget settings{};
    Gtk::Button fullscreenBtn{};
    Gtk::Button cursorBtn{};
    Gtk::Button screenSaverBtn{};
    Gtk::ScaleButton screenBrightnessBtn;
    Gtk::Box quickActionsBox{Gtk::Orientation::HORIZONTAL};

 public:
    MainWindow();
    MainWindow(MainWindow&&) = delete;
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(MainWindow&&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;
    ~MainWindow() override = default;

 private:
    void prep_window();
    void prep_overview_stack_page(Gtk::Stack* stack);
    void prep_setting_stack_page(Gtk::Stack* stack);
    void hide_cursor();
    void show_cursor();

    //-----------------------------Events:-----------------------------
    void on_inspector_clicked();
    void on_full_screen_clicked();
    void on_toggle_full_screen_clicked();
    void on_toggle_cursor_clicked();
    static void on_screen_saver_clicked();
    static void on_screen_brightness_value_changed(double value);
    void on_window_state_changed();
    bool on_key_pressed(guint keyVal, guint keyCode, Gdk::ModifierType modifier);
    void on_full_screen_changed();
};
}  // namespace ui::windows
