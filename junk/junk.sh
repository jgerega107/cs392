#!/bin/bash

#Name: Jacob Gerega
#Pledge: I pledge my honor that I have abided by the Stevens Honor System.
#Date: 2/16/21

help_flag=0
junk_flag=0
purge_flag=0
flag_count=0

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
        (( ++flag_count ))
        ;;
        l) junk_flag=1
        (( ++flag_count ))
        ;;
        p) purge_flag=1
        (( ++flag_count ))
        ;;
        ?) 
        printf "Error: Unknown option '-%s'.\n" $OPTARG
        cat <<HERE
Usage: $base [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
HERE
        exit 1
        ;;
    esac
done

if [ ! -d "$junk" ]; then
    mkdir "$junk"
fi

if [ $flag_count -eq 0 ]; then
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
    exit 0
elif [ $flag_count -gt 1 ]; then
    echo "Error: Too many options enabled."
    cat <<HERE
Usage: $base [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
HERE
exit 1
fi

if [ $help_flag -eq 1 ]; then
    cat <<HERE
Usage: $base [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
HERE
exit 0
elif [ $junk_flag -eq 1 ]; then
    ls -lAF "$junk"
    exit 0
elif [ $purge_flag -eq 1 ]; then
    rm -r "$junk"
    mkdir "$junk"
    exit 0
fi


