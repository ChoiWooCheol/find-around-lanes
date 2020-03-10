#include<ros/ros.h>
#include<autoware_msgs/LaneArray.h>
#include<autoware_msgs/Lane.h>
#include<geometry_msgs/PoseStamped.h>

#include<cmath>
#include<cstdlib>
#include<vector>

#define DISTTHRESHOLD 5.0

class CalcAroundWaypoints{
public:
    CalcAroundWaypoints()
    : cur_x(0.0)
    , cur_y(0.0)
    , cur_z(0.0)
    , subscribe_ok(false)
    {
        lane_sub = nh.subscribe("/lane_waypoints_array2", 10, &CalcAroundWaypoints::LaneCallback, this);
        cur_pose_sub = nh.subscribe("/current_pose", 1, &CalcAroundWaypoints::PoseCallback, this);
        lane_pub = nh.advertise<autoware_msgs::LaneArray>("lane_waypoints_array", 10, true);
        check_same.clear();
        check_same.resize(0);
    }

    bool DistanceCheck(double x, double y, double z){
        double dist;

        dist = 
            sqrt(pow(cur_x - x, 2) 
                + pow(cur_y - y, 2)
                + pow(cur_z - z, 2));

        if(dist > DISTTHRESHOLD)
            return false;
        else
            return true;
    }

    void LaneCallback(const autoware_msgs::LaneArray::ConstPtr& in_lane){
        tmp_lane.id = in_lane->id;
        tmp_lane.lanes = in_lane->lanes;

        if (tmp_lane.lanes.size() != NULL) {
            subscribe_ok = true;
            ROS_INFO("Subscribe OK!");
            ROS_INFO("Total Lane size : %d", tmp_lane.lanes.size());
        }
    }

    void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& in_pose){
        cur_x = in_pose->pose.position.x;
        cur_y = in_pose->pose.position.y;
        cur_z = in_pose->pose.position.z;
    }

    bool isReady(){
        return subscribe_ok;
    }

    bool checkSameLane(std::vector<int>& cur_lanes, std::vector<int>& prev_lanes){
        if(cur_lanes.size() != prev_lanes.size()) 
            return false;

        for(int i = 0; i < cur_lanes.size(); i++){
            if(cur_lanes[i] != prev_lanes[i])
                return false;
        }

        return true;
    }

    void mainLoop(){
        autoware_msgs::LaneArray output_lane;
        output_lane.id = tmp_lane.id;
        output_lane.lanes.clear();
        output_lane.lanes.resize(0);

        std::vector<int> check_same_tmp;
        check_same_tmp.clear();
        check_same_tmp.resize(0);

        bool is_near = false;
        uint seq = 0;

        for(auto& lane : tmp_lane.lanes){
            seq++;
            for(auto& waypoint : lane.waypoints){
                if(DistanceCheck(waypoint.pose.pose.position.x
                                ,waypoint.pose.pose.position.y
                                ,waypoint.pose.pose.position.z))
                {
                    is_near = true;
                    break;
                }
            }

            if(is_near){
                output_lane.lanes.emplace_back(lane);
                is_near = false;
                check_same_tmp.emplace_back(seq);
            }
        }
        if(!checkSameLane(check_same_tmp,check_same)){
            lane_pub.publish(output_lane);
            check_same = check_same_tmp;
            ROS_INFO("Changed around lanes!");
        }
    }

private:
    ros::NodeHandle nh;
    ros::Subscriber lane_sub;
    ros::Subscriber cur_pose_sub;

    std::vector<int> check_same;
    
    autoware_msgs::LaneArray tmp_lane;
    
    ros::Publisher lane_pub;
    bool subscribe_ok;
    double cur_x, cur_y, cur_z;
};

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "calc_around_waypoints");
    CalcAroundWaypoints calc_waypoints;
    ros::Rate loop_rate(3);
    while(ros::ok()){
        ros::spinOnce();
        if(calc_waypoints.isReady()){
            calc_waypoints.mainLoop();
        }
        loop_rate.sleep();
    }
    return 0;
}
