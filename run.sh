#!/bin/bash

exit_msg = "  Thanks for using Touse v1.0  "
#keyboard interrupt handler
control_c() {
	echo "ctrl+c pressed"
	echo "Killing processes..."
	echo -n "sudo killall touchscreen"
	echo -n "sudo killall 3_pyProcess*"
	echo "done"
	echo $exit_msg
	exit $?
}


echo "set title '************** Welcome to Touse v1.0 ***************' "
user = `whoami`
#if [ $user != 'root' ]
#then 
#	echo "You are not root"
#	exit
#fi

if [ "$(lsusb | grep -o 16c0:05df)" == "16c0:05df" ]
then
	### device detected.pop up notification
	notify-send "Touse:v1.0" "Running" -t 1000
#	exit #remove this
#	echo -n "sudo -s"
	echo "Running touchscreen"
	echo -n `./touchscreen` &   #tr -d ' '; echo -e '\t' `date "+%H:%M:%S"` 
		#echo -n "running touchscreen.cpp"
		#	echo -n  "running 3_pyProcess.py"
	sleep 1
	echo "python process"
	echo -n `python ./3_pyProcess.py` &
		#if [ "$(pidof gnuplot)" ]
		#	then
		#		true
		#else	
		#	gnuplot $path_of_conf_file/liveplot.gnu &> /dev/null &
	trap control_c SIGINT
	while true
	  do
		sleep 1	
	  done

else
	killall touchscreen &> /dev/null 
	echo "No Device found"
fi
