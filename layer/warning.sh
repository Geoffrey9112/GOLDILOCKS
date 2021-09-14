rm -rf staticWarning
mkdir staticWarning

cd Standard         && make clean && make 2>&1 | tee ../staticWarning/compileST.txt && cd ..
cd DataType         && make clean && make 2>&1 | tee ../staticWarning/compileDT.txt && cd ..
cd Communication    && make clean && make 2>&1 | tee ../staticWarning/compileCM.txt && cd ..
cd Kernel           && make clean && make 2>&1 | tee ../staticWarning/compileKN.txt && cd ..
cd StorageManager   && make clean && make 2>&1 | tee ../staticWarning/compileSM.txt && cd ..
cd ExecutionLibrary && make clean && make 2>&1 | tee ../staticWarning/compileEL.txt && cd ..
cd SqlProcessor     && make clean && make 2>&1 | tee ../staticWarning/compileQP.txt && cd ..
cd Session          && make clean && make 2>&1 | tee ../staticWarning/compileSS.txt && cd ..
cd InstanceManager  && make clean && make 2>&1 | tee ../staticWarning/compileIM.txt && cd ..
cd GlieseLibrary    && make clean && make 2>&1 | tee ../staticWarning/compileZL.txt && cd ..
cd GlieseTool       && make clean && make 2>&1 | tee ../staticWarning/compileZT.txt && cd ..

sST=`grep "warning" staticWarning/compileST.txt | wc -l`
sDT=`grep "warning" staticWarning/compileDT.txt | wc -l`
sCM=`grep "warning" staticWarning/compileCM.txt | wc -l`
sKN=`grep "warning" staticWarning/compileKN.txt | wc -l`
sSM=`grep "warning" staticWarning/compileSM.txt | wc -l`
sEL=`grep "warning" staticWarning/compileEL.txt | wc -l`
sQP=`grep "warning" staticWarning/compileQP.txt | wc -l`
sSS=`grep "warning" staticWarning/compileSS.txt | wc -l`
sIM=`grep "warning" staticWarning/compileIM.txt | wc -l`
sZL=`grep "warning" staticWarning/compileZL.txt | wc -l`
sZT=`grep "warning" staticWarning/compileZT.txt | wc -l`
sTotal=$((sST + sDT + sCM + sKN + sSM + sQP + sSS + sIM + sZL + sZT))

echo " "                                                    >> staticWarning/warningReport.txt
echo "=============================="                       >> staticWarning/warningReport.txt 
echo "Warning Report"                                       >> staticWarning/warningReport.txt
echo "------------------------------"                       >> staticWarning/warningReport.txt
echo "ST: " $sST                                            >> staticWarning/warningReport.txt
echo "DT: " $sDT                                            >> staticWarning/warningReport.txt
echo "CM: " $sCM                                            >> staticWarning/warningReport.txt
echo "KN: " $sKN                                            >> staticWarning/warningReport.txt
echo "SM: " $sSM                                            >> staticWarning/warningReport.txt
echo "EL: " $sEL                                            >> staticWarning/warningReport.txt
echo "QP: " $sQP                                            >> staticWarning/warningReport.txt
echo "SS: " $sSS                                            >> staticWarning/warningReport.txt
echo "IM: " $sIM                                            >> staticWarning/warningReport.txt
echo "ZL: " $sZL                                            >> staticWarning/warningReport.txt
echo "ZT: " $sZT                                            >> staticWarning/warningReport.txt
echo "------------------------------"                       >> staticWarning/warningReport.txt
echo "TOTAL: " $sTotal                                      >> staticWarning/warningReport.txt
echo "=============================="                       >> staticWarning/warningReport.txt 


cat staticWarning/warningReport.txt
