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

######################################## PIES ######################
library(ggplot2)
# ggpie: draws a pie chart.
# give it:
# * `dat`: your dataframe
# * `by` {character}: the name of the fill column (factor) (string)
# * `totals` {character}: the name of the column that tracks (number)
#    the time spent per level of `by` (percentages work too).
# returns: a plot object.
ggpie <- function (dat, by, totals, legname, legitems) {
    ggplot(dat, aes_string(x=factor(1), y=totals, fill=by)) +
        geom_bar(stat='identity', color='black') +
        guides(fill=guide_legend(override.aes=list(colour=NA))) + # removes black borders from legend
        scale_fill_discrete(name=legname, labels=legitems) + 
        coord_polar(theta='y') +
        theme(axis.ticks=element_blank(),
            axis.text.y=element_blank(),
            axis.text.x=element_text(colour='black'),
            axis.title=element_blank()) +
    scale_y_continuous(breaks=cumsum(dat[[totals]]) - dat[[totals]] / 2, labels=dat[[by]])
}

ggpie_orig <- function (dat, by, totals) {
    ggplot(dat, aes_string(x=factor(1), y=totals, fill=by)) +
        geom_bar(stat='identity', color='black') +
        guides(fill=guide_legend(override.aes=list(colour=NA))) + # removes black borders from legend
        coord_polar(theta='y') +
        theme(axis.ticks=element_blank(),
            axis.text.y=element_blank(),
            axis.text.x=element_text(colour='black'),
            axis.title=element_blank()) +
    scale_y_continuous(breaks=cumsum(dat[[totals]]) - dat[[totals]] / 2, labels=dat[[by]])    
}


# attach(df)
# ggpie(df, 'by', 'totals')


# legits = c('BC: 17914','Brandeis: 7322','BU: 43395','Harvard: 25671','Industry: 60459','MIT: 12349','Northeastern: 21570','Tufts: 12024')


# Use molten?
# Pie plots (individual)
ggplot(data=df[df$gender=='Male',], 
         aes(x=factor(1),
         y=Summary,
         fill = factor(response))) + 
 geom_bar(width = 1) + 
 coord_polar(theta="y") +
 xlab('Males') +
 ylab('') +
 labs(fill='Response')

ggplot(data=df[df$gender=='Female',], 
         aes(x=factor(1),
         y=Summary,
         fill = factor(response))) + 
 geom_bar(width = 1) + 
 coord_polar(theta="y") +
 xlab('Females') +
 ylab('') +
 labs(fill='Response')
 

# stacked bars
# data is already melted and is of format:
# date, institution, num_talks
# 11/9/15, mit_priv, 5
# 11/10/15, mit_priv, 4 
#       ...
priv_melt = melt(priv)
ggplot(priv_melt, aes(date, num_talks))
  + geom_bar(aes(colour=institution, fill=institution), 
             position='stack', stat="identity")
  + theme(axis.text.x = element_text(angle=45), hjust=1) # 45 degrees rotation
  + scale_x_discrete(breaks=NULL)
  + scale_y_discrete(breaks=NULL)
  + guide_legend(title='Institutions')
  + scale_fill_discrete(guide = guide_legend(title='Institution'))
  
ggsave(filename='', width=, height=) # Reverts to most recent plot












