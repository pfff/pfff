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
# (to capture the timings you need to redirect stderr)

dir=$1
if [ -z $dir ] ; then
  dir=/export/music
fi

function fetch_hostinfo {
  echo Running tests on `hostname`
  echo ---
  dmesg|egrep -i ata\|ide\|scsi
  cat /proc/cpuinfo
  free
  df
  mount
}

function empty_cache() {
  echo "(Emptying cache)"
  # Empty disk cache
  sudo bash -c "echo 1 > /proc/sys/vm/drop_caches"
  sync
  sudo bash -c "echo 3 > /proc/sys/vm/drop_caches"
  sudo umount /mnt/tmp/ ; sudo mount --bind $dir /mnt/tmp/
  du -sh /mnt/tmp/
  echo "(Cache empty)"
}

function run() {
  echo $*
  time /bin/bash -c "$*" > /dev/null
}

fetch_hostinfo

empty_cache
echo -n Number of files:
find /mnt/tmp/ -type f |wc -l
echo ---
echo Test1a: Only read file sampling positions w. empty cache
empty_cache
run ./naive-fp.rb --positions /mnt/tmp
echo ---
echo Test1b: Only read file sampling positions w. cache
run ./naive-fp.rb --positions /mnt/tmp

echo ---
echo Test2a: File sampling w. empty cache
empty_cache
run ./naive-fp.rb /mnt/tmp 
echo ---
echo Test2b: File sampling w. cache
run ./naive-fp.rb /mnt/tmp 

echo ---
echo Test3a: Dense file sampling w. empty cache
empty_cache
run ./naive-fp.rb --samples 20 /mnt/tmp 
echo ---
echo Test3b: Dense file sampling w. cache
run ./naive-fp.rb --samples 20 /mnt/tmp 

echo ---
echo Test4a: Full file sampling w. empty cache
empty_cache
run ./naive-fp.rb --full /mnt/tmp 
echo ---
echo Test4b: Full file sampling w. cache
run ./naive-fp.rb --full /mnt/tmp 

echo ---
echo Test5a: MD5 w. empty cache
empty_cache
run "/usr/bin/find /mnt/tmp -type f -print0 | xargs -0 md5sum"
echo ---
echo Test5b: MD5 w. cache
run "/usr/bin/find /mnt/tmp -type f -print0 | xargs -0 md5sum" 



