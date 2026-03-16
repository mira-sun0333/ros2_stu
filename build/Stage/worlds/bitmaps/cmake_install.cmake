# Install script for directory: /home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/abc/ROS2_SIM_WS/install/Stage")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stage/worlds/bitmaps" TYPE FILE FILES
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/889_05.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/SFU_1200x615.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/SRI-AIC-kwing.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/autolab.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/cave.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/cave_compact.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/cave_filled.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/frieburg.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/ghost.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/hospital.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/hospital_section.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/human_outline.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/mbicp.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/rink.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/sal2.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/simple_rooms.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/space_invader.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/submarine.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/submarine_small.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/table.png"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/bitmaps/uoa_robotics_lab.png"
    )
endif()

