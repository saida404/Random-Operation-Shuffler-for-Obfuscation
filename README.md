# Random-Operation-Shuffler-for-Obfuscation
Tehnike nasumičnog shufflanja operacija unutar koda algoritma, bez narušavanja njegove funkcionalnosti.

Ovaj kod omogućava nasumično izmiješanje operacija nekog algoritma uz očuvanje njegove funkcionalnosti. Svaka operacija se parsira iz ulaznog stringa i razdvaja na ulazne i izlazne varijable, te se zajedno s originalnim kodom smješta u objekat tipa Operacija. Zatim se od operacija formira graf u kojem svaki čvor predstavlja jednu operaciju, a veze između čvorova predstavljaju zavisnosti – ako neka operacija koristi rezultat druge, uspostavlja se veza u grafu. Na taj način se gradi sistem koji zna koje operacije se mogu izvršiti tek nakon što su prethodne završene.

Nakon što je graf konstruisan, vrši se topološko sortiranje sa dodatnom komponentom nasumičnosti: kada postoji više operacija koje nemaju zavisnosti (tj. ulazni stepen im je 0), algoritam nasumično bira koju će operaciju sljedeću izvršiti. Ovo omogućava da se operacije izvršavaju u različitom redoslijedu svaki put, ali bez narušavanja logike algoritma, jer se poštuju zavisnosti.

FOR-petlje se tretiraju posebno. Kada parser naiđe na FOR-blok, sve operacije unutar njega se analiziraju kao posebna grupa. Te se operacije sortiraju nezavisno, a zatim se FOR-blok kao cjelina tretira kao jedna operacija u glavnom grafu. Time se osigurava da operacije izvan petlje ne uđu unutar nje, niti da se operacije iznutra izmjeste vani.

Na kraju, program ispisuje nasumično, ali zavisno validno, izmiješan redoslijed operacija. Ovo se koristi kao tehnika obfuskacije,tj. zakomplikovan prikaz koda koji i dalje funkcioniše ispravno, ali je teže razumljiv za ljude koji ga čitaju.
