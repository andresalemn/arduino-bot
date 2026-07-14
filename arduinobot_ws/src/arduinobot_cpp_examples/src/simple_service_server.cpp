/**
 * @file simple_service_server.cpp
 * @brief ROS 2 example: minimal service server node.
 *
 * Demonstrates how to create a ROS 2 service server using rclcpp. The node
 * advertises the `add_two_ints` service and, upon receiving a request,
 * computes the sum of two integers and returns it to the caller.
 *
 * The service uses the custom message type defined in the @c arduinobot_msgs
 * package:
 *   - **Request**: two signed integers @c a and @c b.
 *   - **Response**: their @c sum.
 *
 * @par How to run
 * @code{.sh}
 * # Terminal 1 — start the server
 * ros2 run arduinobot_cpp_examples simple_service_server
 *
 * # Terminal 2 — call the service manually
 * ros2 service call /add_two_ints arduinobot_msgs/srv/AddTwoInts "{a: 3, b: 7}"
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <memory> // for std::shared_ptr
#include "rclcpp/rclcpp.hpp"
#include "arduinobot_msgs/srv/add_two_ints.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

/**
 * @class SimpleServiceServer
 * @brief ROS 2 node that provides an addition service (`add_two_ints`).
 *
 * Inherits from @c rclcpp::Node and advertises a service of type
 * @c arduinobot_msgs::srv::AddTwoInts. Each incoming request is handled
 * by @ref serviceCallback, which logs the operands, computes the sum, and
 * populates the response.
 *
 * This example teaches:
 *   - How to create a typed service server with `rclcpp::Node::create_service()`.
 *   - How to write a service callback that reads request fields and fills the response.
 */
class SimpleServiceServer : public rclcpp::Node
{
public:
    /**
     * @brief Construct a new SimpleServiceServer node.
     *
     * Initialises the node with the name `simple_service_server`, creates the
     * service, and logs a ready message.
     */
    SimpleServiceServer() : Node("simple_service_server") 
    {
        server_ = create_service<arduinobot_msgs::srv::AddTwoInts>(
            "add_two_ints",
            std::bind(&SimpleServiceServer::serviceCallback, this, _1, _2));
        RCLCPP_INFO(this->get_logger(), "Service add_two_ints is Ready");
    }

private:
    /// Service server handle for the `add_two_ints` service.
    rclcpp::Service<arduinobot_msgs::srv::AddTwoInts>::SharedPtr server_;

    /**
     * @brief Service callback — computes the sum of two integers.
     *
     * Logs the incoming request values, computes `req->a + req->b`, stores the
     * result in `res->sum`, and logs the computed value before returning.
     *
     * @param req Shared pointer to the incoming request containing fields @c a and @c b.
     * @param res Shared pointer to the response; the @c sum field is populated here.
     */
    void serviceCallback(const std::shared_ptr<arduinobot_msgs::srv::AddTwoInts::Request> req,
                         const std::shared_ptr<arduinobot_msgs::srv::AddTwoInts::Response> res)
    {
        RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "New Request Received a: " << req->a << " b: " << req->b);
        res->sum = req->a + req->b;
        RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Returning sum: " << res->sum);        
    }                         

};

/**
 * @brief Program entry point.
 *
 * Initialises the ROS 2 runtime, instantiates @ref SimpleServiceServer, spins
 * the node until shutdown, and cleans up.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleServiceServer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
