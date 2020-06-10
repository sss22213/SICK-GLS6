#pragma once
#include <ros/ros.h> 
#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <tf/transform_broadcaster.h>
#include <cmath>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Pose2D.h>
using namespace std;

class gls6
{
    private:
        // gls6 node id
        int id;
        // socket 
        int sockfd;
        // gls6 ip address
        std::string IP;
        // gls6 port (default: 2111, 2112)
        int port;
        char *buffer;
        int alloc_recv_buffer_size;
        geometry_msgs::Pose QRCode_pose;
        geometry_msgs::Pose2D QRCode_content;
    public:
        gls6(int ID,std::string IP,int port,int alloc_recv_buffer_size = 50)
        {
            this->id = ID; 
            this->IP = IP;
            this->port = port;
            this->sockfd = socket(AF_INET , SOCK_STREAM , 0);
            // alloc memory to socket receive buffer
            buffer = (char*)malloc(sizeof(char)*alloc_recv_buffer_size);
            //
            this->alloc_recv_buffer_size = alloc_recv_buffer_size;
            // connect gls6
            if(this->gls_connect()==false)
            {
                ROS_INFO_STREAM("GLS6 Connect Fail");
            }
            else
            {
                ROS_INFO_STREAM("GLS6 Connect Success");
            }
        };
        ~gls6(){shutdown(this->sockfd,SHUT_RDWR);};
        bool gls_connect()
        {
            struct sockaddr_in info;
            bzero(&info,sizeof(info));
            info.sin_family = PF_INET;
            info.sin_addr.s_addr = inet_addr(this->IP.c_str());
            info.sin_port = htons(this->port);
            int err = connect(this->sockfd,(struct sockaddr*)&info,sizeof(info));
            if(err == -1)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        // ID get
        int id_get()
        {
            return this->id;
        }
        // receive GLS6 Data
        geometry_msgs::Pose gls6_recv()
        {
            memset(this->buffer,0,sizeof(char)*this->alloc_recv_buffer_size);
            int recv_length = recv(this->sockfd,this->buffer,sizeof(char)*this->alloc_recv_buffer_size,0);
            return this->get_pose();
        }
        //
        geometry_msgs::Pose2D get_content()
        {
            return this->QRCode_content;
        } 
        //
        geometry_msgs::Pose get_pose()
        {
            geometry_msgs::Pose gls6_pose;
            std::string pose_info_string = this->find_info();
            //
            std::vector<std::string> pos;
            /*
            std::size_t found = pose_info_string.find(",");
            cout << pose_info_string[found+1] << endl;
            */
            std::size_t loc = 0;
            while( loc != std::string::npos )
            {
                int loc_prv = loc;
                loc = pose_info_string.find(",",loc + 1);
                if( loc!=std::string::npos )
                {
                    pos.push_back(pose_info_string.substr(loc_prv + 1, loc - loc_prv -1));
                }
            }
            if(pos.size() > 3)
            {
                // content location
                int x_loc = pos[1].find("x");
                int y_loc = pos[1].find("y");
                this->QRCode_content.x = std::stoi(pos[1].substr(x_loc + 1, 3));
                this->QRCode_content.y = std::stoi(pos[1].substr(y_loc + 1, 3));
                /*
                ROS_INFO_STREAM( std::stoi(pos[1].substr(x_loc + 1, 3)) );
                ROS_INFO_STREAM( std::stoi(pos[1].substr(y_loc + 1, 3)) );
                */
                //QRCode_content.x
                //QRCode_content.y
                gls6_pose.position.x = std::stof(pos[2]);
                gls6_pose.position.y = std::stof(pos[3]);
                gls6_pose.orientation = tf::createQuaternionMsgFromYaw( (std::stof(pos[4]) - 180.0) * M_PI / 180.0);
            }
            return gls6_pose;
        }
        //
        int find_specific_byte(char *byte_array,char byte_spec,int byte_begin,int byte_end)
        {
            int idx = -1;
            for(int byte_array_idx = byte_begin; byte_array_idx < byte_end; ++byte_array_idx)
            {
                if(byte_array[byte_array_idx] == byte_spec)
                {
                    idx = byte_array_idx;
                }
            }
            return idx;
        }
        //
        std::string find_info()
        {
            // Find data begin address
            int idx = this->find_specific_byte(this->buffer,0x02,0,this->alloc_recv_buffer_size);
            std::string position_info;
            //std::vector<char> info;
            //
            char *ptr_package = this->buffer + idx;
            while( (*ptr_package) != 0x03 )
            {
                position_info.push_back(*ptr_package);
                ++ptr_package;
            }
            return position_info;
        }

};

// gls6 ary
class gls6_ary_container
{
    private:
        gls6 *gls6_container;
    public:
        gls6_ary_container(int *ID,std::string *IP,int *port,int length)
        {

        }

};
