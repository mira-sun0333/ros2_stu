cmake_minimum_required(VERSION 3.8)
project(fdilink_ahrs)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(serial REQUIRED)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIR}
)

## crc_table library
add_library(crc_table src/crc_table.cpp)
target_include_directories(crc_table PUBLIC include)

## ahrs_driver node
add_executable(ahrs_driver src/ahrs_driver.cpp)
target_include_directories(ahrs_driver PUBLIC include)
ament_target_dependencies(ahrs_driver
  rclcpp
  sensor_msgs
  geometry_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  serial
)
target_link_libraries(ahrs_driver crc_table)
target_include_directories(ahrs_driver PUBLIC ${EIGEN3_INCLUDE_DIR})

## imu_tf node
add_executable(imu_tf src/imu_tf.cpp)
target_include_directories(imu_tf PUBLIC include)
ament_target_dependencies(imu_tf
  rclcpp
  sensor_msgs
  tf2
  tf2_ros
)

install(TARGETS ahrs_driver imu_tf crc_table
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
