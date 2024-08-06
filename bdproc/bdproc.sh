#!/usr/bin/env sh
##########################################
## Check the README.md for more details ##
##########################################

ID=$(id -u);
if [ $ID -eq 0 ]
then
	REG=$(rdmsr -d 0x1FC)
	if [ $(($REG & 1)) -eq 0 ]
	then
		if [ "$1" = "-d" ]
		then
			exit 0;
		fi
		# Enable BD PROCHOT
		REG=$((REG+1));
	else
		if [ "$1" = "-e" ]
		then
			exit 0;
		fi
		# Disable BD PROCHOT
		REG=$((REG-1));
	fi

	# Apply changes
	wrmsr -a 0x1FC $REG;
else
	echo "You are not root!";
	exit 1;
fi
