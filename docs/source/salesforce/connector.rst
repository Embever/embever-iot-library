Integration with Salesforce
===========================

Connecting Your Salesforce Organisation to Embever IoT Core
-----------------------------------------------------------

Your salesforce Organisation can be easily integrated to the Embever IoT core. This enables you to use the data from
Embever IoT core on different business cases on your salesforce organisation.

To connect your salesforce instance to Embever IoT Core follow the following steps:

#. Go to https://console.embever.com/integration/salesforce/new/
#. Select the your type of organisation and an alias. Click on Authorize
#. Next Authorize CAAM Embever by logging in with your salesforce credentials and clicking Allow. After that you will be redirected to the IoT Core console.
#. Install the Embever IoT Core Salesforce App.
#. After installing the IoT Core Salesforce App, Go back to your connection page on the IoT Core console
#. On Install Salesforce App section. Click on "I have already installed the App"
#. Next, select the applications for the integration.

Now you can send or receive data from the devices on the attached applications from salesforce.

Optionally,

#. You can install the sample Embever IoT Core Salesforce App by clicking the "Install Example App in Salesforce" button on the last section.
#. Synchronize all the devices to Salesforce by simply clicking the "Synchronize All Devices to Salesforce" button.


After sucessfully connecting the devices to your salesforce organisation you can now use the platform events defined in the IoT Core App
to communicate to the device via Embever IoT Core.

Synchronizing devices to Salesforce
-----------------------------------
When you synchronizing all the devices to Salesforce from the Embever IoT Core console, a the Device Message Platfrom Event is published to 
your salesforce organisation for each device in your salesforce connection.

Example:  Payload of the device message Platfrom message on Synchronize Devices

.. code-block:: JSON

    {
      "embevercore__DateTime__c": "2022-02-23T11:46:39.357Z",
      "CreatedById": "0057Q000002LlABCQA0",
      "embevercore__DeviceName__c": "HellowWorldDev",
      "CreatedDate": "2022-02-23T11:46:40.212Z",
      "embevercore__ApplicationId__c": "1c43d388-ab3b-4677-9f51-05dfg597a02d",
      "embevercore__Activated__c": true,
      "embevercore__DeviceId__c": "xyzA",
      "embevercore__DeviceType__c": "other"
    }


Receiving Events on Salesforce
------------------------------
All the events from the devices connected to your salesforce organisation will be published to the Inbound message Platform event with type event.
You can read from the Inbound message Platform event from your salesforce instance and can use it further.
E.g. 
Event in Embever IoT Core

 .. code-block:: JSON
    
    {
        "url": "https://api.embever.com/v2/events/15032/",
        "id": 15032,
        "device": "xyzA",
        "sim": "89777777777777766",
        "type": "wakeUp",
        "payload": {
            "lat": 525841746,
            "lon": 112812903
        },
        "created_at": "2022-02-23T08:45:13.889841Z"
    }

Correspondig message received on Subscribing to the Inbound Message Platform event (embevercore__Inbound__e)

.. code-block:: JSON
    
    {
      "embevercore__DateTime__c": "2022-02-23T08:45:13.889Z",
      "embevercore__StatusChangedAt__c": null,
      "embevercore__Name__c": "wakeUp",
      "embevercore__StatusName__c": null,
      "embevercore__ContextId__c": null,
      "embevercore__DeviceId__c": "xyzA",
      "embevercore__StatusId__c": null,
      "CreatedById": "0057Q000002LlABCQA0",
      "embevercore__CreatedByCore__c": false,
      "CreatedDate": "2022-02-23T08:45:15.364Z",
      "embevercore__Type__c": "event",
      "embevercore__Id__c": 15032,
      "embevercore__Payload__c": "{\"lat\": 525841746, \"lon\": 112812903}"
    }


Creating an Action from Salesforce
----------------------------------
An Action in Embever IoT Core is the way to send message to the Device to perform certain actions. To create action on Embever IoT core
you can simply publish to the Outbound Message Platform event from your Salesforce organisation.

Example
Message published on Outbound Message Platform Event to create 'setLED' action for device 'xyzA'.

.. code-block:: JSON

    {
        "embevercore__Operation__c": "create",
        "embevercore__DeviceId__c": "xyzA",
        "embevercore__Name__c": "setLED",
        "embevercore__Payload__c": "{ \"LED\": \"off\" }",
        "embevercore__ServiceLevel__c": 2,
        "embevercore__ContextId__c": "a027Q00000198BpQAI"
    }

Corresponding action created in the Embever IoT Core is as follows

.. code-block:: JSON

    {
        "url": "https://api.embever.com/v2/actions/5530/",
        "id": 5530,
        "device": {
            "id": "xyzA",
            "name": "HellowWorldDev"
        },
        "type": "setLED",
        "payload": {
            "LED": "off"
        },
        "status": {},
        "webhooks": null,
        "service_level": 2,
        "cancelled": false,
        "finished": false,
        "result": null,
        "created_at": "2022-02-23T09:32:23.779159Z",
        "result_code": null,
        "expiry_date": null,
        "expired": false
    }

On creating an action Embever IoT core also publishes a message to the Inbound Message Platform with the information on action created and the corresponding action id.
Example: The payload of the Inbound message for action created is given below. The attributes 'embevercore__StatusName__c' contains the status 'action_created' and 'embevercore__Id__c'
is the id of the action in Embever IoT Core. 

.. code-block:: JSON

    {
      "embevercore__DateTime__c": "2022-02-23T10:35:07.068Z",
      "embevercore__StatusChangedAt__c": null,
      "embevercore__Name__c": "setLED",
      "embevercore__StatusName__c": "action_created",
      "embevercore__ContextId__c": "a027Q00000198BpQAI",
      "embevercore__DeviceId__c": "xyzA",
      "embevercore__StatusId__c": null,
      "CreatedById": "0057Q000002LlABCQA0",
      "embevercore__CreatedByCore__c": true,
      "CreatedDate": "2022-02-23T10:35:07.896Z",
      "embevercore__Type__c": "action",
      "embevercore__Id__c": 5530,
      "embevercore__Payload__c": null
    }



Cancelling an Action from Salesforce
------------------------------------
There are cases you nolonger want the action to be sent to the devices, in that case you can easily cancel the actions from salesforce.
To cancel an action you can use the same Outbound Message Platform event with the operation 'cancel'.

Example
Cancel the action with action id 5530, publish the following message to Outbound Message Platform event

.. code-block:: JSON

    {
        "embevercore__Operation__c": "cancel",
        "embevercore__Id__c": 5530,
        "embevercore__DeviceId__c": "xyzA"
    }

Corresponding action in the Embever IoT Core is as follows, note the cancelled attribute is changed to true.

.. code-block:: JSON

    {
        "url": "https://api.embever.com/v2/actions/5530/",
        "id": 5530,
        "device": {
            "id": "xyzA",
            "name": "HellowWorldDev"
        },
        "type": "setLED",
        "payload": {
            "LED": "off"
        },
        "status": {},
        "webhooks": null,
        "service_level": 2,
        "cancelled": true,
        "finished": false,
        "result": null,
        "created_at": "2022-02-23T09:32:23.779159Z",
        "result_code": null,
        "expiry_date": null,
        "expired": false
    }

On cancelling an action Embever IoT core also publishes a message to the Inbound Message Platform with the information on action cancelled and the corresponding action id.
Example: The payload of the Inbound message for action cancelled is given below. The attributes 'embevercore__StatusName__c' contains the status 'action_cancelled and 'embevercore__Id__c'
is the id of the cancelled action in Embever IoT Core. 

.. code-block:: JSON

    {
        "embevercore__DateTime__c": "2022-02-23T10:41:36.891Z",
        "embevercore__StatusChangedAt__c": null,
        "embevercore__Name__c": "setLED",
        "embevercore__StatusName__c": "action_cancelled",
        "embevercore__ContextId__c": "a027Q00000198BpQAI",
        "embevercore__DeviceId__c": "xyzA",
        "embevercore__StatusId__c": null,
        "CreatedById": "0057Q000002LlABCQA0",
        "embevercore__CreatedByCore__c": true,
        "CreatedDate": "2022-02-23T10:41:37.603Z",
        "embevercore__Type__c": "action",
        "embevercore__Id__c": 5530,
        "embevercore__Payload__c": null
    }


Receiving response of an Action to Salesforce
---------------------------------------------
A device can send the response for an action. This response is stored as an object on the result attribute of the action. For all the devices connected
to the salesforce instance, on receiving the response from the device Embever IoT core publishes the message to the Inbound Message Platfrom event of 
the connected salesforce organisations.

Example: Action on Embever IoT Core with a result

.. code-block:: JSON

    {
        "url": "https://api.embever.com/v2/actions/5531/",
        "id": 5531,
        "device": {
            "id": "xyzA",
            "name": "HellowWorldDev"
        },
        "type": "setLED",
        "payload": {
            "LED": "off"
        },
        "status": {
            "action_sent": "2022-02-23T11:07:50.814981+00:00",
            "action_delivered": "2022-02-23T11:07:50.866270+00:00",
            "response_received": "2022-02-23T11:07:50.962695+00:00"
        },
        "webhooks": null,
        "service_level": 2,
        "cancelled": false,
        "finished": true,
        "result": {
            "led": "off"
        },
        "created_at": "2022-02-23T10:35:07.068386Z",
        "result_code": "Success",
        "expiry_date": null,
        "expired": false
    }
    


Corresponding message on the Salesforce Inbound Message Platform Event is given below. On recevieng the response the attribute 'embevercore__StatusName__c' contains 
'response_received', 'embevercore__StatusId__c' contains the status id from the Embever IoT Core and 'embevercore__Payload__c' contains the result of the action.

.. code-block:: JSON

    {
        "embevercore__DateTime__c": "2022-02-23T11:07:50.962Z",
        "embevercore__StatusChangedAt__c": "2022-02-23T11:07:50.952Z",
        "embevercore__Name__c": "setLED",
        "embevercore__StatusName__c": "response_received",
        "embevercore__ContextId__c": "a027Q00000198BpQAI",
        "embevercore__DeviceId__c": "xyzA",
        "embevercore__StatusId__c": 2522,
        "CreatedById": "0057Q000002LlABCQA0",
        "embevercore__CreatedByCore__c": true,
        "CreatedDate": "2022-02-23T11:07:57.017Z",
        "embevercore__Type__c": "action",
        "embevercore__Id__c": 5531,
        "embevercore__Payload__c": "{\"led\": \"off\"}"
    }










