 g++ ./SimpleProducer.cpp -o sent `pkg-config --cflags glib-2.0` `activemqcpp-config --cflags` `activemqcpp-config --libs` -lpthread -std=c++11
