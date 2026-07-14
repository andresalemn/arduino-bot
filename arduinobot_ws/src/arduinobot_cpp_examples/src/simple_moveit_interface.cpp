/**
 * @file simple_moveit_interface.cpp
 * @brief ROS 2 example: minimal MoveIt 2 motion planning interface.
 *
 * Demonstrates how to use the MoveIt 2 `MoveGroupInterface` to plan and execute
 * joint-space motions for a robot arm and gripper. The node initialises two move
 * groups (`arm` and `gripper`), sets joint-space goals, calls the planner, and
 * executes the resulting trajectories if planning succeeds.
 *
 * ### Motion targets
 * | Move group | Joint goal (rad / m)      |
 * |------------|---------------------------|
 * | `arm`      | `[1.57, 0.0, 0.0]`        |
 * | `gripper`  | `[-0.7, 0.7]`             |
 *
 * @par Prerequisites
 *   - A running MoveIt 2 `move_group` node with the `arm` and `gripper`
 *     planning groups configured in SRDF/URDF.
 *
 * @par How to run
 * @code{.sh}
 * # Terminal 1 — launch MoveIt with the robot description (example)
 * ros2 launch arduinobot_moveit moveit.launch.py
 *
 * # Terminal 2 — run the interface node
 * ros2 run arduinobot_cpp_examples simple_moveit_interface
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include <moveit/move_group_interface/move_group_interface.h>

/**
 * @brief Plan and execute joint-space motions for the arm and gripper.
 *
 * Creates two @c MoveGroupInterface objects — one for the `arm` planning group
 * and one for the `gripper` planning group — and attempts to move both to
 * predefined joint-space targets.
 *
 * ### Algorithm
 * 1. Set joint-value targets for both groups via `setJointValueTarget()`.
 * 2. If either target is outside joint limits, log a warning and return early.
 * 3. Call `plan()` on both groups.
 * 4. If both plans succeed, execute them sequentially (`arm` first, then `gripper`).
 * 5. If either plan fails, log an error and return without moving.
 *
 * @param node Shared pointer to the ROS 2 node used for MoveIt communication
 *             (topic subscriptions, service calls, etc.).
 *
 * @warning Joint limit clamping is currently disabled: if a target is out of
 *          bounds the function logs a warning and returns without moving. You
 *          may modify this to clamp values and continue if desired.
 */
void move_robot(const std::shared_ptr<rclcpp::Node> node)
{
    auto arm_move_group = moveit::planning_interface::MoveGroupInterface(node, "arm");
    auto gripper_move_group = moveit::planning_interface::MoveGroupInterface(node, "gripper");

    std::vector<double> arm_joint_goal {1.57, 0.0, 0.0};
    std::vector<double> gripper_joint_goal {-0.7, 0.7};

    bool arm_within_bounds = arm_move_group.setJointValueTarget(arm_joint_goal);
    bool gripper_within_bounds = gripper_move_group.setJointValueTarget(gripper_joint_goal);

    if (!arm_within_bounds | !gripper_within_bounds)
    {
        RCLCPP_WARN(rclcpp::get_logger("rclcpp"),
                    "Target joint position(s) were outside of limits, but we will plan and clamp to the limits ");
        return;
    }

    moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
    moveit::planning_interface::MoveGroupInterface::Plan gripper_plan;
    bool arm_plan_success = (arm_move_group.plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS);
    bool gripper_plan_success = (gripper_move_group.plan(gripper_plan) == moveit::core::MoveItErrorCode::SUCCESS);

    if(arm_plan_success && gripper_plan_success)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),
                    "Planner SUCCEED, moving the arm and the gripper");
        arm_move_group.move();
        gripper_move_group.move();
    }
    else
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),
                     "One or more planners failed!");
        return;
    }
}

/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, creates a generic @c rclcpp::Node (used by
 * MoveIt 2 internally), calls @ref move_robot to plan and execute the motion,
 * and shuts down cleanly.
 *
 * @note This example does **not** spin the node — MoveIt 2 uses a background
 *       executor internally and `move()` blocks until execution completes.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("simple_moveit_interface");
    move_robot(node);
    rclcpp::shutdown();
    return 0;
}