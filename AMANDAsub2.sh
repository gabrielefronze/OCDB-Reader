FILE=$1

tr ';MTR_' ' ' < $FILE > tmp1.txt
rm tmp2.txt;
echo "1"
tr 'INSIDE_' '0 '  < tmp1.txt > tmp2.txt
rm tmp1.txt;
echo "2"
tr 'OUTSIDE_' '1 '  < tmp2.txt > tmp1.txt
rm tmp2.txt;
echo "3"
tr -d 'MT'  < tmp1.txt > tmp2.txt
rm tmp1.txt;
echo "4"
tr '_RPC' ' ' < tmp2.txt > tmp1.txt
rm tmp2.txt;
echo "5"
tr '_HV.actual.iMon;.' ' 0.'  < tmp1.txt > tmp2.txt
rm tmp1.txt;
echo "6"
tr '_HV.actual.iMon;' ' ' < tmp2.txt > tmp1.txt
rm tmp2.txt;
echo "7"

echo "Done!"