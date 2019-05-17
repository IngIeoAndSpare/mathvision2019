#ifndef _POLYGON_DEMO_H_
#define _POLYGON_DEMO_H_

#include "opencv2/highgui.hpp"

struct PolygonDemoParam
{
    bool compute_area;
    bool draw_line;
    bool check_ptInPoly;
    bool check_homography;
    bool fit_line;
    bool fit_circle;
    bool fit_ellipse;
	bool draw_Straight;
	bool draw_SVD_line;
	bool draw_cauch_line;
	bool draw_ransac_line;

    PolygonDemoParam()
    {
        compute_area = false;
        draw_line = false;
        check_ptInPoly = false;
        check_homography = false;
        fit_line = false;
		fit_circle = false;
        fit_ellipse = false;
		draw_Straight = false;
		draw_SVD_line = false;
		draw_cauch_line = false;
		draw_ransac_line = true;
    }
};

class PolygonDemo
{
public:
    PolygonDemo();
    ~PolygonDemo();

    void refreshWindow();
    void handleMouseEvent(int evt, int x, int y, int flags);
    void drawPolygon(cv::Mat& frame, const std::vector<cv::Point>& vtx, bool closed);
    void setParam(const PolygonDemoParam& param) { m_param = param; }
    PolygonDemoParam getParam() { return m_param; }

    bool ptInPolygon(const std::vector<cv::Point>& vtx, cv::Point pt);
    int polyArea(const std::vector<cv::Point>& vtx);
    enum {NORMAL, CONCAVE, TWIST, REFLECTION, CONCAVE_REFLECTION};
    int classifyHomography(const std::vector<cv::Point>& pts1, const std::vector<cv::Point>& pts2);

    bool fitLine(const std::vector<cv::Point>& pts, cv::Point2d& center, double& radius);
    bool fitCircle(const std::vector<cv::Point>& pts, cv::Point2d& center, double& radius);
	bool fitEllipse(const std::vector<cv::Point>& pts, cv::Point2d& centerPoint, cv::Point2d& axesPoint, float& theta);

	bool drawLineStraight(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize);
	bool drawLineSVD(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize);

	bool drawLineCauchy(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize, bool flag, cv::Mat& residualMat);
	bool drawLineRansac(
		const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint,
		int colSize, float threadHoldNum, int& thUnderCount, int iterationNum,
		cv::Mat& bestConstantMat, cv::Mat& inilerMat);

protected:
    bool m_data_ready;
    PolygonDemoParam m_param;
    std::vector<cv::Point> m_data_pts;
    std::vector<cv::Point> m_test_pts;
};

#endif  //  _POLYGON_DEMO_H_