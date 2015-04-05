ofname=data.txt
echo writing to $ofname
sudo ./gmdaq.py | tee $ofname 
chown halil:halil $ofname
