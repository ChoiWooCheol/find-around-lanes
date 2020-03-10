# find-around-lanes
* autoware에서 waypoint loader를 사용할 때, 차량 주변의 lane들을 파악하여 lane change를 할 수 있는 lane만 planning 가능하도록 republish 해주는 패키지 입니다.
* autoware의 lane change 기능을 대회에서 구현하기 위하여 제작하였습니다.

# Method
* autoware의 waypoint_replanner의 publihser를 변경합니다. ("/lane_waypoints_array" -> "/lane_waypoints_array2")
* autoware의 waypoint_maker package build
* vectormap의 주행가능한 dtLane을 모두 waypoint용 csv파일로 만들고 로드하면 local planner에서 trajectory를 생성하는데 문제가 발생합니다.
* 따라서 현재 자동차 위치를 계산하여 자동차가 변경할 수 있는 lane을 파악하고 로드해주여야 계산시간도 적고, 문제가 발생하지 않습니다.
* 따라서 자동차가 lane change할 수 있는 lane들을 local planner에게 republish 해줍니다.

# Run
```sh
$ rosrun calc_around_waypoints calc_arount_waypoints
``` 

# esult
1. Segmentation boxes & obstacle detection boxes
<img src="LiDAR_obj_detect_pkg/images/Screenshot from 2020-01-20 16-06-20.png" width="100%" height="100%">
