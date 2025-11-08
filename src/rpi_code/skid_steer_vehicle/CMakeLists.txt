cmake_minimum_required(VERSION 3.8)
project(skid_steer_vehicle)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(can_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(state_manager src/state_manager.cpp)
ament_target_dependencies(state_manager rclcpp std_msgs sensor_msgs)

add_executable(topic_to_frame src/topic_to_frame.cpp)
ament_target_dependencies(topic_to_frame rclcpp std_msgs can_msgs)

add_executable(frame_to_topic src/frame_to_topic.cpp)
ament_target_dependencies(frame_to_topic rclcpp std_msgs can_msgs)

add_executable(tcp_sender src/tcp_sender.cpp)
ament_target_dependencies(tcp_sender rclcpp std_msgs)

install(TARGETS
	state_manager
	topic_to_frame
	frame_to_topic
	tcp_sender
	DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY
	launch
	DESTINATION share/${PROJECT_NAME})

target_include_directories(state_manager PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_include_directories(tcp_sender PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
