FROM ubuntu:18.04

WORKDIR /project

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    software-properties-common

RUN add-apt-repository ppa:beineri/opt-qt-5.15.2-bionic && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    wget \
    file \
    pkg-config \
    git \
    libfuse2 \
    zlib1g-dev \
    libusb-1.0-0-dev \
    ca-certificates \
    build-essential \
    libgl-dev \
    qt515tools \
    qt515base \
    qt5153d \
    qt515svg \
    qt515serialport \
    qt515declarative \
    qt515quickcontrols \
    qt515quickcontrols2 \
    qt515graphicaleffects \
    qt515wayland \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O /usr/bin/linuxdeploy \
    && chmod +x /usr/bin/linuxdeploy

RUN wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O /usr/bin/linuxdeploy-plugin-qt \
    && chmod +x /usr/bin/linuxdeploy-plugin-qt

RUN git config --global --replace-all safe.directory '*'

ENV QT_BASE_DIR=/opt/qt515

ENV PATH="${QT_BASE_DIR}/bin:${PATH}"

COPY init.sh /init.sh

ENTRYPOINT ["/init.sh"]
