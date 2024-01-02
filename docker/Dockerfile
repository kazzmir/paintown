FROM ubuntu:22.04

RUN mkdir /build

COPY src /build/src
COPY meson.build /build
COPY easy-compile-ubuntu /build
COPY Makefile /build

RUN apt update && apt install -y sudo

RUN cd /build && ./easy-compile-ubuntu

CMD [ "/bin/bash" ]
