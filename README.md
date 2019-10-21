
<div class="PageDoc">

<div class="header">

<div class="headertitle">

<div class="title">

RuderwegMessSensor, ein Arduino WeMos D1 Mini Projekt zur Erstellung
eines Ruderweg Mess Sensors

</div>

</div>

</div>

<div class="contents">

<div class="textblock">

# <span id="intro_sec_de" class="anchor"></span> Übersicht

Sensor zum Messen von Ruderwinkel und Ruderweg an Modellflugzeugen und
Anzeige auf Web-Seite. Hier ein kurzer Überblick über die Komponenten
des Sensors. Der Sensor besteht aus der GY-521/MPU6050 Sensorplatine,
die über eine I2C-Bus mit der Mikrokontrollerplatine Wemos D1 verbunden
ist. Der Mikrokontroller liest die Werte des Sensor macht die
Berechnungen, stellt einen Web-Server über eine WiFi-Schnittstelle zur
Verfügung, über die sowohl die Messdaten als auch die Konfiguration
durchgeführt werden kann.
![Architektur](https://raw.githubusercontent.com/Pulsar07/RuderwegMessSensor/master/doc/img/RWMS_Architecture.png)

# <span id="hardware_sec_de" class="anchor"></span> Hardware

## <span id="hardware_subsec_de_ms" class="anchor"></span> Messsensor

Als Messsensor wird der GY-521/MPU-6050 benutzt. Die Genauigkeit liegt
nach Kalibrierung bei Winkeln bis +/- 45° kleiner als 0.5°. Der Baustein
MPU-6050 wird von einer wirklich sehr gut gemachten Libs von J.Rowberg
unterstützt (siehe Link)

Hier ein paar
    Links:

  - <https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf>
  - <https://www.az-delivery.de/products/gy-521-6-achsen-gyroskop-und-beschleunigungssensor>
  - <https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050>

## <span id="hardware_subsec_de_mk" class="anchor"></span> Mikrokontroller

Als Mikrokontroller wird der Wemos D1/ESP8266 benutzt, der ausreichende
Rechenpower und Speicherresourcen bietet und eine WLAN Schnittstelle
hat. Auch dieser Mikrokontroller hat super gemachte Bibliotheken, zur
Nutzung der WiFi-Schnittstelle und zur Programmierung des Web-Servers.

Hier ein paar Links:

  - <https://www.az-delivery.de/products/d1-mini>
  - <https://github.com/esp8266/Arduino>

## <span id="hardware_subsec_de_sp" class="anchor"></span> Schaltplan

Der Schaltplan ist denkbar einfach. Es werden nur 4 Verbindungen
zwischen Sensorplatine und Mikrokontroller benötigt. Das Layout und die
Software sind so ausgelegt, dass mit einer Stiftleiste 4x1 das
Sensorboard mit dem Gesicht in Richtung Mikrokontroller direkt verlötet
werden kann.
![Schaltplan](https://raw.githubusercontent.com/Pulsar07/RuderwegMessSensor/master/doc/img/RWMS_Schaltplan.png)

# <span id="hmi_sec_de" class="anchor"></span> Anleitung

## <span id="hmi_subsec_de_ib" class="anchor"></span> Inbetriebnahme

  - Stromversorgung
      - Der Sensor / das Mikroprozessorboard ist mit einem
        Micro-USB-Anschluss ausgestattet, hier kann man jedes
        handelsübliche USB-Netzteil anschließen oder besser jede
        normale Powerbank. Damit ist man in der Werkstatt oder auf dem
        Flugfeld mobil ausgestattet.
  - WiFi
      - Der Sensor muss zuerst mit Smartphone oder PC verbunden werden.
        Dazu stellt der Sensor per WiFi einen Accesspoint mit der SSID
        "UHU" und Kennwort "12345678" zur Verfügung. Ist das Gerät mit
        diesem WLAN verbunden, kann im Web-Browser über die Adresse
        <http://192.168.4.1> die Benutzeroberfläche benutzt und der
        Sensor konfiguriert werden. Sowohl obige SSID als auch das
        Kennwort können danach geändert werden.
      - Auf der Einstellseite kann eine SSID und ein Kennwort für ein
        WLAN (WLAN-Client) konfiguriert werden, mit dem sich der Sensor
        verbinden. Dabei wird dem Sensor eine IP-Adresse zugewiesen, die
        am WLAN-Router abgefragt werden muss. Änderungen der WLAN
        Einstellungen müssen gespeichert werden und werden erst nach
        Neustart aktiv.
      - Ist die Verbindung zu einem WLAN konfiguriert (WLAN-Client),
        kann auf der Einstellungsseite, der Accesspoint deaktiviert
        werden (nach Speichern und Neustart). Kann beim Neustart keine
        Verbindung zum konfigurierten WLAN aufgebaut werden, wird der
        Accesspoint-Mode aber trotzdem aktiviert, damit ein Zugang zum
        Gerät möglich ist.
  - Nutzung des Sensorboard GY-521 mit MPU-6050
      - Genauigkeit: Der MEMS Chip des MPU-6050 sollte Winkelauflösungen
        besser als 0.5° bei 45° Ausschlag messen können, was bei einer
        60mm Rudertiefe von 60mm einen Fehler von kleiner als 0.5mm
        ergibt.
      - Kalibrierung: Damit der MPU-6050 allerdings diese Genauigkeit
        erreicht, muss er nachträglich kalibriert werden. Die Software
        unterstützt diese Funktion und kann die Werte intern speichern.
        Zur Kalibrierung muss die GY-521-Sensorplatine mit der flachen
        Rückseite möglichst exakt horizontal aufgelegt werden. Dann den
        Kalibrier-Button drücken und ca. 5s warten bis die Kalibrierung
        beendet ist. Dabei sollte die Auflagefläche (Tisch) nicht bewegt
        werden und frei von Vibrationen sein.
      - Einbaulage: Die Sensorplatine sollte auch genau so, wie bei der
        Kalibrierung, betrieben werden. Also die flache Seite nach unten
        und die Seite mit den Elektronikbausteinen nach oben. Nur so
        wird die oben genannte Genauigkeit erreicht.
      - Achsen und Anzeige-Genauigkeit: Auf der Konfigurationsseite,
        kann die Bezugs-Achse der Winkelmessung, je nach Einbaulage in
        der Klemmeinrichtung ausgewählt werden. Zudem sind diverse
        Anzeigegenauigkeiten für die Winkel und die Ruderwegs-Messung
        auswählbar. Die Anzeige hat zwar immer auf 2 Dezimalstellen,
        aber intern wird dann gerundet.

## <span id="hmi_subsec_de_me" class="anchor"></span> Messen

  - Der mit dem Mikrokontroller verbundene Messensor sollte mit einer
    Klemmvorrichtung fest verbunden sein, und kann dann einfach an eine
    beliebige Stelle des Ruders aufgeklemmt werden. Die Ruderdrehachse,
    sollte möglichst parallel zur ausgewählten Dreh-Achse (X- oder
    Y-Achse) sein. Wie schon beschrieben, muss der Sensor mit dem
    Gesicht nach oben auf dem Ruder befestigt sein.
  - Einschränkungen: Der Sensor kann nur Winkel in Bezug auf die
    Schwerkraft messen. Somit sind Ruderwegsmessungen für das
    Seitenruder nur möglich wenn der Rumpf um 90° gedreht liegt.
  - Der Ruderweg ist abhänig von der Rudertiefe. Diese ist an der Stelle
    zu Messen, an der man den Ruderweg messen will. In der
    Web-Oberfläche des Sensor kann diese Rudertiefe eingegeben werden.
  - Ist der Sensor so auf dem Ruder angebracht, und die Rudertiefe
    eingestellt, ist die Ruderstellung in die Null-Lage zu bringen.
    Jetzt können Winkel und Ruderweg per "Tara"-Button auf 0 gesetzt
    werden.
  - Bewegt man das Ruder nun nach oben oder unten werden die Ausschläge
    in Grad und Millimeter angezeigt. Sollte das Vorzeichen nicht den
    Erwartungen entsprechen, kann dies bei den Einstellungen angepasst
    werden.
  - Zur Flugphasenmessung kann die Min-/Max-Rudermessung benutzt werden.
    Hier sollte man das Ruder in die Neutralstellung der Flugphase
    bringen. Nun den Schalter für die Min-/Max-Ruderweg-Messung
    aktivieren. Damit wird der aktuelle Ruderausschlag als Neutralwert
    übernommen. Jetzt können die beiden Min-/Max-Werte angefahren
    werden. Alle drei Werte werden bis zur Deaktiverung der Messung
    angezeigt.

Weitere Details gibt es unter [Albatross, Seite für Modellflug und
Technik](http://www.so-fa.de/nh/RuderwegMesssensor)

</div>

</div>

</div>

-----

Generated by
 [![doxygen](doxygen.png)](http://www.doxygen.org/index.html) 1.8.15
