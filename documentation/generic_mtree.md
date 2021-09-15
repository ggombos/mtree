# TODO

- A coveringRadius egy hekkelés, hogyan működik a ranged query? --> more like rangeRadius

Order by hiba:
- be kell billenteni az isOrdered kapcsolót (csak tipp, utána kell nézni)
- az adat elején lévő intet range-el kellene kiváltani !!!!!!

## Stuff happened since last week

Szakdolgozat:
+ https://www.inf.elte.hu/content/programtervezo-informatikus-autonomrendszer-informatikus-msc-diplomamunka-tudnivalok.t.2946?m=510
+ https://www.inf.elte.hu/content/zarovizsga-menetrend.t.1072?m=140y
+ 3 tárgy amelynek köze van a dolgozathoz; legalább egy kötelező legyen.
+ https://www.inf.elte.hu/dstore/document/1157/PTI_MSc_2018%20Szoftvertechnol%C3%B3gia%20nappali%20tagozat%202021%20febru%C3%A1r-m%C3%A1rcius.pdf
+ Témavezető: Gergő

ORDER BY hiba:
+ A hiba csak ASC esetében jön elő (ez a default).
+ Furcsán hívódnak a függvények, a contains/contained egyáltalán nem, ami minimum gyanús, hiszen az operátornak hívnia kellene.

Range:
+ https://www.alibabacloud.com/blog/range-types-in-postgresql-and-gist-indexes_595128
+ Pontosan hogyan nézzen ki ez a range érték?
+ coveringRadius helyett rangeRadius -> tehát akkor a "legkissebb" és "legnagyobb" elemmel akarjuk jelölni az intervallumot?

## New stuff on the meeting

+ Function 8 mire jó?
+ GiST DESC-nél az order by-nak van valami oka, hogy nem működik?
+ DESC-nél nem az indexet használja
+ cube extension -> isLeaf | mi ilyet nem használunk, kellene? befolyásolhatja a számítást
+
