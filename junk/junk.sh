#!/bin/bash

help_flag=0
junk_flag=0
purge_flag=0

readonly junk="$HOME/.junk/"
base=$(basename "$0")

if [ $# -eq 0 ]; then
  cat <<HERE
Usage: $base [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
HERE
  exit 
fi

while getopts ":hlp" option; do
    case "$option" in
        h) help_flag=1
        ;;
        l) junk_flag=1
        ;;
        p) purge_flag=1
        ;;
        ?) 
        help_flag=1  
        printf "Error: Unknown option '-%s'.\n" $OPTARG 
        ;;
    esac
done

if [ ! -d "$junk" ]; then
    mkdir "$junk"
fi

if [ $junk_flag -eq 0 ] && [ $help_flag -eq 0 ] && [ $purge_flag -eq 0 ]; then
    shift "$((OPTIND-1))"
    for f in "$@"; do
        if [ -f "$f" ]; then
            mv $f $junk
        elif [ -d "$f" ]; then
            mv $f $junk
        else
            echo "Warning: '$f' not found."
        fi
    done
fi

if [ $help_flag -eq 1 ]; then
    cat <<HERE
Usage: $base [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
HERE
fi

if [ $junk_flag -eq 1 ]; then
    ls -Al "$junk"
fi

if [ $purge_flag -eq 1 ]; then
    rm -r "$junk"
    mkdir "$junk"
fi


