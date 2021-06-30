FROM ubuntu:18.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    software-properties-common

RUN add-apt-repository ppa:beineri/opt-qt-5.12.9-bionic && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    wget \
    file \
    pkg-config \
    libfuse2 \
    libusb-1.0-0-dev \
    ca-certificates \
    build-essential \
    qt512tools \
    qt512base \
    qt5123d \
    qt512svg \
    qt512serialport \
    qt512declarative \
    qt512quickcontrols \
    qt512quickcontrols2 \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O linuxdeploy \
    && chmod +x linuxdeploy && ln -s "${PWD}/linuxdeploy" /usr/bin/linuxdeploy

RUN wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O linuxdeploy-plugin-qt \
    && chmod +x linuxdeploy-plugin-qt && ln -s "${PWD}/linuxdeploy-plugin-qt" /usr/bin/linuxdeploy-plugin-qt

RUN ln -s /usr/lib/x86_64-linux-gnu/libGL.so.1 /usr/lib/x86_64-linux-gnu/libGL.so

ENV QT_BASE_DIR=/opt/qt512
ENV QTDIR=$QT_BASE_DIR
ENV QMAKE=$QT_BASE_DIR/bin/qmake

COPY init.sh /init.sh

ENTRYPOINT ["/init.sh"]
