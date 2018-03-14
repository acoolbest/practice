static ssize_t xxx_write(struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
	//...    
	DECLARE_WAITQUEUE(wait, current);   /* ����ȴ�����Ԫ�� */    
	add_wait_queue( &xxx_wait , &wait); /* ���Ԫ�� wait ���ȴ����� xxx_wait */    
	/* �ȴ��豸��������д */    
	do{        
		avail = device_writable(...);
		if( avail < 0){
			if(file->f_flags & O_NONBLOCK)  //�Ƿ�����
			{                
				ret = -EAGAIN;
				goto out;
			}
			__set_current_state(TASK_INTERRUPTIBLE); /* �ı����״̬ */
			schedule();                         /* ������������ִ�� */
			if(signal_pending(current))         /* �������Ϊ�źŻ���*/
			{                
				ret = -ERESTARTSYS;
				goto out;
			}        
		}    
	} while( avail < 0);    /* д�豸������ */    
	device_write(...);
out:    
	remove_wait_queue(&xxx_wait, &wait);
	set_current_sate(TASK_RUNNING);
	return ret;
}
