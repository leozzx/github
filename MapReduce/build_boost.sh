#!/bin/bash
cd `dirname $0`

BOOST_DIR="$PWD/boost"
BOOST_BUILD_DIR="$PWD/boost/build"
if [ -d ${BOOST_DIR} ];then
  exit
fi

BOOST_TAR_GZ='boost_*.tar.gz'
if [ ! -f ${BOOST_TAR_GZ} ];then
  echo "Boost file not exists"
  exit 1
fi

mkdir -p ${BOOST_DIR}
mkdir -p ${BOOST_BUILD_DIR}

tar xzf ${BOOST_TAR_GZ} -C ${BOOST_DIR}
cd ${BOOST_DIR}/boost_*

./bootstrap.sh
./b2 --disable-icu tools/bcp

dist/bin/bcp boost/crc.hpp boost/range/algorithm boost/circular_buffer.hpp boost/iostreams boost/asio.hpp boost/uuid boost/smart_ptr.hpp boost/utility.hpp boost/unordered_set.hpp boost/dynamic_bitset.hpp boost/foreach.hpp boost/unordered_map.hpp boost/property_tree boost/system/ boost/type_traits.hpp boost/config.hpp boost/algorithm/string.hpp  boost/pool boost/thread.hpp boost/thread boost/filesystem.hpp boost/serialization/ boost/date_time.hpp boost/program_options.hpp boost/spirit boost/random/ boost/regex ${BOOST_DIR}

./bjam  link=static variant=release runtime-link=static --disable-icu --with-thread threading=multi

mv stage/lib/*.a ${BOOST_BUILD_DIR}

cd ${BOOST_DIR}
rm -rf ${BOOST_DIR}/boost_*
