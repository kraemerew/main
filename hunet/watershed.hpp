#ifndef WATERSHED_HPP
#define WATERSHED_HPP

#include <opencv2/core.hpp>
#include <QList>
#include "contour.hpp"
#include "sscvm.hpp"

namespace SSnWatershed
{
    class Pars : public SScVM
    {
    public:
        explicit Pars(const SScVM& init = SScVM()) : SScVM(init) {}
        explicit Pars(double median = 1.0, double close = 1.0, int binthr = 40, bool inv = false, double dthr = .30);
        inline int close(int w, int h)  const { return oddKernel(w,h,close()); }
        inline int median(int w, int h) const { return oddKernel(w,h,median()); }

        int     close   () const;
        int     median  () const;
        bool    inv     () const;
        int     thr     () const;
        double  dthr    () const;

    private:
        int oddKernel(int w, int h, double perc) const;
        double diag(int w, int h) const;
    };

    QList<SScContour> execute(cv::Mat& src, const Pars& p);

    QList<SScContour> execute(cv::Mat& src, const Pars& p, cv::Mat& bw, cv::Mat& dist, cv::Mat& result);
}
#endif // WATERSHED_HPP
