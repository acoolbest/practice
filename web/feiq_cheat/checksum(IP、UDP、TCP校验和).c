unsigned short checksum(unsigned short *buf, int nword)
{
	unsigned long sum;
	for(sum = 0; nword > 0; nword--)
	{
		sum += htons(*buf);
		buf++;
	}
	sum = (sum>>16) + (sum&0xffff);
	sum += (sum>>16);
	return ~sum;
}
//return 为小端，在发送数据报的数组需要转换为大端
/*
说明：
1.参数
	第一个参数为要进行校验数据的起始地址
	第二个参数为进行校验数据的个数！！！一定要注意这个地方要 除以2，比如要进行校验的数据的长度为42,那么这个参数应该为42/2
*/