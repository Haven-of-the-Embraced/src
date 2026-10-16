#!/bin/sh
set -eu

test_name=${1:-test_vnums}
case "$test_name" in
    test_vnums|test_vicissitude) ;;
    *) echo "Unknown test suite: $test_name" >&2; exit 1 ;;
esac

mkdir -p /tmp/haven/src /tmp/haven/area /tmp/haven/data /tmp/haven/player /tmp/haven/log
cp /input-area/fane.are /tmp/haven/area/
cp /input-data/*.dat /input-data/config.cfg /input-data/music.txt /tmp/haven/data/
printf 'fane.are\n$\n' > /tmp/haven/data/area.lst
if [ "$test_name" = test_vicissitude ]; then
    cp /input-player/Testvamp /tmp/haven/player/Testvamp
fi

objcopy --redefine-sym main=haven_main /build/src/obj/comm.o /tmp/haven/comm.o
set -- /build/src/obj/*.o
objects=""
for object in "$@"; do
    if [ "$object" != /build/src/obj/comm.o ]; then
        objects="$objects $object"
    fi
done
gcc -std=gnu99 -I/build/src "/tests/$test_name.c" $objects /tmp/haven/comm.o -lcrypt -o "/tmp/haven/src/$test_name"
cd /tmp/haven/src
"./$test_name"
"./$test_name"