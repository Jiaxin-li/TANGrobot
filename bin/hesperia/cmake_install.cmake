# Install script for directory: /home/msv/msv/sources/OpenDaVINCI-msv/hesperia

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/opt/msv")
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
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/Parking-sideways.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/ReferenceAxes.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/ObstaclesAtIntersections_StopLines.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/MissingInnerLaneMarking.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/NoObstacles_StopLines.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/ObstacleOnStraightRoad.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/MissingOuterLaneMarking.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/NoObstacles_NoStopLines.scnx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/ObstacleInFirstCurve.scnx"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Scenarios/Models" TYPE FILE FILES
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/Models/FordEscape.objx"
    "/home/msv/msv/sources/OpenDaVINCI-msv/hesperia/Scenarios/Models/RedRocketCar.objx"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/libhesperia/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/camgen/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/cockpit/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/chasecar/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/drivenpath/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/egocontroller/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/irus/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/rec2stdout/cmake_install.cmake")
  INCLUDE("/home/msv/msv/sources/OpenDaVINCI-msv/bin/hesperia/vehicle/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

