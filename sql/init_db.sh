for file in *.sql
do
 echo "Importing $file" & mysql muddb2.0 -u m241dan -pGrc937! < $file
done
echo "Done, enjoy!"
