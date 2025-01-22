# ofxLibcamera

Complex camera support library for Linux, Android and ChromeOS

[libcamera](https://libcamera.org/) [source](https://git.linuxtv.org/libcamera.git/) 


## Install

Use build instructions libCamera and pop this addon in your addons folder:

    apt install meson ninja-build pkg-config libyaml-dev python3-yaml python3-ply python3-jinja2 libudev-dev
    git clone https://git.libcamera.org/libcamera/libcamera.git
    cd libcamera
    meson setup build
    ninja -C build install

