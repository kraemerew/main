#include <QApplication>
#include <QElapsedTimer>
#include "highwaynn//sschighwaynetwork.hpp"
#include "image/image.hpp"
#include "filter/selector.hpp"
#include "cam/cam.hpp"
#include "cam/camwidget.hpp"
#include "cam/frameintervaldescriptor.hpp"
#include "image/moments.hpp"
#include <QTimer>

#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif

class SScMultiCamTester : public QObject
{
    Q_OBJECT
public:
    SScMultiCamTester() : QObject(0), m_cam(NULL)
    {
        QTimer::singleShot(1000,this,SLOT(addCamSlot()));
    }
private slots:
    void addCamSlot()
    {
        // qWarning(">>>>>>>>>>>>>ADDING");
        m_cam = new (std::nothrow) SScCam("/dev/video0","MJPG",320,240);
        Q_CHECK_PTR(m_cam);
        m_cam->streamOn();

        QTimer::singleShot(5000,this,SLOT(delCamSlot()));
    }
    void delCamSlot()
    {
      //  qWarning(">>>>>>>>>>>>>REMOVING");

        m_cam->deleteLater();
        m_cam = NULL;
        QTimer::singleShot(5000,this,SLOT(addCamSlot()));
    }

private:
    SScCam* m_cam;
};

bool bitSet(int v, int nr) { return (v&(0x01<<nr))!=0; }
int bitsSet(int v)
{
    int ret = 0;
    for (int i=0; i<31; ++i) if (bitSet(v,i)) ++ret;
    return ret;
}
bool evenParity(int v) { return ((bitsSet(v)%2)==0); }

void parityTest()
{
    SScHighwayNetwork net;
    net.setTrainingType(SScTrainableParameter::CON_ADAM);
    net.setHiddenActivationType(SScActivation::ACT_RBF);
    net.setOutputActivationType(SScActivation::ACT_RBF);

    const int bi = net.addBiasNeuron    ("Bias"),
              i1 = net.addInputNeuron   ("In1"),
              i2 = net.addInputNeuron   ("In2"),
              i3 = net.addInputNeuron   ("In3"),
              i4 = net.addInputNeuron   ("In4"),
              i5 = net.addInputNeuron   ("In5"),
              i6 = net.addInputNeuron   ("In6"),
              i7 = net.addInputNeuron   ("In7"),
              i8 = net.addInputNeuron   ("In8"),
              h1 = net.addHiddenNeuron  ("H1"),
              h2 = net.addHiddenNeuron  ("H2"),
              h3 = net.addHiddenNeuron  ("H3"),
              h4 = net.addHiddenNeuron  ("H4"),
              h5 = net.addHiddenNeuron  ("H5"),
              h6 = net.addHiddenNeuron  ("H6"),
              h7 = net.addHiddenNeuron  ("H7"),
              h8 = net.addHiddenNeuron  ("H8"),

            o1 = net.addOutputNeuron  ("Out");
    QList<int> il = QList<int>() << i1 << i2 << i3 << i4 << i5 << i6 << i7 << i8,
               hl = QList<int>() << h1 << h2 << h3 << h4 << h5 << h6 << h7 << h8;

    // Bias to carry, hidden and out
    net.connect(bi,o1);
    foreach(int to, hl) net.connect(bi,to);

    // Input to hidden
    foreach(int from, il) foreach(int to, hl) net.connect(from,to);
    foreach(int from, il) net.connect(from,o1);

    // Hidden to output
    foreach(int from, hl) net.connect(from,o1);

    // training preparation
    net.connectForward();
    // training
    QElapsedTimer t, et; t.start(); et.start();
    int c=0, failcount=0;
    double err = 0, lasterr = 0;
    bool done = false;
    do
    {

        //if (c==50) std::exit(0);
        const int p = (++c)%256;
        if (p==0) err = 0;
        const int bits = bitsSet(p);

        for (int i=0; i<8; ++i) if (bitSet(p,i)) net.setInput(il[i],1); else net.setInput(il[i],0);

        const bool even = (bits%2)==0;
        const double trg = even ? 0.0 : 1.0;
        net.setTarget(o1,trg);

        net.reset();


        const double pout = net.idx2n(o1)->out(), pdlt = qAbs(pout-trg), perr = net.idx2n(o1)->perr();
        const bool success = pdlt<0.5;
        qWarning("%s %5d #bits: %2d %s Output %lf Target %lf", success ? "OK ":"NOK", p, bits, even ?"EVEN":"ODD", pout, trg);
        err+=perr;
        if (!success) ++failcount;
//if (c==100) std::exit(1);
        if (p==255)
        {
            if (failcount==0) done = true;
            if (done || (et.elapsed()>200))
            {
                et.restart();
                qWarning("Cycle %d failures %d Error %lf Last %lf %s", c/256, failcount, err, lasterr, err<lasterr ? "lower":"higher");
            }
            if (done) qWarning("Done");

            lasterr = err;
            failcount = 0;
            //if (c>1000) std::exit(1);
        }


        const bool endOfCycle = (p==255);
        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d ms", (int)t.elapsed());
    std::exit(0);
}
void carryTest()
{
    SScHighwayNetwork net;
    net.setTrainingType(SScTrainableParameter::CON_ADAM);
    const int bi = net.addBiasNeuron    ("Bias"),
              i1 = net.addInputNeuron   ("In1"),
              i2 = net.addInputNeuron   ("In2"),
              i3 = net.addInputNeuron   ("In3"),
              i4 = net.addInputNeuron   ("In4"),
              carry = net.addCarryNeuron  ("C"),      //<Carry
              o1 = net.addOutputNeuron  ("Out");

    QList<int> il = QList<int>() << i1 << i2 << i3 << i4;

    net.connect(bi,carry);
    net.connect(bi,o1);
    foreach(int from, il)
    {
        net.connect(from,o1);
        net.connect(from,carry);
    }
    net.setHighway(o1,i1,carry);

    net.connectForward();
    // training
    QElapsedTimer t; t.start();
    int c=0;
    double err = 0, lasterr = 0;
    bool done = false;
    do
    {
if (c==10000) std::exit(0);
        const int p = (++c)%16;
        if (p==0) err = 0;
        int bits = 0;
        if (p&0x01) { net.setInput(i1,1); ++bits; } else net.setInput(i1,0);
        if (p&0x02) { net.setInput(i2,1); ++bits; } else net.setInput(i2,0);
        if (p&0x04) { net.setInput(i3,1); ++bits; } else net.setInput(i3,0);
        if (p&0x08) { net.setInput(i4,1); ++bits; } else net.setInput(i4,0);

        if (p<8)
        {
            // First half: actually use the parity - carry should be random or low
            if (bits%2) net.setTarget(o1,1); else net.setTarget(o1,0);
        }
        else
        {
            // Second half: use the input 1 as target - carry should be high on these
            if (p&0x01)   net.setTarget(o1,1); else net.setTarget(o1,0);
        }

        net.reset();


        qWarning("Pattern %d In1 %lf Output %lf Carry %lf", p, net.idx2n(i1)->out(), net.idx2n(o1)->out(), net.idx2n(carry)->out());
        const double perr = net.idx2n(o1)->perr();
        err+=perr;
        if (p==15)
        {
            err/=16.0;
            if (err<0.0001) done = true;
            qWarning("Cycle %d Error %lf Last %lf %s", c/16, err, lasterr, err<lasterr ? "lower":"higher");
            if (done) qWarning("Done");
            lasterr = err;

            //if (c>1000) std::exit(1);
        }


        const bool endOfCycle = (p==15);
        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d microseconds", (int)t.nsecsElapsed()/1000);
    std::exit(0);
}



#include "filter/filter.hpp"
//#include "rnn/rneuron.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    parityTest();
    /*SScRBiasNeuron* bn = new (std::nothrow) SScRBiasNeuron();
    QList<SScRNeuron*> nl;
    for (int i=0; i<2; ++i) nl << new (std::nothrow) SScRNeuron();
    foreach(SScRNeuron* n1, nl) n1->inputGate()->addConnection(bn,-1.3);
    foreach(SScRNeuron* n1, nl) foreach(SScRNeuron* n2, nl)
    {
        n2->inputGate   ()->addConnection(n1,-0.1);
        n2->outputGate  ()->addConnection(n1, 0.1);
        n2->storeGate   ()->addConnection(n1, 0.2);
        n2->memoryGate  ()->addConnection(n1,-0.3);
    }

    int t = 0;
    while (t<10)
    {
        ++t;
        QElapsedTimer tm; tm.start();
        const double o = nl.last()->out(t);
        qWarning(">>>>>>>>>>>%lf %d", o, (int)tm.elapsed());

    }



    std::exit(0);
    qWarning("----------------------------------------");
    SScSignal<double> *s0 = new SScStdSignal<double>();
    (*s0)[7]=7;
    (*s0)[5]=5;
    (*s0)[11]=11;
    (*s0)[-10]=-10;

    SScSignal<double> *s1 = new SScMapSignal<double>();
    (*s1)[10]=5;
    SScSignal<double> *s2 = new SScHashSignal<double>();
    (*s2)[5]=10;

    for (int i=-1; i<12; ++i) qWarning("%d %lf %lf %lf",i, s0->at(i), s1->at(i), s2->at(i));

  SScFrameIntervalDescriptor dd(1,2);
    SScFrameIntervalDescriptor sd(0,1,1,1,1,3);
    QList<SScFrameIntervalDescriptor> l;
    l<< sd << dd;
    qWarning("2/4 %s", (l.first().allowed(2,4)) ? "allowed":"NOT allowed");
    qWarning("1/2 %s", (l.first().allowed(1,2)) ? "allowed":"NOT allowed");
    qWarning("3/4 %s", (l.first().allowed(3,4)) ? "allowed":"NOT allowed");
    qWarning("1/3 %s", (l.first().allowed(1,3)) ? "allowed":"NOT allowed");
    qWarning("2/3 %s", (l.first().allowed(2,3)) ? "allowed":"NOT allowed");


    QPair<quint32,quint32> fit = SScFrameIntervalDescriptor::fit(1,5,l);
    qWarning("1/5 fit %u %u", fit.first,fit.second);
    exit(0);
*/

    SScImage im;
    im.load("/home/developer/diamond.png");
    SScUCMatrix m = im.grey();

    m.binarize(128,0,1);
    m.save("/home/developer/out.png","PNG");


    SScMoments<uchar> x(m);
    x.dump();

/*    QApplication a(argc, argv);
    SScCamWidget* w = new (std::nothrow) SScCamWidget();
    Q_CHECK_PTR(w);
    w->show();
*/


    //const bool allow = SScCamCapability(0).allowed("MJPG",320,240);
   // qWarning("RES %s", allow?"ALLOWED":"NOT ALLOWED");



    /*auto im = new SScImage();
    im->load("/home/developer/alpha.jpg");
    qWarning("loaded %dx%d",im->width(),im->height());
    QElapsedTimer t; t.start();
    auto r = im->red(), b = im->blue(), g = im->green();
    SScImage im2(r,g,b);
    qWarning("Elapsed %d ms", (int)t.elapsed());
    im2.save("/home/developer/out.jpg");
    SScImage(r).save("/home/developer/out_r.png");
    SScImage(g).save("/home/developer/out_g.png");
    SScImage(b).save("/home/developer/out_b.png");
    qWarning("SAVED %dx%d",im2.width(),im2.height());*/
    //delete im;

   /* // Build net
    SScNetwork net;
    const int bi = net.addNeuron(SScNeuron::NeuronType_Bias),
              i1 = net.addNeuron(SScNeuron::NeuronType_Input),
              i2 = net.addNeuron(SScNeuron::NeuronType_Input),
              h1 = net.addNeuron(SScNeuron::NeuronType_Hidden),
              o1 = net.addNeuron(SScNeuron::NeuronType_Output);
    net.connect(bi,h1, 1.1);
    net.connect(bi,o1, 1.1);
    net.connect(i1,o1, 0.1);
    net.connect(i2,o1, 0.2);
    net.connect(i1,h1, 0.1);
    net.connect(i2,h1,-0.3);
    net.connect(h1,o1, 0.2);

    net.idx2n(o1)->setActivation(SScActivation::Act_SoftPlus);
    net.idx2n(h1)->setActivation(SScActivation::Act_SoftPlus);

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
        case 0: net.idx2n(i1)->setInput(0); net.idx2n(i2)->setInput(0); net.idx2n(o1)->setTarget( 0); break;
        case 1: net.idx2n(i1)->setInput(0); net.idx2n(i2)->setInput(1); net.idx2n(o1)->setTarget( 1); break;
        case 2: net.idx2n(i1)->setInput(1); net.idx2n(i2)->setInput(0); net.idx2n(o1)->setTarget( 1); break;
        case 3: net.idx2n(i1)->setInput(1); net.idx2n(i2)->setInput(1); net.idx2n(o1)->setTarget( 0); break;
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
    qWarning("Training took %d microseconds", (int)t.nsecsElapsed()/1000);
    */
    //return a.exec();
}
