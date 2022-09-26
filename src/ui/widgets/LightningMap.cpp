#include "LightningMap.hpp"
#include "backend/lightning/LightningHelper.hpp"
#include "backend/storage/Settings.hpp"
#include "logger/Logger.hpp"
#include <shumate/shumate-simple-map.h>

namespace ui::widgets {
LightningMap::LightningMap() : map(shumate_simple_map_new()) {
    prep_widget();
    backend::lightning::LightningHelper* lightningHelper = backend::lightning::get_instance();
    lightningHelper->start();
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

    marker = shumate_marker_new();
    ShumateLocation* markerLocation = SHUMATE_LOCATION(marker);
    shumate_location_set_location(markerLocation, settings->data.lightningMapHomeLat, settings->data.lightningMapHomeLong);
    shumate_marker_layer_add_marker(markerLayer, marker);

    markerImage.set_from_icon_name("lightning-map-marker-symbolic");
    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-cstyle-cast)
    shumate_marker_set_child(marker, GTK_WIDGET(markerImage.gobj()));

    // Zoom to marker:
    shumate_location_set_location(SHUMATE_LOCATION(viewPort), settings->data.lightningMapCenterLat, settings->data.lightningMapCenterLong);
    shumate_viewport_set_zoom_level(viewPort, settings->data.lightningMapZoomLevel);
}

//-----------------------------Events:-----------------------------

}  // namespace ui::widgets