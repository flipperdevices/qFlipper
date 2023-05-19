FROM ubuntu:20.04

WORKDIR /project

RUN DEBIAN_FRONTEND=noninteractive apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    software-properties-common \
    wget \
    file \
    pkg-config \
    git \
    curl \
    libfuse2 \
    zlib1g-dev \
    libusb-1.0-0-dev \
    ca-certificates \
    build-essential \
    libgl-dev \
    libxkbcommon-x11-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-sync-dev \
    libdbus-1-dev \
    libmd4c-dev \
    libegl-dev \
    libxcb-xfixes0-dev \
    libsm-dev \
    libice-dev \
    libxcb-glx0-dev \
    libdrm-dev \
    libx11-xcb-dev \
    libopengl-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN echo 'deb http://download.opensuse.org/repositories/home:drunkbatya/xUbuntu_20.04/ /' \
    | tee /etc/apt/sources.list.d/home:drunkbatya.list
RUN curl -fsSL https://download.opensuse.org/repositories/home:drunkbatya/xUbuntu_20.04/Release.key \
    | gpg --dearmor | tee /etc/apt/trusted.gpg.d/home_drunkbatya.gpg > /dev/null

RUN DEBIAN_FRONTEND=noninteractive apt update
RUN DEBIAN_FRONTEND=noninteractive apt -y install \
    qtbase6-static \
    qtwayland6-static \
    qt3d6-static \
    qtdeclarative6-static \
    qtserialport6-static \
    qtshadertools6-static \
    qtsvg6-static \
    qttools6-static \
    qt5compat6-static

RUN wget https://update.flipperzero.one/builds/misc/linuxdeploy-x86_64-29092022.AppImage \
    -O /usr/bin/linuxdeploy && chmod +x /usr/bin/linuxdeploy

RUN wget https://update.flipperzero.one/builds/misc/linuxdeploy-plugin-qt-x86_64-29092022.AppImage \
    -O /usr/bin/linuxdeploy-plugin-qt && chmod +x /usr/bin/linuxdeploy-plugin-qt

RUN git config --global --replace-all safe.directory '*'

ENV QT_BASE_DIR=/opt/qt6.4-static

ENV PATH="${QT_BASE_DIR}/bin:${PATH}"

COPY init.sh /init.sh

ENTRYPOINT ["/init.sh"]
