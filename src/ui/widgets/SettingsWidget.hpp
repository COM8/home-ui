#pragma once

#include <gtkmm.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>

namespace ui::widgets {
class SettingsWidget : public Gtk::Box {
 private:
    Gtk::Entry dbStationId;
    Gtk::CheckButton dbDestRegexCBtn{"Use destination regex"};
    Gtk::Entry dbDestRegex;
    Gtk::CheckButton dbStopsAtRegexCBtn{"Use stops at regex"};
    Gtk::Entry dbStopsAtRegex;
    Gtk::CheckButton dbFilterDepartedTrainsCBtn{"Filter departed trains"};
    Gtk::Entry dbLookAhead;
    Gtk::Entry dbLookBehind;

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