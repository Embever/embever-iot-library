GNSS Simple
=============

| Application name : ``gnss_simple``

Overview
**********

The GNSS Simple sample application is a simple application which is querying GNSS data.
This example does not require any additional hardware, just an Arduino compatible board
and a compatible Embever CaaM device supported by the Embever IoT library.
To wire up the hardware devices,
follow the instruction in the :ref:`wiring_simple` topic.

The working principle of this application is to query and show a different kind of GNSS data.


Flashing
***********

To upload this application to an Arduino compatible board navigate to the examples and select the ``gnss_simple`` from
the Embever IoT library in Arduino IDE. Upload the sketch to the board and see the result in the serial terminal.

Testing the Application
**************************

To test the application, the necessary hardware connections should be set up (as mentioned before). After powering up the device, it starts
querying GNSS data. To receive a valid position and other data, a suitable GPS signal quality must be present.

