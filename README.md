# GMDAQ
Data acquisition code for Arduino Geiger-Muller tube board

- spi_rad_logger folder contains Ardunio code for GM logger.
- report_pt_b folder contains Arduino code for environment sensors

The DAQ code is meant to run on Ubuntu.

These python modules are required by the code:
- pyserial (sudo apt-get install python-serial)
- daemon (developed using python-daemon 2.0.5) (python-daemon): https://pypi.python.org/pypi/python-daemon


also needed (ubuntu): libsqlite3-dev

// When you download and extract the tar.gz package for daemon, do this on linux: sudo python setup.py install

Log file is gmdaq.py.log. Everything is logged there.

The code auto-detects the arduinos connected to the USB ports and starts logging them.

If you'll have empty serial port list: (Ubuntu) Add yourself to the dialup group. Empty list is probably due to permission problems.

crontab entries:

* * * * * test -e /home/halil/work/GMDAQ/maintenance.is || (/usr/bin/nice -n 20 /home/halil/work/GMDAQ/publish_plots.zsh)
* * * * * test -e /home/halil/work/GMDAQ/maintenance.is && (cp /home/halil/work/GMDAQ/maintenance.png /home/halil/www/gmlog/last_hour.png)



