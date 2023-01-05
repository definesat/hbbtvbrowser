#!/bin/sh
# this script launches qtsimplebrowser
#source check_standalone.sh

export QT_QPA_EGLFS_DEPTH=32
export FONTCONFIG_FILE=/usr/local/etc/fonts.conf
export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/bin/qt/plugins/
export QML2_IMPORT_PATH=/usr/local/bin/qt/qml
export LD_LIBRARY_PATH=/usr/local/lib

NUMARGS=$#
ARGS=""
for ((i=1 ; i <= NUMARGS ; i++))
do
    ARGS=$ARGS" ""$1"
    shift
done
echo "Options: "$ARGS 

exec /usr/local/bin/qt/qt-apps/qtsimplebrowser/qtsimplebrowser -platform eglfs -plugin EvdevMouse:/dev/event0 -plugin EvdevKeyboard:/dev/event1 $ARGS
