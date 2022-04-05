# Deadlock and Optimizations

Explain your optimizations if applicable

All optimization codes are in utils.c, all memory freeing codes are in ai.c

My code for deadlock detection is in lines 234 - 300, also look for some changes in line 310, 
314, 318 & 322(Remove the deadlock_check from these lines to check unoptimized version). 
For deadlock optimization, I implemented a simple algorithm which checks whether any straight 
rows/columns have 2 corners but no goals and sets thoserows/columns to be deadlock. By doing so, 
these rows/columns are excluded from further consideration, which reduces the number of nodes 
to be considered when doing the AI Sokoban Algorithm and has pretty significant performance boosts 
for square maps. Some data for optimizedand unoptimized solutions has been shown below along with 
the map being used.

Map: 
###############
#@            #
# $          .#
#             #                      
###############
/-------------------------------/
Unoptimized Solution:                            
dRRRRRRRRRRR

STATS: 
Expanded nodes: 575
Generated nodes: 2296
Duplicated nodes: 1149
Solution Length: 12
Expanded/seconds: 563
Time (seconds): 1.021116
/----------------------------/
Optimized Solution:
dRRRRRRRRRRR

STATS: 
Expanded nodes: 273
Generated nodes: 1088
Duplicated nodes: 537
Solution Length: 12
Expanded/seconds: 245
Time (seconds): 1.114039
/--------------------------/

From the above results, we can see that the optimization leads to over 50% reduction in expanded, generated and 
duplicated nodes. Therefore, the optimization leads to reduced memory usage and since less number of nodes is
being compared, we would also see improved performance in time if larger datasets would have been used.
(Couldn't test on larger datasets due to memory issues in Ed)

