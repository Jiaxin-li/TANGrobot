# Install script for directory: /home/li/workspace/msv/tangyuanrobot/hesperia

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Scenarios" TYPE FILE FILES
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/ObstaclesAtIntersections_StopLines.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/ReferenceAxes.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/NoObstacles_StopLines.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/ObstacleOnStraightRoad.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/MissingOuterLaneMarking.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/MissingInnerLaneMarking.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/Parking-sideways.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/ObstacleInFirstCurve.scnx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/NoObstacles_NoStopLines.scnx"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Scenarios/Models" TYPE FILE FILES
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/Models/RedRocketCar.objx"
    "/home/li/workspace/msv/tangyuanrobot/hesperia/Scenarios/Models/FordEscape.objx"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/libhesperia/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/camgen/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/cockpit/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/chasecar/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/drivenpath/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/egocontroller/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/irus/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/rec2stdout/cmake_install.cmake")
  INCLUDE("/home/li/workspace/msv/tangyuanrobot/hesperia/vehicle/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

