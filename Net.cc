#ifndef NET
#define NET

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>
#include "hello_m.h"
#define RING_SIZE 8

using namespace omnetpp;

class Net: public cSimpleModule {
private:
	cMessage *startHelloEvent;
	int getArrivalGateIndex(cMessage *msg);
	bool isHelloChainComplete(Hello *hello);
	void addSelfToHelloChain(Hello *hello);
	void handleDataPacket(Packet *pkt);
	void handleHelloPacket(Hello *hello);
	void saveTopologyData();
	int getOutGateIndex(Packet *pkt);
	bool isTopologyKnown;
	int * routingTable;
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
	startHelloEvent = new cMessage("startHelloEvent");
	isTopologyKnown = false;
	routingTable = NULL;
	if (this->getParentModule()->getIndex() == 0) {
		// Inicializamos la exploración de topología
		scheduleAt(simTime() + 0, startHelloEvent);
	}
}

void Net::finish() {
}

void Net::handleMessage(cMessage *msg) {
	if (msg == startHelloEvent) {
		Hello * hello = new Hello();
		hello->setKind(2);
		hello->setByteLength(8 * 4);
		for (int i=0; i<8; i++) {
			hello->setNodes(i, -1);
		}
		addSelfToHelloChain(hello);
		send(hello, "toLnk$o", 0);
	} else {
		Packet *pkt = (Packet *) msg;
		if (pkt->getKind() == 2) {
			handleHelloPacket((Hello*)msg);
		} else {
			handleDataPacket(pkt);
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

bool Net::isHelloChainComplete(Hello * hello) {
	return hello->getNodes(7) != -1;
}

void Net::addSelfToHelloChain(Hello * hello) {
	for (int i=0; i<8; i++) {
		if (hello->getNodes(i) == -1) {
			hello->setNodes(i, this->getParentModule()->getIndex());
			std::cout << "Adding node " << this->getParentModule()->getIndex();
			std::cout << " to Hello chain on index " << i << "\n";
			break;
		}
	}
}

void Net::handleDataPacket(Packet *pkt) {
    if (pkt->getDestination() == this->getParentModule()->getIndex()) {
    // We are the destination
    	pkt->setHopCount(pkt->getHopCount() + 1);
    	send(pkt, "toApp$o");
    }
    else {
    	send(pkt, "toLnk$o", routingTable[pkt->getDestination()]);
/*   	if(pkt->getSource() == this->getParentModule()->getIndex()){
    		// Somos el origen
    		send(pkt->dup(), "toLnk$o", 0);
    		send(pkt, "toLnk$o", 1);
    	}
    	else {
    		// No somos el origen
    		pkt->setHopCount(pkt->getHopCount() + 1);
    		if (par("hopsToLive").intValue() - pkt->getHopCount() > 0) {
				send(pkt, "toLnk$o", getOutGateIndex(pkt));
    		} else {
    			delete(pkt);
    		}
*/    	}
    }

void Net::handleHelloPacket(Hello *hello) {
	if (this->getParentModule()->getIndex() == 0) {
		// Soy el iniciador
		std::cout << "// Soy el iniciador\n";
		if (!isTopologyKnown) {
			saveTopologyData();
			isTopologyKnown = true;
			send(hello->dup(), "toLnk$o", 0);
			send(hello, "toLnk$o", 1);
		} else {
			delete(hello);
		}
	} else {
		if (isHelloChainComplete(hello)) {
			// chain complete...
			if (isTopologyKnown) {
				delete(hello);
			} else {
				saveTopologyData();
				isTopologyKnown = true;
				send(hello, "toLnk$o", 1 - getArrivalGateIndex(hello));
			}
		} else {
			addSelfToHelloChain(hello);
			send(hello, "toLnk$o", 1 - getArrivalGateIndex(hello));
		}
	}
}

void Net::saveTopologyData() {
	std::cout << "Saving Topology Data in node: " << this->getParentModule()->getIndex() << "\n";
	routingTable = new int[8];
	int m,k;
	int leftDistance;
	int rightDistance;
	int i = this->getParentModule()->getIndex();
	for(unsigned int j = 0; j<RING_SIZE; j++){
		m = 1;
		k = i<=j ? 0 : 1;
		leftDistance = k*m*(i-j) + (1-k)*((RING_SIZE - 1)+m*(i-j));
		m = -1;
		rightDistance = k*m*(i-j) + (1-k)*((RING_SIZE - 1)+m*(i-j));
		routingTable[j] = leftDistance <= rightDistance ? 1 : 0;
	}
	for(unsigned int j = 0; j<RING_SIZE; j++){
		std::cout << "For node i " << i << " and j " << j << " is " << routingTable[j] << "\n";
	}
}

int Net::getOutGateIndex(Packet *pkt) {
	return 1 - getArrivalGateIndex(pkt);
}
