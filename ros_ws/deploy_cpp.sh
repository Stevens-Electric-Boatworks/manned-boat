#! /bin/bash

echo "     ============="
echo "Stevens Electric Boatworks"
echo "     ============="


echo "Verifying Dependency"
if dpkg-query -W -f='${Status}' "sshpass" 2>/dev/null | grep -q "ok installed"; then
  echo "Dependency Installed"
else
  echo "Not installed"
  sudo apt-get install sshpass
fi


echo "Building nodes (CPP)"
#./cpp_build.sh

echo "Searching for MiniPC"

deploy_code_to_pc () {
  echo "Deploying Code to MiniPC"
  if ! sshpass -p "$EBOAT_PASSWORD" rsync -a build eboat@"$1":~/eboat_src/ros_ws/build; then
    echo -e "\e[31m[ERR] Unable to deploy to MiniPC\e[0m"
    exit 1
  fi
  echo -e "\e[32mSuccess!\e[0m"
}

FOUND_IP=false
try_ip() {
  if ping -c 1 "$1" &> /dev/null
  then
    echo "Found MiniPC on $1"
    FOUND_IP=true
    deploy_code_to_pc "$1"
  else
    echo -e "\e[31m[ERR] Could not find MiniPC on IP $1 \e[0m"
  fi
}

try_ip "mini-pc.local"
try_ip "localhost"

# TailScale IP
try_ip "eboat-nuc.tail617ae5.ts.net"


if $FOUND_IP;then
  echo ""
else
    echo -e "\e[31m[ERR] Unable to find ANY IP to Deploy\e[0m"
fi


