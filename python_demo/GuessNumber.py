import random
print('###猜数字小游戏###')
print('#1.开始游戏 2.任意键退出游戏#')
print('#猜错3次自动退出游戏#')
print('#######################################')

put = input('请输入:')
if put == '1':
	number = 6
	secret = random.randint(1,100)
	print('secret = {}'.format(secret))
	while number > 0:
		temp = input("猜一下我心中的理想数字是几(1-100):")
		print('temp = {}XXX'.format(temp))
		temp2 = temp.strip('abc')
		print('temp2 = {}XXX'.format(temp2))
		if temp2.isdigit():
			temp1 = int(temp2)
			print('temp1 = {}XXX'.format(temp1))
			if temp1 == secret:
				exit('数字是{}，你猜对了'.format(secret))
			elif number == 1:
				exit('答案是{}'.format(secret))
			elif temp1 > secret:
				print('数字比{}小，还剩{}次机会'.format(temp1,number-1))
			else:
				print('数字比{}大，还剩{}次机会'.format(temp1, number - 1))
		else:
			print('Error:请输入整数')
			number += 1
		number -= 1
else:
	exit('退出游戏成功')