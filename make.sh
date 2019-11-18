#!/bin/bash


USBPort=$( dmesg| grep ch341 | tail -1 | cut -d " " -f 10 )
if [ "${USBPort:0:3}" != "tty" ] ; then 
  USBPort=$( dmesg| grep ch341 | tail -1 | cut -d " " -f 9 )
fi

if [ "${USBPort:0:3}" != "tty" ] ; then 
  USBPort=""
fi

TEMP_FOLDER=/tmp/arduino_build_make

if [ ! -d $TEMP_FOLDER ] ; then
  mkdir $TEMP_FOLDER
fi

function connectBoard {
  minicom -b 115200 -D /dev/$USBPort
}

function usage {
  echo "usage: make [options] "
  echo "       options:"
  echo "         build : only build process is triggered"
  echo "         upload: build and upload "
  echo "         connect: build and upload and start minicom"
  echo "         all: build and upload and start minicom"
  echo "         clean: cleanup all temp files"
}

DO_BUILD=1
DO_UPLOAD=2
DO_CONNECT=3
DO_CLEAN=10

DOIT=0

if [ $# -eq 0 ] ; then
  echo "build and upload sketch"
  DOIT=3
elif [ $# -eq 1 ] ; then
  if [ "$1" = "all" ] ; then	
    echo "build and upload sketch and connect console"
    DOIT=3
  fi
  if [ "$1" = "connect" ] ; then	
    echo "connect to arduino"
    DOIT=9
  fi
  if [ "$1" = "upload" ] ; then	
    echo "build and upload sketch"
    DOIT=2
  fi
  if [ "$1" = "build" ] ; then	
    echo "build sketch"
    DOIT=1
  fi
  if [ "$1" = "clean" ] ; then	
    echo "cleanup temporary files"
    DOIT=10
  fi
fi


if [ $DOIT -eq 0 ] ; then
    echo "do nothing, not valid option given"
    exit -1
fi

if [ $DOIT -eq 9 ] ; then
    echo "connect to USB port: $USBPort"
    connectBoard
    exit 0
fi

if [ $DOIT -eq 10 ] ; then
    echo "cleanup of temporary folders"
    rm -rf $TEMP_FOLDER
    rm -f *.ino.elf *.ino.bin *.ino.eep *.ino.hex
    exit 0
fi

if [ $DOIT -gt 1 -a  -z "$USBPort"  ] ; then
    echo "USB Port not connected"
    exit -1
else 
    echo "using USB Port: $USBPort"
fi


if [ $DOIT -gt 0 ] ; then
# /usr/share/arduino/arduino-builder -dump-prefs -logger=machine -hardware /usr/share/arduino/hardware -hardware /home/stransky/.arduino15/packages -tools /usr/share/arduino/tools-builder -tools /home/stransky/.arduino15/packages -built-in-libraries /usr/share/arduino/libraries -libraries /home/stransky/Links/stransky/dev/arduino/libraries -fqbn=esp8266:esp8266:nodemcuv2:xtal=80,vt=flash,exception=disabled,ssl=all,eesz=4M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200 -ide-version=10805 -build-path /tmp/arduino_build_422359 -warnings=default -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.python.path=/home/stransky/.arduino15/packages/esp8266/tools/python/3.7.2-post1 -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/stransky/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-3-20ed2b9 -prefs=runtime.tools.mkspiffs.path=/home/stransky/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-3-20ed2b9 -verbose /home/stransky/Links/stransky/dev/arduino/esp8266/KlebeTester/KlebeTester.ino
/usr/share/arduino/arduino-builder -compile -hardware /usr/share/arduino/hardware -hardware /home/stransky/.arduino15/packages -tools /usr/share/arduino/tools-builder -tools /home/stransky/.arduino15/packages -built-in-libraries /usr/share/arduino/libraries -libraries /home/stransky/Links/stransky/dev/arduino/libraries -fqbn=esp8266:esp8266:nodemcuv2:xtal=80,vt=flash,exception=disabled,ssl=all,eesz=4M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200 -build-path $TEMP_FOLDER -warnings=default -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.python.path=/home/stransky/.arduino15/packages/esp8266/tools/python/3.7.2-post1 -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/stransky/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-3-20ed2b9 -prefs=runtime.tools.mkspiffs.path=/home/stransky/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-3-20ed2b9 ./*.ino
  if [ $? -ne 0 ] ; then
    echo "result : $?"
    exit -1;
  fi
fi

if [ $DOIT -gt 1 ] ; then

/home/stransky/.arduino15/packages/esp8266/tools/python/3.7.2-post1/python /home/stransky/.arduino15/packages/esp8266/hardware/esp8266/2.5.2/tools/upload.py --chip esp8266 --port /dev/$USBPort --baud 115200 --trace version --end --chip esp8266 --port /dev/$USBPort --baud 115200  write_flash 0x0 $TEMP_FOLDER/*.ino.bin --end
  if [ $? -ne 0 ] ; then
    echo "result : $?"
    exit -1;
  fi
sleep 2

fi
if [ $DOIT -gt 2 ] ; then
  connectBoard
fi
