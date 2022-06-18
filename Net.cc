#ifndef NET
#define NET

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>

using namespace omnetpp;

class Net: public cSimpleModule {
private:
	int getArrivalGateIndex(cMessage *msg);
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
    // We are the destination
    	send(msg, "toApp$o");
    }
    else {
    	if(pkt->getSource() == this->getParentModule()->getIndex()){
    		// Somos el origen
    		send(pkt->dup(), "toLnk$o", 0);
    		send(pkt, "toLnk$o", 1);
    	}
    	else {
    		// No somos el origen
    		pkt->setHopCount(pkt->getHopCount() + 1);
    		send(pkt, "toLnk$o", 1 - getArrivalGateIndex(msg));
    	}
    }
}

int Net::getArrivalGateIndex(cMessage *msg) {
	for (int i=0; i<par("interfaces").intValue(); i++) {
		if (msg->arrivedOn("toLnk$i", i)) {
			return i;
		}
	}
	return -1;
}
