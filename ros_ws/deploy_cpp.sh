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

verify_login() {
 sshpass -p "$EBOAT_PASSWORD" ssh -o StrictHostKeyChecking=no -o ConnectTimeout=5 eboat@"$1" exit
  if [ $? -eq 0 ]; then
    echo "Credentials Validated"
  else
    echo "Wrong credientials!"
    exit 1
  fi
}

deploy_code_to_pc () {
  echo "Deploying Code to MiniPC"
  verify_login $1

  if ! sshpass -p "$EBOAT_PASSWORD" rsync --exclude={".vscode",".idea","log","build"} --delete -aLzPh . eboat@"$1":~/eboat_src/ros_ws/; then
    echo -e "\e[31m[ERR] Unable to deploy to MiniPC\e[0m"
    exit 1
  fi
  echo -e "\e[32mSuccess!\e[0m"
  exit 0
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

#build code
./cpp_deploy_build.sh

try_ip "eboat.local"
try_ip "10.3.141.1"
try_ip "192.168.55.1"

# TailScale IP
try_ip "eboat-nuc.tail617ae5.ts.net"


if $FOUND_IP;then
  echo ""
else
    echo -e "\e[31m[ERR] Unable to find ANY IP to Deploy\e[0m"
fi


