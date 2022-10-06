# Base on Fedora 36
FROM fedora:36
# Make sure all sources are up to date
RUN dnf update -y && \
    dnf install -y \ 
    gtkmm4.0-devel libadwaita-devel libcurl-devel g++ clang cmake git openssl-devel \
    libsoup3-devel sqlite-devel ninja-build \
    gdb make libasan libubsan liblsan libtsan \
    flatpak flatpak-builder meson \
    --nodocs --setopt install_weak_deps=False && \
    sudo dnf install -y renderdoc-devel && \
    dnf clean all -y