# GMDAQ
Data acquisition code for Arduino Geiger-Muller tube board

The spi_rad_logger folder contains Ardunio code.

The DAQ code is meant to run on a Raspberry Pi.

Required the python modules:
- pyserial (sudo apt-get install python-serial)
- daemon (developed using python-daemon 2.0.5) (python-daemon): https://pypi.python.org/pypi/python-daemon

(python setup.py install)

if you'll have empty serial port list: add yourself to the dialup group. Empty list may be due to permnission denied error.


