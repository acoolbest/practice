package com.changchong.site.app.command.lock.optimistic;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.changchong.site.app.command.Command;

public abstract class OptimisticLockCommand<T> implements Command<T> {

    private int repeat = 20;

    private ConcurrentHashMap<String, Lock> lockMap = new ConcurrentHashMap<String, Lock>();

    public OptimisticLockCommand(int... repeat) {
        this.repeat = repeat != null && repeat.length > 0 ? repeat[0] : 2;
    }

    @Override
    public T exec() {
        return exec(repeat);
    }

    @Override
    public T exec(int repeat) {
        Lock lock = null;
        for (int i = 0; i < repeat; i++) {
            try {
                T t = lock();
                if (t == null) {
                    return t;
                }
                process(t);
                Lock newLock = new ReentrantLock();
                lock = lockMap.putIfAbsent(t.getClass().getName(), newLock);
                if (lock == null) {
                	//removeCache(t);
                    lock = newLock;
                }
                if (lock.tryLock(5, TimeUnit.MILLISECONDS)) {
                    if (unlock(t) == 0) {
                    	removeCache(t);
                        throw new OptimisticLockException();
                    }
                    return t;
                }
            } catch (OptimisticLockException e) {
            } catch (InterruptedException e) {
            } finally {
                if (lock != null) {
                    lock.unlock();
                }
            }
        }
        throw new OptimisticLockException("failed lock");
    }

    protected abstract int unlock(T t);

    protected abstract void process(T t);

    protected abstract T lock();
    
    protected abstract void removeCache(T t);
}
