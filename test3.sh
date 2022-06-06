rm dramsim.log
nums={98765432198765432 98765432119876543 12345678912345678 98765478945698765}

STARTTIME=$(date +%s)
../../../pin -t obj-intel64/memorySystem.so -- ./obj-intel64/MatMul.exe  #without RAIDR
ENDTIME=$(date +%s)
(( without+=(t2-t1)  ))
mv dramsim.log dramsim_without_RAIDR.log
echo "$(($ENDTIME - $STARTTIME)) seconds, test without RAIDR ended"



STARTTIME=$(date +%s)
../../../pin -t obj-intel64/memorySystem_RAIDR.so -- ./obj-intel64/MatMul.exe  #with RAIDR
ENDTIME=$(date +%s)
(( without+=(t2-t1)  ))
mv dramsim.log dramsim_with_RAIDR.log
echo "$(($ENDTIME - $STARTTIME)) seconds, test with RAIDR ended"


../../../pin -t obj-intel64/memorySystem.so -- ./obj-intel64/fibonacci.exe 123456987654 #without RAIDR
../../../pin -t obj-intel64/memorySystem_RAIDR.so -- ./obj-intel64/fibonacci.exe 123456987654 #with RAIDR