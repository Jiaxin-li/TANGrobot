# CMake generated Testfile for 
# Source directory: /home/msv/msv/sources/OpenDaVINCI-msv/apps/lanedetector
# Build directory: /home/msv/msv/sources/OpenDaVINCI-msv/bin/apps/lanedetector
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(LaneDetectorTestSuite.h-TestSuite "/home/msv/msv/sources/OpenDaVINCI-msv/bin/apps/lanedetector/LaneDetectorTestSuite.h-TestSuite")
SET_TESTS_PROPERTIES(LaneDetectorTestSuite.h-TestSuite PROPERTIES  TIMEOUT "1800")
ADD_TEST(LaneDetectorReplayTestsuite.h-TestSuite "/home/msv/msv/sources/OpenDaVINCI-msv/bin/apps/lanedetector/LaneDetectorReplayTestsuite.h-TestSuite")
SET_TESTS_PROPERTIES(LaneDetectorReplayTestsuite.h-TestSuite PROPERTIES  TIMEOUT "1800")
