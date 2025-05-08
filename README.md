# Mycobot_280jn_gazebo
This is a simple gazebo simulation of the mycobot 280jn with gripper and camera in ros2 (humble)

# DESCRIPTION
This is a  gazebo simulation of my mycobot configuration for pick and place.

# INSTALLATION
```
mkdir -p mycobot_ws/src
cd mycobot_ws/src
git clone https://github.com/logesh1516/Mycobot_280jn_gazebo.git
cd ..
colcon build 
```
# GAZEBO
![demo](https://github.com/user-attachments/assets/65d1b784-50ca-47e5-aec1-8a8f47e063d8)


# SIMULATION
```
cd mycobot_ws
source install/setup.bash
ros2 launch mycobot_gazebo mycobotjn_control.launch.py
ros2 run mycobot_gazebo example_joint_trajectory_publisher_cpp
```
