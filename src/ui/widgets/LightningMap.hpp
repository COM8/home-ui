#pragma once

#include "LightningWidget.hpp"
#include "backend/lightning/Lightning.hpp"
#include <list>
#include <mutex>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <shumate/shumate.h>

namespace ui::widgets {
class LightningMap : public Gtk::Box {
 private:
    ShumateSimpleMap* map{nullptr};
    ShumateMarker* homeMarker{nullptr};
    ShumateMarkerLayer* markerLayer{nullptr};
    Gtk::Image homeMarkerImage{};
    std::list<LightningWidget> lightningMarkers{};
    std::mutex lightningMarkersMutex{};

 public:
    LightningMap();
    LightningMap(LightningMap&&) = delete;
    LightningMap(const LightningMap&) = delete;
    LightningMap& operator=(LightningMap&&) = delete;
    LightningMap& operator=(const LightningMap&) = delete;
    ~LightningMap() override;

    void prep_widget();

    void set_is_being_displayed(bool isBeingDisplayed);

    //-----------------------------Events:-----------------------------
    void on_new_lightnings(const std::vector<backend::lightning::Lightning>& lightnings);
    bool on_tick();
};
}  // namespace ui::widgets