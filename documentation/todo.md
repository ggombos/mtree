# TODO

- A coveringRadius egy hekkelés, hogyan működik a ranged query? --> more like rangeRadius

Order by hiba:
- be kell billenteni az isOrdered kapcsolót (csak tipp, utána kell nézni)
- az adat elején lévő intet range-el kellene kiváltani !!!!!!

## Last week

+ Function 8 mire jó?
+ GiST DESC-nél az order by-nak van valami oka, hogy nem működik?
+ DESC-nél nem az indexet használja
+ cube extension -> isLeaf | mi ilyet nem használunk, kellene? befolyásolhatja a számítást

## Progress

### Function 8
- az interneten még nem találtam semmit erre, de biztosan kell lennie valahol

### DESC-nél miért nem az indexünket használja?
- VACUUM FULL ANALYZE fingerprints2;
- nem segít, ugyanúgy nem használja az indexet

### cube extension -> isLeaf
- valóban nem használjuk ezt a lehetőséget, érdemes lenne
- hogyan kellene megvalósítani?
- GISTENTRY típus, ahol NDBOX a key -> ez lehetne mtree_gist?

## 2021.09.15.

- az ORDER BY futása során keresendő a hiba!
- mi történik a PostgreSQL-en belül egy ORDER BY hívásnál? Mire érdemes szűrni a PostgreSQL logjában (debug log)?
- az ORDER BY hiba csak sok adat esetén jön ki - lehet, hogy strukturális probléma lesz.
  Lehet, hogy csak addig működik jól, ameddig mindenki a levél szinten van.
  Kevés adatra használja az indexet DESC-nél?
  MI A LEGKEVESEBB ADAT, AMIRE MEGROMLIK?
- hány adat fér egy page-re?
- isOrdered flag -> B-fa implementációban be van állítva, érdemes ott megnézni
  https://www.postgresql.org/docs/current/gist-implementation.html
- hstore extension, itt is érdemes nézelődni
- ltree, szintén zenész
- távolságszámolás hogyan lett megoldva, lehet-e itt bármi köze bárminek az order by-hoz

### Progress

- Egy page 8kB.
- Az ORDER BY lekérdezések nem konzisztensek, ugyanarra az adathalmazra más eredményt hoz.
- Az ORDER BY hiba nem függ össze azzal, hogy hány szintes a fa.
  -  150 még jó. (1 szint, determinisztikus)
  -  250 már nem jó.
  -  500 már nem jó.
  -  550 már nem jó.
  -  625 már nem jó.
  -  750 már nem jó.
  - 1000 már nem jó.
  - 1500 már nem jó.
  - 3000 már nem jó.
  - 6000 már nem jó.

## 2021.09.22.

- Lehetne automata tesztelés. (pl.: avk & sort)
- Sejtés: a belső csúcsoknl romlik el, levelenként jól rendezi, utána pedig konkatenál
          picksplit-ben lesz a baj, nézzük meg a B-treeben hogyan csinálják, mit állítanak be
          (A btree kulcsa nem maga az adat, hanem egy intervallum) !!!
          (((compression, decompression ?)))
          keressünk egy létező postgres implementációt (lehetőleg hivatalos), ami jobban hasonlít a mi esetünkre (fa, belső csúcsban tárol) -> rtree, btree nem ilyenek
          cube-ot **lehet**, hogy érdemes megnézni
          - úgy kellene szétszedni, hogy a hasonló távolságban lévő elemek valahogy együtt legyenek

### Progress

Haladás a 2021.09.22. és 2021.09.29. között eltelt 1 hét folyamán.

#### Automata tesztelés

-

#### _ORDER BY_ hiba felderítése

M-fa: https://en.wikipedia.org/wiki/M-tree

A rendezés ott romlik el, amikor a belső csúcsokat beállítjuk, valamit itt nem jól csinálunk.
A cél az, hogy megnézzük, más implementációkban hogyan van implementálva a picksplit.

##### B-tree

A B-fa nem belső csúcsban tárolja az értéket.

##### L-tree

L-fa: https://repository.upenn.edu/cgi/viewcontent.cgi?referer=&httpsredir=1&article=1036&context=db_research

Sajnos az L-fa sem belső csúcsban tárolja az adatot, ott csak egy rendezéshez szükséges másolt érték található.

##### Cube

A cube extension szintén nem belső csúcsokban tárolja az értékeket.

### 2021.09.29.

- Bele kell mászni a PostgreSQL-be és megtudni, milyen függvények hívódnak meg egy ORDER BY hívásnál.
  Ezt jó lenne dokumentálni is valamilyen formában, hogy a jövőben könnyebben felderíthetőek legyenek az
  ilyen problémák.

#### Haladás 2021.09.29. és 2021.10.05. között

Érkezett a hét során jónéhány ötlet:

https://www.postgresql.org/docs/current/gist-extensibility.html

- Korábban nálunk ez nem volt külön esetként kezelve, így potenciális hibalehetőség.

- Probléma: hogyan állapítsuk meg, hogy egy belső csúcsnak kik a gyerekei?
  Azt tudjuk, hogy levél-e, de a gyerekeket honnan tudjuk meg?
  B-fa: gbt_intv_distance (csak példa, nem text típus)
  A B-fa implementációban nem ágaznak el, aszerint, hogy egy csúcs levél-e!
  (Egyébként a B-fa implementációban 8-as számú függvény sincs megadva az OPERATOR CLASS-ban)
  Cube: g_cube_distance
  Na, ez érdekesebb - itt van elágazás aszerint, hogy levél-e, de maga a számolás varázslásnak tűnik.

https://stackoverflow.com/questions/54760557/is-the-relationship-between-index-tuple-in-gist-index-and-user-table-row-many-to

https://medium.com/postgres-professional/indexes-in-postgresql-5-gist-86e19781b5db

- A "gevel" extension telepítésével még szenvedni kell, nem olyan triviális (Rust).
  Rust source code: https://github.com/Horusiath/gevel
- Még nem olvastam végig, elég hosszú és jó lenne közben látni a mi "gevel" adatainkat.

https://www.postgresql.org/docs/current/indexes-ordering.html

-

https://www.postgresql.org/docs/current/xindex.html#XINDEX-ORDERING-OPS

### 2021.10.05.

- A belső csúcsokban tároljuk le a szülőtől való minimális távolságot.
- Mi az a recheck? Mikor kell? Miért kell? Miért lassít ennyire?
  -> Benne van ebben a részfában az adott csúcs? Kéne használni a sugarat a távolság számolásnál,
     ne recheck-eljünk mindig.
- Miért használ szekvenciális szkennelést csökkenő sorrendnél?
  -> https://www.postgresql.org/docs/current/indexes-opclass.html (sort order)
  -> https://www.postgresql.org/docs/current/sql-createopclass.html ()
- FLOAT és INT egyesítést el kell végezni a kódban amit Gergő felpusholt.
- Érdemes lenne egy automata tesztet írni , amivel meg lehet nézni, hogy éppen jó-e a rendezés.

### Események

- Indokolatlan mennyiségű levelezés
  - Oroszok
  - PostgreSQL levelező lista
  - Slack újra próbálása
- Korábbi teszt adatokkal összehasonlítás
  - Egy picit lassabb lett az index, ennek utána kell járni
  - Lassabb az index építés is
    - Ez lehet, hogy PostgreSQL verzió miatt (is?) van, mert a régi index
      is elég lassan készült el (arányaiban)
- Dockerfile készítés
  - Jajj de régen csináltam már ilyet!
  - Valamiért nem akar beimportálódni a GPG kulcs Pestről, ami furcsa.

### 2021.11.10.

- Kezdjük el implementálni az M-tree index-et tömbökre
  - Érdemes lehet először számokkal kezdeni, a véletlenszerű generálás egyszerűsége miatt
    (Tömbökre is megoldott, csak azt meg kell nézni, hogyan)
- Ezt a parancsot ki kellene próbálni:
  sudo sh -c "/bin/echo 3 > /proc/sys/vm/drop_caches"

### Dolgok amik történtek

#### Gergő megszerelte az ORDER BY-t!

Úgy tűnik a gondot a `FLOAT8` típus okozta, eddig ismeretlen okból fakadóan.
Most minden `FLOAT4` és így jónak is tűnik. Ezt nem _"tiltja"_ a dokumentáció
sem, meg van engedve a távolságfüggvénynél a `FLOAT4` típus is.

https://www.postgresql.org/docs/13/gist-extensibility.html

Nálam egyébként a milliós méretű adathalmazra való index építés továbbra is elég
lassú, ez nem tudom, hogy egyéni jelenség-e. Mi a tapasztalat? Kipróbáltam a

```sh
sudo sh -c "/bin/echo 3 > /proc/sys/vm/drop_caches"
```

parancsot is, de nem segített.

#### M-tree index implementálása számokra (majd tömbökre)

Nem biztos, hogy ez `GiST` indexszel triviális feladat lesz. Ismerünk erre példát?

https://stackoverflow.com/questions/4058731/can-postgresql-index-array-columns

https://www.postgresql.org/docs/13/arrays.html#ARRAYS-SEARCHING

https://www.postgresql.org/docs/13/indexes-types.html

A `GiN` biztosan tud ilyet.

https://www.postgresql.org/docs/13/gin-extensibility.html

Az `intarray` extension éppen ilyesmire lett kitalálva, ez segíthet. Nem tiszta,
hogy pontosan azokra az operátorokra van-e értelmezve, amiket mi is használni
szeretnénk majd (`&&, @>, <@, @@` -> ezeket tudja).

https://www.postgresql.org/docs/13/intarray.html
