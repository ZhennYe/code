resize_row <- function (tabl, rownum, scalar, st=3) {
  en = length(tabl[rownum,])
  for (i in st:en) {
    tabl[rownum,i] = as.numeric(tabl[rownum,i])/scalar
  }
  tabl
}


