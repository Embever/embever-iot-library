Embever Serial Protocol driver
===============================

.. include:: ../_defs.rst

About the Embever Serial Protocol
----------------------------------

The Embever Serial Protocol (or ESP) is a simple to use serial protocol design to establish
communication with an Embever IoT device. The ESP is based on the |I2C| protocol and using 2 addition signals.

There are 2 main rules in the ESP: 

* |master| refereed as an application microcontroller (AM)
* |slave| which is the Embever IoT device

Driving the ESP is an inner process of the |EIOT|. In this way there is not necessary to fully understand the working
principles to be able to use it. For more information about the Embever Serial Protocol, please follow this link.

..
    TODO: Missing External link