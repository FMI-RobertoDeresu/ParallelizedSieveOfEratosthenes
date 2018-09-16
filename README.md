# Parallelized sieve of Eratosthenes

Fie:
   - n - numarul de procese
   - val_max - valoare maxima pana la care se cauta numerele prime de tip Mersenne 

Avem n procese, din care 1 de tip Master si n-1 de tip Slave.
   Proces de tip slave - determina numarele prime de tip Mersenne pe subintervalul i de lungime val_max/(n-1), unde i este indicele procesului.
   Proces master - primeste numarele prime de tip Mersenne la procesele de tip Slave, si aplica pentru fiecare teorema lui Euclid–Euler pentru a determina numerele perfecte.

Exemplu:
Notatie: PS - proces de tip slave.
Pentru n = 5 si val_max = 20 => avem 4 PS care determina numerele prime de tip Mersenne de pe intervale de lungime 5 (val_max/(n-1) = 20/4 = 5).

Pentru determinarea numerelor prime se foloseste Ciurul lui Eratostene paralelizat, procesele PS fiind independente intre ele.
Fiecare PS determina numerele prime Mersenne de pe un anume subinterval: PS1 (1-5), PS2 (6-10), PS3 (11-15), PS4 (16-20).
Pentru a determina numerele prime din intervalul sau, un PS procedeaza astfel:
   - determina numerele prime din primul interval (1-5), fie acestea p1 = 2, p2 = 3, p3 = 5.
   - marcheaza in intervalul sau numerele care sunt multipli ai numerelor prime determinate la pasul anterior (p1, p2, p3). Spre exemplu, pentru PS3 se marecheza numerele 12, 14 si 15. Numerele ramase nemarcate reprezinta numerele prime.
   - pentru fiecare numar prin din intervalul sau verifica conditia ca acesta sa fie si de tip Mersenne.
