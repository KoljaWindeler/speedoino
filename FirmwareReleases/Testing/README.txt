12.04.2013  Speedoino_1v13                  + Geschwindigkeitsfilter, Menü zur Auswahl des CAN Typ, CAN bearbeitung im interrupt, 1 Mhz SPI Clock
24.06.2013  Speedoino_2v1_1                 + CAN Warning der MIL Lampe für Triumph deaktiviert
27.06.2013  Speedoino_v2.1-6-g5eff85a.hex   + Bilder werden nicht von den Tachoelemente übermalt, wenn man dem Tacho per Handy anweist, sie anzuzeigen
30.06.2013  Speedoino_v2.1-7-g0a953d1.hex   + Navigationsanweisung mit durchgehendem Balken, "#d" aus der Beschreibung der Navidatei gelöscht (im Auswahl-Menü)
14.07.2013  Speedoino_v2.1-26-g8514c15.hex  + GPS mit 10Hz updaterate und 115200 baud, leeres Laptimer menü, Zeitstempel mit ms im Log, kleine Verbesserungen
19.07.2013  Speedoino_v2.1-36-g12971d0.hex  + Erste Testing Version mit Speedo-LapTimer, 
                                            - Blinker reset im stehen
25.07.2013  Speedoino_v2.1-56-g34d8c55.hex  + Speedo-LapTimer Test Version. Basis Funktion sehen gut aus, GPS logging scheinbar defekt
28.07.2013  Speedoino_v2.1-64-gd85ae67.hex  + Animationen auf dem Tacho mit der Android App mit einstellbarer Geschwindigkeit abspielen
31.07.2013  Speedoino_v2.1-78-g4e1625f.hex  + Zeit der letzte Runde im Laptimer für 5 sec anzeigen,
                                            - Upload/Download von Dateien verbessert (neue Android App nutzen!), 
                                            - Animationen werden auf dem Tacho mit verminderter Helligkeit abgespielt (bessere Graustufen-Unterschiede)
                                            - GPS Logging repariert (dritter Anlauf -> geht jetzt scheinbar)
                                            - Navigation endet beim letzten Punkt und zeigt ihn danach weiterhin an (vorher kam eine Fehlermeldung)
                                            - Kommentar der Android App zur Navigationsdatei wird im Tacho angezeigt
                                            - LapTimer auf beste RundenZeit umgebaut ... aber noch immer Alpha Stadium, Bug mit fehlerhaften Delay korrigiert. 
                                            - Noch auf Serial Debug Mode, also Filedownload/Android App im Racemodus nicht nutzbar.
03.08.2013  Speedoino_v2.1-89-g46962a1.hex  + GPS logging im binären format testweise hinzugefügt, ist momentan dauer aktiv und nicht auswählbar (TODO)
                                            - Binär werden nur 15 statt 63 Byte pro Punkt benötigt, das reduziert die Downloadzeit erheblich.
07.08.2013  Speedoino_v2.1-98-g751cb02.hex  + DZM erheblich beschleunigt!, LapTimer Delay zurücksetzen beim Überfahren der Start/Ziel linie
                                            - LapTimer initialisierung merkwürdig, Fehlerquelle unbekannt, Debug hinzugefügt
14.08.2013  Speedoino_v2.1-118-g9e5101d.hex + DZM für alle Systeme verbessert (Analog+CAN)
                                            - LapTimer RealTime Best Lap Berechnung korrigiert
                                            - Menü für Laptimer Bestlap Modus hinzugefügt
                                            - Laptime Zeitberechnungs Fehler behoben
                                            - Debug Ausgaben wieder vollständig deaktiviert
14.08.2013  Speedoino_v2.1-119-gf842af8.hex + Menü für GPS log format hinzugefügt
                                            - Navipointer und viele andere werte vom EEPROM auf die SD Karte geschoben
                                            - Speicheroptimierung (2520/8192 Byte RAM frei) 
18.08.2013  Speedoino_v2.1-141-gf2aae3e.hex + === POI Warner hinzugefügt === 
                                            - RGB LEDs wechseln auf FlasherFarbe wenn POI nah ist
                                            - Warnung für defekten SpeedSensor hinzugefügt (GPS übernimmt bei fehlendem Sensor die Geschwindigkeit)
                                            - Freien Speicher durch POI-Warner reduziert (2388/8192 Byte RAM frei) (187940/256000 Byte Flash genutzt)
                                            - Die Blitzer DB Download unter:  SD_files/CONFIG/POI.TXT
19.08.2013  Speedoino_v2.1-147-g20644d8.hex + Datei-Auswahl-Menü als Vorversion für den POI Finder hinzugefügt
                                            - Beschleunigter DZM Pendelt .. Ursache unbekannt  
                                            - Freien Speicher durch POI-Finder reduziert (2376/8192 Byte RAM frei) (190092/256000 Byte Flash genutzt)
                                            - Apotheken und Tankstellen hinzugefügt Download unter: /SD_files/POI
21.08.2013  Speedoino_v2.1-160-gb21e7a2.hex + Erste Version des POI Finder
                                            - Freier Speicher vergrößert da die GPS Configstring in den Flash gewandert sind, RAM 2530 Byte, Flash 195184 Byte
                                            - DZM Pendeln eventuell gedaempft: Die exakte wird mehr getiefpasst, die extra Pulse werden erst bei einer Abweichung von 1000 gestartet
22.08.2013  Speedoino_v2.1-166-g7213ebd.hex + Neue Methode die analoge Drehzahl zu messen um Pendeln zu verhindern (Fehler in der Abtastzeitberechnung gefunden), Tiefpasswert wieder reduziert um Verzögerungen zu verhindern
                                            - Bisher recht gute Ergebnisse mit dem DZM: Gleiche Reaktionstreu wie Versioin 98 und kein Pendeln mehr, trotzdem erstmal weiterhin Testing, TESTER Bestätigung nötig!
                                            - POI Warner an verschiedenen Stellen getestet und sehr zufrieden, TESTER Bestätigung nötig!
                                            - GPS Schnittstellengeschwindigkeit auf 9600 Baud reduziert, solange keine 10Hz nötig sind. Vielleicht waren die 3.3V Pegel bei 115k nicht genug für die 5V Logik des ATm2560
01.09.2013  Speedoino_v2.1-188-ge29bf94.hex + DZM hat leichte Schrittverluste daher jetzt mit 25ms dauerfeuer
                                            - GPS Geschwindigkeit scheint Fehler zu beheben, daher jetzt immer 19200 (10Hz passen auch bei 19200)
                                            - Neuer Modus zur Beleuchtung des Zeigers: Die RGB LEDs werden nur im Bereich des Zeigers aktiv (per Menü einstellbar, Vorgabe: alle immer aktiv)
                                            - Abstandschätzer beschleunigt, berechnet nun nach Wunsch die Quadrate des Abstands, dafür keine Wurzel
                                            - Menü Programmierung optimiert, sollte nicht sichtbar sein
05.09.2013  Speedoino_v2.1-212-g84c55e0.hex + Hauptsächlich nur Bug fixing und "Änderungen unter der Haube" 
                                            - Neuer Zeigerbeleuchtungsmodus blieb bei verlassen aktiv: gefixt
                                            - Schaltblitz ging nicht mehr aus: gefixt
                                            - Abstand zum Blitzer wurde falsch berechnet und daher war der Warner quasi sinnfrei: gefixt
                                            - Eine Funktion zum "gleichen design" im Menü hinzugefügt, hauptsächlich um Speicher zu sparen (wurde enger) Möglicherweise gehen ein paar "on/off" oder "auswahl" menüs nicht, hab aber alle getestet
                                            - Reduced Stepper Accel from 900 to 600 wegen Schrittverlusten (großen!)
                                            - Removed unused vars
                                            - 3018 Byte SRAM frei, 195470 Byte Flash belegt
19.10.2013  Speedoino_v2.2-33-g3fc4807.hex  + Neue Ansteuerung des Schrittmotors daher bitte auch die Watchdog.hex flashen!!!
					    - Gar keine Schrittverluste mehr trotz langer Fahrt durch den Harz, Mehr Info in dem News Artikel auf der Homepage
					    - Bin noch dabei die Dämpfung optimal einzustellen, sieht aber schon ganz gut aus. Reagiert schneller!
				   	    - Fix der Winterzeit Berechnung
					    - 2919 Byte SRAM frei, 197582 Byte Flash belegt
================================================================================================================================================================================================


01.08.2013	SpeedoAndroid.apk               + Settings Tab zur Auswahl vom verifizieren hinzugefügt und variable Geschwindigkeit für die Animation eingebaut
                                            - Upload/Download von Dateien verbessert (neue Android App nutzen!),
                                            - Runden Auswertung in den Google Earth
                                            - Zu jedem Waypoint werden Paths erzeugt
                                            - Alle Upload und Download Dialoge zeigen nun Geschwindigkeit und Restzeit an
03.08.2013	SpeedoAndroid.apk               + Dekodieren des binären GPS Formats hinzugefügt	
22.08.2013	SpeedoAndroid.apk               + Upload von POI über Config Button  hinzugefügt
02.10.2013	SpeedoAndroid.apk               + Fix: Upload von Skins ins Richtige Verzeichniss
														
