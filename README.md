# DixieUhr Ansteuerung mit einem Raspi Zero W
In der Zeitschrift Make war ein Artikel über eine Selbstbau Uhr, die den Nixie Röhren nachempfunden ist https://www.heise.de/ratgeber/Bastelprojekt-Digitaluhr-mit-TFT-Displays-im-Stil-von-Nixie-Roehren-4937026.html. 
Das hat mich so fasziniert, dass ich das unbedingt nachbauen musste. 
Die Nixie Röhren werden simuliert durch je ein TFT Display, das in ein Schutzglas eingebaut ist. Da mir die Ukrainische Version der Röhre besser gefällt, habe ich meinen 3D-Drucker bemüht und das ein wenig abgewandelt. Aber darum soll es hier nicht gehen.
Zunächst habe ich die Ansteuerung wie in der Make beschrieben mit einem Arduino gemacht. Statt der Taster habe ich Touchflächen genommen. Der Atmel-Prozessor kann damit ja umgehen. Leider reicht der Speicher nicht wirklich aus und die Geschwindigkeit ist auch zu klein, so dass ich ständig die Uhr nachstellen musste. Deshalb habe ich mich entschieden, die Ansteuerung mit einem Raspi Zero W selbst zu machen. 
![DixieUhr](https://user-images.githubusercontent.com/10975733/215593527-d5595cdf-eeaa-471c-b8f5-ce9c2577ff51.jpg)


# Voraussetzungen
Zunächst müssen ein paar Dinge erledigt werden.
1. Raspi und SD-Karte (ich habe eine 16GB genommen) besorgen. 
2. SD-Karte mit Raspi OS flashen ([https://www.raspberrypi.com/software/](https://www.raspberrypi.com/software/operating-systems/)) und einrichten. Ich habe die Version Raspberry Pi OS with desktop genommen, dann kann man über VNC von der Ferne auf den Raspi zugreifen.
3. Dem Raspi habe ich den Namen "dixieuhr" gegeben und den User "dixie" angelegt. Dies und die Verbindung zum Heimnetz über WLan kann man vorab mit dem Tool "imager", das auch von der Raspi Seite herunter geladen werden kann, anpassen.
5. Dann habe ich im Home Verzeichnis den Ordner "DixieUhr" angelegt. Auf diese Struktur bezieht sich das gesamte Programm. Wer das ändern möchte muss auch im Programm selbst die Pfade anpassen.
6. Treiber für das TFT bzw. das Ansteuer-IC ST7735 installieren. Dazu bitte diesem Link folgen: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI

Wenn das Demo-Programm mit einem Display soweit läuft, dann geht es hier weiter.
In den Ordner "DixieUhr" wird nun die im Repo dargestellt Verzeichnisstruktur erstellt. Das ausführbare Programm steht dabei im Ordner bin bereit. Wer meine Verzeichnisstruktur übernommen hat, kann das Programm direkt nutzen. Es muss allerdings mit sudo ausgeführt werden.

Um das Programm direkt nach dem Einschalten automatisch zu starten, muss das Script dixie aus dem Ordner src nach /etc/init.d und das Programm aus bin nach /usr/local/bin/ verschoben werden:


<code>sudo mv DixieUhr/src/dixie /etc/init.d/</code><br>
<code>sudo mv DixieUhr/bin/dixie /usr/local/bin/</code>

Dann noch ein paar Rechte setzen:


<code>sudo chmod 755 /etc/init.d/dixie</code>

Jetzt können wir das script testen:


<code>sudo /etc/init.d/dixie start</code>

und gleich darauf mit strg+c beenden.

Damit das Skript beim booten auch aufgerufen wird, führen wir folgendes aus:

  
  <code>sudo update-rc.d dixie defaults</code>

  
Nun sollte das Programm nach dem Einschalten der Stromversorgung auch ausgeführt werden. Solltest du eines Tages dich umentscheiden und das Programm aus dem Autostart nehmen wollen, musst du folgendes machen:
  
  
<code>sudo update-rc.d -f  dixie remove</code>

Nach einem Neustart wird nun die Uhr automatisch initialisiert. Selbst ein Stromausfall kann dann keinen Schaden mehr anrichten.

# Die Software
Direkt nach dem Start werden die TFT Displays initialisiert und dann die Uhrzeit angezeigt. Der Doppelpunkt verhält sich dann so: oberer Punkt blinkt -> es sind weniger als 30 Sekunden der nächsten Minute vergangen. Unterer Punkt blinkt -> Es dauert weniger als 30 Sekunden, bis die Uhr umspringt.
Dies Verhalten kann man mit dem linken Taster auf dauerhaft Doppelpunkt umstellen.

Wird der mittlere Taster betätigt, wird auf Datum umgestellt bzw. wieder zurück. Wenn man nicht auf Uhrzeit zurück stellt, wird nach spätestens 2 Minuten automatisch wieder auf Uhrzeit zurück gestellt.

# Die Hardware
Die folgende Tabelle zeigt die Verbindungen, die man zwischen Displays und der 40pol. Stiftleiste des Raspi herstellen muss.
<table>
<tr>
<th>Pin TFT</th><th>Pin Raspi</th><th>Funktion</th>
</tr>
<tr>
<td>1</td><td>6 oder 20</td><td>Ground</td>
</tr>
<tr>
<td>2</td><td>17</td><td>3,3V VCC</td>
</tr>
<tr>
<td>3</td><td>29</td><td>SCL</td>
</tr>
<tr>
<td>4</td><td>31</td><td>SDA</td>
</tr>
<tr>
<td>5</td><td>22</td><td>Reset</td>
</tr>
<tr>
<td>6</td><td>18</td><td>D/C</td>
</tr>
<tr>
<td>7</td><td>XX</td><td>CS</td>
</tr>
<tr>
<td>8</td><td>1</td><td>LED</td>
</tr>
</table>

XX ist Chip Select und für die 5 TFTs wie folgt festgelegt:
<table>
<tr>
<th>TFT Nr</th><th>Pin Raspi</th><th>Funktion</th>
</tr>
<tr>
<td>1</td><td>24</td><td>Stunde 10er</td>
</tr>
<tr>
<td>2</td><td>26</td><td>Stunde 1er</td>
</tr>
<tr>
<td>3</td><td>21</td><td>Doppelpunkt</td>
</tr>
<tr>
<td>4</td><td>19</td><td>Minute 10er</td>
</tr>
<tr>
<td>5</td><td>23</td><td>Minute 1er</td>
</tr>
</table>


Die drei Taster sind an Pin 36 (links), Pin 38 (Mitte) und Pin 40 (rechts) angeschlossen. Werden diese Pins mit Ground verbunden, dann lösen sie die entsprechende Aktion aus. Die Taster werden jede Sekunde einmal abgefragt. Mann muss also im ungünstigsten Fall eine Sekunde den Taster gedrückt halten. Die Aktion ist aber sofort aktiv und wird im Display angezeigt.
![RaspI-Zero-Anschluss](https://user-images.githubusercontent.com/10975733/215592685-c5fab713-e974-4ba1-8314-3fae2f500d09.jpg)
<br>Wie man am Bild der Uhr sieht, habe ich Touch-Taster vorgesehen. Ursprünglich habe ich die mit dem Arduino direkt abgefragt. Das geht mit dem Raspi leider nicht. Also muss man entweder richtige Taster nehmen, oder wie ich Sensorplatinen (China oder Adafruit) einbauen oder den nun arbeitslosen Arduino damit beauftragen. <br>
![Touchsensor](https://user-images.githubusercontent.com/10975733/215832064-42bb1aa4-085e-4800-920d-910371f6455f.jpg)
https://www.amazon.de/AZDelivery-Digitaler-Kapazitiver-kompatibel-inklusive/dp/B089QJW6KC/ref=sr_1_13?keywords=Touch%2BSensor&qid=1675184520&sr=8-13&th=1
<br>
# Anpassungen
Da ich aus Gründen der Störsicherheit die Taster gegen GND schalten lasse, muss dieser Touch-Taster leider umgebaut werden. Im Original schaltet er bei Betätigung den Ausgang auf HIGH. Wer den Taster so betreiben möchte, weil er den Umbau scheut, muss im Programm die Pull-Ups für die Eingänge auf Pull-down ändern, oder ganz weglassen. Im Programm sind daas die Zeilen 302, 304 und 306 die bearbeitet oder gelöscht werden müssen. Danach wird im Ordner "DixieUhr" make aufgerufen. Die ausführbare Datei wird dann wieder im Ordner "bin" bereitgestellt. Dies ist allerdings von mir nicht getestet worden.
<br>Der dritte Taster ist derzeit noch ohne Funktion. Die Taster stammen ja noch aus der "Arduino-Zeit", wo man die Uhrzeit nach jedem neuen Einschalten stellen musste ;-)<br>
Sobald ich dafür eine Verwendung habe, wird es hier dokumentiert.

