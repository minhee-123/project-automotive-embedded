#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>
#include <std_msgs/msg/u_int8.hpp>
#include <std_msgs/msg/u_int8_multi_array.hpp>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "skid_steer_vehicle/state.hpp"

using namespace std::chrono_literals;

class TcpSenderNode : public rclcpp::Node
{
public:
    TcpSenderNode()
        : Node("tcp_sender_node"),
        mode_("Remote"),
        aux_l_(0),
        aux_r_(0),
        rpm_l_(0),
        rpm_r_(0)
    {
        // Subscriptions
        vehicle_state_sub_ = this->create_subscription<std_msgs::msg::UInt8>(
            "vehicle_state", 10,
            std::bind(&TcpSenderNode::vehicleStateCallback, this, std::placeholders::_1));
        aux_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
            "aux_array", 10,
            std::bind(&TcpSenderNode::auxCallback, this, std::placeholders::_1));
        rpm_sub_ = this->create_subscription<std_msgs::msg::Int32MultiArray>(
            "wheel_rpm", 10,
            std::bind(&TcpSenderNode::rpmCallback, this, std::placeholders::_1));

        // Initialize TCP server
        setupServer();

        // Timer for sending data
        timer_ = this->create_wall_timer(
            100ms, std::bind(&TcpSenderNode::sendLoop, this));
    }

    ~TcpSenderNode()
    {
        if (client_sock_ != -1) {
            close(client_sock_);
        }
        if (server_sock_ != -1) {
            close(server_sock_);
        }
    }

private:
    void setupServer()
    {
        server_sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to create socket");
            rclcpp::shutdown();
            return;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
		//addr.sin_addr.s_addr = inet_addr("192.168.20.237"); //raspberry pi ip address
        addr.sin_port = htons(PORT);

        int opt = 1;
        setsockopt(server_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(server_sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            RCLCPP_ERROR(this->get_logger(), "Bind failed");
            rclcpp::shutdown();
            return;
        }

        if (listen(server_sock_, 1) < 0) {
            RCLCPP_ERROR(this->get_logger(), "Listen failed");
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Waiting for TCP client on port %d...", PORT);
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        client_sock_ = accept(server_sock_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_sock_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Accept failed");
            rclcpp::shutdown();
            return;
        }
        RCLCPP_INFO(this->get_logger(), "Client connected: %s:%d",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));
    }

    void vehicleStateCallback(const std_msgs::msg::UInt8::SharedPtr msg)
    {
	if (msg->data == static_cast<uint8_t>(State::MANUAL))
	    mode_ = "Remote";
	else if (msg->data == static_cast<uint8_t>(State::PARKING))
	    mode_ = "Parking";
	else if (msg->data == static_cast<uint8_t>(State::EMERGENCY))
	    mode_ = "AEB";
	else
	    mode_ = "NaN";
    }

    void auxCallback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg)
    {
	if (msg->data[4])
	    aux_l_ ^= 1;
	if (msg->data[5])
	    aux_r_ ^= 1;
    }

    void rpmCallback(const std_msgs::msg::Int32MultiArray::SharedPtr msg)
    {
        rpm_l_ = msg->data[0];
	rpm_r_ = msg->data[1];
    }

    void sendLoop()
    {
        if (client_sock_ < 0) {
            return;
        }
        // Format: "mode,auxL,auxR,rpmL,rpmR\n"
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "%s,%d,%d,%d,%d\n",
            mode_.c_str(), aux_l_, aux_r_, rpm_l_, rpm_r_);
        if (send(client_sock_, buf, len, 0) < 0) {
            RCLCPP_ERROR(this->get_logger(), "Send failed");
            // Attempt to close and reset client
            close(client_sock_);
            client_sock_ = -1;
        }
    }

    // Subscribers
    rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr vehicle_state_sub_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr aux_sub_;
    rclcpp::Subscription<std_msgs::msg::Int32MultiArray>::SharedPtr rpm_sub_;

    // Timer
    rclcpp::TimerBase::SharedPtr timer_;

    // TCP sockets
    static constexpr int PORT = 7000;
    int server_sock_{ -1 };
    int client_sock_{ -1 };

    // Latest data
    std::string mode_;
    int aux_l_;
    int aux_r_;
    int rpm_l_;
    int rpm_r_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TcpSenderNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
