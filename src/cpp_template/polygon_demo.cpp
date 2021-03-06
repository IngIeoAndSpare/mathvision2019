#include "polygon_demo.hpp"
#include "opencv2/imgproc.hpp"
#include "iostream"
#include "stdlib.h"

using namespace std;
using namespace cv;

PolygonDemo::PolygonDemo()
{
    m_data_ready = false;
}

PolygonDemo::~PolygonDemo()
{
}

void PolygonDemo::refreshWindow()
{
    Mat frame = Mat::zeros(480, 640, CV_8UC3);
    if (!m_data_ready)
        putText(frame, "Input data points (double click: finish)", Point(10, 470), FONT_HERSHEY_SIMPLEX, .5, Scalar(0, 148, 0), 1);

    drawPolygon(frame, m_data_pts, m_data_ready);
    if (m_data_ready)
    {
        // polygon area
        if (m_param.compute_area)
        {
            int area = polyArea(m_data_pts);
            char str[100];
            sprintf_s(str, 100, "Area = %d", area);
            putText(frame, str, Point(25, 25), FONT_HERSHEY_SIMPLEX, .8, Scalar(0, 255, 255), 1);
        }

        // pt in polygon
        if (m_param.check_ptInPoly)
        {
            for (int i = 0; i < (int)m_test_pts.size(); i++)
            {
                if (ptInPolygon(m_data_pts, m_test_pts[i]))
                {
                    circle(frame, m_test_pts[i], 2, Scalar(0, 255, 0), CV_FILLED);
                }
                else
                {
                    circle(frame, m_test_pts[i], 2, Scalar(128, 128, 128), CV_FILLED);
                }
            }
        }

        // homography check
        if (m_param.check_homography && m_data_pts.size() == 4)
        {
            // rect points
            int rect_sz = 100;
            vector<Point> rc_pts;
            rc_pts.push_back(Point(0, 0));
            rc_pts.push_back(Point(0, rect_sz));
            rc_pts.push_back(Point(rect_sz, rect_sz));
            rc_pts.push_back(Point(rect_sz, 0));
            rectangle(frame, Rect(0, 0, rect_sz, rect_sz), Scalar(255, 255, 255), 1);

            // draw mapping
            char* abcd[4] = { "A", "B", "C", "D" };
            for (int i = 0; i < 4; i++)
            {
                line(frame, rc_pts[i], m_data_pts[i], Scalar(255, 0, 0), 1);
                circle(frame, rc_pts[i], 2, Scalar(0, 255, 0), CV_FILLED);
                circle(frame, m_data_pts[i], 2, Scalar(0, 255, 0), CV_FILLED);
                putText(frame, abcd[i], m_data_pts[i], FONT_HERSHEY_SIMPLEX, .8, Scalar(0, 255, 255), 1);
            }

            // check homography
            int homo_type = classifyHomography(rc_pts, m_data_pts);
            char type_str[100];
            switch (homo_type)
            {
            case NORMAL:
                sprintf_s(type_str, 100, "normal");
                break;
            case CONCAVE:
                sprintf_s(type_str, 100, "concave");
                break;
            case TWIST:
                sprintf_s(type_str, 100, "twist");
                break;
            case REFLECTION:
                sprintf_s(type_str, 100, "reflection");
                break;
            case CONCAVE_REFLECTION:
                sprintf_s(type_str, 100, "concave reflection");
               break;
            }

            putText(frame, type_str, Point(15, 125), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 1);
        }

        // fit circle
        if (m_param.fit_circle)
        {
            Point2d center;
            double radius = 0;
            bool ok = fitCircle(m_data_pts, center, radius);
            if (ok)
            {
                circle(frame, center, (int)(radius + 0.5), Scalar(0, 255, 0), 1);
                circle(frame, center, 2, Scalar(0, 255, 0), CV_FILLED);
            }
        }
		// fit ellpse
		if (m_param.fit_ellipse) {
			Point2d centerPoint, axesPoint;
			float theta;
			bool ellipseResult = fitEllipse(m_data_pts, centerPoint, axesPoint, theta);
			if (ellipseResult) {
				Size ellipseSize = Size(axesPoint.x, axesPoint.y);
				Scalar ellipseColor = Scalar(0, 255, 204);
				ellipse(frame, centerPoint, ellipseSize, theta, 0, 360, ellipseColor, 2);
			}
		}

		// draw line use Straight
		if (m_param.draw_Straight) {
			Point2d stPoint, edPoint;
			bool drawOk = drawLineStraight(m_data_pts, stPoint, edPoint, frame.cols);
			if (drawOk) {
				line(frame, stPoint, edPoint, Scalar(0, 255, 0), 1);
				string type_str = "y=ax+b";
				putText(frame, type_str, Point(15, 35), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 1);
				if (m_param.draw_SVD_line) {
					drawLineSVD(m_data_pts, stPoint, edPoint, frame.cols);
					type_str = "ax+by+c = 0";
					putText(frame, type_str, Point(15, 75), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 1);
					line(frame, stPoint, edPoint, Scalar(0, 0, 255), 1);
				}
			}
		} else if (m_param.draw_SVD_line) {
			Point2d stPoint, edPoint;
			bool drawOk = drawLineSVD(m_data_pts, stPoint, edPoint, frame.cols);
			if (drawOk) {
				string type_str = "ax+by+c = 0";
				putText(frame, type_str, Point(15, 125), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 1);
				line(frame, stPoint, edPoint, Scalar(0, 0, 255), 1);
			}
		}
		else if (m_param.draw_cauch_line) {
			Point2d stPoint, edPoint;
			Mat residualMat = Mat(m_data_pts.size(), 1, CV_32FC1, float(0));
			int offset = 0, loopCount = 10;
			string type_str = "y=ax+b";
			putText(frame, type_str, Point(15, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1);
			// draw Straight line
			bool drawOk = drawLineStraight(m_data_pts, stPoint, edPoint, frame.cols);
			if (drawOk) {
				line(frame, stPoint, edPoint, Scalar(255, 0, 0), 2);
			}
			// draw cauch line
			type_str = "Robust LS: y =ax + b";
			putText(frame, type_str, Point(15, 55), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 1);
			for (int workNum = 0; workNum < loopCount; workNum++) {
				offset++;
				drawOk = drawLineCauchy(m_data_pts, stPoint, edPoint, frame.cols, true, residualMat);
				if (drawOk) {
					residualMat = abs(residualMat);
					line(
						frame,
						stPoint,
						edPoint,
						(offset != loopCount ? Scalar( 0, 0, 255) : Scalar(0, 255, 0)),
						(offset != loopCount ? 1 : 2)
					);
				}
			}// for end
		}
		else if (m_param.draw_ransac_line) {
			Point2d stPoint, edPoint;
			float thresHoldNum = 300;
			int thUnderCountMax = 0;
			Mat bestConstantMat, inilerMat;
			//ref : https://en.wikipedia.org/wiki/Random_sample_consensus parameters
			//w값 정의는 어떻게 내리면 효과적?
			int iterationNum = round(
				(log(1 - 0.99) / log(1 - pow(0.6, 2))) + (sqrt(1 - pow(0.6, 2)) / pow(0.6, 2))
			);
			// bool inlierCheck[m_data_pts.size()] 이게 왜 안먹히지???
			drawLineRansac(m_data_pts, stPoint, edPoint, frame.cols, thresHoldNum, thUnderCountMax, iterationNum, bestConstantMat, inilerMat);

			line(frame, stPoint, edPoint, Scalar(0, 255, 0), 2);
			string type_str = "ax+by+c = 0";
			putText(frame, type_str, Point(15, 35), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 1);

			for (int itemCount = 0; itemCount < m_data_pts.size(); itemCount++) {
				(
					inilerMat.at<float>(itemCount, 0) == 1 ?
					circle(frame, m_data_pts[itemCount], 5, Scalar(0, 255, 255), CV_FILLED) : ""
				);
			}
		}

		if (m_param.draw_gauss_newton_line) {
			// y = a*sin(bx+c)+d
			// ref : https://stackoverflow.com/questions/10209935/drawing-sine-wave-using-opencv
			const int ITEM_COUNT = 4;
			const int ITORATION = 100;
			int pointSize = m_data_pts.size();
			Mat bestConstantMat = Mat(ITEM_COUNT, 1, CV_32FC1);

			int totalValue = 0;
			float average, standardDeviation = 0;
			// init constant
			for (int item = 0; item < pointSize; item++) {
				totalValue += m_data_pts[item].y;
			}
			average = totalValue / pointSize;
			// meanStdDev() 가 있긴 한데... https://docs.opencv.org/2.4/modules/core/doc/operations_on_arrays.html
			for (int item = 0; item < pointSize; item++) {
				standardDeviation += pow(m_data_pts[item].y - average, 2) / (pointSize - 1);
			}
			standardDeviation = sqrt(standardDeviation);
			bestConstantMat.at<float>(0, 0) = standardDeviation;
			bestConstantMat.at<float>(1, 0) = 1;
			bestConstantMat.at<float>(2, 0) = 1;
			bestConstantMat.at<float>(3, 0) = average;

			for (int count = 0; count < ITORATION; count++) {
				if (drawLineGaussNewton(m_data_pts, bestConstantMat, ITEM_COUNT)) {
					//TODO : draw graph
					float test = bestConstantMat.at<float>(0, 0);
					float test1 = bestConstantMat.at<float>(1, 0);
					float test2 = bestConstantMat.at<float>(2, 0);
					float test3 = bestConstantMat.at<float>(3, 0);
				}
			}
		}
    }

    imshow("PolygonDemo", frame);
}

// return the area of polygon
int PolygonDemo::polyArea(const std::vector<cv::Point>& vtx)
{
	//XXX : vector 자료 형 => https://blockdmask.tistory.com/70
	
	//get vector array length, first point info
	int pointLength = vtx.size();
	Point firstPoint = vtx.front();

	int areaSize = 0;
	
	// ref : https://darkpgmr.tistory.com/86 - 8
	for (int point = 1; point < pointLength - 1; point ++) {
		areaSize += ((vtx[point].x - firstPoint.x) * (vtx[point + 1].y - firstPoint.y) -
			(vtx[point + 1].x - firstPoint.x) * (vtx[point].y - firstPoint.y)) / 2;
	}


	
	// ref : https://darkpgmr.tistory.com/86 - 9
	/*
	int checkAreaSize = 0;
	//copy calculate vector array
	vector<cv::Point> calculateTarget = vtx;
	calculateTarget.push_back(vtx.front());

	pointLength = calculateTarget.size();
	
	for (int point = 0; point < pointLength - 1; point++) {
		checkAreaSize += (calculateTarget[point].x + calculateTarget[point + 1].x) *
			(calculateTarget[point].y - calculateTarget[point + 1].y);
	}
	*/
	/*
	cout << "\npoint : " << pointLength;
	cout << "\nfirst point : " << firstPoint.x << "  y :" << firstPoint.y;
	cout << "\nthred Point : " << vtx[2].x << "   y: " << vtx[2].y;
	cout << "\nget space : " << abs((firstPoint.x - vtx[2].x) * (firstPoint.y - vtx[2].y));
	cout << "\nget space / 2 : " << abs((firstPoint.x - vtx[2].x) * (firstPoint.y - vtx[2].y)) / 2;
	cout << "\n twist space : " << abs((abs(firstPoint.x) - abs(vtx[3].x)) * (abs(firstPoint.y) - abs(vtx[3].y))) / 2;
	*/
    return abs(areaSize);
}



// return true if pt is interior point
bool PolygonDemo::ptInPolygon(const std::vector<cv::Point>& vtx, Point pt)
{
    return true;
}

// return homography type: NORMAL, CONCAVE, TWIST, REFLECTION, CONCAVE_REFLECTION
int PolygonDemo::classifyHomography(const std::vector<cv::Point>& pts1, const std::vector<cv::Point>& pts2)
{
	//XXX : pts1 = H에 변환된 네 꼭지점 영상좌표, pts2 = 영상 좌표

	if (pts1.size() != 4 || pts2.size() != 4) {
		return -1;
	}
	// get vector info
	int vectorSize = pts1.size();
	//XXX: normalCount는 확인용으로 선언한 변수임.
	int oppositeCount = 0, normalCount = 0;

	for (int pointNum = 0; pointNum < vectorSize; pointNum++) {
		// get cross product point
		Point backPoint = pts2[(pointNum < 1 ? vectorSize - 1 : pointNum - 1)];
		Point frontPoint = pts2[(pointNum == (vectorSize - 1) ? 0 : pointNum + 1)];
		Point centerPoint = pts2[pointNum];
		//check sign cross product value
		(
			(backPoint.x - centerPoint.x) * (frontPoint.y - centerPoint.y) -
			(frontPoint.x - centerPoint.x) * (backPoint.y - centerPoint.y) > 0 ? normalCount++ : oppositeCount++
		);
	} // for end
	switch (oppositeCount)
	{
	case 0 :
		return NORMAL;
	case 1 :
		return CONCAVE;
	case 2:
		return TWIST;
	case 3 :
		return CONCAVE_REFLECTION;
	case 4 :
		return REFLECTION;
	default:
		break;
		//Exception
		//throw ExceptionMessage()....
	}
}

// estimate a circle that best approximates the input points and return center and radius of the estimate circle
bool PolygonDemo::fitCircle(const std::vector<cv::Point>& pts, cv::Point2d& center, double& radius)
{
    int pointSize = (int)pts.size();
	if (pointSize < 3) {
		return false;
	}

	// ref : https://stackoverflow.com/questions/19083775/convert-vector-of-points-to-mat-opencv 
	// ref : http://younanote.blogspot.com/2015/08/opencv-mat.html
	// set matrix. CV_32FC1 => floot, CV_64FC3 => double
	//Mat pointMat = Mat(pts, false);
	Mat resultMat = Mat(pointSize, 1, CV_32FC1);
	Mat pointMat = Mat(pointSize, 3, CV_32FC1);
	for (int pointNum = 0; pointNum < pointSize; pointNum++) {
		pointMat.at<float>(pointNum, 0) = pts[pointNum].x;
		pointMat.at<float>(pointNum, 1) = pts[pointNum].y;
		pointMat.at<float>(pointNum, 2) = 1;
		resultMat.at<float>(pointNum, 0) = -(pow(pts[pointNum].x, 2) + pow(pts[pointNum].y, 2));
	}
	
	// ref : http://answers.opencv.org/question/94555/mat-transpose-t-a-nx2-mat-gives-me-a-1x2n-output/
	// ref : http://blog.naver.com/PostView.nhn?blogId=windowsub0406&logNo=220515761001
	// get transpose matrix
	Mat transPointMat = pointMat.t();
	// (A^t * A)^-1
	Mat multiTransAndOri = transPointMat * pointMat;
	Mat pseudoInvMat = multiTransAndOri.inv() * transPointMat;
	// result a, b, c
	Mat result = pseudoInvMat * resultMat;
	
	center.x = - (result.at<float>(0, 0)) / 2;
	center.y = - (result.at<float>(1, 0)) / 2;
	radius =  sqrt(pow(result.at<float>(0, 0), 2) + pow(result.at<float>(1, 0), 2) - result.at<float>(2, 0) * 4) / 2;

	// check
	cout << "====== PointMat X {a, b, c} =====\n";
	cout << pointMat * result;
	cout << "\n====== result Mat =====\n";
	cout << resultMat;

    return true;
}

bool PolygonDemo::fitEllipse(const std::vector<cv::Point>& pts, cv::Point2d& centerPoint, cv::Point2d& axesPoint, float& theta) {
	int pointSize = (int)pts.size();
	const int ARRAY_SIZE = 6;

	if (pointSize <= 5) {
		return false;
	}
	// axesPoint, centerPoint, theta 값을 구하고 return;
	
	Mat pointMat = Mat(pointSize, ARRAY_SIZE, CV_32FC1);
	Mat resultMat = Mat(pointSize, 1, CV_32FC1);

	// T => transposed.
	Mat svdResultSingular, svdResultLeftSingular, svdResultRightSingularT;
	// set point mat ref : 컴퓨터비전 ch3 p.13
	for (int pointNum = 0; pointNum < pointSize; pointNum++) {
		pointMat.at<float>(pointNum, 0) = pow(pts[pointNum].x, 2);
		pointMat.at<float>(pointNum, 1) = pts[pointNum].x * pts[pointNum].y;
		pointMat.at<float>(pointNum, 2) = pow(pts[pointNum].y, 2);
		pointMat.at<float>(pointNum, 3) = pts[pointNum].x;
		pointMat.at<float>(pointNum, 4) = pts[pointNum].y;
		pointMat.at<float>(pointNum, 5) = 1;
	}
	// ref : https://docs.opencv.org/3.4.2/df/df7/classcv_1_1SVD.html 중 compute();
	// ref : https://docs.opencv.org/3.4.2/df/df7/classcv_1_1SVD.html#a4700f5207e66cdd9924bf64e34911832 flag
	SVD::compute(pointMat, svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, SVD::FULL_UV);
	SVD::compute(pointMat, svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, SVD::FULL_UV);
	Mat convertSvdResultRMat = svdResultRightSingularT.t();

	float resultArray[ARRAY_SIZE] = {};
	for (int pointNum = 0; pointNum < ARRAY_SIZE; pointNum++) {
		//XXX: 확인용 mat
		resultMat.at<float>(pointNum, 0) = convertSvdResultRMat.at<float>(pointNum, ARRAY_SIZE - 1);
		//실제 계산 a~f -> 0 ~ 5
		resultArray[pointNum] = convertSvdResultRMat.at<float>(pointNum, ARRAY_SIZE - 1);
	}

	theta = atan(resultArray[1] / (resultArray[0] - resultArray[2])) / 2;

	double centerPointDenominator = pow(resultArray[1], 2) - 4 * resultArray[0] * resultArray[2];
	centerPoint.x = (2 * resultArray[2] * resultArray[3] - resultArray[1] * resultArray[4]) / centerPointDenominator;
	centerPoint.y = (2 * resultArray[0] * resultArray[4] - resultArray[1] * resultArray[3]) / centerPointDenominator;

	float axesPointmolecule = resultArray[0] * pow(centerPoint.x, 2) + resultArray[1] * centerPoint.x * centerPoint.y + resultArray[2] * pow(centerPoint.y, 2) - resultArray[5];
	float cosValue = cos(theta);
	float sinValue = sin(theta);
	float cosPow = pow(cosValue, 2);
	float sinPow = pow(sinValue, 2);
	axesPoint.x = sqrt(axesPointmolecule / (resultArray[0] * cosPow + resultArray[1] * cosValue * sinValue + resultArray[2] * sinPow));
	axesPoint.y = sqrt(axesPointmolecule / (resultArray[0] * sinPow - resultArray[1] * cosValue * sinValue + resultArray[2] * cosPow));

	theta = theta * 180 / 3.141592;

	//cout << pointMat * resultMat;

	return true;
}

bool PolygonDemo::drawLineStraight(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize)
{
	int pointCount = pts.size();
	if (pointCount < 2) {
		return false;
	}
	Mat pointMat = Mat(pointCount, 2, CV_32FC1);
	Mat resultMat = Mat(pointCount, 1, CV_32FC1);
	Mat constantMat = Mat(2, 1, CV_32FC1);
	Mat svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, pointMatInv;

	// ref : 8주차 수업자료
	for (int pointNum = 0; pointNum < pointCount; pointNum++) {
		pointMat.at<float>(pointNum, 0) = pts[pointNum].x;
		pointMat.at<float>(pointNum, 1) = 1;
		resultMat.at<float>(pointNum, 0) = pts[pointNum].y;
	}
	SVD::compute(pointMat, svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, SVD::FULL_UV);
	// ref: https://docs.opencv.org/3.4.0/d2/de8/group__core__array.html#ggaaf9ea5dcc392d5ae04eacb9920b9674ca523b676c90c7a1d2841b1267ba9ba614
	pointMatInv = pointMat.inv(1);
	constantMat = pointMatInv * resultMat;

	stPoint.x = 0;
	edPoint.x = colSize;
	stPoint.y = constantMat.at<float>(0, 0)*stPoint.x + constantMat.at<float>(1, 0);
	edPoint.y = constantMat.at<float>(0, 0)*edPoint.x + constantMat.at<float>(1, 0);
	return true;

}

bool PolygonDemo::drawLineSVD(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize) {
	
	int pointCount = pts.size();
	if (pointCount < 2) {
		return false;
	}

	Mat pointMat = Mat(pointCount, 3, CV_32FC1);
	Mat resultMat = Mat(pointCount, 1, CV_32FC1);
	Mat constantMat = Mat(3, 1, CV_32FC1);
	Mat svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, svdRMat;

	// ref : 8주차 수업자료
	for (int pointNum = 0; pointNum < pointCount; pointNum++) {
		pointMat.at<float>(pointNum, 0) = pts[pointNum].x;
		pointMat.at<float>(pointNum, 1) = pts[pointNum].y;
		pointMat.at<float>(pointNum, 2) = 1;
		resultMat.at<float>(pointNum, 0) = 0;
	}
	SVD::compute(pointMat, svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, SVD::FULL_UV);
	svdRMat = svdResultRightSingularT.t();
	int svdRmatLastColNum = svdRMat.cols - 1;
	// get left colum
	for (int rowNum = 0; rowNum < svdRMat.rows; rowNum++) {
		constantMat.at<float>(rowNum, 0) = svdRMat.at<float>(rowNum, svdRmatLastColNum);
	}
	// y = -1/b(ax+c)
	stPoint.x = 0;
	edPoint.x = colSize;
	stPoint.y = ((constantMat.at<float>(0, 0)*stPoint.x + constantMat.at<float>(2, 0)) / constantMat.at<float>(1, 0))* -1;
	edPoint.y = ((constantMat.at<float>(0, 0)*edPoint.x + constantMat.at<float>(2, 0)) / constantMat.at<float>(1, 0)) * -1;
	return true;
}

bool PolygonDemo::drawLineCauchy(const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint, int colSize, bool flag, cv::Mat& residualMat) {
	int pointCount = pts.size();
	if (pointCount < 2) {
		return false;
	}

	Mat pointMat = Mat(pointCount, 2, CV_32FC1);
	Mat resultMat = Mat(pointCount, 1, CV_32FC1);
	Mat paramMat = Mat(2, 1, CV_32FC1);
	Mat weightMat = Mat(pointCount, pointCount, CV_32FC1, float(0));
	Mat pointMatInv;
	for (int pointNum = 0; pointNum < pointCount; pointNum++) {
		pointMat.at<float>(pointNum, 0) = pts[pointNum].x;
		pointMat.at<float>(pointNum, 1) = 1;
		resultMat.at<float>(pointNum, 0) = pts[pointNum].y;
		if (flag) {
			// get cauchy weight. 
			weightMat.at<float>(pointNum, pointNum) = 1 / (residualMat.at<float>(pointNum, 0) / 1.3998 + 1);
		}
	}

	pointMatInv = pointMat.inv(1);

	if (flag) {
		// robust parameter estimation (참고 : 수업자료 38 ~ 39p)
		paramMat = (pointMat.t() * weightMat * pointMat).inv(1) * pointMat.t() * weightMat * resultMat;
	}
	else {
		paramMat = pointMatInv * resultMat;
	}

	residualMat = pointMat * paramMat - resultMat;
	cout << residualMat << endl;
	stPoint.x = 0;
	edPoint.x = colSize;
	stPoint.y = paramMat.at<float>(0, 0) * stPoint.x + paramMat.at<float>(1, 0);
	edPoint.y = paramMat.at<float>(0, 0) * edPoint.x + paramMat.at<float>(1, 0);

	return true;
}

bool PolygonDemo::drawLineRansac(
	const std::vector<cv::Point>& pts, cv::Point2d& stPoint, cv::Point2d& edPoint,
	int colSize, float thresHoldNum, int& thUnderCount, int iterationNum,
	cv::Mat& bestConstantMat, cv::Mat& inilerMat)
{
	int pointCount = pts.size();
	if (pointCount < 2) {
		return false;
	}

	Mat pointMat = Mat(pointCount, 3, CV_32FC1);

	for (int itemCount = 0; itemCount < pointCount; itemCount++) {
		pointMat.at<float>(itemCount, 0) = pts[itemCount].x;
		pointMat.at<float>(itemCount, 1) = pts[itemCount].y;
		pointMat.at<float>(itemCount, 2) = 1;
	}


	for (int countIt = 0; countIt < iterationNum; countIt++) {
		Mat rdPointMat = Mat(2, 3, CV_32FC1);
		Mat rdConstantMat = Mat(3, 1, CV_32FC1);
		Mat svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, svdRMat;
		// get random index
		int randSelectA = rand() % pointCount;
		int randSelectB = rand() % pointCount;
		if (randSelectA == randSelectB) {
			do {
				randSelectB = rand() % pointCount;
			} while (randSelectB == randSelectA);
		}

		// init random calculate mat
		for (int count = 0; count < 1; count++) {
			rdPointMat.at<float>(count, 0) = (count == 0 ? pts[randSelectA].x : pts[randSelectB].x);
			rdPointMat.at<float>(count, 1) = (count == 0 ? pts[randSelectA].y : pts[randSelectB].y);
			rdPointMat.at<float>(count, 2) = 1;
		}

		// calculate svd 
		SVD::compute(rdPointMat, svdResultSingular, svdResultLeftSingular, svdResultRightSingularT, SVD::FULL_UV);
		svdRMat = svdResultRightSingularT.t();
		int svdRmatLastColNum = svdRMat.cols - 1;
		// get left colum
		for (int rowNum = 0; rowNum < svdRMat.rows; rowNum++) {
			rdConstantMat.at<float>(rowNum, 0) = svdRMat.at<float>(rowNum, svdRmatLastColNum);
		}

		// get residualMat
		Mat residualMat = Mat(pointCount, 1, CV_32FC1);
		for (int itemCount = 0; itemCount < pointCount; itemCount++) {
			residualMat.at<float>(itemCount, 0) =
			abs(
						pointMat.at<float>(itemCount,1)
						- (pointMat.at<float>(itemCount, 0) * rdConstantMat.at<float>(0, 0) + rdConstantMat.at<float>(2, 0)) / rdConstantMat.at<float>(1, 0)
			);
		}
		cout << "\n============================================";
		// get ransac result
		int rdThresholdCount = 0;
		Mat rdInlierMat = Mat::zeros(pointCount, 1, CV_32FC1);
		
		cout << "\n select a :" << randSelectA << "   select b : " << randSelectB;
		for (int checkItemCount = 0; checkItemCount < pointCount; checkItemCount++) {
			if (residualMat.at<float>(checkItemCount, 0) < thresHoldNum) {
				cout << "\n" << thresHoldNum << "  end   " << residualMat.at<float>(checkItemCount, 0) << "  item num " << checkItemCount;
				rdThresholdCount++;
				rdInlierMat.at<float>(checkItemCount, 0) = 1;
			}
		}
		cout << "\n all result => \n" << residualMat;

		// check threadCount
		if (rdThresholdCount > thUnderCount) {
			thUnderCount = rdThresholdCount;
			bestConstantMat = rdConstantMat;
			inilerMat = rdInlierMat;
		}

	} // iteration for end

	cout <<"\n"<< inilerMat;
	stPoint.x = 0;
	edPoint.x = colSize;
	stPoint.y = (bestConstantMat.at<float>(0, 0) * stPoint.x + bestConstantMat.at<float>(2, 0)) * (-1 / bestConstantMat.at<float>(1, 0));
	edPoint.y = (bestConstantMat.at<float>(0, 0) * edPoint.x + bestConstantMat.at<float>(2, 0)) * (-1 / bestConstantMat.at<float>(1, 0));

	return true;
}

bool PolygonDemo::drawLineGaussNewton(const std::vector<cv::Point>& pts, cv::Mat& bestConstantMat, int constantSize)
{
	int size = pts.size();
	// a * sin(bx+c) + d

	Mat residualMat = Mat(size, 1, CV_32FC1),
		jacobianMat = Mat(size, constantSize, CV_32FC1);
	

	for (int pointCount = 0; pointCount < size; pointCount++) {
		float residualValue =
			bestConstantMat.at<float>(0, 0)
			* sin(bestConstantMat.at<float>(1, 0) * pts[pointCount].x + bestConstantMat.at<float>(2, 0))
			+ bestConstantMat.at<float>(3, 0);

		residualMat.at<float>(pointCount, 0) = residualValue;

		// d/da => sin(bx+c)  ||  d/db => ax*cos(bx+c)  || d/dc => a*cos(bx+c)  || d/dd => 1
		jacobianMat.at<float>(pointCount, 0) = sin(bestConstantMat.at<float>(1, 0) * pts[pointCount].x + bestConstantMat.at<float>(2, 0));
		jacobianMat.at<float>(pointCount, 1) = bestConstantMat.at<float>(0, 0) * pts[pointCount].x
					 * cos(bestConstantMat.at<float>(1, 0) * pts[pointCount].x + bestConstantMat.at<float>(2, 0));
		jacobianMat.at<float>(pointCount, 2) = bestConstantMat.at<float>(0, 0) * cos(bestConstantMat.at<float>(1, 0) * pts[pointCount].x + bestConstantMat.at<float>(2, 0));
		jacobianMat.at<float>(pointCount, 3) = 1;
	}

	bestConstantMat = bestConstantMat - (jacobianMat.t() * jacobianMat).inv() * jacobianMat.t() * residualMat;

	return true;
}

void PolygonDemo::drawPolygon(Mat& frame, const std::vector<cv::Point>& vtx, bool closed)
{
    int i = 0;
    for (i = 0; i < (int)m_data_pts.size(); i++)
    {
        circle(frame, m_data_pts[i], 2, Scalar(255, 255, 255), CV_FILLED);
    }
    for (i = 0; i < (int)m_data_pts.size() - 1; i++)
    {
        line(frame, m_data_pts[i], m_data_pts[i + 1], Scalar(255, 255, 255), 1);
    }
    if (closed)
    {
        line(frame, m_data_pts[i], m_data_pts[0], Scalar(255, 255, 255), 1);
    }
}

void PolygonDemo::handleMouseEvent(int evt, int x, int y, int flags)
{
    if (evt == CV_EVENT_LBUTTONDOWN)
    {
        if (!m_data_ready)
        {
            m_data_pts.push_back(Point(x, y));
        }
        else
        {
            m_test_pts.push_back(Point(x, y));
        }
        refreshWindow();
    }
    else if (evt == CV_EVENT_LBUTTONUP)
    {
    }
    else if (evt == CV_EVENT_LBUTTONDBLCLK)
    {
        m_data_ready = true;
        refreshWindow();
    }
    else if (evt == CV_EVENT_RBUTTONDBLCLK)
    {
    }
    else if (evt == CV_EVENT_MOUSEMOVE)
    {
    }
    else if (evt == CV_EVENT_RBUTTONDOWN)
    {
        m_data_pts.clear();
        m_test_pts.clear();
        m_data_ready = false;
        refreshWindow();
    }
    else if (evt == CV_EVENT_RBUTTONUP)
    {
    }
    else if (evt == CV_EVENT_MBUTTONDOWN)
    {
    }
    else if (evt == CV_EVENT_MBUTTONUP)
    {
    }

    if (flags&CV_EVENT_FLAG_LBUTTON)
    {
    }
    if (flags&CV_EVENT_FLAG_RBUTTON)
    {
    }
    if (flags&CV_EVENT_FLAG_MBUTTON)
    {
    }
    if (flags&CV_EVENT_FLAG_CTRLKEY)
    {
    }
    if (flags&CV_EVENT_FLAG_SHIFTKEY)
    {
    }
    if (flags&CV_EVENT_FLAG_ALTKEY)
    {
    }
}
