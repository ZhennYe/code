def breaknum(num):
  nums = []
  go = True
  while go == True:
    r = num % 10
    if r != 0:
      nums.append(r)
    else:
      nums.append(0)
    num = int(num / 10)
    if num == 1:
      nums.append(1)
      go = False
    elif num == 0:
      go = False
  return nums

def checkpalin(l):
  palins = []
  length = len(l)
  if length%2 == 0:
    # even
    half = length/2
    for i in xrange(half):
      if l[i] == l[length-1-i]:
        palins.append(1)
      else:
        palins.append(0)
  else:
    # odd
    half = int(length/2)
    for i in xrange(half):
      if l[i] == l[length-1-i]:
        palins.append(1)
      else:
        palins.append(0)
  product = 1
  for p in palins:
    product = product * p
  return product

def largestpalin(digits):
  num1, num2 = 10**digits-1, 10**digits-1
  prod = checkpalin(breaknum(num1*num2))
  palin = 0
  while int(num1/10**(digits-1)) >= 1:
    num1 = num1 - 1
    num2 = 10**digits-1
    if checkpalin(breaknum(num1*num2)) == 1 and num1*num2 > palin:
      palin = num1*num2
    while int(num2/10**(digits-1)) >= 1:
      num2 = num2 - 1
      if checkpalin(breaknum(num1*num2)) == 1 and num1*num2 > palin:
        palin = num1*num2
  return palin

  
