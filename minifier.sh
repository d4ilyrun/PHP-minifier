#!/bin/sh

# $1 => Dossier contenant les fichiers sources (PHP)
if [[ $# -lt 1 ]]; then
    echo "Error: IN_DIR not given"
    exit 1
fi

START_SIZE=`du -hsb | cut -f1`

for PHP_FILE in `find $IN_DIR -name "*.php" -not -path "**/_dev/*"`; do
    SIZE=`du -hsb $PHP_FILE | cut -f1`
    MINIFIED=`php -w $PHP_FILE`
    echo $MINIFIED > $PHP_FILE
    SIZE=$(( SIZE - `du -hsb $PHP_FILE | cut -f1`))
    echo "-- $PHP_FILE: saved $SIZE bytes"
done

END_SIZE=`du -hsb | cut -f1`
SAVED_BYTES=$((START_SIZE - END_SIZE))

echo "- $IN_DIR/: Saved $SAVED_BYTES bytes"
exit 0

##

SCRIPT=`realpath -s $0`
IN_DIR=$1


# traite recursivement les dossiers contenu à la location
for DIR in `find $IN_DIR -maxdepth 1 -type d`; do
    if [[ `basename $DIR` != "_dev" ]] && [[ $DIR != $IN_DIR ]]; then
        # ignore le dossier '_dev' et la recursion infinie
        sh $SCRIPT $DIR
    fi
done

# Créée une version minimisée et calcule les bytes économisés
for PHP_FILE in `find $IN_DIR -maxdepth 1 -type f`; do
    # remplace le fichier php par sa version minimisée
    # `php -w` ne peut pas etre redirigé vers le fichier source
    MINIFIED=`php -w $PHP_FILE`
    echo $MINIFIED > $PHP_FILE
done
