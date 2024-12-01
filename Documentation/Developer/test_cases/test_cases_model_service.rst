Model Service
==============
This documentation outlines the test cases for the Model service of our project.


Import of text files
--------------------

The model service should be able to import files and show them in the correct folder with the correct
highlighting.

Setup
^^^^^
First we need a setup. We have a test.txt file on the desktop and will import it.
The test.txt will have the summary of the last build.
The setup looks like this:

.. figure:: images/import_files_setup.png
   :width: 400px
   :alt: Alternate text

   As you can see this is the default setup. Means you go on the tab Model and on the right side you
   see the button import text file.

Expectation
^^^^^^^^^^^	

The test.txt file should be imported and shown in the correct folder with the correct highlighting.
It should look like this:

.. figure:: images/import_text_file_expectation.png
   :width: 400px
   :alt: Alternate text
   
   As you can see the file should appear in the folder "Text Files" and .the property syntax highlighting
   should be set to plain. Also you should be able to see the content of the file.

Execution
^^^^^^^^^
To import the text file you need to click on the button "Import Text File" and select the file.
After this it appears in the Project Navigation under "Text Files". You can click on it to show it
and see the content. Also you can edit its properties.




