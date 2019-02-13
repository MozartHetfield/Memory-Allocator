-----------------      ALOCATOR DE MEMORIE	 -----------------

Intrucat scheletul a fost pus la dispozitie de catre echipa, voi specifica
functionalitatea fiecarei instructiuni, pe rand, in ordinea implementarii:

	VARIABILE GLOBALE:			- unsigned char* arena; - arena propriu-zisa
						- uint32_t arenaSize; - dimensiunea arenei, data la initializare
						- unsigned char* arenaHelper; - auxiliar detaliat ulterior

	- INITIALIZE:
	
Aloca matricele (arena, cat si auxiliara), cu dimensiunea <SIZE> si 
initializeaza valoarea fiecarui element cu 0. Matricea auxiliara are rolul
de a tine evidenta clara a zonelor impuse in cerinta. Astfel, arenaHelper
va contine 1 pe zona de gestiune corespunzatoare primului byte (adresa
index-ului blocului urmator), 2 pe zona a 2-a de gestiune (adresa index-ului
blocului anterior) si 3 pe byte-ul de gestiune corespunzator size-ului.

	- FINALIZE:
	
Finalize elibereaza memoria alocata pentru cele 2 matrice.
	
	- DUMP:
	
Afiseaza dupa referinta data modelul de alocator de memorie. Va printa
newline la fiecare valoare_index % 16 == 0, cu exceptia cazului in care
valoarea index-ului este chiar 0.

	- ALLOC:
	
Avem in incipit 2 variabile: destroyer, care, cand e setat pe 1, semnaleaza ca nu
avem niciun spatiu cursiv disponibil pentru a face alocarea pentru acest
numar de bytes (SIZE); setter, care face posibila evitarea parcurgerii cu 
dimensiune de 1 a vectorului pentru a cauta spatiu disponibil. Astfel,
cum gasim un spatiu ocupat, vom trece pe urmatorul pentru a continua cau-
tarile, in loc sa pornim de la clasicul i++. La final, avem index-ul i care
e setat la inceputul gestiunii (pe care trebuie sa o formam). Prin doua
for-uri in cele doua directii vom seta blocurile vecine astfel incat
sa 'pointeze' catre noua zona de memorie (asemenea unei liste dublu inlantuite).
Dupa aceea, se seteaza in arenaHelper zonele de gestiune corespunzatoare,
dupa cum au fost detaliate in cadrul INITIALIZE-ului.

	- FREE:
	
FREE-ul este analogul alocarii din punct de vedere al setarii blocurilor
adiacente, doar ca in loc sa se adauge un bloc nou, se va elimna unul (din nou,
similar listelor dublu inlantuite). Avem nevoie aici de niste verificari pentru
apartenenta vecinilor (maxVerif pentru superior si minVerif pentru inferior, 
setate pe 1 cand au, respectiv 0 cand nu). In cazul in care acestia exista,
index-ul respectiv va fi stocat in variabilele maxSaver si minSaver.

	- FILL:
	
FILL-ul este conceput de un while care se va sfarsi cand vectorul va
iesi din dimensiunile sale (out of bounds, i >= arenaSize), ori cand 
count-ul (numarul de elemente umplute) ating size-ul specificat in comanda.
	
	
