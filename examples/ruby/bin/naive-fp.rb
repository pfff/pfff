#! /usr/bin/ruby
#
#
# Naive version of file finger printing.
#
# You need to umount/remount to make sure nothing is cached, for example:
#
#   sudo umount /mnt/tmp/ ; sudo mount --bind /export/music/ /mnt/tmp/
#
# Running md5 recursively:
#
#   time find /mnt/tmp -type f -print0 | xargs -0 md5sum > /dev/null
#
#   real    4m2.108s
#   user    0m23.717s
#   sys     0m7.568s
#
#   real    3m48.428s
#   user    0m23.093s
#   sys     0m7.056s
#
#   28 Gb GEO - 7407 files (avg 4Mb files) 2MHz; 1Gb
#
#   real    0m42.416s
#   user    0m0.052s
#   sys     0m0.376s
#
# Running naive-fp just printing file positions (--positions) - baseline version 
#
#   time ./naive-fp.rb --positions /mnt/tmp > /dev/null
#
#   real    0m0.102s
#   user    0m0.076s
#   sys     0m0.024s
#
#   28 Gb GEO - 7407 files (avg 4Mb files) 2MHz; 1Gb
#
#   real    0m0.683s
#   user    0m0.428s
#   sys     0m0.252s
#
# Taking file samples at fixed positions (default)
#
#   time ./naive-fp.rb /mnt/tmp > /dev/null
#
#   real    0m19.953s
#   user    0m0.172s
#   sys     0m0.188s
# 
#   28 Gb GEO - 7407 files (avg 4Mb files) 2MHz; 1Gb
#
#   real    7m42.426s
#   user    0m2.644s
#   sys     0m15.381s

SAMPLENUM=5
SAMPLESIZE=5

usage = <<USAGE

    Naive finger printing routines
       
    Use --help for more information

USAGE

require 'optparse'
require 'ostruct'

$options = OpenStruct.new()

opts = OptionParser.new do |opts|
  opts.on("--positions", "Only output sampling positions") do | b |
    $options.output = :positions
  end

  opts.on_tail("-h", "--help", "Print this help") {
    print(usage)
    print(opts)
    print <<EXAMPLE
    
Examples:

EXAMPLE
    exit()
  }
end
opts.parse!(ARGV)

def recurse_dir dirname
  Dir.glob("#{dirname}/**/*").reject { |fn| File.directory? fn }.each do |fn|
    yield fn
  end
end

# Get a sample from the file +f+, at +pos+. Another concatenation of values
def get_sample f, pos
  f.pos = pos
  s = f.read(SAMPLESIZE)
  num = ''
  s.each_byte do | b |
    num += b.to_i.to_s(16)
  end
  num
end

# Calculate the 'fingerprint' for a file
def get_fp fn
  if $options.output != :positions
    f = File.open(fn,'rb')
  end
  begin
    fp = []
    filesize = File.size(fn)
    return [-1] if filesize < SAMPLENUM*SAMPLESIZE
    skipsize = filesize/(SAMPLENUM+1)
    pos = 0
    (0..SAMPLESIZE-1).each do | hop |
      pos += skipsize
      case $options.output
        when :positions
          fp.push pos
        else
          fp.push get_sample(f,pos)
      end
    end
  rescue
    raise "Error reading #{fn} at position #{pos}"
  end
  fp.map { | i | i.to_s }
end

def print_fp fn, id
  print "#{fn}\t#{id.join('-')}\n"
end

count = 0
ARGV.each do | fn |
  if File.directory?(fn)
    recurse_dir(fn) do | fn |
      id = get_fp(fn)
      print_fp(fn, id)
      count += 1
    end
  else
    id = get_fp(fn)
    print_fp(fn, id)
    count += 1
  end
end
print "\n#{count} files read!\n"
