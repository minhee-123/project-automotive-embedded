#include <memory>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/u_int8.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

#include "skid_steer_vehicle/state.hpp"

class StateManagerNode : public rclcpp::Node {
public:
	StateManagerNode() : 
		Node("state_manager_node"),
		state(State::MANUAL),
		buttons_prev{0, }, 
		stopped(1), dirA(1), dirB(1) {
		pwm_pub_ = this->create_publisher<std_msgs::msg::UInt8MultiArray>("pwm_array", 10);
		aux_pub_ = this->create_publisher<std_msgs::msg::UInt8MultiArray>("aux_array", 10);
		state_pub_ = this->create_publisher<std_msgs::msg::UInt8>("vehicle_state", 10);
		joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
				"joy", 10,
				std::bind(&StateManagerNode::joy_callback, this, std::placeholders::_1)
				);
		pFin_sub_ = this->create_subscription<std_msgs::msg::UInt8>(
				"parking_finished", 10,
				std::bind(&StateManagerNode::pFin_callback, this, std::placeholders::_1)
				);
		aeb_sub_ = this->create_subscription<std_msgs::msg::UInt8>(
				"aeb", 10,
				std::bind(&StateManagerNode::aeb_callback, this, std::placeholders::_1)
				);
	}
private:
	State state;
	uint8_t buttons_prev[8];
	int stopped;
	uint8_t dirA, dirB;

	void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {
		std_msgs::msg::UInt8MultiArray aux_msg;
		aux_msg.data.resize(7);

		int send = 0;
		for (int i = 0; i < 7; i++)
			aux_msg.data[i] = 0;

		if (buttons_prev[0] == 0 && msg->buttons[0] == 1) {
			aux_msg.data[0] = 1;
			send++;
		}
		buttons_prev[0] = msg->buttons[0];

		if (buttons_prev[1] == 0 && msg->buttons[1] == 1) {
			aux_msg.data[1] = 1;
			send++;
		}
		buttons_prev[1] = msg->buttons[1];

		if (buttons_prev[2] == 0 && msg->buttons[3] == 1) {	// parking button
			aux_msg.data[2] = 1;
			send++;
			if (state == State::MANUAL) {
				state = State::PARKING;
				std_msgs::msg::UInt8 state_msg;
				state_msg.data = static_cast<uint8_t>(State::PARKING);
				state_pub_->publish(state_msg);
				RCLCPP_INFO(this->get_logger(), "State Changed: Parking");
			} else {
				RCLCPP_INFO(this->get_logger(), "Invalid State Transition: Parking");
			}
		}
		buttons_prev[2] = msg->buttons[3];

		if (buttons_prev[3] == 0 && msg->buttons[2] == 1) {
			aux_msg.data[3] = 1;
			send++;
		}
		buttons_prev[3] = msg->buttons[2];

		if (buttons_prev[4] == 0 && msg->buttons[4] == 1) {
			aux_msg.data[4] = 1;
			send++;
		}
		buttons_prev[4] = msg->buttons[4];

		if (buttons_prev[5] == 0 && msg->buttons[5] == 1) {
			aux_msg.data[5] = 1;
			send++;
		}
		buttons_prev[5] = msg->buttons[5];

		if (buttons_prev[6] == 0 && msg->buttons[12] == 1) {
			aux_msg.data[6] = 1;
			send++;
		}
		buttons_prev[6] = msg->buttons[12];

		if (send) {
			aux_pub_->publish(aux_msg);
			RCLCPP_INFO(this->get_logger(), "buttons message published");
		}

		if (msg->axes.size() < 4)
			return;

		if (state != State::MANUAL && state != State::EMERGENCY)
			return;

		if (std::fabs(msg->axes[1]) < 1e-4f) {
			if (stopped)
				return;
			else
				stopped = 1;
		} else
			stopped = 0;

		std_msgs::msg::UInt8MultiArray pwm_msg;
		pwm_msg.data.resize(4);

		int OFFSET = 64;
		float drive = (255-OFFSET) * msg->axes[1];
		if (drive > 0) {
			drive += OFFSET;
		} else if (drive < 0) {
			drive -= OFFSET;
		}
		float diff = drive * msg->axes[3];
		int pwmA = static_cast<int>(std::round(drive - diff));
		int pwmB = static_cast<int>(std::round(drive + diff));

		if (pwmA > 255)
			pwmA = 255;
		else if (pwmA < -255)
			pwmA = -255;
		if (pwmB > 255)
			pwmB = 255;
		else if (pwmB < -255)
			pwmB = -255;

		if (pwmA < 0) {
			pwm_msg.data[0] = 0;
			pwm_msg.data[1] = static_cast<uint8_t>(-pwmA);
			dirA = 0;
		} else if (pwmA > 0) {
			pwm_msg.data[0] = 1;
			pwm_msg.data[1] = static_cast<uint8_t>(pwmA);
			dirA = 1;
		} else {
			pwm_msg.data[0] = dirA;
			pwm_msg.data[1] = 0;
		}

		if (pwmB < 0) {
			pwm_msg.data[2] = 0;
			pwm_msg.data[3] = static_cast<uint8_t>(-pwmB);
			dirB = 0;
		} else if (pwmB > 0) {
			pwm_msg.data[2] = 1;
			pwm_msg.data[3] = static_cast<uint8_t>(pwmB);
			dirB = 1;
		} else {
			pwm_msg.data[2] = dirB;
			pwm_msg.data[3] = 0;
		}

		pwm_pub_->publish(pwm_msg);
	}


	void aeb_callback(const std_msgs::msg::UInt8::SharedPtr msg) {
		std_msgs::msg::UInt8 state_msg;

		if (msg->data) {
			state = State::EMERGENCY;
			state_msg.data = static_cast<uint8_t>(State::EMERGENCY);
			RCLCPP_INFO(this->get_logger(), "State Changed: EMERGENCY");
		} else {
			state = State::MANUAL;
			state_msg.data = static_cast<uint8_t>(State::MANUAL);
			RCLCPP_INFO(this->get_logger(), "State Changed: MANUAL");
		}

		state_pub_->publish(state_msg);
	}

	void pFin_callback(const std_msgs::msg::UInt8::SharedPtr msg) {
		std_msgs::msg::UInt8 state_msg;

		if (msg->data) {
			state = State::MANUAL;
			state_msg.data = static_cast<uint8_t>(State::MANUAL);
			state_pub_->publish(state_msg);
			RCLCPP_INFO(this->get_logger(), "State Changed: MANUAL");
		}
	}

	rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr pwm_pub_;
	rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr aux_pub_;
	rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr state_pub_;
	rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
	rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr pFin_sub_;
	rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr aeb_sub_;
};

int main(int argc, char* argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<StateManagerNode>());
	rclcpp::shutdown();
	return 0;
}
