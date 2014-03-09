#!/bin/bash
# Diese Datei:
lokal_file=Release/Speedoino.hex
# ... wird hochgeladen nach:
remote_file=/htdocs/hex_files/Speedoino.hex
# ftp starten und die Befehle mit einem here-doc angeben:
ftp -n <<EOFTP
open ftp.digithi.de
user www.speedoino.de speedoino30165
bin
put $lokal_file $remote_file
quit
EOFTP
