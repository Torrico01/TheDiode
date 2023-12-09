# TheDiode
## 1. Setting up the server


### Configuring the OS of the Raspberry
> **_TO DO:_** Download Raspberry Imager
>
> Raspberry Pi OS flashed in SD card
> 
> Raspberry with basic configuration done (Wifi connected, SSH enabled)

Formatting the SD card and flashing the OS: https://www.youtube.com/watch?v=bQYbPIaU0Cs&ab_channel=ElektorTV

(SD card manual repartition: https://www.instructables.com/Repartition-SD-Card-Windows/)


### Configuring the SSH communication
> **_TO DO:_** Download Putty
> 
> Communicate with Raspberry through SSH

Setting up the SSH communication: https://www.onlogic.com/company/io-hub/how-to-ssh-into-raspberry-pi/

> [!TIP]
> Shutdown: sudo shutdown now
> 
> Reboot: sudo reboot


### Configuring static IP addresses and open ports
> **_TO DO:_** Set DHCP static IP address for Raspberry device, from router website
>
> Add open port rule for website

### Installing libraries
> **_TO DO:_** sudo apt-get update
>
> sudo apt-get install mosquitto
>
> sudo apt-get install git
>
> pip install django
