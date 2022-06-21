#ifndef NET
#define NET

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>
#include "hello_m.h"
#define RING_SIZE 8

#define CLOCKWISE_ALGORITHM 0
#define SHORT_FLOOD_ALGORITHM 1
#define CHAINED_HELLO_ALGORITHM 2

#define CURRENT_ALGORITM CLOCKWISE_ALGORITHM

using namespace omnetpp;

class Net: public cSimpleModule {
private:
	void handleDataPacket(Packet *pkt);
	void reroutePacket(Packet *pkt);
	int getArrivalGateIndex(cMessage *msg);

	// Short Flood Algorithm
	void handleRerouteForShortFlood(Packet *pkt);

	// Chained Hello Algorithm
	bool isTopologyKnown;
	int * routingTable;
	cMessage *startHelloEvent;
	void handleHelloPacket(Hello *hello);
	void addSelfToHelloChain(Hello *hello);
	bool isHelloChainComplete(Hello *hello);
	void saveTopologyData();
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

	if (CURRENT_ALGORITM == CHAINED_HELLO_ALGORITHM) {
		if (this->getParentModule()->getIndex() == 0) {
			// Inicializamos la exploración de topología
			scheduleAt(simTime() + 0, startHelloEvent);
		}
	}
}

void Net::finish() {
	delete(startHelloEvent);
	if(routingTable != NULL){
		delete(routingTable);
	}
}

void Net::handleMessage(cMessage *msg) {
	if (msg == startHelloEvent) {
		Hello * hello = new Hello();
		hello->setKind(31000);
		hello->setByteLength(8 * 4);
		for (int i=0; i<8; i++) {
			hello->setNodes(i, -1);
		}
		addSelfToHelloChain(hello);
		send(hello, "toLnk$o", 0);
	} else {
		Packet *pkt = (Packet *) msg;
		if (pkt->getKind() == 31000) {
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

void Net::handleDataPacket(Packet *pkt) {
	pkt->setHopCount(pkt->getHopCount() + 1);
    if (pkt->getDestination() == this->getParentModule()->getIndex()) {
    	// We are the destination
    	send(pkt, "toApp$o");
    } else {
    	reroutePacket(pkt);
	}
}

void Net::reroutePacket(Packet *pkt) {
	switch (CURRENT_ALGORITM) {
	case CLOCKWISE_ALGORITHM:
		send(pkt, "toLnk$o", 0);
		break;
	case SHORT_FLOOD_ALGORITHM:
		handleRerouteForShortFlood(pkt);
		break;
	case CHAINED_HELLO_ALGORITHM:
		send(pkt, "toLnk$o", routingTable[pkt->getDestination()]);
		break;
	default:
		break;
	}
}

/* SHORT FLOOD FUNCTIONS */

void handleRerouteForShortFlood(Packet *pkt) {
	if(pkt->getSource() == this->getParentModule()->getIndex()){
		// Somos el origen
		send(pkt->dup(), "toLnk$o", 0);
		send(pkt, "toLnk$o", 1);
	} else if (par("hopsToLive").intValue() - pkt->getHopCount() > 0) {
		// No somos el origen y todavía hay hops disponibles
		send(pkt, "toLnk$o", 1 - getArrivalGateIndex(pkt));
	} else {
		// No somos el origen y ya no hay hops disponibles
		delete(pkt);
	}
}

/* CHAINED HELLO FUNCTIONS */

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
	routingTable = new int[RING_SIZE];
	int m,k;
	int n = RING_SIZE - 1;
	int leftDistance;
	int rightDistance;
	int i = this->getParentModule()->getIndex();
	for(unsigned int j = 0; j<RING_SIZE; j++){
		m = 1;
		k = i<=j ? 0 : 1;
		leftDistance = n*(m+k) + 2*m*(i-j-k*n)- i + j;
		m = 0;
		rightDistance = n*(m+k) + 2*m*(i-j-k*n)- i + j;
		routingTable[j] = leftDistance <= rightDistance ? 0 : 1;
	}
	routingTable[i] = -1;
}
