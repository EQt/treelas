#/bin/bash

cd python
$PYTHON setup.py install
$STRIP $SP_DIR/treelas/_treelas.*
