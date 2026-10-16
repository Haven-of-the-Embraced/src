#!/bin/sh
set -eu

for directory in area data player; do
    if [ ! -d "../$directory" ]; then
        echo "Missing $directory directory. Mount your local runtime files before starting Haven." >&2
        exit 1
    fi
done

for file in ../data/area.lst ../data/help.dat; do
    if [ ! -s "$file" ]; then
        echo "Missing or empty $file. Supply a compatible Haven data bundle; see README.md." >&2
        exit 1
    fi
done

mkdir -p ../player/bk ../log ../gods ../memdump ../import
exec ./rom "$@"