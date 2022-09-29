#pragma once

#include "LightningWidget.hpp"
#include "backend/lightning/Lightning.hpp"
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
    std::vector<LightningWidget> lightningMarkers{};

 public:
    LightningMap();
    LightningMap(LightningMap&&) = delete;
    LightningMap(const LightningMap&) = delete;
    LightningMap& operator=(LightningMap&&) = delete;
    LightningMap& operator=(const LightningMap&) = delete;
    ~LightningMap() override;

    void prep_widget();

    //-----------------------------Events:-----------------------------
    void on_new_lightnings(const std::vector<backend::lightning::Lightning>& lightnings);
};
}  // namespace ui::widgets