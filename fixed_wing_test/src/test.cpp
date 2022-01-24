#include <ros/ros.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/PositionTarget.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/State.h>
#include <geometry_msgs/PoseStamped.h>
#include <cmath> 
#include <std_msgs/Int32.h>
#include <iostream>
using namespace std;

mavros_msgs::State current_state;
float cur_position[3];
int next_fly_point =0;
void flag_change(const std_msgs::Int32::ConstPtr& msg)
{
    
    
    next_fly_point = msg->data;
        //ROS_INFO(next_fly_point);
    //cout << "1: "<<next_fly_point <<endl;
    
}
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

void sub_position(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    cur_position[0] =  msg->pose.position.x;
    cur_position[1] =  msg->pose.position.y;
    cur_position[2] =  msg->pose.position.z;
}

/*
mavros_msgs::PositionTarget set_fly_position(int type, float x, float y, float z)
{
    mavros_msgs::PositionTarget raw_position;
    raw_position.coordinate_frame = 9;
    raw_position.type_mask = type;
    raw_position.position.x = x;
    raw_position.position.y = y;
    raw_position.position.z = z;
    return raw_position;
}
*/
geometry_msgs::PoseStamped set_fly_position(float x, float y, float z)
{
    geometry_msgs::PoseStamped raw_position;
    //raw_position.coordinate_frame = 9;
    //raw_position.type_mask = type;
    raw_position.pose.position.x = x;
    raw_position.pose.position.y = y;
    raw_position.pose.position.z = z;
    return raw_position;
}

int main(int argc, char **argv)
{
    bool offboard_flag;
    ros::init(argc, argv, "test_node");
    ros::NodeHandle nh;
    ros::Publisher send_position = nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",10);
    ros::ServiceClient setmode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
    ros::ServiceClient arm_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
    ros::Subscriber control_pos = nh.subscribe<std_msgs::Int32>("/plane/fly_next_point",10, flag_change);
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);
    ros::Subscriber position_subs = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose",10, sub_position); 
    mavros_msgs::CommandBool flag;
    //mavros_msgs::PositionTarget point_sent;
    geometry_msgs::PoseStamped point_sent;
    mavros_msgs::SetMode mode_change;
    offboard_flag = true;
    ros::Rate rate(20.0);
    float point[3][3]={100,0,40,340,50,30,500,-80,50};
    int index=0;
    while(ros::ok() && !current_state.connected)
    {
        ros::spinOnce();
        rate.sleep();
    }

    mode_change.request.custom_mode = "AUTO.TAKEOFF";
    flag.request.value = true;
    //bool ros::Duration::sleep();
    if(arm_client.call(flag))
    {
        
        ROS_INFO("TAKE_OFF_MODE CHANGE!");
        setmode_client.call(mode_change);
        ros::spinOnce();
        ros::Duration(3).sleep();        
        //mode_change.request.custom_mode = "OFFBOARD";
        //setmode_client.call(mode_change);
        
    }
    
    mode_change.request.custom_mode = "OFFBOARD";
    setmode_client.call(mode_change);
    while(ros::ok())
    {
        //mode_change.request.custom_mode = "OFFBOARD";
        
        point_sent = set_fly_position(point[index][0],point[index][1],point[index][2]);
        if(offboard_flag)
        {
            ROS_INFO("SENT");
            offboard_flag = false;
        }
        
        send_position.publish(point_sent);
        if(sqrt(pow(cur_position[0]-point[index][0],2) + pow(cur_position[1]-point[index][1],2) + pow(cur_position[2]-point[index][2],2)) < 21.21 && index < 2 && next_fly_point ==1 )
        {
            index ++; 
            ROS_INFO("INDEX++++");
            next_fly_point = 0;
        }
        ros::spinOnce();
        rate.sleep();
    }

}
/*
token
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
*/