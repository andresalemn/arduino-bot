/**
 * @file simple_parameter.cpp
 * @brief ROS 2 example: declaring parameters and reacting to runtime changes.
 *
 * Demonstrates how to declare ROS 2 parameters with default values and how to
 * register a callback that is invoked whenever a parameter value is updated at
 * runtime (e.g. via `ros2 param set`).
 *
 * Two parameters are declared:
 *   - `simple_int_param`    (int)    — default value **28**.
 *   - `simple_string_param` (string) — default value **"Antonio"**.
 *
 * @par How to run
 * @code{.sh}
 * # Terminal 1 — start the node
 * ros2 run arduinobot_cpp_examples simple_parameter
 *
 * # Terminal 2 — change parameters at runtime
 * ros2 param set /simple_parameter simple_int_param 42
 * ros2 param set /simple_parameter simple_string_param "ROS2"
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <string>
#include <vector>
#include <memory>


using std::placeholders::_1;


/**
 * @class SimpleParameter
 * @brief ROS 2 node that declares parameters and listens for their changes.
 *
 * Inherits from @c rclcpp::Node. On construction, two parameters are declared
 * with default values. A parameter-change callback (@ref paramChangeCallback)
 * is registered so the node can react whenever either parameter is updated at
 * runtime through the ROS 2 parameter service.
 *
 * This example illustrates:
 *   - `rclcpp::Node::declare_parameter()` for typed parameter declaration.
 *   - `rclcpp::Node::add_on_set_parameters_callback()` for change notification.
 *   - Validating parameter types inside the callback before consuming the value.
 */
class SimpleParameter : public rclcpp::Node
{
public:
    /**
     * @brief Construct a new SimpleParameter node.
     *
     * Declares the two parameters with their default values and registers the
     * on-set callback that will be invoked for every parameter change request.
     */
    SimpleParameter() : Node("simple_parameter")
    {
        declare_parameter<int>("simple_int_param", 28);
        declare_parameter<std::string>("simple_string_param", "Antonio");

        paramChangeCallback_ = add_on_set_parameters_callback(std::bind(&SimpleParameter::paramChangeCallback, this, _1));
    }

private:
    /**
     * @brief Handle returned by @c add_on_set_parameters_callback.
     *
     * Kept alive for the lifetime of the node so that the callback remains
     * registered. When this shared_ptr is destroyed, the callback is removed.
     */
    OnSetParametersCallbackHandle::SharedPtr paramChangeCallback_;

    /**
     * @brief Called by the parameter service whenever a set-parameter request arrives.
     *
     * Iterates over every parameter included in the request, logs its new value
     * if it matches one of the two declared parameters (and the type matches),
     * and marks the result as successful.
     *
     * @param parameters Vector of @c rclcpp::Parameter objects representing the
     *                   requested changes.
     * @return @c rcl_interfaces::msg::SetParametersResult with @c successful set
     *         to @c true when a recognised parameter is processed.
     *
     * @note The result's @c successful field is only set to @c true when at least
     *       one of the handled parameters matches. For a production node you should
     *       also consider setting it to @c false for unrecognised/invalid parameters
     *       and providing a descriptive @c reason string.
     */
    rcl_interfaces::msg::SetParametersResult paramChangeCallback(const std::vector<rclcpp::Parameter> &parameters)
    {
      rcl_interfaces::msg::SetParametersResult result;   
      for(const auto& param : parameters)
      {
        if(param.get_name() == "simple_int_param" && param.get_type() == rclcpp::ParameterType:: PARAMETER_INTEGER)
        {
          RCLCPP_INFO_STREAM(get_logger(), "Param simple_int_param changed! New value is> " << param.as_int());
          result.successful = true;
        }
        if(param.get_name() == "simple_string_param" && param.get_type() == rclcpp::ParameterType:: PARAMETER_STRING)
        {
          RCLCPP_INFO_STREAM(get_logger(), "Param simple_string_param changed! New value is> " << param.as_string());
          result.successful = true;
        }
      }

      return result;
    }
};

/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, instantiates @ref SimpleParameter, spins
 * the node (blocking until shutdown), and cleans up.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleParameter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
