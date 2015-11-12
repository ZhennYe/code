# csv_tools.py - support for python-R interactions via dict-csv 


#########################################################################
def mean_properties(props, labels, csvfile=None, method='mean'):
  """
  Properties is a dictionary. Labels is a key or list of keys that 
  contain lists that should not be analyzed (file names, cell types, etc).
  Method: how list of lists should be represented.
  """
  if method == 'means' or method == 'mean':
    method = 'mean'
  lengths = [len(props[k]) for k in props.keys()]
  if len(list(set(lengths))) > 1:
    print('Not all keys are same length!'); return None
  if type(labels) is not list:
    labels = [labels]
  table = [[] for i in props[labels[0]]]
  table.append([]) # Add one more for the row column labels
  table[0] = [str(l) for l in labels]
  for k in props.keys():
    if str(k) not in labels:
      table[0].append(str(k))
  # Create each row of the csv table
  for item in range(len(props[labels[0]])):
    # Labels first
    for l in labels:
      table[item+1].append(props[l][item])
    # Now do the rest of the items
    for k in props.keys():
      if k not in labels:
        try:
          table[item+1].append(np.mean(props[k][item]))
        except:
          print('Mean failed for %s' %k)
  if csvfile is not None:
    with open(csvfile, 'w') as fOut:
      for row in table:
        fOut.write(','.join([str(c) for c in row]))
        fOut.write('\n')
  return
  
