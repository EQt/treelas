#/bin/bash

cd python
$PYTHON setup.py install --single-version-externally-managed --record=record.txt
$STRIP $(grep _treelas record.txt)
