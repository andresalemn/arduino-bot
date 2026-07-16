/**
 * @file angle_conversion.cpp
 * @brief ROS 2 node providing services for angle conversions.
 *
 * This file implements a node that provides two services: one to convert Euler angles
 * (roll, pitch, yaw) to quaternions, and another to convert quaternions back to Euler angles
 * using the tf2 library.
 */

#include <memory>  // for std::shared_ptr
#include <tf2/utils.h>
#include "rclcpp/rclcpp.hpp"
#include "arduinobot_msgs/srv/euler_to_quaternion.hpp"
#include "arduinobot_msgs/srv/quaternion_to_euler.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

/**
 * @class AnglesConverter
 * @brief A ROS 2 node that hosts Euler/Quaternion conversion services.
 */
class AnglesConverter : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new Angles Converter object.
   *
   * Initializes the "angles_conversion_service_server_cpp" node and registers the
   * "euler_to_quaternion" and "quaternion_to_euler" services.
   */
  AnglesConverter() : Node("angles_conversion_service_server_cpp")
  {
    euler_to_quaternion_ = create_service<arduinobot_msgs::srv::EulerToQuaternion>(
        "euler_to_quaternion", std::bind(&AnglesConverter::eulerToQuaternionCallback, this, _1, _2));
    quaternion_to_euler_ = create_service<arduinobot_msgs::srv::QuaternionToEuler>(
        "quaternion_to_euler", std::bind(&AnglesConverter::quaternionToEulerCallback, this, _1, _2));
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Angles conversion services are ready");
  }

private:
  rclcpp::Service<arduinobot_msgs::srv::EulerToQuaternion>::SharedPtr euler_to_quaternion_;
  rclcpp::Service<arduinobot_msgs::srv::QuaternionToEuler>::SharedPtr quaternion_to_euler_;

  /**
   * @brief Callback function for the EulerToQuaternion service.
   *
   * Converts Euler angles (roll, pitch, yaw) from the request into a quaternion
   * represented by x, y, z, w in the response.
   *
   * @param req Shared pointer to the service request containing roll, pitch, and yaw.
   * @param res Shared pointer to the service response to be populated with x, y, z, and w.
   */
  void eulerToQuaternionCallback(const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Request> req,
                                 const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Response> res)
  {
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                       "Requested to convert euler angles roll: " << req->roll << ", pitch: " << req->pitch
                                                                  << ", yaw: " << req->yaw << ", into a quaternion.");
    // Calculate quaternion from euler
    tf2::Quaternion q;
    q.setRPY(req->roll, req->pitch, req->yaw);
    res->x = q.getX();
    res->y = q.getY();
    res->z = q.getZ();
    res->w = q.getW();
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Corresponding quaternion x: " << res->x << ", y: " << res->y
                                                                                    << ", z: " << res->z
                                                                                    << ", w: " << res->w);
  }

  /**
   * @brief Callback function for the QuaternionToEuler service.
   *
   * Converts a quaternion represented by x, y, z, w from the request into Euler angles
   * (roll, pitch, yaw) in the response.
   *
   * @param req Shared pointer to the service request containing x, y, z, and w.
   * @param res Shared pointer to the service response to be populated with roll, pitch, and yaw.
   */
  void quaternionToEulerCallback(const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Request> req,
                                 const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Response> res)
  {
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"),
                       "Requested to convert quaternion x: " << req->x << ", y: " << req->y << ", z: " << req->z
                                                             << ", w: " << req->w << ", into euler angles.");
    // Calculate euler from quaternion
    tf2::Quaternion q(req->x, req->y, req->z, req->w);
    tf2::Matrix3x3 m(q);
    m.getRPY(res->roll, res->pitch, res->yaw);
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Corresponding euler angles roll: " << res->roll
                                                                                         << ", pitch: " << res->pitch
                                                                                         << ", yaw: " << res->yaw);
  }
};

/**
 * @brief Entry point for the ROS 2 C++ angle conversion service node.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @return int Execution status.
 */
int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<AnglesConverter>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
