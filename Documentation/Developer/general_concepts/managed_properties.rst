Managed Properties
##################

OpenTwin provides a convenience layer to ease the property handling.
When developing a library, application or service that does not require frontend functionallity the ``OTGui`` class `Managed Property Object <../_static/codedochtml/classot_1_1_managed_property_object.xhtml>`_ provides a simple interface to add properties that will automatically be concidered for serialization.

Frontend components should use the ``OTWidgets`` class `Managed Property Widget Object <../_static/codedochtml/classot_1_1_managed_property_widget_object.xhtml>`_.
This enables :ref:`property linking <property_linking>`.

Property Management structure
*****************************

.. image:: images/managed_properties_class.svg

.. _property_linking:

Property Linking
****************
