#!/bin/bash

# launcher for GLEmodel experiments
PROBLEM_SETUP="IDWbetter_256c_15m_thr4_TEEvsSH"  # something useful to characterize this problem setup
ALGORITHMS=( eNSGAII ) #GDE3 ) #OMOPSO )
NFE=2000000
SEED_START=1
SEED_END=10
EPSILON=1000.0,0.000001
for SEED in $(seq ${SEED_START} ${SEED_END})
do
	for ALGORITHM in ${ALGORITHMS[@]}
	do
		if [ ${ALGORITHM} == "eNSGAII" ]
		then 
			java -Xmx3g -cp ../javaLibs/GLEmodelMOEAinterface.jar:../javaLibs/MOEAFramework-1.17-Executable.jar:../javaLibs/snakeyaml-1.11.jar org.andreaEmanuele.MOLEM.GLEExecutorUtility -a ${ALGORITHM} -b GLEmodel -n ${NFE} -p 50 -e ${EPSILON} -i 0.25 -m 0.02,20.0 -x 1.0,15.0 -q ${SEED}.${PROBLEM_SETUP} -c ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.chkp,1000 -o output.${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.data &> ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.stdout &
		fi
		if [ ${ALGORITHM} == "GDE3" ]
		then
			java -Xmx3g -cp ../javaLibs/GLEmodelMOEAinterface.jar:../javaLibs/MOEAFramework-1.17-Executable.jar:../javaLibs/snakeyaml-1.11.jar org.andreaEmanuele.MOLEM.GLEExecutorUtility -a ${ALGORITHM} -b GLEmodel -n ${NFE} -p 500 -e ${EPSILON} -d 0.25,0.9 -q ${SEED}.${PROBLEM_SETUP} -c ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.chkp,1000 -o output.${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.data &> ${PROBLEM_SETUP}.${ALGORITHM}.${SEED}.stdout &
		fi
		sleep 1
	done
done
