# README CPP

To be able to use the new C++ nodes in this workspace, you have to do a few steps. 

1) Make sure that you pulled the IXWebsockets Git submodule located in this folder
2) Run `$ sudo apt install nlohmann-json3-dev` to download the json library


To build the code, run `$ cpp_build.sh` to compile the code. This script will compile the code in a debug mode with no compiler optimizations. You can modify the build command for a production deployment if you wish.
If you wish to build AND run the `shore_comms_cpp` node, run `$ ./cpp_run.sh`

If you wish to run the unit tests for the `shore_comms_cpp` package, run `$ ./cpp_test.sh`


## Contact
Ishaan Sayal - [isayal@stevens.edu](mailto:isayal@stevens.edu)
