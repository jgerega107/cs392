#!/bin/bash

help_flag=0
junk_flag=0
purge_flag=0

while getopts ":hlp" option; do
    case "$option" in
        h) help_flag=1
        ;;
        l) junk_flag=1
        ;;
        p) purge_flag=1
        ;;
        ?) printf "Error: Unknown option '-%s'.\n" $OPTARG       
        exit 1
        ;;
    esac
done

junk="$HOME/.junk/"
if [ ! -d "$junk" ]; then
    mkdir $junk
fi

if [ $help_flag -eq 1 ]; then
    #do help document
    echo "Help"
fi

if [ $junk_flag -eq 1 ]; then
    echo "Junk"
fi

if [ $purge_flag -eq 1 ]; then
    echo "Purge"
fi

shift "$((OPTIND-1))"
index=0
for f in "$@"; do
    if [ -f "$f" ]; then
        mv $f $junk
    else
        echo "Warning: '$f' not found."
    fi
done

