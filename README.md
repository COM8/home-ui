# home-ui
GTK3 Based UI for home automation using a Raspberry Pi

## Building
```
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
flatpak-builder --force-clean flatpak_build_dir flatpak/de.home_ui.yml
```

### Installing
```
flatpak-builder --user --install --force-clean flatpak_build_dir flatpak/de.home_ui.yml
```

### Uninstalling
```
flatpak uninstall de.home_ui
```

### Executing
```
flatpak run de.home_ui
```