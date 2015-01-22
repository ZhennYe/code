
import math



def angles(A,B,C):
  """
  here, A = midpoint of seg1, B = connection, C = midpoint of seg2
  angle of interest is therefore B
  """
  def sidelength(X,Y):
    lenZ = math.sqrt( (X[0] - Y[0])**2 + (X[1] - Y[1])**2 + (X[2] - Y[2])**2)
    print lenZ
    return lenZ
  
  a, b, c = sidelength(B,C), sidelength(A,C), sidelength(A,B)
  cosB = ((a**2 - b**2 + c**2) / (2*a*c))
  cosA = ((-a**2 + b**2 + c**2) / (2*c*b))
  cosC = ((a**2 + b**2 - c**2) / (2*b*a))
  
  print cosB
  angB = math.acos(cosB)
  return angB, angB*(180/math.pi)



#####################
s1mid, s1end = [-1,0,0], [0,0,0]
s2start, s2mid = [0,0,0], [2,1,0]
bifang, bifdeg = angles(s1mid, s2start, s2mid)

print bifang, bifdeg
