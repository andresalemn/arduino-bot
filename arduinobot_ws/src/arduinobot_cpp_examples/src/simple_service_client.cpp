/**
 * @file simple_service_client.cpp
 * @brief ROS 2 example: minimal asynchronous service client node.
 *
 * Demonstrates how to create a ROS 2 service client using rclcpp and send an
 * asynchronous request. The node connects to the `add_two_ints` service,
 * waits for it to become available, sends a request with two integers provided
 * as command-line arguments, and prints the response.
 *
 * The service uses the custom message type defined in @c arduinobot_msgs:
 *   - **Request**: two signed integers @c a and @c b (from argv).
 *   - **Response**: their @c sum.
 *
 * @par How to run
 * @code{.sh}
 * # Start the server first (in another terminal)
 * ros2 run arduinobot_cpp_examples simple_service_server
 *
 * # Then run the client with two integer arguments
 * ros2 run arduinobot_cpp_examples simple_service_client 3 7
 * @endcode
 *
 * @author Andres Aleman
 * @date 2024
 */

#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "arduinobot_msgs/srv/add_two_ints.hpp"
    
using namespace std::chrono_literals;
using std::placeholders::_1;

/**
 * @class SimpleServiceClient
 * @brief ROS 2 node that asynchronously calls the `add_two_ints` service.
 *
 * Inherits from @c rclcpp::Node. On construction the node:
 *   1. Creates a service client for `add_two_ints`.
 *   2. Waits in a polling loop (1 s intervals) until the service is available.
 *   3. Sends an asynchronous request with the two integer operands.
 *   4. Processes the response in @ref responseCallback.
 *
 * This example illustrates:
 *   - Creating a typed client with `rclcpp::Node::create_client()`.
 *   - Waiting for a service with `wait_for_service()`.
 *   - Sending asynchronous requests with `async_send_request()`.
 *   - Handling the result in a future-based callback.
 */
class SimpleServiceClient : public rclcpp::Node 
{
public:
    /**
     * @brief Construct a new SimpleServiceClient node and immediately send a request.
     *
     * Blocks (within the constructor) until the `add_two_ints` service becomes
     * available, then fires off the asynchronous request. The response will be
     * delivered to @ref responseCallback.
     *
     * @param a First integer operand (maps to `Request::a`).
     * @param b Second integer operand (maps to `Request::b`).
     */
    SimpleServiceClient(int a, int b) : Node("simple_service_client") 
    {     
        client_ = create_client<arduinobot_msgs::srv::AddTwoInts>("add_two_ints");
        auto request = std::make_shared<arduinobot_msgs::srv::AddTwoInts::Request>();
        request ->a = a;
        request->b = b;

        while (!client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) 
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
                return;
            }
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
        }

        auto result = client_->async_send_request(request, std::bind(&SimpleServiceClient::responseCallback, this, _1));    
    }
    
private:
    /// Async service client handle for the `add_two_ints` service.
    rclcpp::Client<arduinobot_msgs::srv::AddTwoInts>::SharedPtr client_;

    /**
     * @brief Response callback — logs the service result.
     *
     * Called automatically by the ROS 2 executor when the shared future
     * returned by `async_send_request()` becomes ready.
     *
     * @param future Shared future holding the server's response. When valid,
     *               `future.get()->sum` contains the addition result.
     */
    void responseCallback(rclcpp::Client<arduinobot_msgs::srv::AddTwoInts>::SharedFuture future)
    {
        if(future.valid())
        {
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Service Response " << future.get()->sum);
        }
        else
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Service Failure");
        }
    }

};

/**
 * @brief Program entry point.
 *
 * Expects exactly two additional command-line arguments (the two integers to
 * add). Logs an error and returns if the argument count is wrong.
 * Otherwise initialises the ROS 2 runtime, creates @ref SimpleServiceClient,
 * spins the node until shutdown, and cleans up.
 *
 * @param argc Number of command-line arguments (must be 3: program + A + B).
 * @param argv Array of command-line argument strings; `argv[1]` = A, `argv[2]` = B.
 * @return int Returns 0 on clean exit.
 */
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    if(argc != 3)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Wrong number of arguments! Usage: simple_service_client A B");
    }

    auto node = std::make_shared<SimpleServiceClient>(atoi(argv[1]), atoi(argv[2]));
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
