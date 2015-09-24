# HeliosTv

HeliosTv is a DVB streaming server. it can be used to receive live content or
DVB section from DVB tuners.

## Building

HeliosTv depends on the following libraries:

- boost
- msgpack
- gstreamer 1.x

Run the following command to build the server:

    ./autogen.sh
    ./configure
    make
    make install

## Getting Started

## Getting Into

One of the aim of HeliosTv is to be able to use it in clients written in any
languages. A [GStreamer source element](https://github.com/HeliosTv/gst-plugins-heliostv)
is available for GStreamer clients. The heliostv client libray is available
for C++ clients that do not use GStreamer. Finally one can write its own client
library based on the HelioTv [protocol](doc/protocol.md).

