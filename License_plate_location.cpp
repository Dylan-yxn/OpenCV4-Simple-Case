#include <opencv.hpp>
#include <opencv2\opencv.hpp>

int lpl()
{
	// 首先读取图片
	Mat img = imread("car4.jpg");
	if (!img.data)
	{
		printf("请确认图像文件是否输入正确");
		return -1;
	}
	imshow("车辆原图",img);

	// 转换成灰度模型
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	imshow("车辆灰度化结果图", gray);

	// 使用高斯滤波去除噪声
	Mat afterGaussBlur_gray;
	GaussianBlur(gray, afterGaussBlur_gray, Size(5, 5), 0, 0, BORDER_DEFAULT);
	imshow("经过高斯滤波的图像", afterGaussBlur_gray);

	
	Mat rectangular = getStructuringElement(0, Size(2, 2));

	// 开运算
	Mat afterOpen;
	morphologyEx(afterGaussBlur_gray, afterOpen, MORPH_OPEN, rectangular);
	addWeighted(gray,1,afterGaussBlur_gray,-1,0,afterGaussBlur_gray);
	imshow("经过开运算", afterOpen);


	// 使用outs大律法进行图像分割
	Mat afterOuts;
	threshold(afterOpen, afterOuts, 100, 255, THRESH_BINARY | THRESH_OTSU);
	imshow("图像分割操作", afterOuts);

	// 进行Canny算法边缘检测
	Mat afterCanny;
	Canny(afterOuts, afterCanny, 100, 200, 3);
	imshow("经过边缘检测", afterCanny);

	// 进行一次闭运算和开运算
	Mat s = getStructuringElement(0, Size(13, 13));

	Mat afterClose, afterOpen1;
	morphologyEx(afterCanny, afterClose, MORPH_CLOSE, s);

	morphologyEx(afterClose, afterOpen1, MORPH_OPEN, s);
	imshow("经过开运算2", afterOpen1);

	vector<vector<Point>> counts;
	Mat out;
	findContours(afterOpen1, counts, out, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> temp;
	for (size_t i = 0; i < counts.size(); i++)
	{
		if (contourArea(counts[i]) > 2000) {
			temp.push_back(counts[i]);
		}
	}
	vector<vector<Point>> car_plate;
	for (size_t i = 0; i < temp.size(); i++)
	{
		RotatedRect res = minAreaRect(temp[i]);
		float width = res.size.width;
		float height = res.size.height;

		float t;
		if (width < height) {
			t = width;
			width = height;
			height = t;
		}

		float aspect_ratio = width / height;

		if (aspect_ratio > 2 && aspect_ratio < 5.5) {
			car_plate.push_back(temp[i]);
			//boxPoints(res,)

		}
	}

	if (car_plate.size() > 0) {
		Rect rect = boundingRect(car_plate[0]);
		rectangle(img,rect,Scalar(0,255,0),2);
		imshow("img", img);
	}
	
	
	waitKey(0);
	return 0;
}
