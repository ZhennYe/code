# fix -1 in connection matrix
# usage: python fixNegOne.py fileToFix newfileName

import os, sys



def readfile(brokenFileName):
  
  with open(newFileName, 'w') as fOut:
  
    with open(brokenFileName, 'r') as fIn:
      
      for line in fIn:
        splitLine = line.split('(')
        
        if splitLine[0] == 'pt3dadd' and float(comsplit[3].split(')')[0]) <= 0:
          comsplit = splitLine[1].split(',')
          rad = float(comsplit[3].split(')')[0])
          comsplit[3]=str(0.1)
          newline = ','.join(comsplit)
          paren = str(')')
          newline.append(paren)
          newline = ''.join(newline)
          pt3d = 'pt3dadd('
          newline = pt3d.append(newline)
          fOut.write(newline)
          fOut.write('\n')
        
        if splitLine[0] == 'connect': and splitLine[2].split('_')[1].split('(')[0] == -1:
          prevsec = int(splitLine[1].split('_')[1].split('(')[0])
          cursec = int(splitLine[2].split('_')[1].split('(')[0])
          if cursec == -1:
            cursec = prevsec + 1
          connect_string = 'connect section_%i(1), section_%i(0)' \
                           % (prevsec, cursec)
          fOut.write(connect_string)
          fOut.write('\n')
          
        else:
          fOut.write(line)
          fOut.write('\n')
        
    


############################################################
if __name__ == '__main__':
  arguments = sys.argv
  brokenFileName = arguments[1]
  newFileName = arguments[2]
