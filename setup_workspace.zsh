#!/usr/bin/env zsh
# ROS 2 Workspace Setup Script for Zsh

WORKSPACE_DIR="/home/iammira/ws_capa"
ROS_DISTRO="/opt/ros/humble"

# 清除所有可能干扰的环境变量
unset AMENT_CURRENT_PREFIX COLCON_CURRENT_PREFIX AMENT_PREFIX_PATH COLCON_PREFIX_PATH

# 显式设置 AMENT_CURRENT_PREFIX
export AMENT_CURRENT_PREFIX="$ROS_DISTRO"

# Source ROS 2（使用 zsh 版本）
source "$ROS_DISTRO/setup.zsh"

# 清除可能再次被设置的环境变量
unset AMENT_CURRENT_PREFIX COLCON_CURRENT_PREFIX

# Source workspace（使用 zsh 版本）
source "$WORKSPACE_DIR/install/local_setup.zsh"

# 清除环境变量
unset AMENT_CURRENT_PREFIX COLCON_CURRENT_PREFIX
