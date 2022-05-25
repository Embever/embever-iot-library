GNSS Report
=============
| Application name : ``gnss_report``

.. include:: ../_defs.rst

Overview
**********

The GNSS Report sample application is using the location report feature of the |CaaM| board.
This example requires additional hardware setup to work properly:
a push button and an LED need to be connected to the arduino board.
To wire up the hardware devices, follow the instruction in the :ref:`wiring_led_btn` topic.

The working principle of this application is to instruct the |CaaM| board to send location data to the cloud.
This is especially useful when the application mcu has limited resources because it does not need to handle
all the gnss data and build the cloud message. The type of the location event and the content of this event is preset, 
the application mcu can not change it.


Flashing
***********

To upload this application to an Arduino compatible board navigate to the examples and select the ``gnss_report`` from
the Embever IoT library in Arduino IDE. Upload the sketch to the board and see the result in the serial terminal.

Testing the Application
**************************
To test the application, the necessary hardware connections should be done (as mentioned before).
After powering up the device, the following information should appear in the serial terminal:

.. code-block:: none

    GNSS Report sample starting

To issue the location report command press the external button.
The following message will appear in the serial terminal:

.. code-block:: none

    Report request submitted

After some time (at maximum 5 minute), based on the result of the location report command
the following message will be sent to the serial terminal:

.. code-block:: none

    Location data sent to the cloud

In case of unsuccessful location report:

.. code-block:: none

    GPS fix timeout, location data not sent

To receive a valid position and other data, a suitable GPS signal quality must be present.

