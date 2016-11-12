#!/usr/bin/env bash 
#
# Ways to view /etc/passwd without using any spaces in the command
#

# {cat,/etc/passwd}

mapfile</etc/passwd&&echo"${MAPFILE[*]}"

exit 0
