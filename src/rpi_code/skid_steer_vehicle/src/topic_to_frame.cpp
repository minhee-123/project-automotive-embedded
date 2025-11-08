#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "can_msgs/msg/frame.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

class TopicToFrameNode : public rclcpp::Node {
public:
	TopicToFrameNode() : Node("topic_to_frame_node") {
		pub_ = this->create_publisher<can_msgs::msg::Frame>("to_can_bus", 10);
		pwm_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
				"pwm_array", 10,
				std::bind(&TopicToFrameNode::pwm_callback, this, std::placeholders::_1)
				);
		aux_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
				"aux_array", 10,
				std::bind(&TopicToFrameNode::aux_callback, this, std::placeholders::_1)
				);
	}
private:
	void pwm_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
		can_msgs::msg::Frame can_msg;

		can_msg.id = 0x100;
		can_msg.is_rtr = false;
		can_msg.is_extended = false;
		can_msg.is_error = false;
		can_msg.dlc = 4;
		can_msg.data[0] = msg->data[0];
		can_msg.data[1] = msg->data[1];
		can_msg.data[2] = msg->data[2];
		can_msg.data[3] = msg->data[3];
		can_msg.data[4] = 0;
		can_msg.data[5] = 0;
		can_msg.data[6] = 0;
		can_msg.data[7] = 0;

		can_msg.header.frame_id = "base_link";
		can_msg.header.stamp = this->now();

		pub_->publish(can_msg);
	}


	void aux_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
		can_msgs::msg::Frame can_msg;

		can_msg.id = 0x102;
		can_msg.is_rtr = false;
		can_msg.is_extended = false;
		can_msg.is_error = false;
		can_msg.dlc = 7;
		can_msg.data[0] = msg->data[0];
		can_msg.data[1] = msg->data[1];
		can_msg.data[2] = msg->data[2];
		can_msg.data[3] = msg->data[3];
		can_msg.data[4] = msg->data[4];
		can_msg.data[5] = msg->data[5];
		can_msg.data[6] = msg->data[6];
		can_msg.data[7] = 0;

		can_msg.header.frame_id = "base_link";
		can_msg.header.stamp = this->now();

		pub_->publish(can_msg);
	}

	rclcpp::Publisher<can_msgs::msg::Frame>::SharedPtr pub_;
	rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr pwm_sub_;
	rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr aux_sub_;
};

int main(int argc, char* argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<TopicToFrameNode>());
	rclcpp::shutdown();
	return 0;
}
