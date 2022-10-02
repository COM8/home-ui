#pragma once

#include "LightningWidget.hpp"
#include "backend/lightning/Lightning.hpp"
#include <chrono>
#include <list>
#include <mutex>
#include <vector>
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
    std::vector<backend::lightning::Lightning> toAddLightnings;
    std::mutex lightningMarkersMutex{};
    std::chrono::steady_clock::time_point lastLatLongUpdate{std::chrono::steady_clock::now()};

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
    static void on_viewport_changed(ShumateViewport* viewPort, G_GNUC_UNUSED GParamSpec* pspec, LightningMap* self);
};
}  // namespace ui::widgets