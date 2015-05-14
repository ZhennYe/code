def primes(int kmax):
  cdef int n,k, i
  cdef int p[1000]
  results = []
  if kmax > 1000:
    kmax = 1000
  k = 0
  n = 2
  while k < kmax:
    i = 0
    while i < k and n % p[i] != 0:
      i = i + 1
    if i == k:
      p[k] = n
      k = k + 1
      results.append(n)
    n = n + 1
  return results
