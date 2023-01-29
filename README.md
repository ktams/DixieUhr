# DixieUhr Ansteuerung mit einem Raspi Zero W
In der Zeitschrift Make war ein Artikel über eine Selbstbau Uhr, die den Nixie Röhren nachempfunden ist https://www.heise.de/ratgeber/Bastelprojekt-Digitaluhr-mit-TFT-Displays-im-Stil-von-Nixie-Roehren-4937026.html. 
Das hat mich so fastziniert, dass ich das unbedingt nachbauen musste. 
Die Nixie Röhren werden simuliert durch je ein TFT Display, das in ein Schutzglas eingebaut ist. Da mir die Ukrainische Version der Röhre besser gefällt, habe ich meinen 3D-Drucker bemüht und das ein wenig abgewandelt. Aber darum soll es hier nicht gehen.
Zunächst habe ich die Ansteuerung wie in der Make beschrieben mit einem Arduino gemacht. Statt der Taster habe ich Touchflächen genommen. Der Atmel-Prozessor kann damit ja umgehen. Leider reicht der Speicher nicht wirklich aus und die Geschwindigkeit ist auch zu klein, so dass ich ständig die Uhr nachstellen musste. Deshalb habe ich mich entschieden, die Ansteuerung mit einem Raspi Zero W selbst zu machen. 

# Voraussetzungen
Zunächst müssen ein paar Dinge erledigt werden.
1. Raspi und SD-Karte (ich habe eine 16GB genommen) besorgen. 
2. SD-Karte mit Raspi OS flashen (https://www.raspberrypi.com/software/)
3. Dem Raspi habe ich den Namen "dixieuhr" gegeben und den User "dixie" angelegt. Dann habe ich im Home Verzeichnis den Ordner "DixieUhr" angelegt. Auf diese Struktur bezieht sich das gesamte Programm. Wer das ändern möchte muss auch im Programm selbst die Pfade anpassen.
4. Treiber für das TFT bzw. das Ansteuer-IC ST7735 installieren. Dazu bitte diesem Link folgen: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI

Wenn das Demo-Programm mit einem Display soweit läuft, dann geht es hier weiter.
In den Ordner "DixieUhr" wird nun die im Repo dargestellt Verzeichnisstruktur erstellt. Das ausführbare Programm steht dabei im Ordner bin bereit. Wer meine Verzeichnisstruktur übernommen hat, kann das Programm direkt nutzen. Es muss allerdings mit sudo ausgeführt werden.


