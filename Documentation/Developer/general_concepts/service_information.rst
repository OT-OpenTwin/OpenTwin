Service Information
===================

Every service knows its name, type, url and id.
The name and type are set by the service itself.
The url is provided by the :ref:`Directory Services <global_directory_service>`.
The id is provided by the :ref:`Local Session Service <local_session_service>`.

When developing a service you should set the service information as soon as it is known at `OTCore/ThisService.h` as shown in the example below.

.. code-block:: c++

    #include "OTCore/ThisService.h"

    void foo() {
        ot::ThisService::instance().setServiceName("<your service name>");
        ot::ThisService::instance().setServiceType("<your service type>");
        ot::ThisService::instance().setServiceURL("<your service url>");
        ot::ThisService::instance().setServiceID(<your service id>);
    }

.. note::

    Note that services that use the `OTServiceFoundation` api don't have to setup the `ThisService` information.

