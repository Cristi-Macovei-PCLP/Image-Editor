# Tema 3 - Editor de imagini

### Macovei Nicolae-Cristian
### Anul I, grupa 312CAb

---

### Citirea comenzilor

Programul stocheaza imaginea curenta si selectia curenta, folosind doua
variabile de tip structura `image_file_t`, respectiv `selection_t`.
<br>
Deoarece sunt doua tipuri de imagini (color si grayscale), structura
`image_file_t` foloseste un pointer de tip `void**`, care este transformat in
cadrul operatiilor la `pgm_point_t` (daca imaginea e grayscale) sau
`ppm_point_t` (daca imaginea e color).
<br>
Pentru fiecare comanda, programul citeste o linie de la `stdin`, care
urmeaza sa fie parsata si executata folosind cele doua variabile pentru
imagine si selectie.

### Comanda LOAD (incarcarea unui fisier)

In primul rand, daca exista un fisier deja incarcat, programul elibereaza
memoria alocata pentru acesta.
<br>
Aceasta comanda primeste un singur argument, calea spre fisier. Daca acest
argument lipseste, programul va afisa mesajul `"Invalid command"`.
<br>
Daca acest argument exista, programul va incerca sa deschida fisierul si sa
citeasca primii 2 octeti din el, care reprezinta constantele magice ale
formatului PBM. In functie de acestea se poate afla tipul imaginii (ex. `P2` =
imagine grayscale, stocata ascii).
<br>
Dupa acestea, programul citeste, pe rand, latimea, inaltimea, valoarea maxima
a culorilor, asa cum sunt specificate in descrierea formatului.
<br>
Daca s-a intampinat vreo eroare in timpul citirii, memoria fisierului curent va
fi eliberata si programul se intoarce la starea unde niciun fisier nu este
incarcat.
<br>
Daca imaginea a fost incarcata cu succes, se afiseaza mesajul `"Loaded <file>
successfully"`. De asemenea, dupa incarcarea unei imagini, se considera ca
aceasta este selectata in intregime.

### Comanda SAVE (salvarea unui fisier)

Aceasta comanda primeste 2 argumente, primul fiind numele fisierului si al
doilea, optional, daca fisierul trebuie salvat in format ASCII sau binar.
<br>
Apoi, programul deschide fisierul cu numele dat si scrie datele imaginii aflate
la momentul curent in memorie in acel fisier.
<br>
Daca formatul specificat este ASCII, culoarea fiecarui pixel se scrie in
fisier, ca text, prin 1 sau 3 numere zecimale. Daca formatul este binar,
valorile pixelilor se va reprezenta binar, fiecare culoarea pe cate 1 sau 2
octeti, in functie de valoarea maxima.
<br>
Daca imaginea a fost salvata cu succes, programul afiseaza mesajul `"Saved
<file>"`

### Comanda SELECT

Aceasta comanda poate primi 4 argumente intregi (x1, y1, x2, y2) sau un singur
argument de tip string (in cazul `SELECT ALL`).
<br>
Daca numarul de argumente nu este corect, sau argumentele au alta forma decat
numeric sau `"ALL"`, se va afisa mesajul `"Invalid command"`. Daca cel putin
una dintre coordonate nu este valida (depaseste dimensiunile imaginii), se
afiseaza mesajul `"Invalid set of coordinates"`. Pentru a face aceasta
distinctie, functia folosita pentru parsarea argumentelor
(`get_select_cmd_args()`) returneaza un numar intreg, care e folosit ca un
"status code": `1` daca e corect, `0` pentru primul mesaj de eroare si `2` pentru al doilea mesaj de eroare.
<br>
La finalul parsarii argumentelor, este necesar sa verificam daca argumentele
primite sunt in ordinea corecta (`x1 < x2` si `y1 < y2`). In caz contrar,
valorile acestora trebuie interschimbate intre ele pentru ca restul comenzilor
sa functioneze corect.
<br>
Daca selectia este valida, se afiseaza mesajul `"Selected <x1> <y1> <x2> <y2>"`

### Comanda HISTOGRAM

Pentru realizarea histogramei am creat un vector de frecventa de dimensiunea
egala cu numarul de bin-uri.
<br>
Pentru fiecare punct din imagine am determinat in ce bin se afla, si am
incrementat valoarea din vectorul de frecventa corespunzatoare.
<br>
Apoi, pentru fiecare bin am afisat numarul necesar de stelute.

### Comanda EQUALIZE

Pentru egalizarea imaginii, am procedat la fel ca la histograma prin crearea
unui vector de frecventa si incrementarea valorilor acestuia la fiecare punct
din imagine.
<br>
Pentru formula de egalizare aveam nevoie de valoarea sumei frecventelor de la
zero la o valoare data, asa ca am creat un nou vector de sume partiale pentru
a calcula aceasta valoare in mod eficient.

### Comanda ROTATE

Rotate se poate realiza pe o selectie patrata sau pe toata imaginea (care nu e
neaparat patrata). Din aceasta cauza, am folosit doua functii diferite, una
pentru rotirea unei selectii patrate a imaginii si una pentru rotirea intregii
imagini. Dupa aplicarea rotirii intregii imagini, este necesar sa schimbam
intre ele valorile inaltimii si latimii imaginii.
<br>
De asemenea, am observat ca nu este nevoie de implementari separate pentru
rotirea la 90, 180, 270 de grade, deoarece acestea se pot realiza prin
aplicarea rotirii la 90 de grade de mai multe ori.  

### Comanda CROP 

Pentru realizarea taierii imaginii, modificarile au fost facute in-place,
urmate apoi de redimensionarea matricei.

### Comanda APPLY

Comanda APPLY primeste un singur parametru, tipul filtrului pe care trebuie
sa il aplice. In functie de acesta, construieste o matrice 3x3 conform
cerintei.
<br>
Apoi, pentru fiecare pixel din selectie am aplicat matricea. Daca pixelul
respectiv nu avea toti 8 vecinii (se afla pe margine), valoarea acestuia
ramanea neschimbata.
<br>
Toate calculele au fost facute pe double si la final am apelat functia `round()` din `math.h`

### Modul de debug
Programul poate fi compilat in modul "debug" prin adaugarea unui define `MODE_DEBUG`, care adauga inca o comanda numita `"SHOW"`, care afiseaza informatii despre imaginea incarcata in memorie la momentul curent.
<br>
De asemenea, pe langa output-ul modului normal, programul compilat cu debug
afiseaza mai multe detalii la fiecare comanda in stream-ul `stderr`.