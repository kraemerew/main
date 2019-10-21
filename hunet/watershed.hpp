#ifndef WATERSHED_HPP
#define WATERSHED_HPP

#include <opencv2/core.hpp>

namespace SSnWatershed
{
    class Pars
    {
    public:
        explicit Pars(double median = 1.0, double close = 1.0, int binthr = 40, bool inv = false, double dthr = .40);
        inline int close(int w, int h)  const { return oddKernel(w,h,m_close); }
        inline int median(int w, int h) const { return oddKernel(w,h,m_median); }
        inline bool inv() const { return m_inv; }
        inline int thr() const { return m_binthr; }
        inline double dthr() const { return m_dthr; }
    private:
        int oddKernel(int w, int h, double perc) const;
        double diag(int w, int h) const;

        double  m_median, m_close, m_dthr;
        int     m_binthr;
        bool    m_inv;
    };
    void execute(cv::Mat& src, cv::Mat& trg, const Pars& p = Pars());
}
#endif // WATERSHED_HPP
