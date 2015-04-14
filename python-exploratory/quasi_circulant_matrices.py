# quasi-circulant matrices
# quasi-circulant matrices have: 
#  1. diagonals are zero
#  2. columns sum to 1
#  3. rows are non-zero (not a neuron in the network since receives no input)
#  4. row sums are unique
#  5. colums are linearly independent (det != 0) (optional??)


import numpy as np
import itertools as it



def gen_matrix(n=5, seed=4):
  print('Seed is %i' %seed)
  # generate an n x n matrix with quasi-circulant properties
  # random numbers to start
  np.random.seed(seed)
  # to make sure none is zero
  cols = np.random.rand(n) 
  # each row in this col will have 1/(this number)
  cols = [int(i*(n-1))+1 for i in cols] 
  mat = np.zeros([n,n])
  # decide which rows get the input
  for i in range(n):
    ops = list(range(n))
    ops.pop(ops.index(i)) # remove diagonal as option
    combs = list(it.combinations(ops, cols[i]))
    choose = combs[int(np.random.rand(1)*len(combs))]
    # for each row
    for r in range(len(mat)):
      # input the chosen elements in each colum
      if r in choose:
        mat[r][i] = 1/cols[i]
  
  """ OLD shit
  # set diagonal to zero
  for i in range(len(mat)):
    mat[i][i] = 0
  colsums = np.zeros(n)
  # columns sum to 1
  for i in range(len(mat)):
    for j in range(len(mat[i])):
      colsums[j] = colsums[j] + mat[i][j]
  for i in range(len(mat)):
    for j in range(len(mat[i])):
      mat[i][j] = mat[i][j] / colsums[j]
  """
  return mat



def check_features(mat):
  # check that the required features of quasi-circulant matrices are fulfilled
  colsums = np.zeros(len(mat))
  for i in range(len(mat)):
    for j in range(len(mat[i])):
      colsums[j] = colsums[j] + mat[i][j]
  if np.prod(colsums) <= 0.99 or np.prod(colsums) >= 1.01:
    print('Columns do not sum to 1!')
    return False
  for i in range(len(mat)):
    if sum(mat[i]) == 0:
      print('Found a zero row!')
      return False
    elif mat[i][i] != 0:
      print('Found a non-zero diagonal element!')
      return False
  if np.linalg.det(mat) == 0:
    print('Columns not linearly independent!')
    return False
  return True



def check_matrix(mat, tol=0.0001):
  HH = check_features(mat)
  if HH is False:
    return
  # show that the max-sorted order of principle evec reflects sum of rows
  w, v = np.linalg.eig(mat)
  ind = None
  for i in range(len(w)):
    if abs(np.real(w[i])-1) < tol and abs(np.imag(w[i])) < tol:
      ind = i
  if ind is not None:
    ## sort by eigenvector ##
    rowsort_eig = []
    sorted_evecs = [h[ind] for h in v]
    g = [h[ind] for h in v]
    while len(g) > 0:
      mind = sorted_evecs.index(max(g))
      rowsort_eig.append(mind)
      g.pop(g.index(sorted_evecs[mind]))
    # check this against row sums
    ## sort by row sum ##
    rowsum = [sum(i) for i in mat]
    sorted_rows = [sum(i) for i in mat]
    rowsort_ = []
    while len(rowsum) > 0:
      mind = sorted_rows.index(max(rowsum))
      rowsort_.append(mind)
      rowsum.pop(rowsum.index(sorted_rows[mind]))
    print('Eigenvector predicts:')
    print(rowsort_eig)
    print('Simply by row: ')
    print(rowsort_)
    if list(rowsort_eig) != list(rowsort_):
      print('Answers do not agree.')
      print(mat)
    else:
      print('Answers agree! Suck it, Tim!')
  else:
    print('No matching eval found')
    print(w)
  return
  


def trial(n=6, seed=5):
  mat = gen_matrix(n,seed)
  sseed = seed
  while check_features(mat) is False:
    sseed = sseed+1
    mat = gen_matrix(n, sseed)
  check_matrix(mat)
  return mat
  
  




