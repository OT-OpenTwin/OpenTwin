Authorisation Service
=====================

Actions
-------

.. list-table:: Actions Overview
    :widths: 25 25 50 50
    :header-rows: 1

    * - Endpoint
      - String
      - Handler
      - Brief Description
    * - :ref:`OT_ACTION_LOGIN_ADMIN <OT-ACTION-LOGIN-ADMIN>`
      - LoginAdmin
      - ServiceBase::handleAdminLogIn
      - Processes the login of an administrator. Compares admin credentials with the database.
    * - :ref:`OT_ACTION_LOGIN <OT-ACTION-LOGIN>`
      - Login
      - ServiceBase::handleLogIn
      - Processes the login of a user. Authenticates the user and, if successful, creates a new session including a temporary user.

.. _OT-ACTION-LOGIN-ADMIN:

OT_ACTION_LOGIN_ADMIN
^^^^^^^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""
(No authentication needed)
Processes the login of an administrator. Compares admin credentials with the database.

Parameters
""""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Macro
      - Macro-String
      - Type
      - Description
    * - OT_PARAM_AUTH_USERNAME
      - Username
      - String
      - User name
    * - OT_PARAM_AUTH_PASSWORD
      - Password
      - String
      - User password
    * - OT_PARAM_AUTH_ENCRYPTED_PASSWORD
      - EncryptedPassword
      - Boolean
      - Is the password encrypted

Response
""""""""

.. list-table:: 
    :widths: 25 25 50 50
    :header-rows: 1

    * - Macro
      - Macro-String
      - Type
      - Description
    * - OT_ACTION_AUTH_SUCCESS
      - successful
      - Boolean
      - Are the credentials correct
    * - OT_PARAM_AUTH_ENCRYPTED_PASSWORD
      - EncryptedPassword
      - String
      - Encrypted password (if successful)

.. _OT-ACTION-LOGIN:

OT_ACTION_LOGIN
^^^^^^^^^^^^^^^

Brief description
"""""""""""""""""
(No authentication needed)
Processes the login of a user. Authenticates the user and, if successful, creates a new session including a temporary user.

Parameters
""""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Macro
      - Macro-String
      - Type
      - Description
    * - OT_PARAM_AUTH_USERNAME
      - Username
      - String
      - User name
    * - OT_PARAM_AUTH_PASSWORD
      - Password
      - String
      - User password
    * - OT_PARAM_AUTH_ENCRYPTED_PASSWORD
      - EncryptedPassword
      - Boolean
      - Is the password encrypted

Response
""""""""

.. list-table:: 
    :widths: 25 25 50 50
    :header-rows: 1

    * - Macro
      - Macro-String
      - Type
      - Description
    * - OT_ACTION_AUTH_SUCCESS
      - successful
      - Boolean
      - Are the credentials correct
    * - OT_PARAM_AUTH_PASSWORD
      - Password
      - String
      - Unencrypted password (if successful)
    * - OT_PARAM_AUTH_ENCRYPTED_PASSWORD
      - EncryptedPassword
      - String
      - Encrypted password (if successful)
