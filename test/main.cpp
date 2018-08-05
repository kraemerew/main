#include <QCoreApplication>
#include <QElapsedTimer>

#include "../nn/sscnetwork.hpp"

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    // Build net
    SScNetwork net;
    const int bi = net.addNeuron(SScNeuron::NeuronType_Bias),
              i1 = net.addNeuron(SScNeuron::NeuronType_Input),
              i2 = net.addNeuron(SScNeuron::NeuronType_Input),
              h1 = net.addNeuron(SScNeuron::NeuronType_Hidden),
              o1 = net.addNeuron(SScNeuron::NeuronType_Output);
    net.connect(bi,h1,1.1);
    net.connect(bi,o1,1.1);
    net.connect(i1,o1, 0.1);
    net.connect(i2,o1, 0.2);
    net.connect(i1,h1,-0.1);
    net.connect(i2,h1,-0.3);
    net.connect(h1,o1, 0.2);

    // training preparation
    net.connectForward();

    // training
    QElapsedTimer t; t.start();
    int c=0;
    double err = 0;
    bool done = false;
    do
    {
        const int p = (++c)%4;
        if (p==0) err = 0;
        switch(p)
        {
        case 0: net.idx2n(i1)->setInput(0); net.idx2n(i2)->setInput(0); net.idx2n(o1)->setTarget(0); break;
        case 1: net.idx2n(i1)->setInput(0); net.idx2n(i2)->setInput(1); net.idx2n(o1)->setTarget(1); break;
        case 2: net.idx2n(i1)->setInput(1); net.idx2n(i2)->setInput(0); net.idx2n(o1)->setTarget(1); break;
        case 3: net.idx2n(i1)->setInput(1); net.idx2n(i2)->setInput(1); net.idx2n(o1)->setTarget(0); break;
        }
        qWarning("Pattern %d Output %lf", p, net.idx2n(o1)->out());
        const double perr = qAbs(net.idx2n(o1)->err());
        err+=perr;
        if (p==3)
        {
            err/=4.0;
            qWarning("Cycle %d Error %lf", c, err);
            if (err<0.1) done = true;
        }

        const bool endOfCycle = (p==3);
        net.idx2n(o1)->trainingStep(endOfCycle);
        net.idx2n(h1)->trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d microsectonds", (int)t.nsecsElapsed()/1000);
    return a.exec();
}
