Display Messages API
####################

The User Interface API provides different possibilities to display messages to the user.

Output Window
*************

The most common approach is to display messages in the default output window of the frontend.
To display a message use the ``displayMessage()`` method of the ``OTServiceFoundation/UIComponent`` if the Frontend is connected.

.. code-block:: c++
    :linenos:

    void MyApplication::uiConnected(components::UiComponent * _ui) {
        _ui->displayMessage("Hello World!");
    }

:ref:`Styled text messages <styled_text_builder>` may also be displayed in the output window.
To do so simpy send the `StyledTextBuilder` to the Frontend.
Read the :ref:`Styled Text Builder <styled_text_builder>` documentation for code examples.

.. code-block:: c++
    :linenos:

    #include "OTGui/StyledTextBuilder.h"
    #include "OTServiceFoundation/UiComponent.h"

    void MyApplication::uiConnected(components::UiComponent * _ui) {
        ot::StyledTextBuilder myMessage;
        // Construct the message
        // ...

        // Send the request
        _ui->displayStyledMessage(myMessage);
    }

.. _styled_text_builder:

Styled Text Builder
*******************

The ``StyledTextBuilder`` can be used to create a text output with different character format and color settings for one string.
The following example will give more clarity.

Example Code
============

.. code-block:: c++
    :linenos:

    #include "OTGui/StyledTextBuilder.h"

    void foo(void) {
        ot::StyledTextBuilder myMessage;
        myMessage << "This message contains different styles:\n" <<
            ot::StyledText::Bold << "- Bold Text\n" <<
            ot::StyledText::Error << "- Bold Error Colored Text\n" << ot::StyledText::ClearStyle <<
            ot::StyledText::Italic << "- Italic Text\n" <<
            ot::StyledText::Bold << "- Bold and Italic Text\n" << ot::StyledText::ClearStyle <<
            "and other combinations of styled color and character format.";
    }

Output of the code example:
===========================

.. raw:: html
    
    <div class="highlight"><pre><p><span class="n">This message contains different styles:</span>
    <span class="n"><span style="font-weight:bold">- Bold Text</span></span>
    <span class="n"><span style="color:red;font-weight:bold">- Bold Error Colored Text</span></span>
    <span class="n"><span style="font-style:italic">- Italic Text</span></span>
    <span class="n"><span style="font-weight:bold;font-style:italic">- Bold and Italic Text</span></span>
    <span class="n">and other combinations of styled color and character format.</span></p></pre></div>


The color references used in the StyledText are used to reference a ``ColorStyleValue`` in the currently set ``ColorStyle`` of the frontend.
