# DixieUhr Ansteuerung mit einem Raspi Zero W
In der Zeitschrift Make war ein Artikel über eine Selbstbau Uhr, die den Nixie Röhren nachempfunden ist https://www.heise.de/ratgeber/Bastelprojekt-Digitaluhr-mit-TFT-Displays-im-Stil-von-Nixie-Roehren-4937026.html. 
Das hat mich so fasziniert, dass ich das unbedingt nachbauen musste. 
Die Nixie Röhren werden simuliert durch je ein TFT Display, das in ein Schutzglas eingebaut ist. Da mir die Ukrainische Version der Röhre besser gefällt, habe ich meinen 3D-Drucker bemüht und das ein wenig abgewandelt. Aber darum soll es hier nicht gehen.
Zunächst habe ich die Ansteuerung wie in der Make beschrieben mit einem Arduino gemacht. Statt der Taster habe ich Touchflächen genommen. Der Atmel-Prozessor kann damit ja umgehen. Leider reicht der Speicher nicht wirklich aus und die Geschwindigkeit ist auch zu klein, so dass ich ständig die Uhr nachstellen musste. Deshalb habe ich mich entschieden, die Ansteuerung mit einem Raspi Zero W selbst zu machen. 

# Voraussetzungen
Zunächst müssen ein paar Dinge erledigt werden.
1. Raspi und SD-Karte (ich habe eine 16GB genommen) besorgen. 
2. SD-Karte mit Raspi OS flashen ([https://www.raspberrypi.com/software/](https://www.raspberrypi.com/software/operating-systems/)) und einrichten. Ich habe die Version Raspberry Pi OS with desktop genommen, dann kann man über VNC von der Ferne auf den Raspi zugreifen.
3. Dem Raspi habe ich den Namen "dixieuhr" gegeben und den User "dixie" angelegt. Dies und die Verbindung zum Heimnetz über WLan kann man vorab anpassen.
5. Dann habe ich im Home Verzeichnis den Ordner "DixieUhr" angelegt. Auf diese Struktur bezieht sich das gesamte Programm. Wer das ändern möchte muss auch im Programm selbst die Pfade anpassen.
6. Treiber für das TFT bzw. das Ansteuer-IC ST7735 installieren. Dazu bitte diesem Link folgen: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI

Wenn das Demo-Programm mit einem Display soweit läuft, dann geht es hier weiter.
In den Ordner "DixieUhr" wird nun die im Repo dargestellt Verzeichnisstruktur erstellt. Das ausführbare Programm steht dabei im Ordner bin bereit. Wer meine Verzeichnisstruktur übernommen hat, kann das Programm direkt nutzen. Es muss allerdings mit sudo ausgeführt werden.

Um das Programm direkt nach dem Einschalten automatisch zu starten, muss das Script dixie aus dem Ordner src nach /etc/init.d und das Programm aus bin nach /usr/local/bin/ verschoben werden:


<code>sudo mv DixieUhr/src/dixie /etc/init.d/</code>
<code>sudo mv DixieUhr/bin/dixie /usr/local/bin/</code>

Dann noch ein paar Rechte setzen:


<code>sudo chmod 755 /etc/init.d/dixie</code>

Jetzt können wir das script testen:


<code>sudo /etc/init.d/dixie start</code>

und gleich darauf mit strg+c beenden.

Damit das Skript beim booten auch aufgerufen wird, führen wir folgendes aus:

  
  <code>sudo update-rc.d dixie defaults</code>

  
Nun sollte das Programm bei starten auch ausgeführt werden. Solltest du eines Tages dich umentscheiden und das Programm aus dem Autostart nehmen wollen, kannst du dies mit:
  
  
<code>sudo update-rc.d -f  dixie remove</code>

Nach einem Neustart wird nun die Uhr automatisch initialisiert. Selbst ein Stromausfall kann dann keinen Schaden mehr anrichten.

# Die Software
Direkt nach dem Start werden die TFT Displays initialisiert und dann die Uhrzeit angezeigt. Der Doppelpunkt verhält sich dann so: oberer Punkt blinkt -> es sind weniger als 30 Sekunden der nächsten Minute vergangen. Unterer Punkt blinkt -> Es dauert weniger als 30 Sekunden, bis die Uhr umspringt.
Dies Verhalten kann man mit dem linken Taster auf dauerhaft Doppelpunkt umstellen.

Wird der mittlere Taster betätigt, wird auf Datum umgestellt bzw. wieder zurück. Wenn man nicht auf Uhrzeit zurück stellt, wird nach spätestens 2 Minuten wieder auf Uhrzeit zurück gestellt.

# Die Hardware
Die folgende Tabelle zeigt die Verbindungen, die man zwischen Displays und der 40pol. Stiftleiste des Raspi herstellen muss.

Ich habe drei Taster vorgesehen

