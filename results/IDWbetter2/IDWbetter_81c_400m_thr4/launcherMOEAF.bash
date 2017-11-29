#!/bin/bash

# launcher for GLEmodel experiments
PROBLEM_SETUP="IDWbetter_81c"  # something useful to characterize this problem setup
ALGORITHMS=( GDE3 ) #OMOPSO )
NFE=20000000
SEED_START=1
SEED_END=1
for SEED in $(seq ${SEED_START} ${SEED_END})
do
	for ALGORITHM in ${ALGORITHMS[@]}
	do
		# java -Xmx3g -cp ../javaLibs/GLEmodelMOEAinterface.jar:../javaLibs/MOEAFramework-1.17-Executable.jar:../javaLibs/snakeyaml-1.11.jar org.andreaEmanuele.MOLEM.GLEExecutorUtility -a ${ALGORITHM} -b GLEmodel -n ${NFE} -p 50 -e 10000.0,100000.0,0.1,0.00001 -i 0.25 -m 0.02,20.0 -x 1.0,15.0 -q ${SEED}.${PROBLEM_SETUP} -c ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.chkp,1000 -o output.${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.data &> ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.stdout &
		java -Xmx3g -cp ../javaLibs/GLEmodelMOEAinterface.jar:../javaLibs/MOEAFramework-1.17-Executable.jar:../javaLibs/snakeyaml-1.11.jar org.andreaEmanuele.MOLEM.GLEExecutorUtility -a ${ALGORITHM} -b GLEmodel -n ${NFE} -p 900 -e 10000.0,100000.0,0.1,0.00001 -d 0.9,0.9 -q ${SEED}.${PROBLEM_SETUP} -c ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.chkp,1000 -o output.${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.data &> ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.stdout &
	done
done
