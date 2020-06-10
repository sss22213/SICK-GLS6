#include <ros/ros.h> 
#include <stdlib.h>
#include <string>
#include <serial/serial.h>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Quaternion.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64MultiArray.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Pose2D.h>
#include <vector>
#include "gls6.hpp"
using namespace std;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "gls6");     
    ros::NodeHandle n;
    // ID,IP,Port
    //gls6 gls6_machine_ary;
    gls6 gls6_machine(0,"192.168.0.24",2112);
    //
    ros::Publisher QRCode_pub = n.advertise<geometry_msgs::Pose>("qrcode_loc", 1024);
    ros::Publisher QRCode_Content_XY_pub = n.advertise<geometry_msgs::Pose2D>("qrcode_content", 1024);
    /*
    ros::Publisher IMU_pub = n.advertise<sensor_msgs::Imu>("imu_data", 1024);
    ros::Publisher IMU_Euler_Angle_pub = n.advertise<std_msgs::Float64MultiArray>("imu_euler_angle", 100);
    ros::Subscriber Odom_sub = n.subscribe("odom", 1000, Odom_CallBack);
    ros::Subscriber Odom_Euler_Angle_sub = n.subscribe("odom_euler_angle", 100, Odom_Euler_Angle_CallBack);
    */
    ros::Rate loop_rate(200);
    while(ros::ok()) 
    {
        QRCode_pub.publish(gls6_machine.gls6_recv());
        QRCode_Content_XY_pub.publish(gls6_machine.get_content());
        ros::spinOnce();
        loop_rate.sleep();
    }
}