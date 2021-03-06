#ifndef COMMON_STRUCTURE_H
#define COMMON_STRUCTURE_H

#include "iostream"
#include <algorithm>

//#ifdef USE_OPENCV
#ifndef USE_OPENCV4
#include "opencv/cv.hpp"
#endif
#include "opencv2/opencv.hpp"
//#endif

using namespace std;



/**
 *
 * We start defining all useful structures here
 * currently lots of usage need declaration of structures
 * while some of them can be standardized and make it public
 * as a common usage
 *
 * such as:
 *
 * BoundingBoxes
 * HumanFaces
 * HumanPoses
 * ....
 *
 * (We do not using protobuf as there no need to add
 * more dependencies to thor)
 *
 */

namespace thor {
enum BoxFormat {
  XYXY,
  TLWH,
  BOTH,
};

// simple box declaration
struct Box {

  // left, top, right, bottom
  // left top is the origin
  int top;
  int left;
  int w;
  int h;

  // box score, only for prediction
  float score;
  // label index
  int idx;

  int xmin;
  int ymin;
  int xmax;
  int ymax;

  BoxFormat format;

  Box(int a, int b, int c, int d, int format) {
	switch (format) {
	  case XYXY:
	    xmin = a;
		ymin = b;
		xmax = c;
		ymax = d;
		this->format = XYXY;
		break;
	  case TLWH:
	    top = a;
		left = b;
		w = c;
		h = d;
		this->format = TLWH;
		break;
	  default:
	    break;
	}
  }

  void to_tlwh() {
	// suppose we have xyxy already
	if (format == XYXY) {
	  this->top = xmin;
	  this->left = ymin;
	  this->w = xmax - xmin;
	  this->h = ymax - ymin;
	  format = BOTH;
	}
  }
  void to_xyxy() {
	if (format == TLWH) {
	  this->xmin = left;
	  this->ymin = top;
	  this->xmax = left + w;
	  this->ymax = top + h;
	  format = BOTH;
	}
  }
  void print() {
    switch (this->format) {
      case XYXY:
        std::cout << "x1:" << this->xmin << ",y1:" << this->ymin
                  << ",x2:" << this->xmax << ",y2:" << this->ymax << std::endl;
        break;
      case TLWH:
        std::cout << "top:" << this->top << ",left:" << this->left
                  << ",w:" << this->w << ",h:" << this->h << std::endl;
        break;
      default:
        std::cout << "x1:" << this->xmin << ",y1:" << this->ymin
                  << ",x2:" << this->xmax << ",y2:" << this->ymax << std::endl;
        break;
    }
  }
};


//#ifdef USE_OPENCV
// human pose decalaration
struct HumanPose {
  std::vector<cv::Point2f> keypoints;
  float score;
  // semantic pose type such as:
  // 1. waving hands; 2. stop; 3. turn left; 4. turn right
  // 5. follow me; 0. null
  int pose_type;
  // indicates same pose through video series, using for pose tracking
  int pose_id = -1;
  // if confirm it is face, this flag will on
  bool is_face = false;

  HumanPose(const std::vector<cv::Point2f> &keypoints = std::vector<cv::Point2f>(),
			const float &score = 0) : keypoints(keypoints),
									  score(score) {};
  Box to_box() {
	const cv::Point2f absentKeypoint(-1.0f, -1.0f);
	// converts human pose to a bounding box
	vector<float> xss;
	vector<float> yss;
	for (auto const &p: keypoints) {
	  // don't count absent points
	  if (p == absentKeypoint) {
		continue;
	  }
	  xss.push_back(p.x);
	  yss.push_back(p.y);
//	  cout << "(" << p.x << "," << p.y << ")" << "\n";
	}

	auto xminmax = std::minmax_element(xss.begin(), xss.end());
	auto yminmax = std::minmax_element(yss.begin(), yss.end());
	Box b((int)*(xminmax.first),
		  (int)*(yminmax.first),
		  (int)*(xminmax.second),
		  (int)*(yminmax.second), BoxFormat::XYXY);
//	b.print();
	return b;
  }
};

// todo: HumanFaces


// camera intrinsics decalaration
struct CameraIntrinsicParams {
  double fx, fy, cx, cy, scale;
  inline void print() {
	std::cout << "fx: " << fx << " fy: " << fy << " cx: " << cx
			  << " cy:" << cy << " scale: " << scale << endl;
  };
  inline cv::Mat toCameraMatrix() {
	// convert to a cv::Mat matrix
	double camera_matrix_data[3][4] = {
		{fx, 0, cx},
		{0, fy, cy},
		{0, 0, 1}
	};
	cv::Mat camera_mat(3, 3, CV_64F, camera_matrix_data);
	return camera_mat;
  }
};
//#endif

}

#endif // COMMON_STRUCTURE_H
