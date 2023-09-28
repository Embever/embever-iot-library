.. _integration-layer:

The Integration Layer
=====================

.. include:: ../_defs.rst

The |INT_LAYER| aims to connect the |EIOT| with an external set of functions
or with a library which can provide the necessary functions
to drive the |I2C| bus and handle the GPIO interface. In this way, the |EIOT| is capable of
use any underlying implementation to issue commands on the |I2C| bus and read digital states of
a GPIO pin.

To achieve this functionality, the designed functions shall be provided to the |EIOT| trough
an internal type. This methodology is widely used to set up interfaces in C language.
There are already supported platforms in the |INT_LAYER|, which can be used off-the-shelf.

The supported frameworks are:

* Arduino

Using the supported platforms is straightforward. There are macro definitions for setting up the
necessary functions and passing them to the |INT_LAYER|.

The example below shows how to set it up for the Arduino Wire Library.

.. code-block:: c

    #include <Arduino.h>
    #include <Wire.h>
    #include "ebv_iot.h"

    // Define interface functions
    EBV_SETUP_ARDUINO_CB;

    void setup() {
        // Passing the interface to the Integration layer
        EBV_REGISTER_ARDUINO_CB;
    }

    void loop(){

    }

To get more information about how to set up and use the |INT_LAYER| please refer
the :ref:`integration-guide` topic.