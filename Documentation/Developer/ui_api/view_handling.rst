View Handling
=============

Class Diagram
*************

.. image:: images/view_handling_class.png
    :scale: 50%

Sequence Diagram
****************

.. image:: images/view_handling_sequence.png
    :scale: 50%

Creating new widget view
^^^^^^^^^^^^^^^^^^^^^^^^

A new widget view can be created by simply inheriting from WidgetView and overriding the ``getViewWidget`` method.
A widget view should call ``addWidgetToDock`` when creating the root widget.

