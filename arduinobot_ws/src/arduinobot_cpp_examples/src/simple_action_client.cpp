/**
 * @file simple_action_client.cpp
 * @brief ROS 2 example: minimal action client for the Fibonacci action.
 *
 * Demonstrates the ROS 2 action client pattern using `rclcpp_action`. The node
 * connects to the `fibonacci` action server and sends a goal requesting the
 * first 10 elements of the Fibonacci sequence. It then handles:
 *   - The server's acceptance (or rejection) of the goal.
 *   - Incremental feedback published by the server during execution.
 *   - The final result once the sequence is complete.
 *
 * Key concepts illustrated:
 *   - Creating an action client and waiting for the action server.
 *   - Sending a goal with `async_send_goal()` and `SendGoalOptions`.
 *   - Binding goal-response, feedback, and result callbacks.
 *   - Shutting down the node automatically after receiving the final result.
 *
 * The node is built as a composable ROS 2 component (shared library) and
 * registered with @c RCLCPP_COMPONENTS_REGISTER_NODE, which also exposes
 * the `simple_action_client_node` standalone executable.
 *
 * @par How to run
 * @code{.sh}
 * # Terminal 1 — start the action server
 * ros2 run arduinobot_cpp_examples simple_action_server_node
 *
 * # Terminal 2 — run the client
 * ros2 run arduinobot_cpp_examples simple_action_client_node
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include "arduinobot_msgs/action/fibonacci.hpp"

using namespace std::chrono_literals;

using std::placeholders::_1;
using std::placeholders::_2;

/**
 * @namespace arduinobot_cpp_examples
 * @brief Namespace grouping all composable nodes in the arduinobot C++ examples package.
 */
namespace arduinobot_cpp_examples
{

/**
 * @class SimpleActionClient
 * @brief ROS 2 action client that requests a Fibonacci sequence of order 10.
 *
 * Inherits from @c rclcpp::Node and creates an action client for the
 * `arduinobot_msgs::action::Fibonacci` action type on the `fibonacci` action name.
 *
 * ### Goal sent
 * | Field   | Value | Description               |
 * |---------|-------|---------------------------|
 * | `order` | 10    | Length of the sequence.   |
 *
 * ### Callback chain
 * 1. A 1-second one-shot wall-clock timer fires → @ref timerCallback.
 * 2. Timer is cancelled; if the server is available the goal is sent.
 * 3. @ref goalCallback confirms whether the server accepted the goal.
 * 4. @ref feedbackCallback logs each partial sequence received.
 * 5. @ref resultCallback logs the final sequence and calls `rclcpp::shutdown()`.
 */
class SimpleActionClient : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new SimpleActionClient node.
   *
   * Creates the action client for the `fibonacci` action and sets up a 1-second
   * one-shot timer that triggers @ref timerCallback. Using a timer (rather than
   * sending the goal directly in the constructor) ensures the node is fully
   * initialised and the executor is running before making the first network call.
   *
   * @param options Node options forwarded to @c rclcpp::Node.
   */
  explicit SimpleActionClient(const rclcpp::NodeOptions& options)
    : Node("simple_action_client", options)
  {
    client_ = rclcpp_action::create_client<arduinobot_msgs::action::Fibonacci>(this, "fibonacci");
    timer_ = create_wall_timer(1s, std::bind(&SimpleActionClient::timerCallback, this));
  }

private:
  /// Action client handle for the `fibonacci` action.
  rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SharedPtr client_;

  /// One-shot timer used to defer goal sending until the executor is spinning.
  rclcpp::TimerBase::SharedPtr timer_;

  /**
   * @brief One-shot timer callback — waits for the server and sends the goal.
   *
   * Cancels itself immediately (one-shot behaviour). If the action server is
   * not available, shuts down the node with an error. Otherwise builds a goal
   * message (`order = 10`), attaches the three response callbacks via
   * `SendGoalOptions`, and sends the goal asynchronously.
   */
  void timerCallback()
  {
    timer_->cancel();

    if (!client_->wait_for_action_server())
    {
      RCLCPP_ERROR(get_logger(), "Action server not available after waiting");
      rclcpp::shutdown();
    }

    auto goal_msg = arduinobot_msgs::action::Fibonacci::Goal();
    goal_msg.order = 10;

    RCLCPP_INFO(get_logger(), "Sending goal");

    auto send_goal_options =
        rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SendGoalOptions();
    send_goal_options.goal_response_callback =
        std::bind(&SimpleActionClient::goalCallback, this, _1);
    send_goal_options.feedback_callback =
        std::bind(&SimpleActionClient::feedbackCallback, this, _1, _2);
    send_goal_options.result_callback =
        std::bind(&SimpleActionClient::resultCallback, this, _1);
    client_->async_send_goal(goal_msg, send_goal_options);
  }

  /**
   * @brief Called when the action server accepts or rejects the sent goal.
   *
   * Logs a message indicating whether the server accepted the goal or rejected it.
   *
   * @param goal_handle Shared pointer to the client-side goal handle. A null
   *                    handle means the goal was rejected by the server.
   */
  void goalCallback(const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr& goal_handle)
  {
    if (!goal_handle)
    {
      RCLCPP_ERROR(get_logger(), "Goal was rejected by server");
    }
    else
    {
      RCLCPP_INFO(get_logger(), "Goal accepted by server, waiting for result");
    }
  }

  /**
   * @brief Called each time the action server publishes feedback.
   *
   * Logs the current partial Fibonacci sequence received in the feedback message.
   *
   * @param goal_handle Client-side goal handle (unused; present to match the callback signature).
   * @param feedback    Shared pointer to the feedback message containing `partial_sequence`.
   */
  void feedbackCallback(rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr,
                        const std::shared_ptr<const arduinobot_msgs::action::Fibonacci::Feedback> feedback)
  {
    std::stringstream ss;
    ss << "Next number in sequence received: ";
    for (auto number : feedback->partial_sequence)
    {
      ss << number << " ";
    }
    RCLCPP_INFO(get_logger(), ss.str().c_str());
  }

  /**
   * @brief Called when the action server sends the final result.
   *
   * Checks the result code:
   *   - `SUCCEEDED` → logs the full sequence and shuts down.
   *   - `ABORTED`   → logs an error and returns.
   *   - `CANCELED`  → logs an error and returns.
   *   - otherwise   → logs an unknown-code error and returns.
   *
   * @param result Wrapped result containing the result code and, on success,
   *               the complete Fibonacci `sequence`.
   */
  void resultCallback(
      const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::WrappedResult& result)
  {
    switch (result.code)
    {
      case rclcpp_action::ResultCode::SUCCEEDED:
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(get_logger(), "Goal was aborted");
        return;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(get_logger(), "Goal was canceled");
        return;
      default:
        RCLCPP_ERROR(get_logger(), "Unknown result code");
        return;
    }
    std::stringstream ss;
    ss << "Result received: ";
    for (auto number : result.result->sequence)
    {
      ss << number << " ";
    }
    RCLCPP_INFO(get_logger(), ss.str().c_str());
    rclcpp::shutdown();
  }
};
}  // namespace arduinobot_cpp_examples

RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_cpp_examples::SimpleActionClient)