
import itertools

num=['2','3','4','5']
opr=['-','+','/']

numlist=list(itertools.permutations(num,4))
#print(numlist)
#for i in numlist:
#	print (i)
oprlist=list(itertools.permutations(opr,3))
#print(oprlist)
'''
print("&&&&&&&&&&&&&&&")
print(*zip(num,opr))

seq1 = ['hello','good','boy','doiido']
print (' '.join(seq1))
print (':'.join(seq1))
print (''.join(seq1))
'''
opr=opr+['']
print(opr)



print(''.join([a+b for a,b in zip(num,opr)]))
aaa = ''.join([a+b for a,b in zip(num,opr)])
print (aaa)
print (eval(aaa))
print("&&&&&&&&&&&&&&&")
for i in numlist:
    for j in oprlist:
        print (i)
        j=j+('',)
        expresStr=''.join([a+b for a,b in zip(i,j)])
        value=eval(expresStr)
        if value==24:
            print(expresStr,'=',value)

