// License: Apache 2.0. See LICENSE file in root directory.
// Copyright 2018 Intel Corporation. All Rights Reserved.

#include "task/DwaController.hpp"

DwaController::DwaController(const std::string & name, Robot * robot)
: ControlTask(name, robot)
{
  RCLCPP_INFO(get_logger(), "DwaController::DwaController");
}

DwaController::~DwaController()
{
  RCLCPP_INFO(get_logger(), "DwaController::~DwaController");
}

void
DwaController::executePlan()
{
  RCLCPP_INFO(get_logger(), "DwaController::executePlan");
}

void
DwaController::workerThread()
{
  RCLCPP_INFO(get_logger(), "DwaController::workerThread");
}