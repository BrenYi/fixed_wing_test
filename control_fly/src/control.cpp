#include <stdio.h>
#include <iostream>
#include <ros/ros.h>
#include <std_msgs/Int32.h>
using namespace std;
int main(int argc, char **argv)
{
    ros::init(argc, argv, "control_node");
    ros::NodeHandle nh;
    ros::Publisher set_state = nh.advertise<std_msgs::Int32>("/plane/fly_next_point",1);
    std_msgs::Int32 x;
    char character_input;
    while(ros::ok())
    {
        character_input = getchar();
        char m = ' ';
        if(int(character_input) == 32)  //按下空格发送指令
        {
            x.data = 1;
            for(int i=0; i<500; i++)
            {
                set_state.publish(x);
            }
            
            //printf(character_input);
            cout << character_input <<endl;
            ROS_INFO("sent");

        }
        //rewind(stdin);
    }



}