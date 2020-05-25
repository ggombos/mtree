echo "VP-tree start"
cc -fPIC -c -I /home/ggombos/mtreeproject/PostgreSQL_11/install/include/postgresql/server pg-spgist_hamming/vptree/vptree.c -o pg-spgist_hamming/vptree/vptree.o
cc -shared -o pg-spgist_hamming/vptree/vptree.so pg-spgist_hamming/vptree/vptree.o
cp pg-spgist_hamming/vptree/vptree.so /home/ggombos/mtreeproject/PostgreSQL_11/install/lib
cp pg-spgist_hamming/vptree/vptree.control /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/
sed -i "s/\$libdir/\/home\/ggombos\/mtreeproject\/PostgreSQL_11\/install\/lib/g" /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/vptree.control
cp pg-spgist_hamming/vptree/vptree--1.0.sql /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/
echo "VP-tree OK"

echo "BK-tree start"
cc -fPIC -c -I /home/ggombos/mtreeproject/PostgreSQL_11/install/include/postgresql/server pg-spgist_hamming/bktree/bktree.c -o pg-spgist_hamming/bktree/bktree.o
cc -fPIC -c -I /home/ggombos/mtreeproject/PostgreSQL_11/install/include/postgresql/server pg-spgist_hamming/bktree/bktree_utils.c -o pg-spgist_hamming/bktree/bktree_utils.o


cc -shared -o pg-spgist_hamming/bktree/bktree.so pg-spgist_hamming/bktree/bktree.o pg-spgist_hamming/bktree/bktree_utils.o
cp pg-spgist_hamming/bktree/bktree.so /home/ggombos/mtreeproject/PostgreSQL_11/install/lib



cp pg-spgist_hamming/bktree/bktree.control /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/
sed -i "s/\$libdir/\/home\/ggombos\/mtreeproject\/PostgreSQL_11\/install\/lib/g" /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/bktree.control
cp pg-spgist_hamming/bktree/bktree--1.0.sql /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/
echo "BK-tree OK"

# echo "M-tree start"
# cp mtree/mtree.c mtree/mtree_tmp.c
# cc -fPIC -c -I /home/ggombos/mtreeproject/PostgreSQL_11/install/include/postgresql/server mtree/mtree_tmp.c -o mtree/mtree.o
# cc -shared -o mtree/mtree.so mtree/mtree.o
# cp mtree/mtree.so /home/ggombos/mtreeproject/PostgreSQL_11/install/lib/mtree.so
# cp mtree/mtree.control /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree.control
# cp mtree/mtree--1.0.sql /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree--1.0.sql
# echo "M-tree OK"

echo "M-tree start"
picksplitStrategies=(
  PicksplitRandom
  PicksplitFirstTwo
  PicksplitMaxDistanceFromFirst
  PicksplitMaxDistancePair
  PicksplitSamplingMinCoveringSum
  PicksplitSamplingMinCoveringMax
  PicksplitSamplingMinOverlapArea
  PicksplitSamplingMinAreaSum
)

i=1

for picksplitStrategy in "${picksplitStrategies[@]}"
do
	echo "----"
	echo $picksplitStrategy
	cp mtree/mtree.c mtree/mtree_tmp.c
	sed -i -e "s/%picksplitStrategy%/$picksplitStrategy/g" mtree/mtree_tmp.c 
	
	cc -fPIC -c -I /home/ggombos/mtreeproject/PostgreSQL_11/install/include/postgresql/server mtree/mtree_tmp.c -o mtree/mtree.o
	rm mtree/mtree_tmp.c
	
	cc -shared -o mtree/mtree.so mtree/mtree.o

	cp mtree/mtree.so /home/ggombos/mtreeproject/PostgreSQL_11/install/lib/mtree${i}.so

	cp mtree/mtree.control /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree${i}.control
	
	sed -i "s/\$libdir/\/home\/ggombos\/mtreeproject\/PostgreSQL_11\/install\/lib/g" /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree${i}.control

	sed -i "s/%picksplitStrategy%/$picksplitStrategy/g" /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree${i}.control

	sed -i "s/\$mtree/mtree${i}/g" /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree${i}.control

	cp mtree/mtree--1.0.sql /home/ggombos/mtreeproject/PostgreSQL_11/install/share/postgresql/extension/mtree${i}--1.0.sql
	
	i=$((i+1))
	
done;

echo "M-tree OK"
