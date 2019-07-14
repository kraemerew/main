#include <QApplication>
#include <QElapsedTimer>
#include "highwaynn//network.hpp"
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

void parityTest(int pow)
{
    const int pmax = qPow(2,pow), plast = pmax-1;
    SScHighwayNetwork net;
    net.setTrainingType(SScTrainableParameter::ADAM);
    net.setHiddenActivationType(SScActivation::RBF);
    net.setOutputActivationType(SScActivation::MHAT);
   // net.setConnectionRange(1,0);
   // net.setGainRange(0,1);
    const int o1 = net.addOutputNeuron  ("Out");
    const int bi = net.addBiasNeuron    ("Bias");
    QList<int> il, hl;
    for (int i=0;i<pow; ++i) il << net.addInputNeuron (QString("I%1").arg(i));
    for (int i=0;i<2*pow; ++i) hl << net.addHiddenNeuron(QString("H%1").arg(i));

    // Bias to hidden and out
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
        const int p = (++c)%pmax;
        if (p==0) err = 0;
        const int bits = bitsSet(p);

        for (int i=0; i<pow; ++i) if (bitSet(p,i)) net.setInput(il[i],1); else net.setInput(il[i],0);

        const bool even = (bits%2)==0;
        const double trg = even ? 0.0 : 1.0;
        net.setTarget(o1,trg);

        net.reset();


        const double pout = net.idx2n(o1)->out(), pdlt = qAbs(pout-trg), perr = net.idx2n(o1)->perr();
        const bool success = pdlt<0.5;
       // qWarning("%s %5d #bits: %2d %s Output %lf Target %lf", success ? "OK ":"NOK", p, bits, even ?"EVEN":"ODD", pout, trg);
       // std::exit(1);
        err+=perr;
        if (!success) ++failcount;
//if (c==100) std::exit(1);
        if (p==plast)
        {
            if (failcount==0) done = true;
            if (done || (et.elapsed()>200))
            {
                et.restart();
                qWarning("Cycle %d failures %d Error %lf Last %lf %s", c/pmax, failcount, err, lasterr, err<lasterr ? "lower":"higher");
            }
            if (done) qWarning("Done");

            lasterr = err;
            failcount = 0;
            //if (c>1000) std::exit(1);
        }


        const bool endOfCycle = (p==plast);
        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d ms", (int)t.elapsed());
    std::exit(0);
}

void loadTest()
{
    SScHighwayNetwork net;
    net.load("test.net");
    net.dump();
    net.save("test2.net");


    std::exit(0);
}
void carryTest(int pow)
{
    const int pmax = qPow(2,pow), plast = pmax-1;
    SScHighwayNetwork net;
    net.setTrainingType(SScTrainableParameter::ADAM);
    net.setHiddenActivationType(SScActivation::MHAT);
    net.setOutputActivationType(SScActivation::MHAT);
    // net.setConnectionRange(1,0);
    // net.setGainRange(0,1);
    const int o1 = net.addOutputNeuron  ("Out");
    const int bi = net.addBiasNeuron    ("Bias");
    const int cn = net.addCarryNeuron   ("Carry");

    QList<int> il, hl;
    for (int i=0;i<pow; ++i) il << net.addInputNeuron (QString("I%1").arg(i));
    for (int i=0;i<pow; ++i) hl << net.addHiddenNeuron(QString("H%1").arg(i));
    for (int i=0;i<pow; ++i) net.setHighway(hl[i],il[i],cn);

    // Bias to hidden and out
    net.connect(bi,o1);
    net.connect(bi,cn);

    foreach(int to, hl) net.connect(bi,to);

    // Input to hidden, out and carry
    foreach(int from, il) foreach(int to, hl) net.connect(from,to);
    foreach(int from, il) { net.connect(from,o1); net.connect(from,cn); }

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
        const int p = (++c)%pmax;
        if (p==0) err = 0;
        const int bits = bitsSet(p);

        for (int i=0; i<pow; ++i) if (bitSet(p,i)) net.setInput(il[i],1); else net.setInput(il[i],0);

        const bool even = (bits%2)==0;
        const double trg = even ? 0.0 : 1.0;
        net.setTarget(o1,trg);

        net.reset();


        const double pout = net.idx2n(o1)->out(), pdlt = qAbs(pout-trg), perr = net.idx2n(o1)->perr();
        const bool success = pdlt<0.5;
       // qWarning("%s %5d #bits: %2d %s Output %lf Target %lf", success ? "OK ":"NOK", p, bits, even ?"EVEN":"ODD", pout, trg);
       // std::exit(1);
        err+=perr;
        if (!success) ++failcount;
//if (c==100) std::exit(1);
        if (p==plast)
        {
            if (failcount==0) done = true;
            if (done || (et.elapsed()>200))
            {
                et.restart();
                qWarning("Cycle %d failures %d Error %lf Last %lf %s", c/pmax, failcount, err, lasterr, err<lasterr ? "lower":"higher");
            }
            if (done) qWarning("Done");

            lasterr = err;
            failcount = 0;
            //if (c>1000) std::exit(1);
        }


        const bool endOfCycle = (p==plast);
        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d ms", (int)t.elapsed());
   net.save("test.net");
   loadTest();
   std::exit(0);
}

#include "filter/filter.hpp"
//#include "rnn/rneuron.hpp"

void poolTest()
{
    SScHighwayNetwork net;
    net.setTrainingType(SScTrainableParameter::ADAM);
    net.setHiddenActivationType(SScActivation::RBF);
    net.setOutputActivationType(SScActivation::IDENTITY);

    net.setConnectionRange(0,2);
    net.setGainRange(1,0);
    const int i1 = net.addInputNeuron   ("I1"),
              i2 = net.addInputNeuron   ("I2"),
              o1 = net.addOutputNeuron  ("Out"),
              pn = net.addMaxPoolNeuron ("MaxPool");

    net.connect(i1,pn);
    net.connect(i2,pn);
    net.connect(pn,o1);

    // training preparation
    net.connectForward();

    // training
    QElapsedTimer t, et; t.start(); et.start();
    int c=0, failcount=0;
    double err = 0, lasterr = 0, in1 = 0.0, in2 = 1-in1;
    bool done = false;
    do
    {
        in1+=.1;
        if (in1>1.0)
        {
            in1=0.0;
            in2+=0.1;
            if (in2>1) in2 =0;
        }
        ++c;
        const bool endOfCycle = (in1==0.0) && (in2==0);

        const double trg = qMax(in1,in2);
        net.setInput(i1,in1);
        net.setInput(i2,in2);
        net.setTarget(o1,trg);

        net.reset();


        const double pout = net.idx2n(o1)->out(), perr = net.idx2n(o1)->perr();
        const bool success = qRound(qMax(10*in1,10*in2))==qRound(10.0*pout);
        if (!success) ++failcount;
        //if (!success) qWarning("Cycle %d In1 %d In2 %d PN %lf Out %lf %s", c, qRound(10.0*in1), qRound(10.0*in2), 10.0*net.idx2n(pn)->out(), 10.0*pout, success ? "SUCCESS":"FAILURE");

        //if (c==200) std::exit(0);
        err+=perr;

        if (endOfCycle)
        {
            if (failcount==0) done = true;
            if (done || (et.elapsed()>20))
            {
                et.restart();
            }
            qWarning("End of cycle %d - failures %d error %lf last %lf %s", c, failcount, err, lasterr, err<lasterr ? "lower":"higher");
            if (done) qWarning("Done");

            lasterr = err;
            err=0;
            failcount=0;
            //if (c>1000) std::exit(1);
        }

        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d ms", (int)t.elapsed());
   std::exit(0);

}
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    poolTest();
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
