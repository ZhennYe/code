#!/bin/bash


# Usage: ./batch_makeHoc folder xvox yvox zvox
#       if args 2-4 are blank, no scaling is done


# Run all the xml/nml files in a particular folder through the following
# process:
#    1. convert to hoc (XmlToHoc_simple.py)
#    2. remove loops (pyramidal_traceback.py)
#    3. re-scale coordinates (knossos_scaleCoords.py)
#       --> This requires inputs 2,3,4 (all or none, 1 for no change)

# Parse inputs
if [ "$#" -ne 4 ]; then
  X=1
  Y=1
  Z=1
else
  X=$2
  Y=$3
  Z=$4
fi

# Create the list of files (assumes .xml, can change this to .nml, etc)
FLIST=$1
FLIST+="temp.txt"
DIR=$1
DIR+="*.xml"
ls $DIR > $FLIST

# Run each file
while IFS='' read -r line || [[ -n "$line" ]]; do
  # Prepare the file name
  FNAM=$line
  # IFS='/' read -ra ITEM <<< $FNAM
  arrFNAM=(${FNAM//// }) # Split with '/'
  preFIL=${arrFNAM[-1]} # Just the file name, no path
  arrFIL=(${preFIL//./ }) # Split with '.'
  FIL=$1
  FIL+=${arrFIL[0]}
  FIL+=".hoc"
  
  # XmlToHoc_simple.py: python XmlToHoc_simple.py skelfile hocfile
  python XmlToHoc_simple.py $FNAM $FIL
  
  # pyramidal_traceback.py: python pyramidal_traceback.py hocfile
  python pyramidal_traceback.py $FIL
  TEMPHOC=$1
  TEMPHOC+="temphoc.hoc"
  rm $TEMPHOC # Remove the temporary hoc file
  
  # knossos_scaleCoords.py: python knossos_scaleCoords.py x y z
  python knossos_scaleCoords.py $FIL $X $Y $Z
  rm $FIL # New file is FIL+_scaled.hoc
  
  echo "Finished with $FIL"
done < $FLIST

# Clean up the temporary file
rm $FLIST










