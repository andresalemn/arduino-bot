/**
 * @file simple_action_server.cpp
 * @brief ROS 2 example: minimal action server that computes Fibonacci sequences.
 *
 * Demonstrates the ROS 2 action server pattern using `rclcpp_action`. The node
 * advertises a `fibonacci` action of type `arduinobot_msgs::action::Fibonacci`.
 * When a goal is accepted, the server computes the Fibonacci sequence up to the
 * requested order, publishing intermediate feedback at 1 Hz and returning the
 * full sequence as the final result.
 *
 * Key concepts illustrated:
 *   - Registering goal, cancel, and accepted callbacks.
 *   - Offloading long-running execution to a detached thread to avoid blocking the executor.
 *   - Publishing incremental feedback during execution.
 *   - Handling cancellation gracefully.
 *
 * The node is built as a composable ROS 2 component (shared library) and
 * registered with @c RCLCPP_COMPONENTS_REGISTER_NODE, which also exposes
 * the `simple_action_server_node` standalone executable.
 *
 * @par How to run
 * @code{.sh}
 * # As a standalone executable
 * ros2 run arduinobot_cpp_examples simple_action_server_node
 *
 * # Or as a component (loaded into a component container)
 * ros2 component load /ComponentManager arduinobot_cpp_examples arduinobot_cpp_examples::SimpleActionServer
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <memory>  // for std::shared_ptr
#include "rclcpp/rclcpp.hpp"
#include <rclcpp_action/rclcpp_action.hpp>
#include "arduinobot_msgs/action/fibonacci.hpp"
#include <rclcpp_components/register_node_macro.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

/**
 * @namespace arduinobot_cpp_examples
 * @brief Namespace grouping all composable nodes in the arduinobot C++ examples package.
 */
namespace arduinobot_cpp_examples
{

/**
 * @class SimpleActionServer
 * @brief ROS 2 action server that computes Fibonacci sequences on demand.
 *
 * Inherits from @c rclcpp::Node and creates an action server for the
 * `arduinobot_msgs::action::Fibonacci` action type on the `fibonacci` action name.
 *
 * ### Action interface
 * | Part     | Type / field                          | Description                             |
 * |----------|---------------------------------------|-----------------------------------------|
 * | Goal     | `int32 order`                         | Length of the Fibonacci sequence.        |
 * | Feedback | `int32[] partial_sequence`            | Sequence computed so far.               |
 * | Result   | `int32[] sequence`                    | Full Fibonacci sequence up to `order`.  |
 *
 * ### Execution flow
 * 1. A goal arrives → @ref goalCallback accepts it unconditionally.
 * 2. @ref acceptedCallback spawns a detached thread that calls @ref execute.
 * 3. @ref execute builds the sequence step by step, publishing feedback every second.
 * 4. If a cancel is requested (checked each iteration) → @ref cancelCallback accepts it
 *    and @ref execute sends the partial result with `canceled()`.
 * 5. On successful completion @ref execute calls `succeed()` with the full sequence.
 */
class SimpleActionServer : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new SimpleActionServer node.
   *
   * Creates the action server and binds the three required callbacks:
   * goal, cancel, and accepted.
   *
   * @param options Node options forwarded to @c rclcpp::Node (e.g. component parameters).
   */
  explicit SimpleActionServer(const rclcpp::NodeOptions& options = rclcpp::NodeOptions())
    : Node("simple_action_server", options)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Starting the Server");
    action_server_ = rclcpp_action::create_server<arduinobot_msgs::action::Fibonacci>(
        this, "fibonacci", std::bind(&SimpleActionServer::goalCallback, this, _1, _2),
        std::bind(&SimpleActionServer::cancelCallback, this, _1),
        std::bind(&SimpleActionServer::acceptedCallback, this, _1));
  }

private:
  /// Action server handle for the `fibonacci` action.
  rclcpp_action::Server<arduinobot_msgs::action::Fibonacci>::SharedPtr action_server_;

  /**
   * @brief Called when a new goal request is received.
   *
   * Logs the requested Fibonacci order and unconditionally accepts every goal.
   *
   * @param uuid Universally unique identifier of the incoming goal (unused).
   * @param goal Shared pointer to the goal message containing the requested `order`.
   * @return @c rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE for all goals.
   */
  rclcpp_action::GoalResponse goalCallback(const rclcpp_action::GoalUUID& uuid,
                                           std::shared_ptr<const arduinobot_msgs::action::Fibonacci::Goal> goal)
  {
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Received goal request with order: " << goal->order);
    (void)uuid;
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  /**
   * @brief Called immediately after a goal is accepted.
   *
   * Spawns a detached thread to run @ref execute so that the executor is not
   * blocked during the (potentially long) Fibonacci computation.
   *
   * @param goal_handle Handle to the accepted goal, forwarded to the execution thread.
   */
  void acceptedCallback(
      const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle)
  {
    // this needs to return quickly to avoid blocking the executor, so spin up a new thread
    std::thread{ std::bind(&SimpleActionServer::execute, this, _1), goal_handle }.detach();
  }

  /**
   * @brief Main execution function — runs in a detached thread.
   *
   * Iteratively builds the Fibonacci sequence up to `goal->order`, publishing
   * the partial sequence as feedback after each new element (at 1 Hz). If a
   * cancellation is detected the partial result is sent and the function returns.
   * On normal completion the full sequence is returned via `succeed()`.
   *
   * @param goal_handle Handle to the active goal, used to read the goal, publish
   *                    feedback, and set the final outcome.
   */
  void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Executing goal");
    rclcpp::Rate loop_rate(1);

    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<arduinobot_msgs::action::Fibonacci::Feedback>();
    auto& sequence = feedback->partial_sequence;

    sequence.push_back(0);
    sequence.push_back(1);

    auto result = std::make_shared<arduinobot_msgs::action::Fibonacci::Result>();

    for (int i = 1; (i < goal->order) && rclcpp::ok(); ++i)
    {
      // Check if there is a cancel request
      if (goal_handle->is_canceling())
      {
        result->sequence = sequence;
        goal_handle->canceled(result);
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Goal canceled");
        return;
      }
      // Update sequence
      sequence.push_back(sequence[i] + sequence[i - 1]);
      // Publish feedback
      goal_handle->publish_feedback(feedback);
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Publish feedback");

      loop_rate.sleep();
    }

    // Check if goal is done
    if (rclcpp::ok())
    {
      result->sequence = sequence;
      goal_handle->succeed(result);
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Goal succeeded");
    }
  }

  /**
   * @brief Called when a cancel request is received for an active goal.
   *
   * Unconditionally accepts every cancel request. The actual cancellation is
   * performed inside @ref execute when `goal_handle->is_canceling()` returns true.
   *
   * @param goal_handle Handle to the goal being cancelled (unused here).
   * @return @c rclcpp_action::CancelResponse::ACCEPT for all cancel requests.
   */
  rclcpp_action::CancelResponse
  cancelCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Received request to cancel goal");
    (void)goal_handle;
    return rclcpp_action::CancelResponse::ACCEPT;
  }
};
}  // namespace arduinobot_cpp_examples

RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_cpp_examples::SimpleActionServer)