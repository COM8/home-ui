#pragma once

#include <gtkmm.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>

namespace ui::widgets {
class SettingsWidget : public Gtk::Box {
 private:
    Gtk::CheckButton mvgBusCBtn{"Bus"};
    Gtk::CheckButton mvgUbanCBtn{"U-Bahn"};
    Gtk::CheckButton mvgSbahnCBtn{"S-Bahn"};
    Gtk::CheckButton mvgTramCBtn{"Tram"};
    Gtk::Entry mvgLocation;
    Gtk::CheckButton mvgDestRegexCBtn{"Use destination regex"};
    Gtk::Entry mvgDestRegex;

    Gtk::Entry weatherLat;
    Gtk::Entry weatherLong;
    Gtk::Entry openWeatherApiKey;

 public:
    SettingsWidget();
    SettingsWidget(SettingsWidget&&) = default;
    SettingsWidget(const SettingsWidget&) = delete;
    SettingsWidget& operator=(SettingsWidget&&) = default;
    SettingsWidget& operator=(const SettingsWidget&) = delete;
    ~SettingsWidget() override = default;

 private:
    void prep_widget();
    void load_settings();
    void save_settings();

    //-----------------------------Events:-----------------------------
    void on_save_clicked();
};
}  // namespace ui::widgets