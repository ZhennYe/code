#!/bin/bash

# Unzip all files in a directory and sub-directory
# Put the contents right where they belong, renamed to the folder name

# Usage: ./batch_unzip directory 
#   inplace -- defaults to true, no other options at the moment

# Save results of search in a temp file in directory 
TARGET=$1
OUT=$TARGET
OUT+="temp.txt"
find $1 -name "*.zip*" > $OUT

# Load each line of the file and make a temp file name
while IFS='' read -r line || [[ -n "$line" ]]; do
  FNAM=$line
  # IFS='/' read -ra ITEM <<< $FNAM
  arrFNAM=(${FNAM//// }) # Split with '/'
  preFIL=${arrFNAM[-1]} # Just the file name, no path
  arrFIL=(${preFIL//./ }) # Split with '.'
  FIL=$TARGET
  FIL+=${arrFIL[0]}
  FIL+=".xml"
  unzip $line -d $TARGET
  
  # Rename the annotation file -- assumes the zipped file is "annotation.xml" !!
  ANN=$TARGET
  ANN+="annotation.xml"
  mv $ANN $FIL
  echo "$preFIL unzipped as $ANN, renamed to $FIL"
done < $OUT

# Clean up the temporary file
rm $OUT
