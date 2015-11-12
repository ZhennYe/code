resize_row <- function (tabl, rownum, scalar, st=3) {
  en = length(tabl[rownum,])
  for (i in st:en) {
    tabl[rownum,i] = as.numeric(tabl[rownum,i])/scalar
  }
  tabl
}


min_tree <- function (corr) {
  require(stats)
  d <- dist(corr)
  PC <- prcomp(corr)
  M <- mst(d)
  opar <- par()
  par(mfcol = c(2, 2))
  plot(M)
  plot(M, graph = "nsca")
  plot(M, x1 = PC$x[, 1], x2 = PC$x[, 2])
  par(opar)
}



# 1. for m hypothesis (m pairwise comparisons) tests there are 
#    m p values Pi = P1, P2...Pm
# 2. order the P values with 0 < p1 < p2 ... < pm; there are also 
#    corresponding Hi values which are 1 if H < alpha (reject null) 
#    and 0 otherwise
#    * aside: T is a map of [0,1]^m -> [0,1] where we reject each hypothesis (i) with pi <= T(P^m)
# 3. False discovery proportion = # false discoveries / # (true? total?) 
#    discoveries, and is a function of t:
#    FDP(t) = (sum(1){Pi <= t}*(1-Hi)) / (sum(1){Pi <= t} + 1*{all Pi > t})
# 4. FRD = Exp Value (FDP(T))
# 5. Benjamini-Hochberg procedure:
#    Tbh = max{Pi : Pi < alpha*(i/m);
#    i.e.: the largest (i-th, ordered) P value subject to the 
#    constraint that it's less than or equal to alpha*i/m

# Generate random matrix
randmat <- function (cols=50, len=1000) {
  g = replicate(cols, rnorm(len))
  g
}

pairwise_t <- function (mat) {
  combs = combn(seq(1,dim(mat)[2]), 2)
  t = c(t.test(mat[1],mat[2])$p.value)
  for (i in seq(2,length(combs)/2)) {
    t = append(t, t.test(mat[combs[1,i]], mat[combs[2,i]])$p.value)
  }
  t
}









