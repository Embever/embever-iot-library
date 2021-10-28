.. _ebv-iot-msg:

Embever IoT Messaging
=====================

.. include:: ../_defs.rst

The Embever IoT Messaging layer is the top layer of the |EIOT| and
is the only one that the user application shall interact with.
The Embever IoT Messaging layer contains all the necessary functions
which can be required during transferring data between the cloud and devices.
The high level functions are designed to be easy to understand and simple to use.

Data exchange with the Embever Cloud system
-------------------------------------------

To be able to use the Embever IoT solutions successfully, a few working principles need to be clarified.
Exchanging data with the Embever Cloud System is done in 3 major forms:

* Events
* Actions
* Responses

The Embever Cloud Event
***********************
An event is the way to send data from the device to the cloud and has
two fiels:

* **type** *(string)*: event kind
* **payload** *(key-pair string)*: event content

This fields can be used freely during designing an application.
The **payload** is organized by key-value pairs like the following:

.. code-block:: JSON

    {
        "key":"value",
        "led_state":"on"
    }

The Embever Cloud Action
************************
An action is the way to send data from the cloud to the device and has four fields:

*  **type** *(string)*: action kind
*  **id** *(integer)*: action unique identification
*  **payload** *(key-pair string)*: action content
*  **service_level** *(integer)*: sets the action lifetime and supports 3 different service levels:

   *  **0**: The action lives in the cloud while it has been sent to the device
   *  **1**: The action lives in the cloud while a response is not submitted for it
   *  **2**: The action lives in the cloud while a successful response is not submitted to it

For example, an action with **service level 0** will be in the cloud while it is not queried by the device.
As soon as the device queries it, it will be marked in the cloud and not be sent to the device anymore.
On the other hand, actions with **service level 1** or **2** will be send all the time, when
the device queries actions until a response is not submitted for them.
The difference between **service level 1** and **service level 2** is manner of the response result.
In case of **service level 1** the action will be marked and not be sent to the device again if it get
any valid response, the result code in this response can be indicate failure or success, the action will be marked
in both cases. In case of **service level 2** the action will be marked only if the response contain a success result code.

The **id** is automatically assigned on the cloud side and can be omitted, the other fields can be used freely during designing an application.
The **payload** is organized by key-value pairs.

The Embever Cloud Response
**************************
A response is a way to set the result of an action. It is a special type of event, where the corresponding action identifier
should be carried by the device and sent to the cloud.
It has tree fields:

* **id** *(integer)*: should match the corresponding action
* **result_code** *(integer)*: should be **0** for success and **1** for failure (as default on unix systems)
* **payload** *(key-pair string)*: response *optional* content

..
    TODO: Need to be revisited
    there are more fields that we use, like the result which usually uses the HTML result code format
    "result": {
            "status": 200
    },
    should we add those as well?

As seen, the organization of the user payload is made by key-value pairs just like a JSON object.
This form makes possible to easily separate and identify each value during their transfer
to the cloud.

Message type rules
******************
The ``type`` field of an Embever IoT message is used to identify the corresponding message.
There are some rules in this naming schema which need to be fulfil. The rules are the followings:

*  Can not start with ``_`` (e.g. ``_sensor1`` is invalid)
*  Can not be one of the following, this are reserved for the Embever Cloud System

   *  ``getFile``

Sending data to the cloud with the |EIOT|
-----------------------------------------
.. code-block:: c

    ebv_iot_initGenericEvent("event_type_1");
    ebv_iot_addGenericPayload("key1", "value1");
    ebv_iot_addGenericPayload("key2", "value2");
    ebv_iot_submitGenericEvent();

Sending data to the cloud is as simple as the above lines of code.
First create an event and set its type. The type can be any string which fullfils the mentioned rules.

Next, specify a 2 key-value pair as part of the payload. Call this function several times if
you want to add more fields to the payload.
Finally, call the last function to submit the event to the cloud.

Let's see a practical example: sending temperature data of an apartment to the cloud.
A possible implementation would be the following:

.. code-block:: c

    ebv_iot_initGenericEvent("Temperature");
    ebv_iot_addGenericPayload("living_room", 24.7);
    ebv_iot_addGenericPayload("bath_room", 28.2);
    ebv_iot_addGenericPayload("kitchen", 26.1);
    ebv_iot_addGenericPayload("bed_room", 23);
    ebv_iot_addGenericPayload("is_alarm_triggered", false);
    ebv_iot_submitGenericEvent();

The sample above covers temperature data from 4 different places around the apartment and indicating an alarm state too.
The equivalent data in JSON is the following:

.. code-block:: json

    [
        "Temperature",
        {
            "living_room" : 24.7,
            "bath_room" : 28.2,
            "kitchen" : 26.1,
            "bed_room" : 23,
            "is_alarm_triggered" : false
        }
    ]

API documentation
-------------------
| Header file: :file:`ebv_iot.h`
| Source file: :file:`ebv_iot.cpp`