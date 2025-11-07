# README CPP

To be able to use the new C++ nodes in this workspace, you have to do a few steps. 

1) Make sure that you pulled the IXWebsockets Git submodule located in this folder
2) Run `$ sudo apt install nlohmann-json3-dev` to download the json library


To build the code, run `$ cpp_build.sh` to compile the code. This script will compile the code in a debug mode with no compiler optimizations. You can modify the build command for a production deployment if you wish.
If you wish to build AND run the test_node, run `$ cpp_run.sh`
