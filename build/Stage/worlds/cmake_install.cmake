# Install script for directory: /home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stage/worlds" TYPE FILE FILES
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/amcl-sonar.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/autolab.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/camera.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/everything.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/lsp_test.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/mbicp.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/nd.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/roomba.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/simple.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/test.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/uoa_robotics_lab.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/vfh.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/wavefront-remote.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/wavefront.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/wifi.cfg"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/SFU.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/autolab.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/camera.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/circuit.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/everything.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/fasr.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/fasr2.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/fasr_plan.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/large.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/lsp_test.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/mbicp.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/pioneer_flocking.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/pioneer_follow.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/pioneer_walle.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/roomba.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/sensor_noise_demo.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/sensor_noise_module_demo.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/simple.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/uoa_robotics_lab.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/wifi.world"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/beacons.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/chatterbox.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/hokuyo.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/irobot.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/map.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/objects.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/pantilt.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/pioneer.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/sick.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/ubot.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/uoa_robotics_lab_models.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/walle.inc"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/cfggen.sh"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/test.sh"
    "/home/abc/ROS2_SIM_WS/src/sim_stage/Stage/Stage/worlds/worldgen.sh"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/abc/ROS2_SIM_WS/build/Stage/worlds/benchmark/cmake_install.cmake")
  include("/home/abc/ROS2_SIM_WS/build/Stage/worlds/bitmaps/cmake_install.cmake")
  include("/home/abc/ROS2_SIM_WS/build/Stage/worlds/wifi/cmake_install.cmake")

endif()

