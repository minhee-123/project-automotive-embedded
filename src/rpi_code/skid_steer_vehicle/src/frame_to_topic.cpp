#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "can_msgs/msg/frame.hpp"

class FrameToTopicNode : public rclcpp::Node {
public:
	FrameToTopicNode() : Node("frame_to_topic_node") {
		rpm_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("wheel_rpm", 10);
		pFin_pub_ = this->create_publisher<std_msgs::msg::UInt8>("parking_finished", 10);
		aeb_pub_ = this->create_publisher<std_msgs::msg::UInt8>("aeb", 10);
		sub_ = this->create_subscription<can_msgs::msg::Frame>(
				"/from_can_bus", 10,
				std::bind(&FrameToTopicNode::can_callback, this, std::placeholders::_1)
				);
	}
private:
	void can_callback(const can_msgs::msg::Frame::SharedPtr msg) {
		if (msg->id == 0x201) {
			std_msgs::msg::Int32MultiArray rpm_msg;
			rpm_msg.data.resize(2);

			rpm_msg.data[0] =
				static_cast<int32_t>(msg->data[0])
				| (static_cast<int32_t>(msg->data[1]) << 8)
				| (static_cast<int32_t>(msg->data[2]) << 16)
				| (static_cast<int32_t>(msg->data[3]) << 24);

			rpm_msg.data[1] =
				static_cast<int32_t>(msg->data[4])
				| (static_cast<int32_t>(msg->data[5]) << 8)
				| (static_cast<int32_t>(msg->data[6]) << 16)
				| (static_cast<int32_t>(msg->data[7]) << 24);

			rpm_pub_->publish(rpm_msg);
		} else if (msg->id == 0x300) {
			std_msgs::msg::UInt8 aeb_msg;
			aeb_msg.data = msg->data[0];
			aeb_pub_->publish(aeb_msg);
		} else if (msg->id == 0x301) {
			std_msgs::msg::UInt8 pFin_msg;
			pFin_msg.data = msg->data[0];
			pFin_pub_->publish(pFin_msg);
		}
	}

	rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr rpm_pub_;
	rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr pFin_pub_;
	rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr aeb_pub_;
	rclcpp::Subscription<can_msgs::msg::Frame>::SharedPtr sub_;
};

int main(int argc, char* argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<FrameToTopicNode>());
	rclcpp::shutdown();
	return 0;
}
