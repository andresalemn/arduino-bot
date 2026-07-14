/**
 * @file simple_subscriber.cpp
 * @brief ROS 2 example: minimal topic subscriber node.
 *
 * Demonstrates how to subscribe to a ROS 2 topic using rclcpp.
 * The node listens on the @c chatter topic and prints every received
 * @c std_msgs::msg::String message to the console via the ROS 2 logger.
 *
 * @par Topic
 *   - **Subscribed**: `/chatter` (`std_msgs/msg/String`) – incoming string messages.
 *
 * @par How to run
 * @code{.sh}
 * ros2 run arduinobot_cpp_examples simple_subscriber
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>


using std::placeholders::_1;

/**
 * @class SimpleSubscriber
 * @brief Minimal ROS 2 subscriber node that logs incoming string messages.
 *
 * Inherits from @c rclcpp::Node and creates a subscription on the `chatter`
 * topic with a queue depth of 10. Every received message is printed to the
 * ROS 2 logger via @ref msgCallback.
 *
 * This is the counterpart to @c SimplePublisher and together they illustrate
 * the fundamental publish/subscribe communication pattern in ROS 2.
 */
class SimpleSubscriber : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new SimpleSubscriber node.
   *
   * Initialises the node with the name `simple_subscriber` and creates the
   * subscription, binding incoming messages to @ref msgCallback.
   */
  SimpleSubscriber() : Node("simple_subscriber")
  {
    sub_ = create_subscription<std_msgs::msg::String>(
        "chatter", 10, std::bind(&SimpleSubscriber::msgCallback, this, _1));
  }

private:
  /// Subscription handle for the `chatter` topic.
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;

  /**
   * @brief Subscription callback — invoked for every received message.
   *
   * Logs the content of the received @c std_msgs::msg::String message at the
   * INFO level using the node's logger.
   *
   * @param msg Const reference to the received message.
   */
  void msgCallback(const std_msgs::msg::String &msg) const
  {
    RCLCPP_INFO_STREAM(this->get_logger(), "I heard: " << msg.data.c_str());
  }
};


/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, instantiates @ref SimpleSubscriber, spins
 * the node until shutdown, and cleans up.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimpleSubscriber>());
  rclcpp::shutdown();
  return 0;
}