FILE=$1


perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/;MTR_/ /g;
		print;
	}
' $FILE > tmp1.txt
rm tmp2.txt
echo "1"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/INSIDE_/0 /g;
		print;
	}
' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "2"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/OUTSIDE_/1 /g;
		print;
	}
' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "3"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/MT//g;
		print;
	}
' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "4"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/_RPC/ /g;
		print;
	}
' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "5"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/_HV.actual.iMon;\./ 0./g;
		print;
	}
' tmp1.txt > tmp2.txt
rm tmp1.txt
echo "6"

perl -e '
	$/=\65536;
	while(<>) {
		if(/\r\z/) {
			if(length($nextblock=<>)) {
				$_.=$nextblock;
				redo;
			}
		}
		s/_HV.actual.iMon;/ /g;
		print;
	}
' tmp2.txt > tmp1.txt
rm tmp2.txt
echo "7"

mv tmp1.txt script_output.txt

echo "Done!"
