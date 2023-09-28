Embever IoT Core Salesforce App
===============================
Embever IoT Core Salesforce App contains the necessary salesforce components that are used to connect the salesforce organisation to Embever IoT Core.
This app should be installed in the salesforce organistion to be able to communicate with the Embever IoT Core from salesforce. You can install this app
while creating a salesforce connection in the Embever IoT Core console.

The package contents can be categorized into mainly two groups. 

    1. :ref:`platform_events`
    2. :ref:`utility_functions`

.. _platform_events:

Platform Events
+++++++++++++++
Embever IoT Core Salesforce App Package contains three different Platform Events definition, which is used to communicate between your salesforece organisatio 
and Embever IoT Core.

The platform events are defined are as follows:

    1. :ref:`Device Message`
    2. :ref:`Outbound Message`
    3. :ref:`Inbound Message`

.. _Device Message:

Device Messsage
---------------
Platform Event for Devices is primarily used to syncronize devices from IoT Core to Salesforce. In Salesforce users can listen
to this platform event and update or create their own custom Device objects using the information from the platform event.

.. list-table:: Custom Fields of Device Message platform event
    :widths: 15 50 50 25 10
    :header-rows: 1

    * - Label
      - API Name
      - Description
      - Data Type
      - Required
    * - DeviceId
      - embevercore__DeviceId__c
      - ID of the device in Embever IoT Core
      - Text
      - Yes
    * - Device Name
      - embevercore__DeviceName__c
      - Alias Name of the device in Embever IoT Core
      - Text
      - Yes
    * - Device Type
      - embevercore__DeviceType__c
      - Indicates the type of the device connectivity technology. Contains 'virtual' for virtual devices. Otherwise contains the type of connectivity technology the device is using to communicate to Embever Core.
      - Text
      - Yes
    * - Application Id
      - embevercore__ApplicationId__c
      - The Application Id of the application the device has been assigned to in Embever Core.
      - Text
      - Yes
    * - Activated
      - embevercore__Activated__c
      - Indicates the activation status of the device. If false, the device has not been activated and cannot send or receive messages. Devices can be activated and deactivated in Embever Core.
      - Checkbox
      - Yes
    * - Date/Time
      - embevercore__DateTime__c
      - The Date/Time a message was issued.
      - Date/Time
      - Yes

Example:

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

.. _Inbound Message:

Inbound Message
---------------
Inbound Message event is used to publish the transaction messages from Embever IoT core to the salesforce organisation. 
Cases when inbound message platform events are used:

    1. When device sends an event
    2. When device sends a result for an action in Embever IoT core
    3. When an action status is updated
    
By simply subscribing to this Plaform event from salesforce you can get the transaction messages from Embever IoT Core.
Here we refer both the IoT Core Event and Action as a Transaction. 

.. list-table:: Custom Fields of an Inbound Message platform event
    :widths: 15 50 50 25 10
    :header-rows: 1

    * - Label
      - API Name
      - Description
      - Data Type
      - Required
    * - Type
      - embevercore__Type__c
      - Indicates the type of the message. Allowed values are 'action' and 'event'.
      - Text
      - Yes
    * - Context Id
      - embevercore__ContextId__c
      - Contains the embevercore__Outbound__e.embevercore__ContextId__c for a specific action to link between the outbound and inbound action messag if desired.In most cases this should be the Salesforce Record Id of the Device Action Record that triggered the creation of the Outbound Message
      - Text
      - No
    * - Created By Core
      - embevercore__CreatedByCore__c
      - ndicates whether the message originated in Embever Core or within Salesforce. When true, the messages originated outside of this Salesforce Organization.
      - Checkbox
      - No (Defaults to Unchecked)
    * - Date/Time
      - embevercore__DateTime__c
      - The Date/Time a message was issued.
      - Checkbox
      - Yes
    * - Device Id
      - embevercore__DeviceId__c
      - Contains the Embever Core Device Id of the device the message is related to.
      - Text
      - Yes
    * - Id
      - embevercore__Id__c
      - Unique id of the message within Embever Core. Can be either action id or event id from Embever IoT Core.
      - Number
      - Yes
    * - Name
      - embevercore__Name__c
      - Contains the event or action name the device used when sending the payload, e.g. "currentLocation".
      - Text
      - Yes
    * - Payload
      - embevercore__Payload__c	
      - Contains the message payload in JSON format. Can be a JSON object, JSON array, or JSON value.
      - Long Text Area
      - No
    * - Status Id
      - embevercore__StatusId__c
      - Id of the Status of the Action/Event in Action/Event Lifecycle (e.g. 'action_sent') as in the Embever IoT Core.
      - Number
      - No
    * - Status Name
      - embevercore__StatusName__c
      - Status of the Action/Event in Action/Event Lifecycle (e.g. 'action_sent')
      - Text
      - No
    * - Status Changed At
      - embevercore__StatusChangedAt__c
      - Date Time when the status of the transaction changed.
      - DateTime
      - No
    
       
Example:

Inbound Platfrom event body when sending IoT core event to Salesforce.

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


.. _Outbound Message:

Outbound Message
----------------
Outbound Message platform event is used to send message from salesforce organisation to Embever IoT Core.

Cases when outbound message platform events are used:

    1. When an action is to be sent to the device
    2. Cancel an action

.. list-table:: Custome Fields of an outbound messge platform event
    :widths: 15 50 50 25 10
    :header-rows: 1

    * - Label
      - API Name
      - Description
      - Data Type
      - Required
    * - Type
      - embevercore__Type__c
      - Indicates the type of the message. Only 'action' is allowed as a value for outbound messages.
      - Text
      - Yes (defaults to "action")
    * - Operation
      - embevercore__Operation__c
      - Indicates if the message should create a new action or cancel a pending action. If set to 'cancel', the action id to be canceled needs to be provided in embevercore__Id__c
      - Text
      - No
    * - ContextI d
      - embevercore__ContextId__c
      - Can be set to a value that will be sent back in the embevercore__Inbound__e.embevercore__ContextId__c field for related action responses. In most cases this should be the Salesforce Record Id of the Device Action Record that triggered the creation of the Outbound Message.
      - Text
      - No
    * - Created By Core
      - embevercore__CreatedByCore__c
      - Indicates whether the message originated in Embever Core or within Salesforce. When true, the messages originated outside of this Salesforce Organization.
      - Checkbox
      - Yes (Defaults to Unchecked)
    * - Device Id
      - embevercore__DeviceId__c
      - Contains the Embever Core Device Id of the device the message is related to.
      - Text
      - Yes
    * - Id
      - embevercore__Id__c
      - Unique id of the message within Embever Core. Can be either action id or event id from Embever IoT Core.
      - Number
      - No
    * - Name
      - embevercore__Name__c
      - Needs to be set to the Action Name that tells the Device how to handle the Payload, e.g. "setSettings".
      - Text
      - Yes
    * - Payload
      - embevercore__Payload__c	
      - Contains the message payload in JSON format. Can be a JSON object, JSON array, or JSON value.
      - Long Text Area
      - No    
    * - Service Level
      - embevercore__ServiceLevel__c
      - Can be set to the desired service level for the action. Defaults to 0. Valid service levels are: 0 - fire and forget, 1 - ensure the device received the message, 2 - ensure the device processed the message (with guaranteed response)
      - Number
      - No
    
  

Outbound Platfrom event body when sending IoT core Action from Salesforce.

.. code-block:: JSON

    {
        "embevercore__Operation__c": "create",
        "embevercore__DeviceId__c": "xyzA",
        "embevercore__Name__c": "setLED",
        "embevercore__Payload__c": "{ \"LED\": \"off\" }",
        "embevercore__ServiceLevel__c": 2,
        "embevercore__ContextId__c": "a027Q00000198BpQAI"
    }


.. _utility_functions:

Utility Functions
+++++++++++++++++
Apart from the platform events IoT core salesforce app also contains some utility functions which can be used to transform data.
The utilty functions defined on the IoT Core salesforce app as an Apex Class are shown below.

.. list-table:: Apex Classes in IoT core saelesforce App
    :widths: 20 80
    :header-rows: 1

    * - Name
      - Description
    * - GetJsonBoolean
      - Returns the value for a specific key with Boolean type from a JSON string
    * - GetJsonDateTime
      - Returns the value for a specific key wtih DateTime type from a JSON string
    * - GetJsonDecimal
      - Returns the value for a specific key wtih Decimal type from a JSON string
    * - GetJsonKeyExists
      - Returns true if a specific key exists in a JSON string else returns false
    * - GetJsonString
      - Returns the value for a specific key wtih string type from a JSON string
    * - GetJsonValue
      - Returns a valid JSON object as a JSON string
    
   