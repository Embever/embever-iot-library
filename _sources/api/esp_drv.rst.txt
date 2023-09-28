.. _esp-prot-drv:

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

To read more about the the |ESP| refer the following documentation:
`ESP protocol definition <https://github.com/Embever/embever-iot-library/blob/main/docs/source/res/EmbeverSerialProtocol.pdf>`_
