if [[ "$*" == *debug* ]]
then
	su -c "../postgre/bin/psql -f /home/data/mtree_gist/data/mtreeOrderbyhibaGG.sql" postgres
else
	su -c "../postgre/bin/psql -f /home/data/mtree_gist/data/mtreeOrderbyhibaGG.sql" postgres | grep "|" | sed -e 's/   */ /g' | cut -d"|" -f4 | uniq -c
fi