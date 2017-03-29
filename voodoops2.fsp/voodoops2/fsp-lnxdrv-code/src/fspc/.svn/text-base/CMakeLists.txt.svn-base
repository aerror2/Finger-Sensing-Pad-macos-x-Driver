# The configuration file for CMake(http://www.cmake.org/) tool to create
# platform/compiler-specific building environments.

# CMAKE directive.
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_VERBOSE_MAKEFILE false)

# Project-wise setting.
PROJECT (FSPC)

# Specify external packeges.

## Set wxWidgets_CONFIG_EXECUTABLE only when CMake can't find wxWidgets automatically.
# SET(wxWidgets_CONFIG_EXECUTABLE "/usr/bin/wx-config") 
# IF(DEBUG) 
#   SET(wxWidgets_USE_DEBUG 1) 
#   SET(wxWidgets_USE_UNICODE 1) 
# ELSE(DEBUG) 
#   SET(wxWidgets_USE_UNICODE 1) 
# ENDIF(DEBUG)

SET(wxWidgets_USE_LIBS base core xrc html adv)
FIND_PACKAGE(wxWidgets)

IF(!wxWidgets_FOUND)
    MESSAGE(FATAL_ERROR "wxWidgets not found!")
ENDIF(!wxWidgets_FOUND)

# Specify Compiler options.
INCLUDE(${wxWidgets_USE_FILE})
ADD_DEFINITIONS(-Wall)
IF(DEBUG)
    # ADD_DEFINITIONS(-g -DDEBUG -DDBG -D__WXDEBUG__ -DMANUAL_TEST)
    ADD_DEFINITIONS(-g -DDEBUG -DDBG -D__WXDEBUG__)
ENDIF(DEBUG)

# Source codes.
SET (FSPC_SRCS fspcApp.cpp fspcMainFrame.cpp fspcInfoFrame.cpp fspcConfig.cpp fspcCli.cpp msaccel.c)
SET (FSPC_SRCS_CLI_TEST fspcCliUnitTest.cpp fspcCli.cpp fspcConfig.cpp  msaccel.c)

# Extra including/library files.
INCLUDE_DIRECTORIES (../libfsp ../kernel-driver)
LINK_DIRECTORIES (../libfsp)

# Executable file.
ADD_EXECUTABLE (fspc ${FSPC_SRCS})
ADD_EXECUTABLE (fspcCliTest ${FSPC_SRCS_CLI_TEST})

# Specify link options and libraries.
TARGET_LINK_LIBRARIES(fspc ${wxWidgets_LIBRARIES} fsp)
TARGET_LINK_LIBRARIES(fspcCliTest ${wxWidgets_LIBRARIES} fsp)

# Files to be installed.
INSTALL_FILES( /bin FILES fspc)
INSTALL_FILES(/share/fspc FILES fspcui.xrc fspcui_zh_CN.xrc fspcui_zh_TW.xrc)
INSTALL_FILES(/share/fspc/pic FILES
	pic/Acceleration.png
	pic/HEnabled.png
	pic/Logo.png
	pic/OpClick.png
	pic/VEnabled.png
	pic/VHDisabled.png
	pic/VHEnabled.png
	pic/panelAcceleration.png
	pic/panelClickOnPad.png
	pic/panelDeclaration.png
	pic/panelScrollOnPad.png
	pic/panelSwInfo.png
)


# EOF.
