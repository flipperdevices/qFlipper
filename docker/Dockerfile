FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    wget \
    file \
    pkg-config \
    libfuse2 \
    libusb-1.0-0-dev \
    ca-certificates \
    build-essential \
    qttools5-dev-tools \
    qt5-default \
    qtdeclarative5-dev \
    libqt5serialport5-dev \
    qml-module-qtquick2 \
    qml-module-qtquick-extras \
    qml-module-qtquick-controls \
    qml-module-qtquick-controls2 \
    qml-module-qtquick-dialogs \
    qml-module-qtquick-layouts \
    qml-module-qtquick-window2 \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O linuxdeploy \
    && chmod +x linuxdeploy && ln -s "${PWD}/linuxdeploy" /usr/bin/linuxdeploy

RUN wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O linuxdeploy-plugin-qt \
    && chmod +x linuxdeploy-plugin-qt && ln -s "${PWD}/linuxdeploy-plugin-qt" /usr/bin/linuxdeploy-plugin-qt

COPY init.sh /init.sh

ENTRYPOINT ["/init.sh"]
