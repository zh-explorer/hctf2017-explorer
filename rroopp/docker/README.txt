put you all binary in bin dir
change flag salt in getflag function
change you cmd in set_process's args, just seem like all files in bin dir are in current dir
change ssh key in dockerfile

than build docker
docker build -t "pwntest" .

and run docker, port 12345 is for Challenge and port 22 for ssh
docker run -d -p "0.0.0.0:pubport:12345" -p "0.0.0.0:pubport:22" -h "ptest" --name="ptest" pwntest


enter docker use ssh
run daemo use python /root/black_zone/zone/py


PS:remove checkToken and getName for private use
PS2:This docker base on https://github.com/zh-explorer/black_zone
