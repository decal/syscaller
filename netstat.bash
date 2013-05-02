#!/bin/bash
# netstat.bash by Derek Callaway <decal %at% ethernet %org%>
# Sun Feb 14 15:56:26 EST 2010 DC/SO

function netstat()
{
echo 'Active Internet connections (w/o servers)'
echo -e 'Proto\tLocal Addr\t\tForeign Addr'

while read -r sl la ra st tx rx tr tm rn smt uid
do if [ $sl == 'sl' ];then continue;fi

l1=${la:0:2}&&l2=${la:2:2}&&l3=${la:4:2}&&l4=${la:6:2}&&li=${la:10:4}
r1=${ra:0:2}&&r2=${ra:2:2}&&r3=${ra:4:2}&&r4=${ra:6:2}&&ri=${ra:10:4}
fmt="tcp\t%u.%u.%u.%u:%u\t%u.%u.%u.%u:%u\n"

if [ $r1 == '00' ];then fmt="tcp\t%u.%u.%u.%u:%u\t\t%u.%u.%u.%u:%u\n";fi

printf $fmt 0x$l4 0x$l3 0x$l2 0x$l1 0x$li 0x$r4 0x$r3 0x$r2 0x$r1 0x$ri
done < /proc/net/tcp
# Replace with /proc/net/udp to view UDP info
