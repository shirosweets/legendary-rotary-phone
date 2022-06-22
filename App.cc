#ifndef APP
#define APP

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>

using namespace omnetpp;

class App: public cSimpleModule {
private:
    cMessage *sendMsgEvent;
    cStdDev delayStats;
    cOutVector delayVector;
    cOutVector hopsCount;
    cStdDev ** receivedFromStdDevArray;
public:
    App();
    virtual ~App();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(App);

#endif /* APP */

App::App() {
}

App::~App() {
    for (int i=0; i<par("amountHosts").intValue(); ++i) {
        cStdDev * receivedFrom = (cStdDev *)(receivedFromStdDevArray[i]);
        delete(receivedFrom);
        receivedFromStdDevArray[i] = NULL;
    }
    delete(receivedFromStdDevArray);
}

void App::initialize() {
    // If interArrivalTime for this node is higher than 0
    // initialize packet generator by scheduling sendMsgEvent
    if (par("interArrivalTime").doubleValue() != 0) {
        sendMsgEvent = new cMessage("sendEvent");
        scheduleAt(par("interArrivalTime"), sendMsgEvent);
    }

    // Initialize statistics
    delayStats.setName("TotalDelay");
    delayStats.collect(0);
    delayVector.setName("Delay");
    delayVector.record(0);
    hopsCount.setName("Hops");
    hopsCount.record(0);
    int amountHosts = par("amountHosts").intValue();
    receivedFromStdDevArray = new cStdDev*[amountHosts];
    for (int i=0; i<amountHosts; ++i) {
        cStdDev* receivedFrom = new cStdDev();
        std::string name =
            std::to_string(i) +
            "->" +
            std::to_string(this->getParentModule()->getIndex());
        std::cout << "??? " << name << "\n";
        receivedFrom->setName(name.c_str());
        receivedFrom->collect(0);
        receivedFromStdDevArray[i] = receivedFrom;
    }
}

void App::finish() {
    // Record statistics
    recordScalar("Average delay", delayStats.getMean());
    recordScalar("Number of packets", delayStats.getCount());
    for (int i=0; i<par("amountHosts").intValue(); ++i) {
        std::string scalarName =
            "Packets from " +
            std::to_string(i) +
            " to " +
            std::to_string(this->getParentModule()->getIndex());
        std::cout << scalarName << "\n";
        cStdDev* receivedFrom = receivedFromStdDevArray[i];
        recordScalar(scalarName.c_str(), receivedFrom->getCount());
    }
}

void App::handleMessage(cMessage *msg) {

    // if msg is a sendMsgEvent, create and send new packet
    if (msg == sendMsgEvent) {
        // create new packet
        Packet *pkt = new Packet("packet",this->getParentModule()->getIndex());
        pkt->setByteLength(par("packetByteSize"));
        pkt->setSource(this->getParentModule()->getIndex());
        pkt->setHopCount(0);
        pkt->setDestination(par("destination"));

        // send to net layer
        send(pkt, "toNet$o");

        // compute the new departure time and schedule next sendMsgEvent
        simtime_t departureTime = simTime() + par("interArrivalTime");
        scheduleAt(departureTime, sendMsgEvent);

    }
    // else, msg is a packet from net layer
    else {
        // compute delay and record statistics
        simtime_t delay = simTime() - msg->getCreationTime();
        cStdDev* receivedFrom = receivedFromStdDevArray[((Packet*)msg)->getSource()];
        receivedFrom->collect(1);
        delayStats.collect(delay);
        delayVector.record(delay);
        Packet *pkt = (Packet *) msg;
        hopsCount.record(pkt->getHopCount());
        // delete msg
        delete (msg);
    }

}
