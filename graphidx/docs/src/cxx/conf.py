#!/usr/bin/env python
import os
import subprocess
import sphinx_rtd_theme

project = 'GraphIdx'
html_theme = "sphinx_rtd_theme"
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
html_logo = "../julia/assets/logo.png"
extensions = ['breathe']
breathe_projects = { 'graphidx': '../../xml' }
source_suffix = {
    '.rst': 'restructuredtext',
    # '.md': 'markdown',
}
master_doc = 'index'
copyright = '2019, Elias Kuthe'
author = 'Elias Kuthe'
# html_logo = 'path...'
exclude_patterns = []
highlight_language = 'c++'
pygments_style = 'sphinx'
