#!/bin/bash

gcc "$1" -o "$2" -fsyntax-only 2>/dev/null

if [ $? -eq 0 ]; then 
  echo "success"
  gcc "$1" -o "$2"
  ./"$2"
else
  echo "error"
fi
