.. _session_management_top:

Session Management
##################

OpenTwin provides four services that handle the session management:

1. Global Session Service (:doc:`GSS </services/global_session_service>`)
2. Local Session Service (:doc:`LSS </services/local_session_service>`)
3. Global Directory Service (:doc:`GDS </services/global_directory_service>`)
4. Local Directory Service (:doc:`LDS </services/local_directory_service>`)

These services handle the startup logic for each session, monitor the running services and try to balance the CPU & GPU load on each node.

The Directory Services (:doc:`GDS </services/global_directory_service>` and :doc:`LDS </services/local_directory_service>`) handle the service management itself.
They are responsible for starting, monitoring and shutting down services.

The Session Services (:doc:`GSS </services/global_session_service>` and :doc:`LSS </services/local_session_service>`) handle the session management itself.
They are use to create a session, request services from the :doc:`GDS </services/global_directory_service>` and broadcast messages in a session.


Service Information Overview
****************************

The following diagram shows which Session and Service related information is known in the four :ref:`session management<session_management_top>` related services.

.. image:: images/service_information_overview.svg
    :width: 55%


Create a Session
****************

The session creation can be initiated from any service.
Since currently the Frontend Application is the only "Service" that can be started by a user, it is the only possible way to start a Session.

...

.. image:: images/service_start_logic_sequence.svg
    :width: 40%

Close a Session
***************

Any service can initiate a session close.

.. image:: images/session_close_sequence.svg
    :width: 40%

