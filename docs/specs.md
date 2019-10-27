# Peer to Peer chat Specifikáció

A projekem célja egy Peer to Peer chat alkalmazás létrehozása. A lénye az, hogy több gép egy hálózatot alkotva kommunikál egymással,azaz nincs egy központi szerver,ami az üzeneteket továbbítja a klienseknek,hanem minden üzenet közvetlenül megy a feladótól a címzettig.

## Peer to Peer model

Ahogy már az előbb említettem a cél egy olyan hálózat kiépítése amiben minden gép csatlakozik az összes többihez.Ennek érdekében szükséges az hogy minden egyes számítógép rendelkezzen egy „hallgató” (listening) socket-el. 

![1. Ábra](abra/abra01.png)

