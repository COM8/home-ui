# home-ui
GTK4 Based UI for home automation using a Raspberry Pi.

## Building
```
sudo dnf install libsoup3-devel
```

## Flatpak
The home-ui can be build and installed using Flatpak.

### Requirements
#### Fedora
```
sudo dnf install flatpak flatpak-builder
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.gnome.Sdk//42 org.gnome.Platform//42
```

```
sudo dnf install sqlite-devel libsoup-devel ninja-build
git clone https://gitlab.gnome.org/GNOME/libshumate.git
cd libshumate
meson build . -Dgtk_doc=false
cd build
ninja
sudo ninja install
```

#### Debian/Ubuntu
```
sudo apt install flatpak flatpak-builder
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.gnome.Sdk//42 org.gnome.Platform//42
```

### Building
```
git clone https://github.com/COM8/home-ui.git
cd home-ui
flatpak-builder --force-clean flatpak_build_dir de.home_ui.cli.yml
```

### Installing
```
flatpak-builder --user --install --force-clean flatpak_build_dir de.home_ui.cli.yml
```

### Uninstalling
```
flatpak uninstall de.home_ui.cli
```

### Executing
```
flatpak run de.home_ui.cli
```
