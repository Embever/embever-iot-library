.. _ebv-esp-gnss:

Embever GNSS
=============

.. include:: ../_defs.rst

The Embever GNSS API is part of the |EIOT|. This API provides access to the CaaM development board GNSS submodule.
The list of the obtainable data is the following:

* Latitude, Longitude, Accuracy, Altitude
* Speed, Heading
* Date, Time

ESP GNSS API
**************

Queries GNSS information from the CaaM development board is possible by calling a query function:

.. code-block:: c

    ebv_gnss_data_t gnss_data;
    bool ret = ebv_local_query_gnss(&gnss_data);

The code snippet above will populate the ``gnss_data`` structure. This type has the following elements:

.. code-block:: c

    typedef struct{
        double lat;
        double lon;
        float altitude;
        float accuracy;
        float speed;
        float heading;
        bool has_fix;
        esp_gnss_datetime_t datetime;
    } ebv_gnss_data_t;

Checking the state of the GPS receiver module is possible by reading the ``has_fix`` flag. In case of a ``true`` value, the 
GPS data is available.

Another way of getting GNSS data is to build a custom query.

.. code-block:: c

    ebv_gnss_data_t gnss_data;
    ebv_local_query_gnss_custom_init();
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_SPEED);
    ebv_local_query_gnss_custom_add(EBV_GNSS_REQUEST_DATETIME);
    ebv_local_query_gnss_custom_add_submit(&gnss_data);

The code sample above builds a custom GNSS query. This can be useful if not all the available GNSS data is necessary to read.
While building a custom query the following enumerations are valid to pass as an argument:

.. code-block:: c

    typedef enum {
        EBV_GNSS_REQUEST_LOCATION = 0,
        EBV_GNSS_REQUEST_SPEED,
        EBV_GNSS_REQUEST_DATETIME,
    } ebv_gnss_request_kind;

API documentation
-------------------
| Header file: :file:`ebv_local.h`
| Source file: :file:`ebv_local.cpp`