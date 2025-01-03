Circuit Simulator
=================
This documentation outlines the test cases for the circuit simulator of our project. 
To ensure that all components of the simulator function correctly, it is important to place elements and connections in different and arbitrary orders. 
This helps identify potential errors or unexpected behavior that may not occur with standardized workflows. 
The test cases therefore cover various configuration possibilities to ensure a comprehensive validation of the system.
Also it is important to test all test cases with and without the GND Element such that always the correct results are obtained.

DC Simulation 
-------------

Voltage Divider Circuit
^^^^^^^^^^^^^^^^^^^^^^^


.. image:: images/circuit_voltage_divider.png
    :width: 400px
    

.. list-table::
   :header-rows: 1

   * - Component
     - Value
     - Description
   * - Voltage Source
     - 100 V
     - DC Voltage Source
   * - Resistor R1
     - 200 Ohm
     - -
   * - Resistor R2
     - 200 Ohm
     - -
   * - GND
     - -
     - Ground
   * - Voltage Meter
     - -
     - Voltage Meter

Solver Properties
+++++++++++++++++
.. list-table::
   :header-rows: 1

   * - Simulation Type
     - Element
     - From
     - To
     - Step
   * - DC
     - Voltage Source
     - 0
     - 100
     - 10

.. figure:: images/circuit_result_voltage_divider.png
   :width: 400px
   :alt: Alternate text

   The graph shows that when two equal resistors are used in a voltage divider, 
   the resulting voltage is half of the swept voltage, 
   resulting in a linear relationship where Voltage = 0.5 * swept voltage.

    

Current Divider Circuit
^^^^^^^^^^^^^^^^^^^^^^^


.. figure:: images/circuit_current_divider.png
    :width: 400px


.. list-table::
   :header-rows: 1

   * - Component
     - Value
     - Description
   * - Voltage Source
     - 100 V
     - DC Voltage Source
   * - Resistor R1
     - 200 Ohm
     - -
   * - Resistor R2
     - 200 Ohm
     - -
   * - GND
     - -
     - Ground
   * - Voltage Meter
     - -
     - Voltage Meter


Solver Properties
+++++++++++++++++
.. list-table::
   :header-rows: 1

   * - Simulation Type
     - Element
     - From
     - To
     - Step
   * - DC
     - Voltage Source
     - 0
     - 100
     - 10

.. figure:: images/circuit_result_current_divider.png
    :width: 400px 
    :alt: Alternate text

    The current divider circuit consists of two or more resistors connected in parallel, 
    allowing the input current to be divided among the branches according to their resistance values. 
    The graph shows that the current through each resistor is inversely proportional to its resistance value.


AC Simulation
-------------


Series RLC Circuit
^^^^^^^^^^^^^^^^^^	


.. image:: images/circuit_series_RLC.png
    :width: 400px



.. list-table::
   :header-rows: 1

   * - Component
     - Value
     - Description
   * - Voltage Source
     - 1V Amplitude
     - AC Voltage Source
   * - Resistor R1
     - 10 Ohm
     - -
   * - Inductor
     - 100 mH
     - -
   * - Capacitor
     - 10 uF
     - - 
   * - GND
     - -
     - Ground
   * - Voltage Meter
     - -
     - Voltage Meter



Solver Properties
+++++++++++++++++
.. list-table::
   :header-rows: 1

   * - Simulation Type
     - Variation
     - Number of Points
     - Starting Frequency
     - Final Frequency
   * - AC
     - linear
     - 100
     - 100
     - 500


.. figure:: images/circuit_result_series_RLC.png
    :width: 400px 
    :alt: Alternate text

    The graph shows the Voltage Difference vs Frequency plot of a series RLC circuit at the Capacitor.


Parallel RLC Circuit
^^^^^^^^^^^^^^^^^^^^	


.. image:: images/circuit_parallel_RLC.png
    :width: 400px


.. list-table::
    :header-rows: 1
  
    * - Component
      - Value
      - Description
    * - Voltage Source
      - 1V Amplitude
      - AC Voltage Source
    * - Resistor R1
      - 10 Ohm
      - -
    * - Inductor
      - 100 mH
      - -
    * - Capacitor
      - 10 uF
      - - 
    * - GND
      - -
      - Ground
    * - Voltage Meter
      - -
      - Voltage Meter


Solver Properties
+++++++++++++++++
.. list-table::
    :header-rows: 1

    * - Simulation Type
      - Variation
      - Number of Points
      - Starting Frequency
      - Final Frequency
    * - AC
      - linear
      - 100
      - 100
      - 500

.. figure:: images/circuit_result_parallel_RLC.png
    :width: 400px 
    :alt: Alternate text

    The Graph shows two curves. One for the voltage Difference at the Resistor and one for the voltage Difference at the Capacitor.

Transient Simulation
--------------------


Rectifier Circuit
^^^^^^^^^^^^^^^^^


.. image:: images/circuit_rectifier.png
    :width: 400px


.. list-table::
    :header-rows: 1

    * - Component
      - Value
      - Description
    * - Voltage Source
      - Pulse(0 10 0 1 1 5 10)
      - Transient Voltage Source
    * - Resistor R1
      - 1000 Ohm
      - -
    * - Diode
      - D1N4148 D(IS=2.52E-9,RS=0.01,N=1.5,Vfwd=0.7)
      - -
    * - Capacitor
      - 100 u
      - -
    * - GND
      - -
      - Ground
    * - Voltage Meter
      - -
      - Voltage Meter


Solver Properties
+++++++++++++++++
.. list-table::
    :header-rows: 1

    * - Simulation Type
      - Duration
      - Time Steps
    * - Tran
      - 40
      - 1
    

.. figure:: images/circuit_result_rectifier.png
    :width: 400px 
    :alt: Alternate text

The graph shows the output voltage of a rectifier circuit,
which converts an AC input signal into a DC output signal. 
The rectifier circuit consists of a diode, a resistor, and a capacitor. 
The diode allows current to flow in only one direction, 
resulting in a pulsating DC output signal that is smoothed by the capacitor, 
providing a more stable voltage level.