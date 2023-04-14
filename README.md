// Copyright 2021-2022 Cristian-Vlad Albu & Mihai-Alexandru Georgescu, 312CA

Tema 3 SD - Linux File System

=========================  Description  ===================================

-> Programul implementeaza unui sistem de fisiere arborescent, ce poate
fi manipulat prin intermediul a numeroase comenzi in stil bash: ls, pwd,
mv, cp etc.

===========================================================================
functia main:

-> Primeste anumite comenzi pe baza carora apeleaza functiile implementate
de noi in vederea manipularii sistemului de fisiere.

================================  touch  =================================

-> Comanda:             touch <filename> [filecontent]
				
-> Folosind aceasta comanda, utilizatorul poate crea un nou fisier in
nodul (directorul) curent in care se afla;

-> Daca numele fisierului exista deja in directorul parinte, functia nu
va avea niciun efect;

-> La final, functia va adauga noul fisier intr-un nou nod din lista
directorului parinte.

=================================  ls  ===================================

-> Comanda:                     ls [arg]

-> Folosind aceasta comanda fara un argument, utilizatorul poate afisa
toate resursele directorului curent;

-> Daca argumentul este un director, se afiseaza toate resursele continute
de acesta;

-> In cazul unui argument de tip fisier, functia afiseaza continutul
text al acestuia (in stilul comenzii "cat" Linux).

================================  mkdir  ================================

-> Comanda: 				mkdir <dirname> 

-> Folosind aceasta comanda, utilizatorul poate crea un nou director in
nodul (directorul) curent in care se afla;

-> Numele acestuia nu trebuie sa mai existe in directorul parinte;

-> La final, functia va adauga noul director intr-un nou nod din lista
directorului parinte.

=================================  cd  ==================================

-> Comanda:                     cd <path>

-> Folosind aceasta comanda, utilizatorul se poate muta in directorul
catre care conduce calea path (absoluta sau relativa);

-> Ne folosim de un token si de delimitatorul "/" pentru a ne muta in
directorul respectiv din path.

================================  tree  =================================

-> Comanda:                    tree [path]

-> Folosind aceasta comanda, utilizatorul are posibilitatea sa afiseze
ierarhia de fisiere si directoare pornind de la path-ul dat;

-> In cazul in care argumentul path lipseste, functia afiseaza ierarhia
pornind de la nodul curent;

-> Pentru a realiza afisarii, ne folosim de functia print_tree, careia
i-am atribuit parametrii:
    node - nodul pe care vrem sa il afisam (recursiv daca e director nenul)
    tabs - numarul de tab uri pentru afisare (adica adancimea in ierarhie)
    files, folders - adresele care retin numarul de fisiere, respectiv
directoare de pe tot parcursul functiei tree.

================================  pwd  ==================================

-> Comanda:                       pwd

-> Folosind aceasta comanda, utilizatorul afiseaza calea absoluta a
directorului in care se afla.

================================  rmdir  ================================

-> Comanda:					 rmdir <dirname>

-> Folosind aceasta comanda, utilizatorul poate elimina un director gol
continut de directorul curent, restabilind legaturile dintre celelalte
resurse din lista parintelui.

================================   rm   =================================

-> Comanda:					  rm <filename> 

-> Folosind aceasta comanda, utilizatorul poate elimina un fisier 
aflat in directorul curent, restabilind legaturile dintre celelalte
resurse din lista parintelui.

================================  rmrec  ================================

-> Comanda:                 rmrec <resourcename>

-> Folosind aceasta comanda, utilizatorul poate sterge un fisier sau
director din directorul in care se afla;

-> Daca resursa primita ca parametru este un director care nu este gol,
functia o va sterge recursiv, alaturi de tot ce contine in el.

================================   cp   =================================

-> Comanda:         cp <source_path> <destination_path>

-> Folosind aceasta comanda, utilizatorul poate copia la destination_path
fisierul regasit la source_path;

-> In cazul in care fisierul de la destination_path nu exista sau deja
exista, continutul sau va deveni cel al fisierului regasit la source_path.

================================   mv   =================================

-> Comanda:         mv <source_path> <destination_path>

-> Folosind aceasta comanda, utilizatorul poate muta o resursa (fisier
sau director) din source_path in destination_path;
-> Analog comenzii cp, continutul de la source_path este copiat la 
destination_path, diferenta fiind ca in cazul comenzii mv se sterge mai
apoi continutul de la source_path.

