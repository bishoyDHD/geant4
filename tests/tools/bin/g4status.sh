#!/usr/local/bin/bash
###################################################
# Check status of build&run or killing specific job
###################################################

PID=$1

if [ X$PID = X-h ]
then
echo
echo "Usage: g4status.sh [PID]" 
echo

exit
fi 


ACCOUNT=stesting

echo -e PID "\t" HOSTNAME "\t" DEV/PROD "\t" DEB/OPT
ps uaxww|grep "l $ACCOUNT"|grep rsh|sort -k 15|awk '{print $2,$14,$16,$17}' -|while read pid host devprod debopt
do

#if [ X`echo $pname|cut -d "." -f3` = Xd ]
# then
#    flavour=DEBUG
# else
#    flavour=OPTIM
#fi

echo -e $pid "\t" $host "\t" $devprod "\t\t" $debopt

#if [ X$PLATFORM = X$host -a X$MODE = X$flavour ]
#then
#    if [ X$SIGNAL  != X ]
#	then
#	    kill $pid
#	    exit
#	else
#	    echo $pid $host $flavour
#    fi
#else
#    if [ X$PLATFORM = Xall ]
#	then
#	    echo $pid,$host,$pnam
#	else
#	    
#    fi
#fi

done 

if [ X$PID != X ]
    then
	echo
	echo -n Indeed to stop $PID ?! [N]:" "
	read answer

	if [ X$answer = Y -o X$answer = y ]
	    then
		kill $PID
		exit
	fi

fi
