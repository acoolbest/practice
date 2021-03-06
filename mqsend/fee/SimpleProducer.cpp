/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <unistd.h>

using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

//---------------------基本参数配置-----------------------------------------
//free-h5
#define QRNUM 4 //定义二维码数量
#define CHARGETIME 1 //定义充电时间（分钟）
#define MESSNUM 2 //mq消息条数
//#define MESSNUM 50000 //消息条数
#define SLEEPTIME 10 //定义消息间隔时间s
char ch_qrcode[4][10]={"12450034","12450035","12449957","12449958"};
char desturi[20]="BJTEST10000163";
//---------------------基本参数配置-----------------------------------------
////////////////////////////////////////////////////////////////////////////////
class SimpleProducer : public Runnable {
private:

    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    bool useTopic;
    bool clientAck;
    unsigned int numMessages;
    std::string brokerURI;
    std::string destURI;

private:

    SimpleProducer( const SimpleProducer& );
    SimpleProducer& operator= ( const SimpleProducer& );

public:

    SimpleProducer( const std::string& brokerURI, unsigned int numMessages,
                    const std::string& destURI, bool useTopic = false, bool clientAck = false ) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        producer(NULL),
        useTopic(useTopic),
        clientAck(clientAck),
        numMessages(numMessages),
        brokerURI(brokerURI),
        destURI(destURI) {
    }

    virtual ~SimpleProducer(){
        cleanup();
    }

    void close() {
        this->cleanup();
    }

    virtual void run() {
        try {

            // Create a ConnectionFactory
            unique_ptr<ActiveMQConnectionFactory> connectionFactory(
                new ActiveMQConnectionFactory( brokerURI ) );

            // Create a Connection
            try{
                connection = connectionFactory->createConnection();
                connection->start();
            } catch( CMSException& e ) {
                e.printStackTrace();
                throw e;
            }

            // Create a Session
            if( clientAck ) {
                session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
            } else {
                session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
            }

            // Create the destination (Topic or Queue)
            if( useTopic ) {
                destination = session->createTopic( destURI );
            } else {
                destination = session->createQueue( destURI );
            }

            // Create a MessageProducer from the Session to the Topic or Queue
            producer = session->createProducer( destination );
            producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

            // Create the Thread Id String
            string threadIdStr = Long::toString( Thread::currentThread()->getId() );

            // Create a messages
            string text = (string)"Hello world! from thread " + threadIdStr;
	    char ch_type[3][2]={"2","1","3"};

for(int j=0;j<MESSNUM;j++)//fee-app
{

	for(int i=0;i<2;i++)
	{
		for( unsigned int ix=0; ix<numMessages; ++ix )
		{            
			char msg[1000]="";
			if(1==i%2)	
				//{"id":"3129025","time":"1","portCode":"12449998","type":"1","dataSource":"0","timeout":120}	//#define CHARGETIME 1 //定义充电时间（分钟）
				sprintf(msg,"{\"id\":\"%d\",\"time\":\"%d\",\"portCode\":\"%s\",\"type\":\"%d\",\"dataSource\":\"0\",\"timeout\":120}", j*4+ix+1,CHARGETIME,ch_qrcode[ix],2-i%2);
			else
				sprintf(msg,"{\"id\":\"%d\",\"time\":\"0\",\"portCode\":\"%s\",\"type\":\"%d\",\"dataSource\":\"0\",\"timeout\":120}", j*4+ix+1,ch_qrcode[ix],2-i%2);

			text=msg;                
			TextMessage* message = session->createTextMessage( text );
			printf("%s\n",msg);
		        message->setIntProperty( "Integer", ix );

		        // Tell the producer to send the message
		        producer->send( message );
			//getchar();
		        delete message;
		}
		sleep(SLEEPTIME);
	}
sleep(120);
}


        }catch ( CMSException& e ) {
            e.printStackTrace();
        }
    }

private:

    void cleanup(){

        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        destination = NULL;

        try{
            if( producer != NULL ) delete producer;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        producer = NULL;

        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch ( CMSException& e ) { e.printStackTrace(); }

        try{
            if( session != NULL ) delete session;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        session = NULL;

        try{
            if( connection != NULL ) delete connection;
        }catch ( CMSException& e ) { e.printStackTrace(); }
        connection = NULL;
    }
};
void * send_thread(void * arg)
{
    std::string brokerURI =
        "failover://(tcp://120.26.8.53:61616"
//        "?wireFormat=openwire"
//        "&connection.useAsyncSend=true"
//        "&transport.commandTracingEnabled=true"
//        "&transport.tcpTracingEnabled=true"
//        "&wireFormat.tightEncodingEnabled=true"
        ")";

    //============================================================
    // Total number of messages for this producer to send.
    //============================================================
    unsigned int numMessages = QRNUM;//???

//    char desturi[20]="";
//    sprintf(desturi,"BJTEST10000164");//"zhzq-wangjing-test");//,*(int *)arg);
//    sprintf(desturi,"BJTEST10000109");//"test");//,*(int *)arg); 
    std::string destURI(desturi);

    //============================================================
    // set to true to use topics instead of queues
    // Note in the code above that this causes createTopic or
    // createQueue to be used in the producer.
    //============================================================
    bool useTopics = true;

    // Create the producer and run it.
    SimpleProducer producer( brokerURI, numMessages, destURI, useTopics );

    // Publish the given number of Messages
    producer.run();

    // Before exiting we ensure that all CMS resources are closed.
    producer.close();

}
////////////////////////////////////////////////////////////////////////////////
int main(int argc AMQCPP_UNUSED, char* argv[] AMQCPP_UNUSED) {

    activemq::library::ActiveMQCPP::initializeLibrary();

    std::cout << "=====================================================\n";
    std::cout << "Starting the example:" << std::endl;
    std::cout << "-----------------------------------------------------\n";
    int ret=0;
    pthread_t pThreadId[100]={0};
    for(int i=0;i<1;i++)
	{
		int x=i+1;
		ret=pthread_create(pThreadId+x-1,NULL,send_thread,(void*)(&x));
	}
    if(ret==0) pthread_join(pThreadId[0],NULL);
    
    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\n";

    activemq::library::ActiveMQCPP::shutdownLibrary();
}
