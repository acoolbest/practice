package com.changchong.site.app.command;

public interface Command<T> {

	T exec();

    T exec(int repeat);
}
