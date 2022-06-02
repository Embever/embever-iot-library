Power Down Mode
================
| Application name : ``ebv_pwr_down``

.. include:: ../_defs.rst

Overview
**********

This sample application is using the power down mode feature of the |CaaM| board.
This example requires additional hardware setup to work properly:
a push button and an LED need to be connected to the arduino board.
To wire up the hardware devices, follow the instruction in the :ref:`wiring_led_btn` topic.

The working principle of this application is to illustrate the |CaaM| board power down mode.
This is especially useful for battery powered applications.

Flashing
***********

To upload this application to an Arduino compatible board navigate to the examples and select the ``ebv_pwr_down`` from
the Embever IoT library in Arduino IDE. Upload the sketch to the board and see the result in the serial terminal.

Testing the Application
**************************
To test the application, the necessary hardware connections should be done (as mentioned before).
After powering up the device, the following information should appear in the serial terminal:

.. code-block:: none

    Power down mode test sample

To start the demo press the external button.
The following message will appear in the serial terminal:

.. code-block:: none

    Sending sample event
    Event sent
    Put CaaM board to power down mode and wait 1 minute...
    Sleep mode activated

After a sample event is sent to the cloud, the power down mode is activated.
About 1 minute later the sample application going to wake up the |CaaM| board to send
the sample event again.