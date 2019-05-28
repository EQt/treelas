#!/usr/bin/env python
import os
import subprocess
import sphinx_rtd_theme

project = 'TreeLas'
html_theme = "sphinx_rtd_theme"
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
extensions = ['breathe']
breathe_projects = { 'treelas': '../../xml' }
source_suffix = {
    '.rst': 'restructuredtext',
}
master_doc = 'index'
copyright = '2019, Elias Kuthe'
author = 'Elias Kuthe'
exclude_patterns = []
highlight_language = 'c++'
pygments_style = 'sphinx'
