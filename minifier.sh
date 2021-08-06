#!/bin/sh

# $1 => Dossier contenant les fichiers sources (PHP)
if [[ $# -lt 1 ]]; then
    echo "Error: IN_DIR not given"
    exit 1
fi

SAVED_BYTES=0
IN_DIR=$1

# Build parser
make build

for PHP_FILE in `find $IN_DIR -name '*.php' -not -path '**/_dev/*'`; do
    SIZE=`cat $PHP_FILE | wc -c`
    ./parser "$PHP_FILE" $SIZE
    SAVED=$((SIZE - `cat $PHP_FILE | wc -c`))
    SAVED_BYTES=$((SAVED_BYTES + SAVED))
    echo "-- $PHP_FILE: saved $SAVED bytes"
done

echo "- $IN_DIR: Saved $SAVED_BYTES bytes"
