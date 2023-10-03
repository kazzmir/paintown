FROM ubuntu:20.04

# bash as default
SHELL ["/bin/bash", "-c"]

# Common
RUN apt update -y && apt install -y cmake python2-dev python3-dev vim-tiny git make g++

# install
ADD fn-ubuntu.sh /
RUN source /fn-ubuntu.sh && install_dependencies

# run
CMD make build-cmake
