echo "pass: pg2021"
su -c "../postgre/bin/pg_ctl -D ../postgre/data/ -l ../postgre/logfile stop" postgres
sudo bash ../script/installGG.sh
su -c "../postgre/bin/pg_ctl -D ../postgre/data/ -l ../postgre/logfile start" postgres
# su -c ../postgre/bin/psql postgres
