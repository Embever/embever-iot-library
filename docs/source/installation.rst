.. include:: _defs.rst

Installation
============

Dependencies
------------
The |EIOT| depends on the CWPack library written by Claes Wihlborg (`available here <https://github.com/clwi/CWPack>`_).
To use the |EIOT| all dependencies should be fulfilled.

Install as an Arduino library
-----------------------------
The easiest way to install the |EIOT| is to add it as an external library since
the structure of the |EIOT| repository is design to be compatible with an Arduino library. 

The |EIOT| is distributed on Github where can be downloaded as a compressed
file (to get more information about handling a compressed libraries in Arduino IDE
please refer the following `article <https://www.arduino.cc/en/guide/libraries>`_).
Installing by arduino-cli is also possible using the ``arduino-cli lib install``
command.
The arduino-cli can install a library directly from a remote repository, in this way, downloading an archive file is not necessary.
For more information follow this `link <https://arduino.github.io/arduino-cli/0.19/commands/arduino-cli_lib_install/>`_.


.. note::
   | Using ``arduino-cli`` to install the |EIOT| may require to enable unsafe sources.
   | Use the following command to enable installing from zip file:
   | ``arduino-cli config set library.enable_unsafe_install true``

Verifying the installation
---------------------------
To make sure that the libraries are correctly installed, try to compile one of the sample application which
can be found between the Arduino sample applications.