#pragma once

#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <shumate/shumate.h>

namespace ui::widgets {
class LightningMap : public Gtk::Box {
 private:
    ShumateSimpleMap* map{nullptr};
    ShumateMarker* marker{nullptr};
    ShumateMarkerLayer* markerLayer{nullptr};

    Gtk::Image markerImage{};

 public:
    LightningMap();
    LightningMap(LightningMap&&) = delete;
    LightningMap(const LightningMap&) = delete;
    LightningMap& operator=(LightningMap&&) = delete;
    LightningMap& operator=(const LightningMap&) = delete;
    ~LightningMap() override = default;

    void prep_widget();

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets