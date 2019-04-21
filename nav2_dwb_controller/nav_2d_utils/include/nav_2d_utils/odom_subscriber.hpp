/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2017, Locus Robotics
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NAV_2D_UTILS__ODOM_SUBSCRIBER_HPP_
#define NAV_2D_UTILS__ODOM_SUBSCRIBER_HPP_

#include <memory>
#include <mutex>
#include <string>
#include <cctype>
#include <algorithm>
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nav_2d_msgs/msg/twist2_d_stamped.hpp"

namespace nav_2d_utils
{

enum RobotType
{
  DIFFERENTIAL,
  HOLONOMIC
};

/**
 * @class OdomSubscriber
 * Wrapper for some common odometry operations. Subscribes to the topic with a mutex.
 */
class OdomSubscriber
{
public:
  /**
   * @brief Constructor that subscribes to an Odometry topic
   *
   * @param nh NodeHandle for creating subscriber
   * @param default_topic Name of the topic that will be loaded of the odom_topic param is not set.
   */
  explicit OdomSubscriber(rclcpp::Node & nh, std::string default_topic = "odom")
  {
    std::string odom_topic;
    bool robot_type;
    nh.get_parameter_or("odom_topic", odom_topic, default_topic);
    nh.get_parameter_or("holonomic_robot", robot_type, true);
    robot_type_ = robot_type ? HOLONOMIC : DIFFERENTIAL;
    odom_sub_ =
      nh.create_subscription<nav_msgs::msg::Odometry>(odom_topic,
        [&](const nav_msgs::msg::Odometry::SharedPtr msg) {odomCallback(msg);},
        1);
  }

  inline nav_2d_msgs::msg::Twist2D getTwist() {return odom_vel_.velocity;}
  inline nav_2d_msgs::msg::Twist2DStamped getTwistStamped() {return odom_vel_;}

protected:
  void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    // ROS_INFO_ONCE("odom received!");
    std::lock_guard<std::mutex> lock(odom_mutex_);
    odom_vel_.header = msg->header;
    odom_vel_.velocity.x = msg->twist.twist.linear.x;
    if (robot_type_ == HOLONOMIC) {
      odom_vel_.velocity.y = msg->twist.twist.linear.y;
    } else {
      odom_vel_.velocity.y = 0;
    }
    odom_vel_.velocity.theta = msg->twist.twist.angular.z;
  }

  RobotType convertStringToRobotType(std::string robot_type)
  {
    std::transform(begin(robot_type), end(robot_type), begin(robot_type), ::tolower);
    if (robot_type == "differential") {
      return DIFFERENTIAL;
    } else if (robot_type == "holonomic") {
      return HOLONOMIC;
    } else {
      throw std::runtime_error(
              "robot_type parameter is invalid. Must be 'holonomic' or 'differential'");
    }
  }

  std::shared_ptr<rclcpp::Subscription<nav_msgs::msg::Odometry>> odom_sub_;
  nav_2d_msgs::msg::Twist2DStamped odom_vel_;
  std::mutex odom_mutex_;
  RobotType robot_type_;
};

}  // namespace nav_2d_utils

#endif  // NAV_2D_UTILS__ODOM_SUBSCRIBER_HPP_
