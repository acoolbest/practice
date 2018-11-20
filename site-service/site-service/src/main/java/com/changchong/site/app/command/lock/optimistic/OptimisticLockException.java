package com.changchong.site.app.command.lock.optimistic;

public class OptimisticLockException extends RuntimeException {


    public OptimisticLockException() {
        super();
    }

    public OptimisticLockException(String message) {
        super(message);
    }
}
