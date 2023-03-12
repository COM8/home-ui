#pragma once

#include "backend/db/Departure.hpp"
#include "ui/widgets/DepartureWidget.hpp"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <glibmm/dispatcher.h>
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/listbox.h>

namespace ui::widgets {
class DbWidget : public Gtk::Box {
 private:
    Gtk::ListBox departureslistBox{};
    Gtk::Box loadingBox{Gtk::Orientation::VERTICAL};

    bool shouldRun{false};
    std::unique_ptr<std::thread> updateThread{nullptr};
    Glib::Dispatcher disp;
    std::vector<std::shared_ptr<backend::db::Departure>> departures{};
    std::vector<DepartureWidget> departureWidgets{};
    std::mutex departuresMutex{};

 public:
    DbWidget();
    DbWidget(DbWidget&&) = delete;
    DbWidget(const DbWidget&) = delete;
    DbWidget& operator=(DbWidget&&) = delete;
    DbWidget& operator=(const DbWidget&) = delete;
    ~DbWidget() override;

 private:
    void prep_widget();
    void start_thread();
    void stop_thread();

    void update_departures();
    void update_departures_ui();
    void thread_run();

    //-----------------------------Events:-----------------------------
    void on_notification_from_update_thread();
};
}  // namespace ui::widgets