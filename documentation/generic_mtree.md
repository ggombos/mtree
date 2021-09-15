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
- hány adat fér egy page-re?
- isOrdered flag -> B-fa implementációban be van állítva, érdemes ott megnézni
  https://www.postgresql.org/docs/current/gist-implementation.html
- hstore extension, itt is érdemes nézelődni
- ltree, szintén zenész
- távolságszámolás hogyan lett megoldva, lehet-e itt bármi köze bárminek az order by-hoz
