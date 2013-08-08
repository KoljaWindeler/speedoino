12.04.2013 	Speedoino_1v13					+ Geschwindigkeitsfilter, Menü zur Auswahl des CAN Typ, CAN bearbeitung im interrupt, 1 Mhz SPI Clock
24.06.2013	Speedoino_2v1_1					+ CAN Warning der MIL Lampe für Triumph deaktiviert
27.06.2013	Speedoino_v2.1-6-g5eff85a.hex	+ Bilder werden nicht von den Tachoelemente übermalt, wenn man dem Tacho per Handy anweist, sie anzuzeigen
30.06.2013	Speedoino_v2.1-7-g0a953d1.hex	+ Navigationsanweisung mit durchgehendem Balken, "#d" aus der Beschreibung der Navidatei gelöscht (im Auswahl-Menü)
14.07.2013	Speedoino_v2.1-26-g8514c15.hex	+ GPS mit 10Hz updaterate und 115200 baud, leeres Laptimer menü, Zeitstempel mit ms im Log, kleine Verbesserungen
19.07.2013	Speedoino_v2.1-36-g12971d0.hex	+ Erste Testing Version mit Speedo-LapTimer, 
						- Blinker reset im stehen
25.07.2013	Speedoino_v2.1-56-g34d8c55.hex	+ Speedo-LapTimer Test Version. Basis Funktion sehen gut aus, GPS logging scheinbar defekt
28.07.2013	Speedoino_v2.1-64-gd85ae67.hex	+ Animationen auf dem Tacho mit der Android App mit einstellbarer Geschwindigkeit abspielen
31.07.2013	Speedoino_v2.1-78-g4e1625f.hex	+ Zeit der letzte runde im Laptimer für 5 sec anzeigen,
						+ Upload/Download von Dateien verbessert (neue Android App nutzen!), 
						+ Animationen werden auf dem Tacho mit verminderter Helligkeit abgespielt (bessere Graustufen-Unterschiede)
						+ GPS Logging repariert (dritter Anlauf -> geht)
						+ Navigation endet beim letzten Punkt und zeigt ihn danach weiterhin an (vorher kam eine Fehlermeldung)
						+ Kommentar der Android App zur Navigationsdatei wird im Tacho angezeigt
						+ LapTimer auf beste RundenZeit umgebaut ... aber noch immer Alpha Stadium, Bug mit fehlerhaften Delay korrigiert. 
						- Noch auf Serial Debug Mode, also Filedownload/Android App im Racemodus nicht nutzbar.
03.08.2013	Speedoino_v2.1-89-g46962a1.hex	+ GPS logging im binären format testweise hinzugefügt, ist momentan dauer aktiv und nicht auswählbar (TODO)
						- Binär werden nur 15 statt 63 Byte pro Punkt benötigt, das reduziert die Downloadzeit erheblich. 											  
07.08.2013	Speedoino_v2.1-98-g751cb02.hex	+ DZM erheblich beschleunigt!, LapTimer Delay zurücksetzen beim Überfahren der Start/Ziel linie
						- LapTimer initialisierung merkwürdig, Fehlerquelle unbekannt, Debug hinzugefügt

================================================================================================================================================================================================


01.08.2013	SpeedoAndroid.apk		+ Settings Tab zur Auswahl vom verifizieren hinzugefügt und variable Geschwindigkeit für die Animation eingebaut
						- Upload/Download von Dateien verbessert (neue Android App nutzen!),
						- Runden Auswertung in den Google Earth
						- Zu jedem Waypoint werden Paths erzeugt
						- Alle Upload und Download Dialoge zeigen nun Geschwindigkeit und Restzeit an
03.08.2013	SpeedoAndroid.apk		+ Dekodieren des binären GPS Formats hinzugefügt															
