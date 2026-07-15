/**
 * @file slider_control.cpp
 * @brief ROS 2 node that bridges joint GUI slider commands to the arm and
 *        gripper trajectory controllers.
 *
 * Subscribes to a JointState topic produced by joint_state_publisher_gui (or
 * any other source remapped to /joint_commands) and republishes each incoming
 * message as separate JointTrajectory goal messages — one for the 3-DOF arm
 * and one for the gripper — so the ros2_control trajectory controllers can
 * execute them immediately.
 *
 * This node is intentionally stateless: every incoming joint-state message is
 * forwarded directly without buffering or interpolation.
 */

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>

#include <chrono>

using namespace std::chrono_literals;
using std::placeholders::_1;

/**
 * @brief Converts incoming JointState commands into JointTrajectory goals.
 *
 * Subscribes to `joint_commands` (sensor_msgs/JointState) and publishes to:
 * - `arm_controller/joint_trajectory`    (joint_1, joint_2, joint_3)
 * - `gripper_controller/joint_trajectory` (joint_4)
 */
class SliderControl : public rclcpp::Node
{
public:
  /// Initialises publishers, subscriber, and logs a startup message.
  SliderControl() : Node("slider_control")
  {
    sub_ = create_subscription<sensor_msgs::msg::JointState>("joint_commands", 10,
                                                             std::bind(&SliderControl::sliderCallback, this, _1));
    arm_pub_ = create_publisher<trajectory_msgs::msg::JointTrajectory>("arm_controller/joint_trajectory", 10);
    gripper_pub_ = create_publisher<trajectory_msgs::msg::JointTrajectory>("gripper_controller/joint_trajectory", 10);
    RCLCPP_INFO(get_logger(), "Slider Control Node started");
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr sub_;
  rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr arm_pub_;
  rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr gripper_pub_;

  /**
   * @brief Splits a JointState message into arm and gripper trajectory goals.
   *
   * Expects at least 4 positions in @p msg:
   * - positions[0..2] → arm joints (joint_1, joint_2, joint_3)
   * - positions[3]    → gripper joint (joint_4)
   *
   * @param msg Incoming joint state from the GUI slider or any remapped source.
   */
  void sliderCallback(const sensor_msgs::msg::JointState& msg) const
  {
    trajectory_msgs::msg::JointTrajectory arm_command, gripper_command;
    arm_command.joint_names = { "joint_1", "joint_2", "joint_3" };
    gripper_command.joint_names = { "joint_4" };

    trajectory_msgs::msg::JointTrajectoryPoint arm_goal, gripper_goal;
    arm_goal.positions.insert(arm_goal.positions.end(), msg.position.begin(), msg.position.begin() + 3);
    gripper_goal.positions.push_back(msg.position.at(3));

    arm_command.points.push_back(arm_goal);
    gripper_command.points.push_back(gripper_goal);

    arm_pub_->publish(arm_command);
    gripper_pub_->publish(gripper_command);
  }
};

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SliderControl>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}