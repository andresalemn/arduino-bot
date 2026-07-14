/**
 * @file simple_publisher.cpp
 * @brief ROS 2 example: minimal topic publisher node.
 *
 * Demonstrates the basic publisher/subscriber pattern in ROS 2 using rclcpp.
 * The node publishes a @c std_msgs::msg::String message on the @c chatter topic
 * at a fixed rate of 1 Hz, appending an incrementing counter to each message.
 *
 * @par Topic
 *   - **Published**: `/chatter` (`std_msgs/msg/String`) – greeting string with counter.
 *
 * @par How to run
 * @code{.sh}
 * # In your ROS 2 workspace:
 * ros2 run arduinobot_cpp_examples simple_publisher
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <chrono>

using namespace std::chrono_literals;

/**
 * @class SimplePublisher
 * @brief Minimal ROS 2 publisher node that periodically sends string messages.
 *
 * Inherits from @c rclcpp::Node and creates:
 *   - A publisher on the `chatter` topic.
 *   - A 1-second wall-clock timer that triggers @ref timerCallback.
 *
 * This is a typical "Hello World" example for learning the ROS 2 publisher API.
 */
class SimplePublisher : public rclcpp::Node
{
public:
    /**
     * @brief Construct a new SimplePublisher node.
     *
     * Initialises the node with the name `simple_publisher`, creates the
     * publisher and the periodic timer, and logs the publish rate.
     */
    SimplePublisher() : Node("simple_publisher"), counter_(0)
    {
        pub_ = create_publisher<std_msgs::msg::String>("chatter", 10);
        timer_ = create_wall_timer(1s, std::bind(&SimplePublisher::timerCallback, this));
    
        RCLCPP_INFO(get_logger(), "Publishing at 1Hz");
    }

    /**
     * @brief Timer callback invoked at 1 Hz.
     *
     * Builds a @c std_msgs::msg::String message whose @c data field contains a
     * greeting string followed by the current value of @ref counter_, then
     * publishes the message and increments the counter.
     */
    void timerCallback()
    {
        auto message = std_msgs::msg::String();
        message.data = "Hello ROS2 - counter: " + std::to_string(counter_++);
        pub_->publish(message);
    }
    
private:
    /// Running counter appended to every published message.
    unsigned int counter_;

    /// Publisher handle for the `chatter` topic.
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;

    /// Wall-clock timer that fires the publish callback every second.
    rclcpp::TimerBase::SharedPtr timer_;
};

/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, instantiates @ref SimplePublisher, spins
 * the node (blocking until SIGINT or @c rclcpp::shutdown()), and then cleans up.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main (int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimplePublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}