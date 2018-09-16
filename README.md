# Parallelized sieve of Eratosthenes

Fie:
   - n - numarul de procese
   - val_max - valoare maxima pana la care se cauta numerele prime de tip Mersenne 
   
Intevalul [1,val_max] este impartin in n subintervale de lungime val_max/n.

Avem n procese, din care 1 de tip Master si n-1 de tip Slave.
   Proces de tip slave - determina numarele prime de tip Mersenne din ultimele (n-1) subintervale.
   Proces master - determina numarele prime de tip Mersenne din primul subinterval, si le reuneste cu rezultatele de la procesele de tip slave. Dupa determinarea numarelor prime de tip Mersenne aplica pentru fiecare teorema lui Euclid–Euler pentru a determina numerele perfecte.

Exemplu:
Notatie: P - proces.
Pentru n = 5 si val_max = 25

Pentru determinarea numerelor prime se foloseste Ciurul lui Eratostene paralelizat, procesele fiind independente intre ele.
Fiecare proces determina numerele prime Mersenne de pe un anume subinterval: P1 (1-5), P2 (6-10), P3 (11-15), P4 (16-20), P5 (21-25).
Pentru a determina numerele prime din intervalul sau, un proces procedeaza astfel:
   - determina numerele prime din primul interval (1-5), fie acestea p1 = 2, p2 = 3, p3 = 5.
   - marcheaza in intervalul sau numerele care sunt multipli ai numerelor prime determinate la pasul anterior (p1, p2, p3). Spre exemplu, pentru P3 se marecheza numerele 12, 14 si 15. Numerele ramase nemarcate reprezinta numerele prime.
   - pentru fiecare numar prim din intervalul sau verifica conditia ca acesta sa fie si de tip Mersenne.
Dupa determinarea numarelor prime de tip Mersenne, procesul master aplica pentru fiecare teorema lui Euclid–Euler pentru a determina numerele perfecte.
