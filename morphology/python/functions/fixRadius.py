# correct illegal radius

from sys import argv
script, old_filename, new_filename = argv

print 'Opening file %r.' % new_filename

old_file = open(old_filename, 'r')
new_file = open(new_filename, 'w')

def pt3dadd(line):
    cols = line.split(',')
    string_rad = cols[3].split(')')[0] 
    rad = float(string_rad)
    if rad < 0.09:
        rad = 0.09
        newrad = '%f)' %rad
        cols[3] = newrad
        out = ','.join(cols)
        new_filename.write(out)
        new_filename.write('\n')
    else:
        pass

# control of file
for line in old_file:
    cols = line.split('(')
    if cols[0] == '  pt3dadd':
        line.pt3dadd()
    elif cols[0] == 'pt3dadd':
		line.pt3dadd()
    else:
        new_file.write(line)
        new_file.write('\n')

