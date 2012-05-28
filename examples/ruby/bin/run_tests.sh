#! /bin/sh
#
# Runs the Ruby naive fingerprinting tests on a single directory 
# which gets mounted on /mnt/tmp. The mounting makes sure the disk cache 
# is empty before starting. You may need root do do this, or add 
#
#   WHEEL   ALL=/bin/bash
#   WHEEL   ALL= NOPASSWD: /bin/mount, /bin/umount
#
# to sudoers (visudo command). You may want to remove it afterwards.
#
# Run the tests twice on each system, with and without resource contention.  A
# resource contention test is, for example, to run md5sum on a separate
# directory in parallel "find $path -type f -print0 | xargs -0 md5sum" 
#
# Usage:
#
#   run_tests.sh /export/music &> host.log
#
# timins without context:
#
#   run_tests.sh -q /export/music &> host.log
# 
# (to capture the timings you need to redirect stderr)

dir=$1
if [ "$dir" = "-q" ]; then
  shift
  dir=$1
else
  verbose=1
  echo "verbose mode"
fi
if [ -z $dir ] ; then
  dir=/export/music
fi

function do_show() {
  if [ ! -z $verbose ]; then
    echo $*
  fi
}

function fetch_hostinfo() {
  if [ ! -z $verbose ]; then
    do_show Running tests on `hostname`
    do_show ---
    dmesg|egrep -i ata\|ide\|scsi
    cat /proc/cpuinfo
    free
    df
    mount
  fi
}

function empty_cache() {
  do_show "(Emptying cache)"
  # Empty disk cache
  sudo bash -c "echo 1 > /proc/sys/vm/drop_caches"
  sync
  sudo bash -c "echo 3 > /proc/sys/vm/drop_caches"
  sudo umount /mnt/tmp/ ; sudo mount --bind $dir /mnt/tmp/
  if [ ! -z $verbose ]; then
    du -sh /mnt/tmp/
  fi
  do_show "(Cache empty)"
}

function run() {
  do_show $*

  if [ -z $verbose ]; then
    env TIME="\n%e\n%U\n%S" /usr/bin/time /bin/bash -c "$*" > /dev/null
  else
    time /bin/bash -c "$*" > /dev/null
  fi
}

do_show $dir
fetch_hostinfo

empty_cache
do_show ---
do_show Test1a: Only read file sampling positions w. empty cache
empty_cache
run ./naive-fp.rb --positions /mnt/tmp
do_show ---
do_show Test1b: Only read file sampling positions w. cache
run ./naive-fp.rb --positions /mnt/tmp

do_show ---
do_show Test2a: File sampling w. empty cache
empty_cache
run ./naive-fp.rb /mnt/tmp 
do_show ---
do_show Test2b: File sampling w. cache
run ./naive-fp.rb /mnt/tmp 

do_show ---
do_show Test3a: Dense file sampling w. empty cache
empty_cache
run ./naive-fp.rb --samples 20 /mnt/tmp 
do_show ---
do_show Test3b: Dense file sampling w. cache
run ./naive-fp.rb --samples 20 /mnt/tmp 

do_show ---
do_show Test4a: Full file sampling w. empty cache
empty_cache
run ./naive-fp.rb --full /mnt/tmp 
do_show ---
do_show Test4b: Full file sampling w. cache
run ./naive-fp.rb --full /mnt/tmp 

do_show ---
do_show Test5a: MD5 w. empty cache
empty_cache
run "/usr/bin/find /mnt/tmp -type f -print0 | xargs -0 md5sum"
do_show ---
do_show Test5b: MD5 w. cache
run "/usr/bin/find /mnt/tmp -type f -print0 | xargs -0 md5sum" 

do_show -n Number of files:
find /mnt/tmp/ -type f |wc -l
echo ""
do_show -n Total:
du -sh /mnt/tmp/|awk '{ print $1 }'
if [ -z $verbose ]; then
  du -bs /mnt/tmp |awk '{ print $1 }'
fi

