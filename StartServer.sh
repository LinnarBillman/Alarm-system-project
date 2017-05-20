#! /bin/sh

gnome-terminal --working-directory="$HOME/Desktop/Alarm-system-project" -x sudo bash -c 'perl serial-log.pl; exec bash' 
gnome-terminal --working-directory="$HOME/Desktop/Alarm-system-project" -e sudo make login 
gnome-terminal --working-directory="$HOME/Desktop/Alarm-system-project" -e java -jar AlarmSystemServer.jar
