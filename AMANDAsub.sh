FILE=$1

sed 's/;MTR_/ /g' $FILE > tmp && mv tmp $FILE
sed 's/INSIDE_/0 /g' $FILE > tmp && mv tmp $FILE
sed 's/OUTSIDE_/1 /g' $FILE > tmp && mv tmp $FILE
sed 's/MT//g' $FILE > tmp && mv tmp $FILE
sed 's/_RPC/ /g' $FILE > tmp && mv tmp $FILE
sed 's/_HV.actual.iMon;\./ 0./g' $FILE > tmp && mv tmp $FILE
sed 's/_HV.actual.iMon;/ /g' $FILE > tmp && mv tmp $FILE

echo "Done!"
