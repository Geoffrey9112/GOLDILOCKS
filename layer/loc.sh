rm -rf staticLineCount
mkdir staticLineCount

wc -l `find . -name "*.[c|h]" -o -name "*.java"             | grep -v test`          >> staticLineCount/locSummary.txt

wc -l `find Standard -name "*.[c|h]"                        | grep -v test`          >> staticLineCount/locST.txt
wc -l `find DataType -name "*.[c|h]"                        | grep -v test`          >> staticLineCount/locDT.txt
wc -l `find Communication -name "*.[c|h]"                   | grep -v test`          >> staticLineCount/locCM.txt
wc -l `find Kernel -name "*.[c|h]"                          | grep -v test`          >> staticLineCount/locKN.txt
wc -l `find StorageManager -name "*.[c|h]"                  | grep -v test`          >> staticLineCount/locSM.txt
wc -l `find ExecutionLibrary -name "*.[c|h]"                | grep -v test`          >> staticLineCount/locEL.txt
wc -l `find SqlProcessor -name "*.[c|h]"                    | grep -v test`          >> staticLineCount/locQP.txt
wc -l `find Session -name "*.[c|h]"                         | grep -v test`          >> staticLineCount/locSS.txt
wc -l `find ServerLibrary -name "*.[c|h]"                   | grep -v test`          >> staticLineCount/locSL.txt
wc -l `find GlieseLibrary -name "*.[c|h]" -o -name "*.java" | grep -v test`          >> staticLineCount/locZL.txt
wc -l `find GlieseTool -name "*.[c|h]" -o -name "*.java"    | grep -v test`          >> staticLineCount/locZT.txt


echo " "                                                              >> staticLineCount/locReport.txt
echo "=============================="                                 >> staticLineCount/locReport.txt
echo "LOC Report"                                                     >> staticLineCount/locReport.txt
echo "------------------------------"                                 >> staticLineCount/locReport.txt
echo Total: `cat staticLineCount/locSummary.txt | grep -v "[c|h|j]" | awk '{print $1}'`      >> staticLineCount/locReport.txt
echo "------------------------------"                                                        >> staticLineCount/locReport.txt
cat staticLineCount/locST.txt | grep -v "[c|h]"   | awk '{printf "Standard         :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locDT.txt | grep -v "[c|h]"   | awk '{printf "DataType         :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locCM.txt | grep -v "[c|h]"   | awk '{printf "Communication    :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locKN.txt | grep -v "[c|h]"   | awk '{printf "Kernel           :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locSM.txt | grep -v "[c|h]"   | awk '{printf "StorageManager   :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locEL.txt | grep -v "[c|h]"   | awk '{printf "ExecutionLibrary :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locQP.txt | grep -v "[c|h]"   | awk '{printf "QueryProcessor   :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locSS.txt | grep -v "[c|h]"   | awk '{printf "Session          :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locSL.txt | grep -v "[c|h]"   | awk '{printf "ServerLibrary    :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locZL.txt | grep -v "[c|h|j]" | awk '{printf "GlieseLibrary    :%7s\n", $1}'  >> staticLineCount/locReport.txt
cat staticLineCount/locZT.txt | grep -v "[c|h|j]" | awk '{printf "GlieseTool       :%7s\n", $1}'  >> staticLineCount/locReport.txt
echo "=============================="                                 >> staticLineCount/locReport.txt


cat staticLineCount/locReport.txt

