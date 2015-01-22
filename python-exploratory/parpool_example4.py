# example 4 for threading with python using pool and map
# from: https://medium.com/@thechriskiehl/parallelism-in-one-line-40e9b2b36148

import urllib.request
from multiprocessing.dummy import Pool as ThreadPool

urls = [
  'http://www.python.org',
  'http://www.yahoo.com',
  'http://www.google.com',
  'http://www.python.org/about',
  'http://www.python.org/doc',
  'http://www.python.org/download',
  'http://www.apple.com' ]

# make pool of workers
pool = ThreadPool(4)

# open urls and return their results
results = pool.map(urllib.request.urlopen, urls)
# close pool and wait for work to finish
pool.close()
pool.join()
