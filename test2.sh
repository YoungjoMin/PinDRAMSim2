../../../pin -t obj-intel64/memorySystem.so -- timeout 100s "./obj-intel64/MatMul.exe"  #without RAIDR
echo "test without RAIDR ended"
../../../pin -t obj-intel64/memorySystem_RAIDR.so -- timeout 100s "./obj-intel64/MatMul.exe"  #without RAIDR
echo "test with RAIDR ended"