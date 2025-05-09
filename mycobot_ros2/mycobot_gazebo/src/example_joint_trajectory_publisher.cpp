#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "std_msgs/msg/header.hpp"

using namespace std::chrono_literals;

// Define constants
const std::vector<std::string> arm_joints = {
    "link1_to_link2",
    "link2_to_link3",
    "link3_to_link4",
    "link4_to_link5",
    "link5_to_link6",
    "link6_to_link6flange"
};

const std::vector<std::string> gripper_joints = {
    "gripper_controller"
};

class ExampleJointTrajectoryPublisherCpp : public rclcpp::Node
{
public:
    ExampleJointTrajectoryPublisherCpp()
        : Node("example_joint_trajectory_publisher_cpp")
    {
        // Create the publisher of the desired arm and gripper goal poses
        arm_pose_publisher_ = create_publisher<trajectory_msgs::msg::JointTrajectory>("/arm_controller/joint_trajectory", 1);
        gripper_pose_publisher_ = create_publisher<trajectory_msgs::msg::JointTrajectory>("/grip_controller/joint_trajectory", 1);

        // Create a timer to periodically call the timerCallback function
        timer_ = create_wall_timer(5s, std::bind(&ExampleJointTrajectoryPublisherCpp::timerCallback, this));

        frame_id_ = "base_link";

        // Desired time from the trajectory start to arrive at the trajectory point.
        // Needs to be less than or equal to the timer period above to allow
        // the robotic arm to smoothly transition between points.
        duration_sec_ = 2;
        duration_nanosec_ = 0.5 * 1e9;  // (seconds * 1e9)

        // Set the desired goal poses for the robotic arm (3 different positions)
        arm_positions_ = {
            {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},  // Home position
            {-1.0, -0.5, 0.5, -0.5, 0.5, -1.0},  // Position 1
            {0.5, -1.0, 1.0, -0.8, 0.3, -0.5},   // Position 2
            {1.0, -0.3, 0.2, -1.0, 0.7, -1.2}    // Position 3
        };

        gripper_positions_ = {
            {0.0},    // Open gripper
            {-0.5},   // Partially closed
            {-0.7},  // Closed
            {0.0}     // Open again
        };

        // Keep track of the current trajectory we are executing
        index_ = 0;
    }

private:
    void timerCallback()
    {
        // Create new JointTrajectory messages for arm and gripper
        auto msg_arm = trajectory_msgs::msg::JointTrajectory();
        msg_arm.header.frame_id = frame_id_;
        msg_arm.joint_names = arm_joints;

        auto msg_gripper = trajectory_msgs::msg::JointTrajectory();
        msg_gripper.header.frame_id = frame_id_;
        msg_gripper.joint_names = gripper_joints;

        // Create JointTrajectoryPoints for arm and gripper
        auto point_arm = trajectory_msgs::msg::JointTrajectoryPoint();
        point_arm.positions = arm_positions_[index_];
        point_arm.time_from_start = rclcpp::Duration(duration_sec_, duration_nanosec_);
        msg_arm.points.push_back(point_arm);
        arm_pose_publisher_->publish(msg_arm);

        auto point_gripper = trajectory_msgs::msg::JointTrajectoryPoint();
        point_gripper.positions = gripper_positions_[index_];
        point_gripper.time_from_start = rclcpp::Duration(duration_sec_, duration_nanosec_);
        msg_gripper.points.push_back(point_gripper);
        gripper_pose_publisher_->publish(msg_gripper);

        // Log the current position
        RCLCPP_INFO(this->get_logger(), "Moving to position %ld", index_);

        // Update the index to cycle through positions
        index_ = (index_ + 1) % arm_positions_.size();
    }

    // Publishers for arm and gripper joint trajectories
    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr arm_pose_publisher_;
    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr gripper_pose_publisher_;

    // Timer for periodic callback
    rclcpp::TimerBase::SharedPtr timer_;

    // Frame ID for the joint trajectories
    std::string frame_id_;

    // Duration for each trajectory point
    int duration_sec_;
    int duration_nanosec_;

    // Desired goal poses for the robotic arm and gripper
    std::vector<std::vector<double>> arm_positions_;
    std::vector<std::vector<double>> gripper_positions_;

    // Index to keep track of the current trajectory point
    size_t index_;
};

int main(int argc, char * argv[])
{
    // Initialize the ROS 2 client library
    rclcpp::init(argc, argv);

    // Create an instance of the ExampleJointTrajectoryPublisherCpp node
    auto node = std::make_shared<ExampleJointTrajectoryPublisherCpp>();

    // Spin the node to execute the callbacks
    rclcpp::spin(node);

    // Shutdown the ROS 2 client library
    rclcpp::shutdown();

    return 0;
}
