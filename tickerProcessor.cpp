/*
* Basic program used to take input from a file that has NYSE abbreviations and store the price of the stock in another file. 
*/

#include <blpapi_session.h>
#include <blpapi_eventdispatcher.h>

#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_element.h>
#include <blpapi_name.h>
#include <blpapi_request.h>
#include <blpapi_subscriptionlist.h>
#include <blpapi_defs.h>
#include <blpapi_exception.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace blpapi;
namespace {
    const Name LAST_PRICE("LAST_PRICE");
}

class MyEventHandler :public EventHandler {
    // Process events using callback


    public:
    bool processEvent(const Event &event, Session *session) {
        try {
			
			
            if (event.eventType() == Event::SUBSCRIPTION_DATA) {
				std::ofstream output("price.txt");
                MessageIterator msgIter(event);
                while (msgIter.next()) {
                    Message msg = msgIter.message();
                    if (msg.hasElement(LAST_PRICE)) {
                        Element field = msg.getElement(LAST_PRICE);
                        output << field.getValueAsString() << std::endl;
                    }
					output.close();
                }
            }
            return true;
        } catch (Exception &e) {
            std::cerr << "Library Exception!!! " << e.description()
                << std::endl;
        } catch (...) {
            std::cerr << "Unknown Exception!!!" << std::endl;
        }
        return false;
    }
};

class SimpleBlockingRequestExample {

    CorrelationId d_cid;
    EventQueue d_eventQueue;
    std::string         d_host;
    int                 d_port;

    void printUsage()
    {
        std::cout << "Usage:" << std::endl
            << "    Retrieve reference data " << std::endl
            << "        [-ip        <ipAddress  = localhost>" << std::endl
            << "        [-p         <tcpPort    = 8194>" << std::endl;
    }

    bool parseCommandLine(int argc, char **argv)
    {
        for (int i = 1; i < argc; ++i) {
            if (!std::strcmp(argv[i],"-ip") && i + 1 < argc) {
                d_host = argv[++i];
                continue;
            }
            if (!std::strcmp(argv[i],"-p") &&  i + 1 < argc) {
                d_port = std::atoi(argv[++i]);
                continue;
            }
            printUsage();
            return false;
        }
        return true;
    }

public:
    SimpleBlockingRequestExample(): d_cid((int)1) {
	}


    void run(int argc, char **argv) {
		
        d_host = "localhost";
        d_port = 8194;
        if (!parseCommandLine(argc, argv)) return;

        SessionOptions sessionOptions;
        sessionOptions.setServerHost(d_host.c_str());
        sessionOptions.setServerPort(d_port);

        std::cout << "Connecting to " <<  d_host << ":" << d_port << std::endl;
        Session session(sessionOptions, new MyEventHandler());
        if (!session.start()) {
            std::cerr << "Failed to start session." << std::endl;
            return;
        }
        if (!session.openService("//blp/mktdata")) {
            std::cerr << "Failed to open //blp/mktdata" << std::endl;
            return;
        }
        if (!session.openService("//blp/refdata")) {
            std::cerr <<"Failed to open //blp/refdata" << std::endl;
            return;
        }
		std::string str;
		std::ifstream input ("ticker.txt");
		input >> str;
		str += " US Equity";
		const char *pass = str.c_str();

		
        std::cout << "Subscribing to IBM US Equity" << std::endl;
        SubscriptionList subscriptions;
		subscriptions.add(pass, "LAST_PRICE", "", d_cid);
        session.subscribe(subscriptions);

        std::cout << "Requesting reference data F US Equity" << std::endl;
        Service refDataService = session.getService("//blp/refdata");
        Request request = refDataService.createRequest("ReferenceDataRequest");
        request.append("securities", pass);
        request.append("fields", "DS002");

        CorrelationId cid(this);
        session.sendRequest(request, cid, &d_eventQueue);
        while (true) {
            Event event = d_eventQueue.nextEvent();
            MessageIterator msgIter(event);
            while (msgIter.next()) {
                Message msg = msgIter.message();
                msg.print(std::cout);
            }
            if (event.eventType() == Event::RESPONSE) {
                break;
            }
        }

        std::cout << "Press ENTER to quit" << std::endl;
        char dummy[2];
        std::cin.getline(dummy, 2);
    }

};

int main(int argc, char **argv) {
    SimpleBlockingRequestExample example;
        example.run(argc, argv);


    return 0;
}
