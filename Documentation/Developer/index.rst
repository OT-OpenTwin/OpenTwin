.. OpenTwin documentation master file, created by
   sphinx-quickstart on Fri Jan 27 16:56:26 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.


Welcome
=======

OpenTwin is a microservices based open source platform for supporting multiphysical multiscale simulations. 

The data storage adopts a MongoDB data base driven approach where data lifecycle management is fundamentally supported by the data model. 
Communication between the services is based on HTTP with (m)TLS security. The software relies on a large number of powerful third party libraries.


Get started
===========

These sections cover the basics of getting started with OpenTwin, including setting up the development environment, building and debugging the sofware, as well as the introduction of general concepts and how to's.

.. toctree::
   :maxdepth: 1

   get_started/setup_development_environment
   get_started/building_the_software
   get_started/managing_certificates
   get_started/architecture_overview
   get_started/package_overview

.. toctree::
   :maxdepth: 2

   general_concepts/general_concepts

Howto
=====

.. toctree::
   :maxdepth: 1

   how_to/debug_the_software
   how_to/create_new_services
   how_to/load_create_entities
   how_to/build_user_interface
   how_to/create_documentation

.. toctree::
   :maxdepth: 2
   
   block_editor/block_editor

Learn the core
==============

.. toctree::
   :maxdepth: 2

   services/services_list

.. toctree::
   :maxdepth: 2

   workflow/workflow

.. toctree::
   :maxdepth: 2

   otoolkit/otoolkit


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`



