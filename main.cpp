#include <QApplication>
#include <QElapsedTimer>
#include "highwaynn//network.hpp"
#include "image/image.hpp"
#include "filter/selector.hpp"
#include "cam/cam.hpp"
#include "cam/camwidget.hpp"
#include "cam/frameintervaldescriptor.hpp"
#include "image/moments.hpp"
#include "convunit.hpp"
#include <QTimer>
#include "image/convimageconverter.hpp"
#include "blasvector.hpp"

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
    net.setTrainingType(SScTrainableParameter::ADAMCORR);
    net.setHiddenActivationType(SScActivation::MHAT);
    net.setOutputActivationType(SScActivation::LOGISTIC);
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
    net.setOutputActivationType(SScActivation::LOGISTIC);
    // net.setConnectionRange(1,0);
    // net.setGainRange(0,1);
    const int o1 = net.addOutputNeuron  ("Out");
    const int bi = net.addBiasNeuron    ("Bias");
    const int cn = net.addCarryNeuron   ("Carry");

    QList<int> il, hl;
    for (int i=0;i<pow; ++i) il << net.addInputNeuron (QString("I%1").arg(i));
    for (int i=0;i<pow; ++i) hl << net.addHiddenNeuron(QString("H%1").arg(i));

    net.setHighway(o1,il.first(),cn);

    // Bias to carry and out
    net.connect(bi,o1);
    net.connect(bi,cn);

    foreach(int to, hl) net.connect(bi,to);

    // Input to hidden, out and carry
    foreach(int from, il) foreach(int to, hl) net.connect(from,to);
    foreach(int from, il) { net.connect(from,o1); net.connect(from,cn); }

    // Hidden to output
    foreach(int from, hl) net.connect(from,o1);

    // training
    QElapsedTimer t, et; t.start(); et.start();
    int c=0, failcount=0;
    double err = 0, lasterr = 0;
    bool done = false, tlocked = false;
    do
    {
        const int p = (++c)%pmax;
        if (p==0) err = 0;
        if (c==1000)
        {
            foreach(int n, hl) net.lockTraining(n,true);
            net.lockTraining(o1,true);
            tlocked = true;
        }
        const int bits = bitsSet(p);


        for (int i=0; i<pow; ++i) if (bitSet(p,i)) net.setInput(il[i],1); else net.setInput(il[i],0);

        const bool even = (bits%2)==0; bool fake = false;
        double trg = even ? 0.0 : 1.0;
        if (bitSet(p,2)) { trg = bitSet(p,0); fake = true; }
        net.setTarget(o1,trg);

        net.reset();


        const double i0 = net.idx2n(il.first())->out(), pout = net.idx2n(o1)->out(), pdlt = qAbs(pout-trg), perr = net.idx2n(o1)->perr(), carryo = net.idx2n(cn)->out();
        const bool success = pdlt<0.5;
       // Carry output should become pretty high at all lines marked with +
        if (fake) qWarning("%s + %s %5d #bits: %2d %s Output %lf Target %lf I0 %lf Carry %lf", tlocked ? "L":" ", success ? "OK ":"NOK", p, bits, even ?"EVEN":"ODD", pout, trg, i0, carryo);
        else      qWarning("%s  %s %5d #bits: %2d %s Output %lf Target %lf I0 %lf Carry %lf", tlocked ? "L":" ", success ? "OK ":"NOK", p, bits, even ?"EVEN":"ODD", pout, trg, i0, carryo);

        // std::ex it(1);
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
    net.setTrainingType(SScTrainableParameter::AMSGRAD);
    net.setHiddenActivationType(SScActivation::MHAT);
    net.setOutputActivationType(SScActivation::SWISH);

    net.setConnectionRange(0,2);
    net.setGainRange(1,0);
    const int i1 = net.addInputNeuron   ("I1"),
              i2 = net.addInputNeuron   ("I2"),
              i3 = net.addInputNeuron   ("I3"),
              bi = net.addBiasNeuron    ("B"),
              o1 = net.addOutputNeuron  ("Out"),
              min= net.addMinPoolNeuron ("MinPool");

    //net.idx2n(o1)->setLock(true);
    QList<int> hl0;
    for (int i=0; i<30; ++i)
        hl0 << net.addHiddenNeuron(QString("H_0_%1").arg(i));

    // Bias and input to hidden
    foreach(int idx, QList<int>() << bi << i1 << i2 << i3)
        foreach(int toidx, hl0) net.connect(idx,toidx);
    foreach(int toidx, hl0) net.connect(bi,toidx);
    // Hidden to pooling
    foreach(int idx, hl0) net.connect(idx,min);
    // pool and bias to output
    foreach(int idx, QList<int>() << min << bi) net.connect(idx,o1);

    // training
    QElapsedTimer t, et; t.start(); et.start();
    int c=0;
    double err = 0, lasterr = 0;
    bool done = false, doLogging = false;
    int failcount = 11000;
    do
    {
       // if (c==10) std::exit(0);
        const double in1   = (double)rand()/RAND_MAX,
                     in2   = (double)rand()/RAND_MAX,
                     in3   = (double)rand()/RAND_MAX,
                     inmin = qMin(qMin(in1,in2),in3),
                     inmax = qMax(qMax(in1,in2),in3),
                     trg   = inmin;

        ++c;
        const bool endOfCycle = (c%10000)==0;
        net.setInput(i1,in1);
        net.setInput(i2,in2);
        net.setInput(i3,in3);
        net.setTarget(o1,trg);
        net.reset();

        const double minout = net.idx2n(min)->out(),
                     pout   = net.idx2n(o1) ->out(),
                     perr   = net.idx2n(o1)->perr();

        if (doLogging)
            qWarning("Cycle %d: %.2lf %.2lf %.2lf Trg %lf MinOut %lf Out %lf - err %lf", c, in1, in2, in3, trg, minout, pout, perr);
        //if (c==10) std::exit(0);
        err+=perr;
        if (perr>.05) ++failcount;
        if (endOfCycle)
        {
            err/=10000;
            if (failcount==0) done = true;
            if (done || (et.elapsed()>200))
            {
                const bool isLocked = net.idx2n(o1)->conLock();
                et.restart();
                qWarning("End of cycle %d / fail %d- error %lf %s %s",
                         c, failcount, err, err<lasterr ? "lower":"higher", isLocked ? "L":"");
                if (failcount<100)
                {
                    net.idx2n(o1)->setLock(true);
                }
                if (failcount<200) doLogging=true;
            }
            if (done) qWarning("Done");
            failcount = 0;
            lasterr = err;
            err=0;
            //if (c>1000) std::exit(1);
        }

        net.trainingStep(endOfCycle);
    }
    while (!done);
    qWarning("Training took %d ms", (int)t.elapsed());
    net.save("poolTest.net");
    std::exit(0);
}
#include "conv.hpp"
void convTest()
{
    SScHighwayNetwork net;
    SScInputConvUnit cu(&net,3,3,5,5,1);
    cu.addPattern("/home/developer/1.png");
    cu.addPattern("/home/developer/2.jpg");
    int c = 0;
    cu.resetTraining();
    while (true)
    {
        bool cdone;
        const QString pid = cu.nextPattern(cdone);

        qWarning(">>>>PATTERN %s %s", qPrintable(pid), cdone ? "END":"");

        for (int i=0; i<cu.units(); ++i) qWarning("Unit %d: %lf", i, cu.output(0,i)->out());
        if (++c>5) std::exit(1);
    }
}

void convImageTest()
{
    SScConvImageConverter imcv(100,100,50,2,2,false,"/home/developer/2.jpg");

    imcv.cut(0,0).save("/home/developer/00.jpg");
    imcv.cut(1,0).save("/home/developer/10.jpg");
    imcv.cut(0,1).save("/home/developer/01.jpg");
    imcv.cut(1,1).save("/home/developer/11.jpg");
}

#include "convhelpers.hpp"


void convHelperTest()
{


    const auto f = SSnConvHelper::fitToInput(QSize(5,5),QSize(3,3), QSize(1,1));
    qWarning(">>>>>%d %d",f.width(),f.height());
   std::exit(0);
    QImage im("/home/developer/1.png");
    int idx=-1;
    //foreach(const auto& i, SSnConvHelper::images(im,QSize(128,128),QSize(64,64),QSize(3,3))) i.save(QString("/home/developer/test%1.png").arg(++idx));
    const auto ks = QSize(128,128);
    foreach(auto line, SSnConvHelper::matrix(im,ks,ks,QSize(3,3)))
    {
        QImage im2(ks,QImage::Format_RGB32);
        im2.setColorTable(im.colorTable());

        for (int j=0; j<im2.height(); ++j) for (int i=0; i<im2.width(); ++i)
        {
            im2.setPixel(QPoint(i,j),line[(j*ks.height())+i]);
        }
        im2.save(QString("/home/developer/TEST%1.png").arg(++idx));
    }

}
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    convHelperTest();
    std::exit(0);
    QVector<double> a, b;
    a << 1 << 2 << 3 <<
         4 << 5 << 6 <<
         7 <<8 << 9 <<
         10 << 11 << 12;
    b << 1 << 1 << 1;
    auto c = SSnBlas::mxv(a,b);
    for (int i=0; i<c.size(); ++i) qWarning(">>>>#%d: %lf", i, c[i]);
    std::exit(0);

    convImageTest();

/*
    auto aaa = SSnConvHelpers::inputSize(3,4,2,2,1);
    auto bbb = SSnConvHelpers::convMaskFits(3,3,1,9,9);
    qWarning(">>>>%dx%d",bbb.width(),bbb.height());
    auto ccc = SSnConvHelpers::convMaskIndexes(3,3,1,1,0,5,3);
    foreach(auto i, ccc) qWarning("#%d",i);
    std::exit(0);*/

    convTest();
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
