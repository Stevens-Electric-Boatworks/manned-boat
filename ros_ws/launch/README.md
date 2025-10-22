# Launch Files

This folder contains all the launch files for use with ROS. Please familiarize yourself with the ROS launch file documentation.

* `eboat_real.yaml`
  * This is the one that runs all the real code, looking for real serial devices, and will publish real data.
* `eboat_canmotor.yaml` (DEPRECATED)
  * An old launch file which is meant to just run the motion node

## Test Launch Files

These test files are special, in that they are used for debugging the shore, generating test data, or replaying back `ros bag` files in a way that will mimic the first time around.

* `test_eboat_allnodes.yaml`
  * This launch file will generate random test data for use in verifying shore components
  * It only publishes data that would actually exist on the shore 
  * You can run this by itself

* \[!]`replay.yaml` \[!]
  * This replay launch is special, in that the nodes are reconfigured for replay mode
  * If you wish to replay data, follow the below steps
    * Run this launch file with `$ ros2 launch launch/replay.yaml`
    * We will use the `$ ros bag play` command to replay your ROS bag files, but with some specific settings to allow for full replayability
      * `$ ros2 bag play --publish-service-requests {BAG_FILE} --remap /rosout:=/logout`
        * We remap `/rosout` to `/logout` so we can seperate log files made RIGHT NOW as we are running the replay, and the old log files from the bag file
        * We also tell it to send service requests so the `alarms_manager_node` is able to replay that data
    * Make sure to launch this file first, then replay the bag file
* 