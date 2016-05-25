FILE=$1

perl -pe 's/;MTR_/ /g' $FILE > tmp1.txt
rm tmp2.txt
echo "1"
perl -pe 's/INSIDE_/0 /g' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "2"
perl -pe 's/OUTSIDE_/1 /g' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "3"
perl -pe 's/MT//g' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "4"
perl -pe 's/_RPC/ /g' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "5"
perl -pe 's/_HV.actual.iMon;\./ 0./g' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "6"
perl -pe 's/_HV.actual.iMon;/ /g' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "7"

mv tmp1.txt script_output.txt

echo "Done!"
