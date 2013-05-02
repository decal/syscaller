#!/bin/bash
# ps.bash by Derek Callaway <decal %at% ethernet %org%>
# Sun Sep  5 15:37:05 EDT 2010 DC/SO

alias uname='cat /proc/version' hostname='cat /proc/sys/kernel/hostname'
alias domainname='cat /proc/sys/kernel/domainname' vim='vi'

function uptime() {
  declare loadavg=$(cat /proc/loadavg | cut -d' ' -f1-3)
  let uptime=$(($(awk 'BEGIN {FS="."} {print $1}' /proc/uptime) / 60 / 60 / 24 ))
  echo "up $uptime day(s), load average: $loadavg"
}

function ps() {
    local file base pid state ppid uid
    echo 'S USER     UID   PID  PPID CMD'
    for file in /proc/[0-9]*/status
        do base=${file%/status} pid=${base#/proc/}

        { read _ st _; read _ ppid; read _ _ _ _ uid; } < <(egrep '^(State|PPid|Uid):' "$file")
        IFS=':' read user _ < <(getent passwd $uid) || user=$uid
        printf "%1s %-6s %5d %5d %5d %s\n" $st $user $uid $pid $ppid "$(tr \ \ <"$base/cmdline")"
    done
}

#EOF#
