# ofxLibcamera

Complex camera support library for Linux, Android and ChromeOS

[libcamera](https://libcamera.org/) [source](https://git.linuxtv.org/libcamera.git/) 


## Install

    cd /OpenFrameworks/Addons/
    git clone --recurse https://github.com/ofxyz/ofxLibcamera.git
    sudo apt install libyaml-dev python3-yaml python3-ply python3-jinja2
    cd /OpenFrameworks/Addons/ofxLibcamera/libs/libcamera
    meson setup build
    ninja -C build install

   > Note: Fork

    #define YAML_VERSION_MAJOR 0
    #define YAML_VERSION_MINOR 2
    #define YAML_VERSION_PATCH 4
    #define YAML_VERSION_STRING "0.2.4"


## TODO

  [] Create build script
  [] Relink submodule to own fork (Needs fixing)
