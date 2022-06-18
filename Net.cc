#ifndef NET
#define NET

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>

using namespace omnetpp;

class Net: public cSimpleModule {
private:
	cMessage *hello;
public:
    Net();
    virtual ~Net();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Net);

#endif /* NET */

Net::Net() {
}

Net::~Net() {
}

void Net::initialize() {

}

void Net::finish() {
}

void Net::handleMessage(cMessage *msg) {

    // All msg (events) on net are packets
    Packet *pkt = (Packet *) msg;

    if (pkt->getDestination() == this->getParentModule()->getIndex()) {
    	send(msg, "toApp$o");
    }
    else {
    	if(pkt->getSource() == this->getParentModule()->getIndex()){
    		send(pkt, "toLnk$o", 0);
    	}
    	else {
    		pkt->setHopCount(pkt->getHopCount() + 1);
    		send(pkt, "toLnk$o", 0);
    	}
    }
}
