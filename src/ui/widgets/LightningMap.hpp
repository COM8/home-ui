#pragma once

#include <gtkmm/box.h>
#include <shumate/shumate.h>

namespace ui::widgets {
class LightningMap : public Gtk::Box {
 public:
    LightningMap() = default;
    LightningMap(LightningMap&&) = delete;
    LightningMap(const LightningMap&) = delete;
    LightningMap& operator=(LightningMap&&) = delete;
    LightningMap& operator=(const LightningMap&) = delete;
    ~LightningMap() override;

    //-----------------------------Events:-----------------------------
};
}  // namespace ui::widgets