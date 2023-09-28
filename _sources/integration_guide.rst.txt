.. _integration-guide:

Integration Guide
=====================

.. include:: /_defs.rst

To be able to use the |EIOT| the |INT_LAYER| needs to be set up.
The |INT_LAYER| provides access to the hardware resources in order to establish communication
with an Embever IoT device. For more information about the |INT_LAYER| refer to the :ref:`integration-layer`
topic under the API section.

Integrating the |EIOT| will take 3 parts: the Embever |I2C| driver, the Embever GPIO driver and
the |DELAY_FUNC|.
The integration layer needs to be set up before using the other parts of the |EIOT|.

Integrating the |DELAY_FUNC|
---------------------------------------
The |DELAY_FUNC| is necessary to the Embever IoT Library to detect timeout events.
The following example shows how to set up the |DELAY_FUNC| using the Arduino framework.

.. code-block:: c

    ebv_delay_register(delay);

.. note::
    The functions passed to the delay registration routine expects only 1 argument, which defines the delay value in milliseconds.

The function definition is the following:

.. code-block:: c

    void ebv_delay_register(void (*delay)(unsigned long ms));

Integrating the |GPIO_DRV|
------------------------------------

The |GPIO_DRV| requires 2 functions to be able to read the digital signals on the designed pins.
To set it up, you need to provide those 2 functions and then register them on ``ebv_esp_gpio_cb`` (more details bellow).
Take a look at the following code sample, using the Arduino framework:

.. code-block:: c

    #define PIN_EBV_IRQ     ARDUINO_PIN_A2
    #define PIN_EBV_READY   ARDUINO_PIN_A3

    // Interface function for reading the ready state
    bool gpio_readReady(){
        return digitalRead(PIN_EBV_READY);
    }

    // Interface function for reading the irq state
    bool gpio_readIrq(){
        return digitalRead(PIN_EBV_IRQ);
    }

The above example shows the 2 required interface functions which can fullfil the |GPIO_DRV| requirements.
The functions should have ``boolean`` return type based on the following logic.

.. list-table:: Return value logic
   :widths: 25 25
   :header-rows: 1
   :align: center

   * - GPIO Pin logic level
     - Function return
   * - HIGH
     - ``true``
   * - LOW
     - ``false``

After defining those functions they should be registered into the |GPIO_DRV| using the ``ebv_esp_gpio_cb`` structure.
A possible implementation might look's like the following:

.. code-block:: c

    struct ebv_esp_gpio_cb gpio_cb;
    gpio_cb.readReady = gpio_readReady;
    gpio_cb.readIRQ = gpio_readIrq;
    ebv_esp_gpio_registerGpio(&gpio_cb);

Integrating the Embever |I2C| Driver
---------------------------------------
To integrate the Embever |I2C| Driver, a few interface functions need to be set up.
Starting with a possible implementation first and then discussing them in details.

.. code-block:: c

    void wire_begin(uint8_t address){
        Wire.beginTransmission(address);
    }

    void wire_end(){
        Wire.endTransmission();
    }

    int wire_available(){
        return Wire.available();
    }

    size_t wire_write(uint8_t data){
        return Wire.write(data);
    }

    void wire_requestFrom(uint8_t address, uint8_t nof_bytes){
        Wire.requestFrom(address, nof_bytes);
    }

    int wire_read(){
        return Wire.read();
    }

void wire_begin (uint8_t address)
**************************************
Initiate the device address by the ``address`` parameter and prepare the peripheral to transmit data
(but don't transmit anything to the bus yet).

size_t wire_write (uint8_t data)
**************************************
Copy ``data`` to the internal buffer. Not sending anything to the bus yet. Return 1 if the operation was successful, return 0 if not.

..
    is it returning always 1 in case of success? Since the return type is size_t I thought it was returning
    the amount of data transmitted. If it's not the case then I think it's better to change the type to int

    little bit confusing, this function copy only 1 byte of data to the buffer, so this is why we can say,
    that it is returning with the amount of copied data. If the buffer is full, then it was not able to do the copy,
    so return with 0.
    TODO: Revisit

void wire_end ()
**************************************
Flushes the internal buffers, if any, by writing them to the |I2C| bus and issue a stop sequence.

void wire_requestFrom(uint8_t address, uint8_t nof_bytes)
*********************************************************
Issues a read sequence on the |I2C| bus and tries to read ``nof_bytes`` from device specified by ``address``.
Puts the incoming data into an internal buffer.

int wire_available ()
**************************************
Returns the number of received bytes during the last transaction.

int wire_read()
********************
Reads the incoming data from the internal buffer. This function reads and returns only 1 byte from the internal buffer.
This function should called only if there are data in the internal buffer only. If the internal buffer is empty, should return with -1.

As the above topic suggest, implementing the required |I2C| driver can be more challenging than the |GPIO_DRV| implementation.
Using an internal receive and transmit buffer might also be necessary to fullfil the requirements. The Embever |I2C| Driver interface
was inspired by the Arduino Wire library. To get more information about the working principles and the expectation of the Embever |I2C|
Driver, take look at the `Arduino Wire library <https://github.com/arduino/ArduinoCore-avr>`_.