cmake_minimum_required(VERSION 3.8)
project(error_metrics)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(rclcpp REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(Eigen3 REQUIRED)

## INCLUDE(S)
include_directories(include)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})

# executables
add_executable(error_metrics_node src/error_metrics_node.cpp)

# dependencies
ament_target_dependencies(error_metrics_node rclcpp tf2 tf2_ros)

install(TARGETS error_metrics_node
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
