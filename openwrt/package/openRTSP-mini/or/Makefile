##### Change the following for your environment:
CROSS_COMPILE=        mips-openwrt-linux-musl-
COMPILE_OPTS =        $(INCLUDES) -I. -Os -pipe -mno-branch-likely -mips32r2 -mtune=34kc -fno-caller-saves -fhonour-copts -Wno-error=unused-but-set-variable -msoft-float -mips16 -minterlink-mips16 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DLOCALE_NOT_USED
C =            c
C_COMPILER =        $(CROSS_COMPILE)gcc
CFLAGS +=        $(COMPILE_OPTS)
C_FLAGS = $(CFLAGS)
CPP =            cpp
CPLUSPLUS_COMPILER =    $(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =    $(COMPILE_OPTS) -Wall -DBSD=1
CPLUSPLUS_FLAGS += $(CPPFLAGS) -fexceptions
OBJ =            o
LINK =            $(CROSS_COMPILE)g++ -o
LINK_OPTS =        -L. $(LDFLAGS)
CONSOLE_LINK_OPTS =    $(LINK_OPTS)
LIBRARY_LINK =        $(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =    
LIB_SUFFIX =            a
LIBS_FOR_CONSOLE_APPLICATION = $(CXXLIBS)
LIBS_FOR_GUI_APPLICATION = $(LIBS_FOR_CONSOLE_APPLICATION)
EXE =
##### End of variables to change

LIVEMEDIA_DIR = src/liveMedia
GROUPSOCK_DIR = src/groupsock
USAGE_ENVIRONMENT_DIR = src/UsageEnvironment
BASIC_USAGE_ENVIRONMENT_DIR = src/BasicUsageEnvironment

TESTPROGS_DIR = src/testProgs

MEDIA_SERVER_DIR = src/mediaServer

PROXY_SERVER_DIR = src/proxyServer

all:
	cd $(LIVEMEDIA_DIR) ; $(MAKE)
	cd $(GROUPSOCK_DIR) ; $(MAKE)
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE)
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE)
	cd $(TESTPROGS_DIR) ; $(MAKE)
	cd $(MEDIA_SERVER_DIR) ; $(MAKE)
	cd $(PROXY_SERVER_DIR) ; $(MAKE)

install:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) install
	cd $(GROUPSOCK_DIR) ; $(MAKE) install
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) install
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) install
	cd $(TESTPROGS_DIR) ; $(MAKE) install
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) install
	cd $(PROXY_SERVER_DIR) ; $(MAKE) install

clean:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) clean
	cd $(GROUPSOCK_DIR) ; $(MAKE) clean
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(TESTPROGS_DIR) ; $(MAKE) clean
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) clean
	cd $(PROXY_SERVER_DIR) ; $(MAKE) clean

distclean: clean
	-rm -f $(LIVEMEDIA_DIR)/Makefile $(GROUPSOCK_DIR)/Makefile \
	  $(USAGE_ENVIRONMENT_DIR)/Makefile $(BASIC_USAGE_ENVIRONMENT_DIR)/Makefile \
	  $(TESTPROGS_DIR)/Makefile $(MEDIA_SERVER_DIR)/Makefile \
	  $(PROXY_SERVER_DIR)/Makefile Makefile
