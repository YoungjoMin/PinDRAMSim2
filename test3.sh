rm dramsim.log

STARTTIME=$(date +%s)
../../../pin -t obj-intel64/memorySystem.so -- ./obj-intel64/fibonacci.exe 98765432198765432  #without RAIDR
ENDTIME=$(date +%s)
(( without+=(t2-t1)  ))
mv dramsim.log dramsim_without_RAIDR.log
echo "$(($ENDTIME - $STARTTIME)) seconds, test without RAIDR ended"



STARTTIME=$(date +%s)
../../../pin -t obj-intel64/memorySystem_RAIDR.so -- ./obj-intel64/fibonacci.exe 98765432198765432  #without RAIDR
ENDTIME=$(date +%s)
(( without+=(t2-t1)  ))
mv dramsim.log dramsim_with_RAIDR.log
echo "$(($ENDTIME - $STARTTIME)) seconds, test with RAIDR ended"