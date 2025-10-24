#!/usr/bin/bash

set -x
TARGET_DIR=${HOME}/.local/

tag=$(curl https://github.com/LunarWatcher/umbra/releases/latest -sv 2>&1 | grep location | sed -e "s|^.*/tag/||" | tr -d "\r\n")
if [ $? != 0 ]; then
    echo "Failed to get latest tag from github"
    exit -1
fi

raw_arch=$(uname -m)
if [[ "$raw_arch" == "x86_64" ]]; then
    arch="x64"
else 
    echo "$raw_arch does not have pre-compiled binaries :("
    exit -1
fi


wget "https://github.com/LunarWatcher/Umbra/releases/download/$tag/umbra-linux-$arch.tar.gz" -O /tmp/umbra.tar.gz
tar -xvf /tmp/umbra.tar.gz -C $TARGET_DIR --strip-components 1

echo "Output into $TARGET_DIR"

echo "$TARGET_DIR/bin:"
ls $TARGET_DIR/bin | grep umbra
