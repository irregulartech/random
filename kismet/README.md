What is this?  It's a tool that automatically detects hardware connected to your computer and generates a kismet_site.conf for it.

I will happily accept bug reports and pull requests for more hardware that is supported by kismet but not detected by this script.

I have included an example kismet.service file.  The purpose of this file is to install in /etc/systemd/system/ and use to run kismet.  This modified version is the same as the upstream kismet version except it runs /usr/local/sbin/kismet-autodetect first to generate a config.  Don't forget to actually put kismet-autodetect in /usr/local/sbin if you use this service file.
I have also included an example override.conf for kismet.service.  Instead of using the included kismet.service, you can use the upstream kismet.service without modification and install override.conf in /etc/systemd/system/kismet.service.d/ which will effectively add the lines from the override into the upstream service file without change of becoming out of date.  This expects kismet-autoconfig installed in /usr/local/sbin/ and runs it before kismet to create a config

Currently available flags:
    `--bluehydra`
        Skip first detected hci0 and ubertooth devices, assuming blue_hydra will be using them instead of kismet
    `--log-location <some_location>`
        By default kismet logs different places on different systems, depending on how it was installed.  If a specific location is desired, kismet-autoconfig can set the location for you as well as check permissions (and existance) of the target.
    `--rtl-skip-zero`
        Skip first detected rtl_sdr devices, assuming something else will be using it instead of kismet
    `--skip-lar`
        Intel wireless devices have an internal "Location Aware Regulatory" feature and do NOT respect system regulatory settings.  By default, kismet-autoconfig will detect that LAR hasn't done it's job and set the correct regulatory domain yet.  When this is detect, kismet-autodetect will trigger a scan which allows LAR to set the correct regulatory domain and enable disabled channels (as appropriate).  This is *required* for WiFi 6E 6GHz support.  When LAR has already enabled 7115 MHz (highest WiFi 6E channel) then a scan isn't run, as it's not required.  This flag disables running a active scan, trading the ability to sniff on potentially disabled channels for additional stealth
    `--truncate-data`
        Configure all wifi devices to truncate_data to decrease logging stress, or various other reasons.  https://kismetwireless.net/docs/readme/datasources/wifi-linux/

kismet-autoconfig.conf
In addition to the available flags, you can simply add extra config options into the kismet-autoconfig.conf file (example included).  One copy of this file will be read and added to the final auto generated config.  Search path is /etc/kismet then /usr/local/etc/kismet
