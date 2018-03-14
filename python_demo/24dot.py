
import itertools

num=['2','3','4','5']
opr=['-','+','/','*']
numlist=list(itertools.permutations(num,4))
oprlist=list(itertools.permutations(opr,3))
print (numlist)
for i in numlist:
	for j in oprlist:
		j=j+('',)
		expresStr=''.join([a+b for a,b in zip(i,j)])
		value=eval(expresStr)
		print(expresStr,'=',value)
		#if value==24:
		#	print(expresStr,'=',value)
			