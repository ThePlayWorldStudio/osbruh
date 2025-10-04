#!/bin/bash

ls -a $2 | grep -i "\."$3 > $1
