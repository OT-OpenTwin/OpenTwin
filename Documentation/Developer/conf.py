# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'OpenTwin'
copyright = '2024, OpenTwin'
author = 'OpenTwin'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [ 'sphinx.ext.imgmath' ]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

#html_theme = 'agogo'
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_style = 'css/my_theme.css'
html_logo = 'images/logo_small.png'

# -- Prolog for every file   -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-rst_prolog
# Here we can add useful substitutions

rst_prolog = """
.. # define a hard line break for HTML. br = break
.. |br| raw:: html

   <br />
"""
