add2 <- function(x,y) {
        x + y
}

above <- function(x,n=10) {
         use <- x > n
         x[use]
}

columnmean <- function(y) {
              nc <- ncol(y)
              means <- numeric(nc) # empty vector
              for (i in 1:nc) {
                means[i] <- mean(y[, i])
              }
              means
}

newfunc <- function(x=10) {
           print(x)
}

