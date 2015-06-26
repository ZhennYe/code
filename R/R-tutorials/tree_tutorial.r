tree1 <- function() {
  library(tree)
  calif = read.table('~/Dropbox/stats/data/cadata.dat', header=TRUE)
  require(tree)
  treefit = tree(log(MedianHouseValue) ~ Longitude+Latitude, data=calif)
  plot(treefit)
  text(treefit, cex=0.75)
  price.deciles = quantile(calif$MedianHouseValue, 0:10/10)
  cut.prices = cut(calif$MedianHouseValue, price.deciles, include.lowest=TRUE)
  plot(calif$Longitude, calif$Latitude, col=grey(10:2/11)[cut.prices],pch=20, xlab="Longitude", ylab="Latitude")
  partition.tree(treefit, ordvars=c("Longitude", "Latitude"), add=TRUE)
  summary(treefit)
}


# This one has a ton of leaves because minimum change in deviance (mindev)
# is turned way down (10x)
treefit2 <- tree(log(MedianHouseValue) ~ Longitude+Latitude,
                 mindev=0.001, data=calif)
# Then prune with cross-validation of deviance (RMS error) (cv$dev)
treefit2.cv <- cv.tree(treefit2)
# Optimal tree
opt.trees = which(treefit2.cv$dev == min(treefit2.cv$dev)) 
best.leaves = min(treefit2.cv$size[opt.trees])
treefit2.pruned = prune.tree(treefit2,best=best.leaves)
plot(treefit2.pruned)


# treefit3 <- tree(log(MedianHouseValue) ~ ., data=calif)
# plot(treefit3)
# text(treefit3, cex=0.75)
# summary(treefit3)


# Here is more pruning code
my.tree = tree(y ~ x1 + x2, data=my.data) # Fits tree
prune.tree(my.tree,best=5) # Returns best pruned tree with 5 leaves, evaluating
# error on training data
prune.tree(my.tree,best=5,newdata=test.set) # Ditto, but evaluates on test.set
my.tree.seq = prune.tree(my.tree) # Sequence of pruned tree sizes/errors
plot(my.tree.seq) # Plots size vs. error
my.tree.seq$dev # Vector of error rates for prunings, in order
opt.trees = which(my.tree.seq$dev == min(my.tree.seq$dev)) # Positions of
# optimal (with respect to error) trees
min(my.tree.seq$size[opt.trees]) # Size of smallest optimal tree
