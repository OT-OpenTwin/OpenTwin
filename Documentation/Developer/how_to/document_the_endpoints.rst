.. _document_the_endpoints:

Document the endpoints
######################


Example
*******

The following example shows how an endpoint is documented.
The endpoint documentation starts with the first trimmed line that contains the "//api" prefix and ends if the next line does not contain the prefix.

.. code-block:: c++

    //api @security mTLS
    //api @action OT_ACTION_CMD_MyAction
    //api @brief Some short brief description.
    //api Here is a more detailed description.
    //api Here we split the text
    //api
    //api into two paragraphs.
    //api @warning This warning will be added to the detailed section.
    //api This line is part of the warning message.
    //api
    //api The warning can be split into multiple paragraphs aswell.
    //api @detailed To end the warning and go back to the detailed section "@detail or @detailed" must be specified.
    //api @param OT_ACTION_PARAM_MyParam1 String My parameter description.
    //api The parameter description can also contain...
    //api
    //api ...paragraphs.
    //api @note ...also the paramter description can contain nested blocks...
    //api @param OT_ACTION_PARAM_MyParam1 ...and to continue the paramter description we specify the param argument again.
    //api @param OT_ACTION_PARAM_MyParam2 Object Another parameter description.
    //api This is still the parameter description of Param 2.
    //api Since the paramter is an object we need to somehow specify the structure of the nested object.
    //api Ideally we would want to document the structure of the serialized object in the serialization methods of said object.
    //api So a reference to the class documentation would be great, maybe by using the following syntax:
    //api @ref ot::ClassName Text to display for the hyperlink label.
    //api Since a reference is a "one-liner" and will be placed in the currently active segment we are here continuing with the parameter description.
    //api @return Here we type any information about the response.
    //api Which can also contain nested blocks and paragraphs.
    //api
    //api If the response will return a JSON object we use the same syntax rules as with the "@param".
    //api @rparam OT_ACTION_PARAM_MyParam3 String My return parameter description.
    OT_HANDLER(handlerFunctionName, HandlerClassName, OT_ACTION_CMD_MyAction, ot::SECURE_MESSAGE_TYPES)

    //api @security tls
    //api @action OT_ACTION_CMD_MyAction2
    //api @brief Another short brief description.
    //api @param OT_ACTION_PARAM_MyParam4 String My parameter description.
    //api @return This endpoint does not return anything.
    //api If not providing any return text a default text indicating that this enpoint has no return value should be generated.
    OT_HANDLER(shortHandlerFunction, HandlerClassName, OT_ACTION_CMD_MyAction2, ot::SECURE_MESSAGE_TYPES)

    //api @security mtls
    //api @action OT_ACTION_CMD_MyAction3
    //api @brief Short brief description.
    OT_HANDLER(minimalHandlerFunction, HandlerClassName, OT_ACTION_CMD_MyAction3, ot::SECURE_MESSAGE_TYPES)

Example Result
==============

When documenting the actions and parameters we specify the macro since we use it in the code.
The actual string will be looked up in the ``OTCommunication/ActionTypes.h`` file.

This is the resulting documentation:

My Service
----------

*This section is written in Sphinx. The documentation of the endpoints will be generated in an external file that will be imported here...*

Actions
^^^^^^^

.. list-table:: Actions Overview
    :widths: 25 50 25
    :header-rows: 1

    * - Endpoint
      - Brief Description
      - Macro
    * - :ref:`My.Custom.Action <EXAMPLE-ENDPOINT-ONE>`
      - Some short brief description.
      - OT_ACTION_CMD_MyAction
    * - :ref:`My.Other.Action <EXAMPLE-ENDPOINT-TWO>`
      - Another short brief description.
      - OT_ACTION_CMD_MyAction2
    * - :ref:`My.Other.Action <EXAMPLE-ENDPOINT-THREE>`
      - Short brief description.
      - OT_ACTION_CMD_MyAction3

.. _EXAMPLE-ENDPOINT-ONE:

My.Custom.Action
^^^^^^^^^^^^^^^^

Some short brief description.


Here is a more detailed description.
Here we split the text

into two paragraphs.

.. warning::
    This warning will be added to the detailed section.
    This line is part of the warning message.

    The warning can be split into multiple paragraphs aswell.

To end the warning and go back to the detailed section "@detail or @detailed" must be specified.

Parameters
""""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Parameter
      - Type
      - Description
      - Macro
    * - Parameter.Name
      - String
      - My parameter description.The parameter description can also contain...
        
        ...paragraphs.

        .. note::
            ...also the paramter description can contain nested blocks...
        
        ...and to continue the paramter description we specify the param argument again.
      - OT_ACTION_PARAM_MyParam1
    * - AnotherParameterName
      - Object
      - Another parameter description.
        This is still the parameter description of Param 2.
        Since the paramter is an object we need to somehow specify the structure of the nested object.
        Ideally we would want to document the structure of the serialized object in the serialization methods of said object.
        So a reference to the class documentation would be great, maybe by using the following syntax:
        :ref:`Text to display for the hyperlink label. <EXAMPLE-ENDPOINT-ONE>`
        Since a reference is a "one-liner" and will be placed in the currently active segment we are here continuing with the parameter description.
      - OT_ACTION_PARAM_MyParam2

Response
""""""""

Here we type any information about the response.
Which can also contain nested blocks and paragraphs.

If the response will return a JSON object we use the same syntax rules as with the "@param".

.. list-table:: 
    :widths: 25 25 50 50
    :header-rows: 1

    * - Parameter
      - Type
      - Description
      - Macro
    * - YetAnotherParameter
      - String
      - My return parameter description.
      - OT_ACTION_PARAM_MyParam3

Security
""""""""

mTLS

----

.. _EXAMPLE-ENDPOINT-TWO:

My.Other.Action
^^^^^^^^^^^^^^^

Another short brief description.

Parameters
""""""""""
.. list-table::
    :widths: 25 25 50 50
    :header-rows: 1

    * - Parameter
      - Type
      - Description
      - Macro
    * - Parameter.Name
      - String
      - My parameter description.
      - OT_ACTION_PARAM_MyParam4 

Response
""""""""

This endpoint does not return anything.
If not providing any return text a default text indicating that this enpoint has no return value should be generated.

Security
""""""""

TLS

----

.. _EXAMPLE-ENDPOINT-THREE:

My.Minimal.Action
^^^^^^^^^^^^^^^^^

Short brief description.

Response
""""""""

This endpoint does not return anything.

Security
""""""""

mTLS
