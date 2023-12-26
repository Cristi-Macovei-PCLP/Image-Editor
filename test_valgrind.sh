#!/bin/bash

if test -f "./image_editor"
then
  echo "[+] Exe exists already"
else
  echo "[-] No exe found, building it..."
  make build
  echo "[+] Built exe"
fi

echo "[*] Running tests 00 .. 68 with valgrind (this will take a while)"

for i in $(seq -f %02g 0 68); do
  echo "[*] Running test $i";
  
  infile="./tasks/image_editor/tests/$i-image_editor/$i-image_editor.in";
  valfile="./out_valgrind/$i.txt";

  # if [[ $1 == "-v"]]
  # then
  #   echo "[*] Running command 'valgrind --log-file=$valfile [flags] [exe] < $infile'"
  # fi 

  valgrind --log-file=$valfile --leak-check=full --show-leak-kinds=all --track-origins=yes ./image_editor < $infile &>/dev/null

  result=$(cat out_valgrind/00.txt | grep -E "All heap blocks were freed -- no leaks are possible|ERROR SUMMARY: 0 errors from 0 contexts \(suppressed: 0 from 0\)" | wc -l)

  if [[ result -eq 2 ]]
  then
    echo "[+] Correct!"
  else
    echo "[-] Esti prost facut gramada!"
  fi
  
done;