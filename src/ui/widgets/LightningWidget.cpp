#include "LightningWidget.hpp"
#include "backend/lightning/Lightning.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <bits/chrono.h>
#include <glibmm/main.h>

namespace ui::widgets {
LightningWidget::LightningWidget(const backend::lightning::Lightning& lightning, ShumateMarkerLayer* markerLayer) : lightning(lightning),
                                                                                                                    markerLayer(markerLayer) {
    prep_widget();
}

void LightningWidget::prep_widget() {
    marker = shumate_marker_new();
    ShumateLocation* markerLocation = SHUMATE_LOCATION(marker);
    shumate_location_set_location(markerLocation, lightning.lat, lightning.lon);
    shumate_marker_layer_add_marker(markerLayer, marker);

    std::chrono::milliseconds milliElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lightning.time);
    if (std::chrono::duration_cast<std::chrono::seconds>(milliElapsed) < std::chrono::seconds(10)) {
        image.set_from_icon_name("lightning-symbolic");
        newLightning = true;
    } else {
        image.set_from_icon_name("lightning-circle-symbolic");
        newLightning = false;
    }

    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
    shumate_marker_set_child(marker, GTK_WIDGET(this->gobj()));

    drawingArea.set_draw_func(sigc::mem_fun(*this, &LightningWidget::on_draw_handler));
    set_can_target(false);
    set_can_focus(false);

    set_child(image);
    add_overlay(drawingArea);

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &LightningWidget::on_tick), 100);
}

const backend::lightning::Lightning& LightningWidget::get_lightning() const {
    return lightning;
}

void LightningWidget::remove() {
    shumate_marker_layer_remove_marker(markerLayer, marker);
}

//-----------------------------Events:-----------------------------
void LightningWidget::on_draw_handler(const Cairo::RefPtr<Cairo::Context>& /*ctx*/, int /*width*/, int /*height*/) {
    // TODO: Draw thunder
}

bool LightningWidget::on_tick() {
    SPDLOG_INFO("Tick");
    if (newLightning) {
        std::chrono::milliseconds milliElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lightning.time);
        if (std::chrono::duration_cast<std::chrono::seconds>(milliElapsed) > std::chrono::seconds(10)) {
            image.set_from_icon_name("lightning-circle-symbolic");
            newLightning = false;
        }
    }
    return true;
}

}  // namespace ui::widgets