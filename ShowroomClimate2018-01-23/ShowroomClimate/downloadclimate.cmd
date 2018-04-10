pscp -P 36531 -pw ToNa8a7W ./release/ShowroomClimate.hex pi@193.34.219.54:/home/pi/
plink -l pi -pw ToNa8a7W -m ./avrdude.txt -load "_pi_maket_HS" _pi_maket_HS