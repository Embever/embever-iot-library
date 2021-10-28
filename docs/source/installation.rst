.. include:: _defs.rst

Installation
============

Dependencies
------------
The |EIOT| depends on the CWPack library written by Claes Wihlborg (`available here <https://github.com/clwi/CWPack>`_).
To use it you just need to build this library (one source and three header files) together with |EIOT|.

Install as an Arduino library
-----------------------------
There are several ways to use the |EIOT| with Arduino but this document is going
to show the most frequent way: adding it as an external library.
In this way, any Arduino sketch will be able to use it.

The |EIOT| is distributed on GitLab where it can be downloaded as a compressed
file (to get more information about handling compressed libraries in Arduino IDE
please refer the following `article <https://www.arduino.cc/en/guide/libraries>`_).
Installing it straight from arduino-cli is also possible with ``arduino-cli lib install``
command and using a remote repository URL or a compressed file as parameter.
For more information follow this `link <https://arduino.github.io/arduino-cli/0.19/commands/arduino-cli_lib_install/>`_.


Verifying the installation
---------------------------
To make sure that the libraries are ready to use, try to compile one of the sample application.
They can be found between the Arduino sample applications.