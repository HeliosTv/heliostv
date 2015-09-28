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

### Channel Line up


### The daemon
> Important : You need to install the HeliosTv GStreamer plugins to start the
> daemon

Start the daemon:

    heliostv 31105 31106

You can then start a client. A sample client using gst-launch is available in
the example directory :

    ./example/heliostv-play Channel1

Where Channel1 is the channel number to play.

## Getting Into

One of the aim of HeliosTv is to be able to use it in clients written in any
languages. A [GStreamer source element](https://github.com/HeliosTv/gst-plugins-heliostv)
is available for GStreamer clients. The heliostv client libray is available
for C++ clients that do not use GStreamer. Finally one can write its own client
library based on the HelioTv [protocol](doc/protocol.md).

