#!/bin/bash

echo "Monitorando alterações em arquivos *.c..."
last_hash=""

while true; do
    new_hash=$(cat *.c 2>/dev/null | md5sum)

    if [ "$new_hash" != "$last_hash" ]; then
        clear

        echo "Alterações detectadas, recompilando..."

        make && ./build/game

        last_hash=$new_hash
    fi

    sleep 1
done
