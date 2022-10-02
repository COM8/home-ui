#include "LightningMap.hpp"
#include "backend/lightning/Lightning.hpp"
#include "backend/lightning/LightningHelper.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "ui/widgets/LightningWidget.hpp"
#include <cassert>
#include <chrono>
#include <thread>
#include <glibmm/main.h>
#include <gtkmm/mediafile.h>
#include <shumate/shumate-simple-map.h>
#include <shumate/shumate-viewport.h>

namespace ui::widgets {
LightningMap::LightningMap() : map(shumate_simple_map_new()) {
    prep_widget();
    backend::lightning::LightningHelper* lightningHelper = backend::lightning::get_instance();
    assert(lightningHelper);

    lightningHelper->newLightningsEventHandler.append([this](const std::vector<backend::lightning::Lightning>& lightnings) { this->on_new_lightnings(lightnings); });

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &LightningMap::on_tick), 100);
}

LightningMap::~LightningMap() {
    backend::lightning::get_instance()->stop();
}

void LightningMap::prep_widget() {
    const backend::storage::Settings* settings = backend::storage::get_settings_instance();

    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
    GtkWidget* mapWidget = GTK_WIDGET(map);
    gtk_widget_set_hexpand(mapWidget, true);
    gtk_box_append(gobj(), mapWidget);
    ShumateMapSourceRegistry* registry = shumate_map_source_registry_new_with_defaults();
    // Based on the tutorial from: https://www.jwestman.net/2022/01/29/embedded-maps-with-shumate-simple-map.html
    ShumateMapSource* source = shumate_map_source_registry_get_by_id(registry, SHUMATE_MAP_SOURCE_OSM_MAPNIK);
    shumate_simple_map_set_map_source(map, source);

    // Add home marker:
    ShumateViewport* viewPort = shumate_simple_map_get_viewport(map);
    markerLayer = shumate_marker_layer_new(viewPort);
    shumate_simple_map_add_overlay_layer(map, SHUMATE_LAYER(markerLayer));

    homeMarker = shumate_marker_new();
    ShumateLocation* markerLocation = SHUMATE_LOCATION(homeMarker);
    shumate_location_set_location(markerLocation, settings->data.lightningMapHomeLat, settings->data.lightningMapHomeLong);
    shumate_marker_layer_add_marker(markerLayer, homeMarker);

    homeMarkerImage.set_from_icon_name("lightning-map-marker-symbolic");
    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
    shumate_marker_set_child(homeMarker, GTK_WIDGET(homeMarkerImage.gobj()));

    // Zoom to marker:
    shumate_location_set_location(SHUMATE_LOCATION(viewPort), settings->data.lightningMapCenterLat, settings->data.lightningMapCenterLong);
    shumate_viewport_set_zoom_level(viewPort, settings->data.lightningMapZoomLevel);
}

void LightningMap::set_is_being_displayed(bool isBeingDisplayed) {
    backend::lightning::LightningHelper* lightningHelper = backend::lightning::get_instance();
    assert(lightningHelper);
    if (isBeingDisplayed) {
        lightningHelper->start();
    } else {
        lightningHelper->stop();
        lightningMarkersMutex.lock();
        for (LightningWidget& widget : lightningMarkers) {
            widget.remove();
        }
        lightningMarkers.clear();
        lightningMarkersMutex.unlock();
    }
}

//-----------------------------Events:-----------------------------
void LightningMap::on_new_lightnings(const std::vector<backend::lightning::Lightning>& lightnings) {
    lightningMarkersMutex.lock();
    for (const backend::lightning::Lightning& lightning : lightnings) {
        LightningWidget widget(lightning, markerLayer);
        lightningMarkers.push_back(std::move(widget));
    }
    lightningMarkersMutex.unlock();
}

bool LightningMap::on_tick() {
    lightningMarkersMutex.lock();
    std::list<LightningWidget>::iterator marker = lightningMarkers.begin();
    while (marker != lightningMarkers.end()) {
        std::chrono::seconds secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - marker->get_lightning().time);
        if (secondsElapsed < std::chrono::minutes(1)) {
            marker->update();
            marker++;
        } else {
            lightningMarkers.erase(marker++);
            marker->remove();
        }
    }
    lightningMarkersMutex.unlock();
    return true;
}

}  // namespace ui::widgets