#!/bin/bash
echo "Lancement du fichier .csv test  :" $1
#echo "Repertoire de courant :" $PWD
#echo "Test fait avec 3 image par ligne"
id_user=123
heure=$(date +%H%M)
Date=$(date +%Y%m%d)"_"$heure
# On test d'abord si le fichier et le repertoire existe
absolute_path=$(readlink -m $PWD"/"$1)  
#echo $absolute_path
file=$absolute_path"/"$(basename $absolute_path)"_main.csv"
echo "Lancement du fichier .csv test  :" $file
#echo $file
if [ -e $file ]; then 
	if [ -z $1 ]; then
		echo " Erreur fichier ou repertoire non connu : usage <file_name> "
   		../build-promo-Desktop_Qt_5_2_1_GCC_64bit-Debug/promo 
	else 
		../build-promo-Desktop_Qt_5_2_1_GCC_64bit-Debug/promo $file $id_user $Date 	
	fi
else
	echo " Erreur fichier ou repertoire non connu : usage <file_name> "
        ../build-promo-Desktop_Qt_5_2_1_GCC_64bit-Debug/promo 
fi
