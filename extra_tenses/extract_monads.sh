#!/bin/bash

# Copyright © 2017 Ezer IT Consulting.
# Released under an MIT License.

grep '^.*{ [0-9]* } true.*' $1 | sed -e 's/^.*{ \([0-9]*\) } true.*$/\1/'
