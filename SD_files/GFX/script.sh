#!/bin/bash

for(( A=0;$A<63;A++ )) do 
	if [[ $A -lt 10 ]] ; then 
		B="000";
	elif [[ $A -lt 100 ]] ; then
		B="00";
	else
		B="0";
	fi;
	

	cat "jtm"$A".txt" >> "Jtm.txt";
done;
