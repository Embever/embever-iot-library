Logging Module
==============

.. include:: ../_defs.rst

The |LOG_MOD| makes it possible to write logs by calling a simple function.
The usage is the same as the standard ``printf`` function:

.. code-block:: c

    p("Starting application: version %d", app_version);

Setting up the |LOG_MOD|
*****************************

The process of setting up the |LOG_MOD| is similar to the process which is used by the |INT_LAYER|.
For example, using the Arduino Serial Library, the setup would be the following:

.. code-block:: c

    // Integration function
    void log_print(char *str){
        Serial.print(str);
    }

    void setup(){
        Serial.begin(9600);
        // Integration function registration
        p_registerPrint(log_print);
    }

..
    I think we could skip this previous example since the Integration Layer came before
    and just use an example on the same format as bellow

As the code examples shows, the registered function should be able to handle a string argument.
Setting up the |LOG_MOD| using the Arduino framework can be done by using the predefined support macros:

.. code-block:: c

    LOG_SETUP_ARDUINO;

    void setup() {
        Serial.begin(9600);  // start serial for output
        LOG_REGISTER_ARDUINO;
    }
