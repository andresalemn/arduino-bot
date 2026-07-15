/**
 * @file simple_lifecycle_node.cpp
 * @brief ROS 2 example: managed (lifecycle) node that subscribes to a topic.
 *
 * Demonstrates the ROS 2 lifecycle node pattern using `rclcpp_lifecycle`. A
 * lifecycle node has a well-defined set of states and transitions, giving
 * operators fine-grained control over node start-up and shutdown without
 * restarting the process.
 *
 * ### Lifecycle states used
 * | Transition      | Callback          | Action performed                         |
 * |-----------------|-------------------|------------------------------------------|
 * | configure       | @ref on_configure | Creates the `chatter` subscription.      |
 * | activate        | @ref on_activate  | Calls base `on_activate`; sleeps 2 s.    |
 * | deactivate      | @ref on_deactivate| Calls base `on_deactivate`.              |
 * | cleanup         | @ref on_cleanup   | Destroys the subscription.              |
 * | shutdown        | @ref on_shutdown   | Destroys the subscription.              |
 *
 * While the node is in the **active** state, messages received on `/chatter`
 * are printed to the console. In any other state they are silently discarded.
 *
 * @par How to run
 * @code{.sh}
 * # Terminal 1 — start the lifecycle node
 * ros2 run arduinobot_cpp_examples simple_lifecycle_node
 *
 * # Terminal 2 — manage lifecycle transitions
 * ros2 lifecycle nodes                               # list managed nodes
 * ros2 lifecycle get /simple_lifecycle_node          # check current state
 * ros2 lifecycle set /simple_lifecycle_node configure
 * ros2 lifecycle set /simple_lifecycle_node activate
 *
 * # Terminal 3 — publish test messages
 * ros2 topic pub /chatter std_msgs/msg/String "data: 'Hi'"
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <std_msgs/msg/string.hpp>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using std::placeholders::_1;

/**
 * @class SimpleLifecycleNode
 * @brief ROS 2 lifecycle node that conditionally subscribes to `/chatter`.
 *
 * Inherits from @c rclcpp_lifecycle::LifecycleNode. The subscription to the
 * `chatter` topic is created during the *configure* transition and destroyed
 * during *cleanup* or *shutdown*. Messages are only printed when the node is
 * in the **active** state.
 *
 * This example teaches:
 *   - Overriding lifecycle transition callbacks.
 *   - Deferring resource allocation to `on_configure()` and releasing them in
 *     `on_cleanup()` / `on_shutdown()`.
 *   - Checking the current lifecycle state inside a subscription callback.
 */
class SimpleLifecycleNode : public rclcpp_lifecycle::LifecycleNode
{
public:
  /**
   * @brief Construct a new SimpleLifecycleNode.
   *
   * @param node_name          Name of the node as seen in the ROS 2 graph.
   * @param intra_process_comms If @c true, enables intra-process communication
   *                            optimisation (zero-copy for same-process publishers).
   */
  explicit SimpleLifecycleNode(const std::string& node_name, bool intra_process_comms = false)
    : rclcpp_lifecycle::LifecycleNode(node_name, rclcpp::NodeOptions().use_intra_process_comms(intra_process_comms))
  {
  }

  /**
   * @brief Lifecycle transition callback: *unconfigured → configuring*.
   *
   * Creates the `chatter` subscription so that the node is ready to receive
   * messages once it transitions to the *active* state.
   *
   * @param state Previous lifecycle state (unused).
   * @return @c CallbackReturn::SUCCESS always.
   */
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_configure(const rclcpp_lifecycle::State&)
  {
    sub_ = create_subscription<std_msgs::msg::String>("chatter", 10,
                                                      std::bind(&SimpleLifecycleNode::msgCallback, this, _1));
    RCLCPP_INFO(get_logger(), "Lifecycle node on_configure() called.");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  /**
   * @brief Lifecycle transition callback: *inactive → activating*.
   *
   * Delegates to the base class implementation (which activates managed
   * publishers/subscriptions) and then sleeps for 2 seconds to simulate
   * a hardware initialisation delay.
   *
   * @param state Previous lifecycle state, forwarded to the base class.
   * @return @c CallbackReturn::SUCCESS always.
   */
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_activate(const rclcpp_lifecycle::State& state)
  {
    LifecycleNode::on_activate(state);
    RCLCPP_INFO(get_logger(), "Lifecycle node on_activate() called.");
    std::this_thread::sleep_for(2s);
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  /**
   * @brief Lifecycle transition callback: *active → deactivating*.
   *
   * Delegates to the base class implementation, which deactivates managed
   * publishers and subscriptions (they still exist but stop processing).
   *
   * @param state Previous lifecycle state, forwarded to the base class.
   * @return @c CallbackReturn::SUCCESS always.
   */
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_deactivate(const rclcpp_lifecycle::State& state)
  {
    LifecycleNode::on_deactivate(state);
    RCLCPP_INFO(get_logger(), "Lifecycle node on_deactivate() called.");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  /**
   * @brief Lifecycle transition callback: *inactive → cleaningup*.
   *
   * Destroys the subscription (resets the shared_ptr), freeing all associated
   * resources. After cleanup the node returns to the *unconfigured* state.
   *
   * @param state Previous lifecycle state (unused).
   * @return @c CallbackReturn::SUCCESS always.
   */
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_cleanup(const rclcpp_lifecycle::State&)
  {
    sub_.reset();
    RCLCPP_INFO(get_logger(), "Lifecycle node on_cleanup() called.");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  /**
   * @brief Lifecycle transition callback: any state → *shuttingdown*.
   *
   * Destroys the subscription if it exists and logs the shutdown.
   *
   * @param state Previous lifecycle state (unused).
   * @return @c CallbackReturn::SUCCESS always.
   */
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn on_shutdown(const rclcpp_lifecycle::State&)
  {
    sub_.reset();
    RCLCPP_INFO(get_logger(), "Lifecycle node on_shutdown() called");
    return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
  }

  /**
   * @brief Subscription callback for the `chatter` topic.
   *
   * Checks the current lifecycle state before logging. If the node is not in
   * the **active** state the message is silently ignored. This guards against
   * stale callbacks that may be queued while the node is transitioning.
   *
   * @param msg Const reference to the received @c std_msgs::msg::String message.
   */
  void msgCallback(const std_msgs::msg::String& msg)
  {
    auto state = get_current_state();
    if (state.label() == "active")
    {
      RCLCPP_INFO_STREAM(get_logger(), "Lifecycle node heard: " << msg.data.c_str());
    }
  }

private:
  /// Subscription handle for the `chatter` topic (null when not configured).
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
};

/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, creates a @ref SimpleLifecycleNode, registers
 * it with a @c SingleThreadedExecutor, and spins until shutdown.
 *
 * A @c SingleThreadedExecutor is used instead of the default spinning helper
 * so the lifecycle node's managed interface is properly wired into the executor.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor ste;

  std::shared_ptr<SimpleLifecycleNode> simple_lifecycle_node =
      std::make_shared<SimpleLifecycleNode>("simple_lifecycle_node");

  ste.add_node(simple_lifecycle_node->get_node_base_interface());
  ste.spin();
  rclcpp::shutdown();

  return 0;
}