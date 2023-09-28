Activating the Embever Account
******************************
When an order is submitted for the Embever Cloud As A Module development kit, an account is automatically created with the given details and an email is sent to activate the newly created account.
To set up the account simply follow the link in the received email. With the personal credentials, logging in to the `Embever Cloud console <https://console.embever.com>`_ or to 
the `Embever Cloud Browsable API <https://api.embever.com>`_ is now possible.

To manage the resources use the Embever Cloud Browsable API. Go to the https://api.embever.com/v2/devices/ where all the personal devices should be listed.

Getting Started with the API
****************************

Getting an access token
***********************
The API uses JWT tokens to authorize requests to resources. To obtain an access toke an application token is necessary.
To create an application token, use the `tokens endpoint <https://api.embever.com/v2/tokens/>`_ in the Embever Cloud Browsable API and make a POST request with an alias.

.. code-block:: JSON

    {
        "alias": "AppTokenHelloWorld"
    }

The response should be similar:

.. code-block:: JSON

    {
        "id": 45,
        "url": "https://api.embever.com/v2/tokens/45/",
        "alias": "AppTokenHelloWorld",
        "expiry_date": null,
        "token": "eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJhbGlhcyI6IkldfhelsseInVzZXJfaWQiOjE4fQ.MT1Zp2emXHjew8nqYF3CrbjIfk6eqH6zrandtokenBhlpJ9jPlpKChs6lbeybnL1AMgq3R68g",
        "scope": {
            "organisation": "b4ask977-05c5-470f-a936-d3340bff",
            "application": null,
            "user": 36
        },
        "created_at": "2022-01-28T08:26:12.892335Z"
    }

.. note:: 
    The token is displayed only once. Keep it secure as this is used to get the access and refresh tokens to access personal resources in the Embever IoT Core.


Obtaining an access token is now possible by posting an application token to the ``/authenticate`` endpoint.

.. code-block:: console

    curl --location --request POST 'https://api.embever.com/v2/authenticate/' \
    --header 'Content-Type: application/json' \
    --data-raw '{
        "application_token" : "eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJhbGlhcyI6IkldfhelsseInVzZXJfaWQiOjE4fQ.MT1Zp2emXHjew8nqYF3CrbjIfk6eqH6zrandtokenBhlpJ9jPlpKChs6lbeybnL1AMgq3R68g"
    }'

    {
        "refresh": "eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJ0b2tlbl90eXBlIjoicmVmcmVz2QwZDQ1N2M2NDBhYWIzNzk2NmI2ZmYwMjBmMTQiLCJ1c2VyX2lkIjoxOH0.lC-3jlgeOBYuk5Ul3zTqRFpVA1SEouJ3sHKkSH5Q",
        "access": "eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJ0b2tlbl90eXBlIjoiYWNjZXNzIiwiZXhwIjoxNjQzMA0ZTgyODA5YmI1MzEwZmQ3MzczNSIsInVzZXJfaWQi.dDjwoOpOpxvysqw1-Iotthq9Zrpx1BQQxBzk2Z7h"
    }

Access token can now be passed to the Authorization header to access the resources in the API.

To confirm the above steps, let's try to get a devices by using the ``curl`` utility.

.. code-block:: console

    curl --location --request GET 'https://api.embever.com/v2/devices/' \
    --header 'Authorization: Bearer eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJ0b2tlbl90eXBlIjoiYWNjZXNzIiwiZXhwIjoxNjQzMA0ZTgyODA5YmI1MzEwZmQ3MzczNSIsInVzZXJfaWQi.dDjwoOpOpxvysqw1-Iotthq9Zrpx1BQQxBzk2Z7h'

    [
        {
        "url": "https://api.embever.com/v2/devices/EzJei1BX/",
        "id": "EzJei1BX",
        "name": "HelloWorldDevice",
        "activated": true,
        "application": null,
        "type": "other",
        "webhooks": null,
        "meta": null,
        "sim": "123456789019876",
        "created_at": "2022-01-25T15:58:37.293802Z"
        }
    ]

Accessing other resources in the API using the access token is also possible.

Creating an action
******************
The following example shows how to create actions in the API.

.. code-block:: console

    curl --location --request POST 'https://api.embever.com/v2/actions/' \
    --header 'Authorization: Bearer eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJ0b2tlbl90eXBlIjoiYWNjZXNzIiwiZXhwIjoxNjQzMA0ZTgyODA5YmI1MzEwZmQ3MzczNSIsInVzZXJfaWQi.dDjwoOpOpxvysqw1-Iotthq9Zrpx1BQQxBzk2Z7h'
    --header 'Content-Type: application/json' \
    --data-raw '{
        "device": {
            "id": "EzJei1BX"
        },
        "type": "setLED",
        "payload": {
            "LED": "on"
        }
    }'

    {
        "url": "https://api.embever.com/v2/actions/1003691/",
        "id": 1003691,
        "device": {
            "id": "EzJei1BX",
            "name": "HelloWorldDevice"
        },
        "type": "setLED",
        "payload": {
            "LED": "on"
        },
        "status": {},
        "webhooks": null,
        "service_level": 0,
        "cancelled": false,
        "finished": false,
        "result": null,
        "created_at": "2022-01-28T09:32:25.379189Z",
        "result_code": null,
        "expiry_date": null,
        "expired": false
    }



Getting events from the Device
******************************
The following example shows how to get events from the API.

.. code-block:: console

    curl --location --request GET 'https://api.embever.com/v2/devices/EzJb1BX/events/' \
    --header 'Authorization: Bearer eyJ0eXAiOiJKdsdfOiJUBGsMiJ9.eyJ0b2tlbl90eXBlIjoiYWNjZXNzIiwiZXhwIjoxNjQzMA0ZTgyODA5YmI1MzEwZmQ3MzczNSIsInVzZXJfaWQi.dDjwoOpOpxvysqw1-Iotthq9Zrpx1BQQxBzk2Z7h'
    
    [
        {
            "url": "https://api.embever.com/v2/events/70664807/",
            "id": 70664807,
            "device": "EzJei1BX",
            "sim": "123456789019876",
            "type": "wakeUp",
            "payload": {
                "lat": 525841746,
                "lon": 122812903
            },
            "created_at": "2022-01-13T16:09:21.991434Z"
        },
        ...
    ]

For more details please visit the `Embever core API documentations <https://api-docs.embever.com/>`_.