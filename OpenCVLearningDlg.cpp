
// OpenCVLearningDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OpenCVLearning.h"
#include "OpenCVLearningDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COpenCVLearningDlg 对话框



COpenCVLearningDlg::COpenCVLearningDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OPENCVLEARNING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCVLearningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COpenCVLearningDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Start_BUTTON, &COpenCVLearningDlg::OnBnClickedStartButton)
END_MESSAGE_MAP()


// COpenCVLearningDlg 消息处理程序

BOOL COpenCVLearningDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COpenCVLearningDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COpenCVLearningDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COpenCVLearningDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//1，Haar人脸检测
void HaarFindFace()
{
	//加载Haar特征检测分类器
	const string CascadeFileName = "E:\\tools\\opencv\\opencv2.13\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml";
	CascadeClassifier cascade = CascadeClassifier::CascadeClassifier(CascadeFileName);

	// 载入图像
	const string pstrImageName = "D:\\test\\hog.png";
	Mat SrcImage = imread(pstrImageName, CV_LOAD_IMAGE_COLOR);
	Mat GrayImage;

	cvtColor(SrcImage, GrayImage, CV_BGR2GRAY);

	// 人脸识别与标记
	if (!cascade.empty())
	{
		CvScalar FaceCirclecolors[] =
		{
			{ { 0, 0, 255 } },
			{ { 0, 128, 255 } },
			{ { 0, 255, 255 } },
			{ { 0, 255, 0 } },
			{ { 255, 128, 0 } },
			{ { 255, 255, 0 } },
			{ { 255, 0, 0 } },
			{ { 255, 0, 255 } }
		};

		vector<cv::Rect> faces;

		DWORD dwTimeBegin, dwTimeEnd;
		dwTimeBegin = GetTickCount();
		// 识别
		cascade.detectMultiScale(GrayImage, faces);
		dwTimeEnd = GetTickCount();

		cout << "人脸个数:" << faces.end() - faces.begin()
			<< "识别用时:" << dwTimeEnd - dwTimeBegin << "ms\n";
		// 标记
		int n = 0;
		for (vector<cv::Rect>::const_iterator i = faces.begin(); i <faces.end(); i++, n++)
		{
			Point center;
			int radius;
			center.x = cvRound((i->x + i->width * 0.5));
			center.y = cvRound((i->y + i->height * 0.5));
			radius = cvRound((i->width + i->height) * 0.25);
			circle(SrcImage, center, radius, FaceCirclecolors[n % 8], 2);
		}
	}
	namedWindow("人脸识别");
	imshow("人脸识别", SrcImage);

	cvWaitKey(0);

	return;
}

#include <opencv2/nonfree/nonfree.hpp>//SIFT

//SIFT特征点检测
void SIFT_FeatureDetector()
{
	Mat srcImg1 = imread("D://test//lena.jpg");
	Mat srcImg2 = imread("D://test//lena_2.jpg");
	//定义SIFT特征检测类对象
	SiftFeatureDetector siftDetector;//SIFT=SiftFeatureDetector=SiftDescriptorExtractor    别名//定义KeyPoint变量
	vector<KeyPoint>keyPoints1;
	vector<KeyPoint>keyPoints2;
	//特征点检测
	siftDetector.detect(srcImg1, keyPoints1);
	siftDetector.detect(srcImg2, keyPoints2);
	//绘制特征点(关键点)
	Mat feature_pic1, feature_pic2;
	//drawKeypoints(srcImg1, keyPoints1, feature_pic1, Scalar(0, 0, 255));
	drawKeypoints(srcImg2, keyPoints2, feature_pic2, Scalar(0, 255, 0));
	drawKeypoints(srcImg1, keyPoints1, feature_pic1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);//颜色随机，带有方向
	//drawKeypoints(srcImg2, keyPoints2, feature_pic2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//显示原图
	//imshow("src1", srcImg1);
	//imshow("src2", srcImg2);
	//显示结果
	imwrite("D:\\feature1.jpg", feature_pic1);
	imwrite("D:\\feature2.jpg", feature_pic2);
	//imshow("feature1", feature_pic1);
	//imshow("feature2", feature_pic2);
	//waitKey(0);
	return;

}

//1,雕刻&浮雕
void Emboss()
{
	Mat src = imread("D:\\test\\26.png");
	Mat img0(src.size(), CV_8UC3);
	Mat img1(src.size(), CV_8UC3);
	for (int y = 1; y<src.rows - 1; y++)
	{
		uchar *p0 = src.ptr<uchar>(y);
		uchar *p1 = src.ptr<uchar>(y + 1);

		uchar *q0 = img0.ptr<uchar>(y);
		uchar *q1 = img1.ptr<uchar>(y);
		for (int x = 1; x<src.cols - 1; x++)
		{
			for (int i = 0; i<3; i++)
			{
				int tmp0 = p1[3 * (x + 1) + i] - p0[3 * (x - 1) + i] + 128;//浮雕
				if (tmp0<0)
					q0[3 * x + i] = 0;
				else if (tmp0>255)
					q0[3 * x + i] = 255;
				else
					q0[3 * x + i] = tmp0;

				int tmp1 = p0[3 * (x - 1) + i] - p1[3 * (x + 1) + i] + 128;//雕刻
				if (tmp1<0)
					q1[3 * x + i] = 0;
				else if (tmp1>255)
					q1[3 * x + i] = 255;
				else
					q1[3 * x + i] = tmp1;
			}
		}
	}

	Mat gray_img0, gray_img1;
	cvtColor(img0, gray_img0, CV_BGR2GRAY);  
	cvtColor(img1, gray_img1, CV_BGR2GRAY); 


	//imwrite("D:\\浮雕.jpg", gray_img0);
	//imwrite("D:\\雕刻.jpg", gray_img1);
	//imshow("src", src);
	imshow("浮雕", gray_img0);
	imshow("雕刻", gray_img1);
	waitKey();
}

//2,扩张挤压
void ExpansionExtrusion()
{
	Mat src = imread("D:\\test\\26.png");
	int width = src.cols;
	int heigh = src.rows;
	Point center(width / 2, heigh / 2);
	Mat img1(src.size(), CV_8UC3);
	Mat img2(src.size(), CV_8UC3);
	src.copyTo(img1);
	src.copyTo(img2);

	//【1】放大
	int R1 = sqrtf(width*width + heigh*heigh) / 2; //直接关系到放大的力度,与R1成正比;

	for (int y = 0; y<heigh; y++)
	{
		uchar *img1_p = img1.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			int dis = norm(Point(x, y) - center);
			if (dis<R1)
			{
				int newX = (x - center.x)*dis / R1 + center.x;
				int newY = (y - center.y)*dis / R1 + center.y;

				img1_p[3 * x] = src.at<uchar>(newY, newX * 3);
				img1_p[3 * x + 1] = src.at<uchar>(newY, newX * 3 + 1);
				img1_p[3 * x + 2] = src.at<uchar>(newY, newX * 3 + 2);
			}
		}
	}

	//【2】挤压
	for (int y = 0; y<heigh; y++)
	{
		uchar *img2_p = img2.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			double theta = atan2((double)(y - center.y), (double)(x - center.x));//使用atan出现问题~


			int R2 = sqrtf(norm(Point(x, y) - center)) * 8; //直接关系到挤压的力度，与R2成反比;

			int newX = center.x + (int)(R2*cos(theta));

			int newY = center.y + (int)(R2*sin(theta));

			if (newX<0) newX = 0;
			else if (newX >= width) newX = width - 1;
			if (newY<0) newY = 0;
			else if (newY >= heigh) newY = heigh - 1;


			img2_p[3 * x] = src.at<uchar>(newY, newX * 3);
			img2_p[3 * x + 1] = src.at<uchar>(newY, newX * 3 + 1);
			img2_p[3 * x + 2] = src.at<uchar>(newY, newX * 3 + 2);
		}
	}
	imshow("src", src);
	imshow("img1", img1);
	imshow("img2", img2);
	waitKey();
	imwrite("D:\\扩张.jpg", img1);
	imwrite("D:\\挤压.jpg", img2);
}

//3,颜色变换
void ColorTransformation()
{
	Mat src = imread("D:\\test\\26.png");
	int width = src.cols;
	int heigh = src.rows;
	Mat gray;
	Mat imgColor[12];
	Mat display(heigh * 3, width * 4, CV_8UC3);

	cvtColor(src, gray, CV_BGR2GRAY);
	for (int i = 0; i<12; i++)
	{
		applyColorMap(gray, imgColor[i], i);
		int x = i % 4;
		int y = i / 4;
		Mat displayROI = display(Rect(x*width, y*heigh, width, heigh));
		resize(imgColor[i], displayROI, displayROI.size());
	}
	imshow("colorImg", display);
	waitKey();
	imwrite("D:\\幻彩颜色02.jpg", display);
}

//4,波浪：正弦
double angle;
int deltaI = 10;	//波浪周期;
int A = 10;		//波浪振幅;
Mat src, img;
static void onTrackbar(int, void*)
{
	int width = src.cols;
	int heigh = src.rows;
	angle = 0.0;

	for (int y = 0; y<heigh; y++)
	{
		int changeX = A*sin(angle);
		uchar *srcP = src.ptr<uchar>(y);
		uchar *imgP = img.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			if (changeX + x<width && changeX + x>0)		//正弦分布（-1,1）
			{
				imgP[3 * x] = srcP[3 * (x + changeX)];
				imgP[3 * x + 1] = srcP[3 * (x + changeX) + 1];
				imgP[3 * x + 2] = srcP[3 * (x + changeX) + 2];
			}
			//每行开始和结束的空白区;
			else if (x <= changeX)
			{
				imgP[3 * x] = srcP[0];
				imgP[3 * x + 1] = srcP[1];
				imgP[3 * x + 2] = srcP[2];
			}
			else if (x >= width - changeX)
			{
				imgP[3 * x] = srcP[3 * (width - 1)];
				imgP[3 * x + 1] = srcP[3 * (width - 1) + 1];
				imgP[3 * x + 2] = srcP[3 * (width - 1) + 2];
			}
		}
		angle += ((double)deltaI) / 100;
	}
	imshow("Waves map", img);
}

void WaveSine()
{
	src = imread("D:\\test\\26.png");

	src.copyTo(img);

	namedWindow("Waves map", 1);

	// create a toolbar
	createTrackbar("振幅", "Waves map", &A, 100, onTrackbar);

	createTrackbar("频率", "Waves map", &deltaI, 100, onTrackbar);
	// Show the image
	onTrackbar(0, 0);

	waitKey();
	imwrite("D://wave.jpg", img);

}

//5,径向模糊：缩放&旋转
int num = 20;//num：均值力度;

//缩放
void RadialBlurZoom()
{
	Mat src = imread("D:\\test\\26.png");
	Mat src1u[3];
	split(src, src1u);

	int width = src.cols;
	int heigh = src.rows;
	Mat img;
	src.copyTo(img);
	Point center(width / 2, heigh / 2);


	for (int y = 0; y<heigh; y++)
	{

		uchar *imgP = img.ptr<uchar>(y);

		for (int x = 0; x<width; x++)
		{
			int R = norm(Point(x, y) - center);
			double angle = atan2((double)(y - center.y), (double)(x - center.x));

			int tmp0 = 0, tmp1 = 0, tmp2 = 0;

			for (int i = 0; i<num; i++)		//num：均值力度 ，i为变化幅度;
			{
				int tmpR = (R - i)>0 ? (R - i) : 0;

				int newX = tmpR*cos(angle) + center.x;
				int newY = tmpR*sin(angle) + center.y;

				if (newX<0)newX = 0;
				if (newX>width - 1)newX = width - 1;
				if (newY<0)newY = 0;
				if (newY>heigh - 1)newY = heigh - 1;

				tmp0 += src1u[0].at<uchar>(newY, newX);
				tmp1 += src1u[1].at<uchar>(newY, newX);
				tmp2 += src1u[2].at<uchar>(newY, newX);

			}
			imgP[3 * x] = (uchar)(tmp0 / num);
			imgP[3 * x + 1] = (uchar)(tmp1 / num);
			imgP[3 * x + 2] = (uchar)(tmp2 / num);
		}

	}
	imshow("径向模糊", img);
	waitKey();
	imwrite("D://径向模糊（缩放）.jpg", img);
}

//旋转
void RadialBluRotate()
{
	Mat src = imread("D:\\test\\26.png");
	Mat src1u[3];
	split(src, src1u);

	int width = src.cols;
	int heigh = src.rows;
	Mat img;
	src.copyTo(img);
	Point center(width / 2, heigh / 2);


	for (int y = 0; y<heigh; y++)
	{

		uchar *imgP = img.ptr<uchar>(y);

		for (int x = 0; x<width; x++)
		{
			int R = norm(Point(x, y) - center);
			double angle = atan2((double)(y - center.y), (double)(x - center.x));

			int tmp0 = 0, tmp1 = 0, tmp2 = 0;

			for (int i = 0; i<num; i++)	//均值力度;
			{

				angle += 0.01;        //0.01控制变化频率，步长

				int newX = R*cos(angle) + center.x;
				int newY = R*sin(angle) + center.y;

				if (newX<0)newX = 0;
				if (newX>width - 1)newX = width - 1;
				if (newY<0)newY = 0;
				if (newY>heigh - 1)newY = heigh - 1;

				tmp0 += src1u[0].at<uchar>(newY, newX);
				tmp1 += src1u[1].at<uchar>(newY, newX);
				tmp2 += src1u[2].at<uchar>(newY, newX);

			}
			imgP[3 * x] = (uchar)(tmp0 / num);
			imgP[3 * x + 1] = (uchar)(tmp1 / num);
			imgP[3 * x + 2] = (uchar)(tmp2 / num);
		}

	}
	imshow("径向模糊", img);
	waitKey();
	imwrite("D:\\径向模糊(旋转).jpg", img);
}

//6,风
int num0 = 10;//	num：风线密度
int num1 = 20;//	num1：风线长度

void Wind()
{
	Mat src = imread("D:\\test\\26.png");
	Mat src1u[3];
	split(src, src1u);

	int width = src.cols;
	int heigh = src.rows;
	Mat img;
	src.copyTo(img);

	Point center(width / 2, heigh / 2);

	RNG rng;

	for (int y = 0; y<heigh; y++)
	{
		uchar *imgP = img.ptr<uchar>(y);

		//		for (int x=0; x<width; x++)
		{

			for (int i = 0; i<num0; i++)		//	num：风线密度
			{
				int newX = rng.uniform(i*width / num0, (i + 1)*width / num0);
				int newY = y;

				if (newX<0)newX = 0;
				if (newX>width - 1)newX = width - 1;

				uchar tmp0 = src1u[0].at<uchar>(newY, newX);
				uchar tmp1 = src1u[1].at<uchar>(newY, newX);
				uchar tmp2 = src1u[2].at<uchar>(newY, newX);

				for (int j = 0; j<num1; j++)	//num1：风线长度
				{
					int tmpX = newX - j;//减：风向左；加：风向右

					if (tmpX<0)tmpX = 0;
					if (tmpX>width - 1)tmpX = width - 1;

					imgP[tmpX * 3] = tmp0;
					imgP[tmpX * 3 + 1] = tmp1;
					imgP[tmpX * 3 + 2] = tmp2;
				}
			}
		}
	}
	imshow("径向模糊", img);
	waitKey();
	imwrite("D:/风.jpg", img);
}

//7漩涡
template<typename T> T sqr(T x) { return x*x; }
double Pi = 3.14;
double Para = 8;

void Swirl()
{
	Mat src = imread("D:\\test\\26.png");
	int heigh = src.rows;
	int width = src.cols;
	Point center(width / 2, heigh / 2);
	Mat img;
	src.copyTo(img);
	Mat src1u[3];
	split(src, src1u);

	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* srcP = src.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			int R = norm(Point(x, y) - center);
			double angle = atan2((double)(y - center.y), (double)(x - center.x));
			double delta = Pi*Para / sqrtf(R + 1);
			int newX = R*cos(angle + delta) + center.x;
			int newY = R*sin(angle + delta) + center.y;

			if (newX<0) newX = 0;
			if (newX>width - 1) newX = width - 1;
			if (newY<0) newY = 0;
			if (newY>heigh - 1) newY = heigh - 1;

			imgP[3 * x] = src1u[0].at<uchar>(newY, newX);
			imgP[3 * x + 1] = src1u[1].at<uchar>(newY, newX);
			imgP[3 * x + 2] = src1u[2].at<uchar>(newY, newX);
		}
	}
	imshow("vortex", img);
	waitKey();
	imwrite("D://漩涡.jpg", img);
}

//8素描
void Sketch()
{
	Mat src = imread("D:\\test\\26.png");
	int width = src.cols;
	int heigh = src.rows;
	Mat gray0, gray1;
	//去色
	cvtColor(src, gray0, CV_BGR2GRAY);
	//反色
	addWeighted(gray0, -1, NULL, 0, 255, gray1);
	//高斯模糊,高斯核的Size与最后的效果有关
	GaussianBlur(gray1, gray1, Size(11, 11), 0);

	//融合：颜色减淡
	Mat img(gray1.size(), CV_8UC1);
	for (int y = 0; y<heigh; y++)
	{

		uchar* P0 = gray0.ptr<uchar>(y);
		uchar* P1 = gray1.ptr<uchar>(y);
		uchar* P = img.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			int tmp0 = P0[x];
			int tmp1 = P1[x];
			P[x] = (uchar)min((tmp0 + (tmp0*tmp1) / (256 - tmp1)), 255);
		}

	}
	imshow("素描", img);
	waitKey();
	imwrite("D:/素描.jpg", img);
}

//9扩散（毛玻璃）
void DiffusionGroundGlass()
{
	Mat src = imread("D:\\test\\26.png");
	int width = src.cols;
	int heigh = src.rows;
	RNG rng;
	Mat img(src.size(), CV_8UC3);
	for (int y = 1; y<heigh - 1; y++)
	{
		uchar* P0 = src.ptr<uchar>(y);
		uchar* P1 = img.ptr<uchar>(y);
		for (int x = 1; x<width - 1; x++)
		{
			int tmp = rng.uniform(0, 9);
			P1[3 * x] = src.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3));
			P1[3 * x + 1] = src.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3) + 1);
			P1[3 * x + 2] = src.at<uchar>(y - 1 + tmp / 3, 3 * (x - 1 + tmp % 3) + 2);
		}

	}
	imshow("扩散", img);
	waitKey();
	imwrite("D:/扩散.jpg", img);
}

//10怀旧色 & 连环画 & 熔铸 & 冰冻
//怀旧
void Nostalgic()
{
	Mat src = imread("D:\\test\\66.jpg");
	int width = src.cols;
	int heigh = src.rows;
	RNG rng;
	Mat img(src.size(), CV_8UC3);
	for (int y = 0; y<heigh; y++)
	{
		uchar* P0 = src.ptr<uchar>(y);
		uchar* P1 = img.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			float B = P0[3 * x];
			float G = P0[3 * x + 1];
			float R = P0[3 * x + 2];
			float newB = 0.272*R + 0.534*G + 0.131*B;
			float newG = 0.349*R + 0.686*G + 0.168*B;
			float newR = 0.393*R + 0.769*G + 0.189*B;
			if (newB<0)newB = 0;
			if (newB>255)newB = 255;
			if (newG<0)newG = 0;
			if (newG>255)newG = 255;
			if (newR<0)newR = 0;
			if (newR>255)newR = 255;
			P1[3 * x] = (uchar)newB;
			P1[3 * x + 1] = (uchar)newG;
			P1[3 * x + 2] = (uchar)newR;
		}

	}
	imshow("怀旧色", img);
	waitKey();
	imwrite("D:/怀旧色.jpg", img);
}

//连环画
void ComicStrip()
{
	Mat src = imread("D:\\test\\66.jpg");
	int width = src.cols;
	int heigh = src.rows;
	RNG rng;
	Mat img(src.size(), CV_8UC3);
	for (int y = 0; y<heigh; y++)
	{
		uchar* P0 = src.ptr<uchar>(y);
		uchar* P1 = img.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			float B = P0[3 * x];
			float G = P0[3 * x + 1];
			float R = P0[3 * x + 2];
			float newB = abs(B - G + B + R)*G / 256;
			float newG = abs(B - G + B + R)*R / 256;
			float newR = abs(G - B + G + R)*R / 256;
			if (newB<0)newB = 0;
			if (newB>255)newB = 255;
			if (newG<0)newG = 0;
			if (newG>255)newG = 255;
			if (newR<0)newR = 0;
			if (newR>255)newR = 255;
			P1[3 * x] = (uchar)newB;
			P1[3 * x + 1] = (uchar)newG;
			P1[3 * x + 2] = (uchar)newR;
		}

	}
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	normalize(gray, gray, 255, 0, CV_MINMAX);
	imshow("连环画", gray);
	waitKey();
	imwrite("D:/连环画.jpg", gray);
}

//熔铸
void casting(const Mat& src)
{
	Mat img;
	src.copyTo(img);
	int width = src.cols;
	int heigh = src.rows;
	Mat dst(img.size(), CV_8UC3);
	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* dstP = dst.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			float b0 = imgP[3 * x];
			float g0 = imgP[3 * x + 1];
			float r0 = imgP[3 * x + 2];

			float b = b0 * 255 / (g0 + r0 + 1);
			float g = g0 * 255 / (b0 + r0 + 1);
			float r = r0 * 255 / (g0 + b0 + 1);

			r = (r>255 ? 255 : (r<0 ? 0 : r));
			g = (g>255 ? 255 : (g<0 ? 0 : g));
			b = (b>255 ? 255 : (b<0 ? 0 : b));

			dstP[3 * x] = (uchar)b;
			dstP[3 * x + 1] = (uchar)g;
			dstP[3 * x + 2] = (uchar)r;
		}
	}
	imshow("熔铸", dst);
	imwrite("D://熔铸.jpg", dst);

}

//冰冻
void freezing(const Mat& src)
{
	Mat img;
	src.copyTo(img);
	int width = src.cols;
	int heigh = src.rows;
	Mat dst(img.size(), CV_8UC3);
	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* dstP = dst.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			float b0 = imgP[3 * x];
			float g0 = imgP[3 * x + 1];
			float r0 = imgP[3 * x + 2];

			float b = (b0 - g0 - r0) * 3 / 2;
			float g = (g0 - b0 - r0) * 3 / 2;
			float r = (r0 - g0 - b0) * 3 / 2;

			r = (r>255 ? 255 : (r<0 ? -r : r));
			g = (g>255 ? 255 : (g<0 ? -g : g));
			b = (b>255 ? 255 : (b<0 ? -b : b));
			// 			r = (r>255 ? 255 : (r<0? 0 : r));
			// 			g = (g>255 ? 255 : (g<0? 0 : g));
			// 			b = (b>255 ? 255 : (b<0? 0 : b));
			dstP[3 * x] = (uchar)b;
			dstP[3 * x + 1] = (uchar)g;
			dstP[3 * x + 2] = (uchar)r;
		}
	}
	imwrite("D://冰冻.jpg", dst);
}

void Casting()
{
	Mat src = imread("D:\\test\\66.jpg");
	imshow("src", src);
	casting(src);
	freezing(src);

	waitKey();

}

//11高反差保留
int R = 5;
void HighContrastRetention()
{
	Mat src = imread("D:\\test\\26.png");
	int width = src.cols;
	int heigh = src.rows;
	Mat img;
	src.copyTo(img);
	Mat avg;
	//GaussianBlur(img,avg,Size(R,R),0.0);
	blur(img, avg, Size(R, R));
	Mat dst(img.size(), CV_8UC3);
	float tmp;
	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* avgP = avg.ptr<uchar>(y);
		uchar* dstP = dst.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			float r0 = abs((float)imgP[3 * x] - (float)avgP[3 * x]) / 128;
			tmp = abs(((float)imgP[3 * x])*r0 + 128 * (1 - r0));
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x] = (uchar)(tmp);

			float r1 = abs((float)imgP[3 * x + 1] - (float)avgP[3 * x + 1]) / 128;
			tmp = (uchar)abs(((float)imgP[3 * x + 1])*r1 + 128 * (1 - r1));
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x + 1] = (uchar)(tmp);

			float r2 = abs((float)imgP[3 * x + 2] - (float)avgP[3 * x + 2]) / 128;
			tmp = (uchar)abs(((float)imgP[3 * x + 2])*r2 + 128 * (1 - r2));
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x + 2] = (uchar)(tmp);
		}
	}
	imshow("high", dst);

	//高通滤波测试

	Mat kern = (Mat_<char>(3, 3) << -1, -1, -1,
		-1, 5, -1,
		-1, -1, -1);
	Mat dstF;
	filter2D(img, dstF, img.depth(), kern);
	imshow("kernel", dstF);

	waitKey();
	imwrite("D:/高反差保留.jpg", dst);
	imwrite("D:/高通滤波.jpg", dstF);
}

//12计算模式（强光）
void CalculationMode()
{
	Mat src = imread("D:\\test\\12.jpg");
	imshow("src", src);
	int width = src.cols;
	int heigh = src.rows;
	Mat img;
	src.copyTo(img);

	Mat dst(img.size(), CV_8UC3);
	Mat dst1u[3];


	float tmp, r;
	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* dstP = dst.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			r = (float)imgP[3 * x];
			if (r>127.5)
				tmp = r + (255 - r)*(r - 127.5) / 127.5;
			else
				tmp = r*r / 127.5;
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x] = (uchar)(tmp);

			r = (float)imgP[3 * x + 1];
			if (r>127.5)
				tmp = r + (255 - r)*(r - 127.5) / 127.5;
			else
				tmp = r*r / 127.5;
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x + 1] = (uchar)(tmp);

			r = (float)imgP[3 * x + 2];
			if (r>127.5)
				tmp = r + (255 - r)*(r - 127.5) / 127.5;
			else
				tmp = r*r / 127.5;
			tmp = tmp>255 ? 255 : tmp;
			tmp = tmp<0 ? 0 : tmp;
			dstP[3 * x + 2] = (uchar)(tmp);
		}
	}
	imshow("强光", dst);

	split(dst, dst1u);
	imshow("绿通道强光", dst1u[1]);

	waitKey();
	imwrite("D://强光.jpg", dst);
	imwrite("D://强光_蓝通道.jpg", dst1u[0]);
	imwrite("D://强光_绿通道.jpg", dst1u[1]);
	imwrite("D://强光_红通道.jpg", dst1u[2]);

}

//13羽化
float mSize = 0.5;

void Feathering()
{
	Mat src = imread("D:\\test\\lena.jpg");
	imshow("src", src);
	int width = src.cols;
	int heigh = src.rows;
	int centerX = width >> 1;
	int centerY = heigh >> 1;

	int maxV = centerX*centerX + centerY*centerY;
	int minV = (int)(maxV*(1 - mSize));
	int diff = maxV - minV;
	float ratio = width >heigh ? (float)heigh / (float)width : (float)width / (float)heigh;

	Mat img;
	src.copyTo(img);

	Scalar avg = mean(src);
	Mat dst(img.size(), CV_8UC3);
	Mat mask1u[3];
	float tmp, r;
	for (int y = 0; y<heigh; y++)
	{
		uchar* imgP = img.ptr<uchar>(y);
		uchar* dstP = dst.ptr<uchar>(y);
		for (int x = 0; x<width; x++)
		{
			int b = imgP[3 * x];
			int g = imgP[3 * x + 1];
			int r = imgP[3 * x + 2];

			float dx = centerX - x;
			float dy = centerY - y;

			if (width > heigh)
				dx = (dx*ratio);
			else
				dy = (dy*ratio);

			int dstSq = dx*dx + dy*dy;

			float v = ((float)dstSq / diff) * 255;

			r = (int)(r + v);
			g = (int)(g + v);
			b = (int)(b + v);
			r = (r>255 ? 255 : (r<0 ? 0 : r));
			g = (g>255 ? 255 : (g<0 ? 0 : g));
			b = (b>255 ? 255 : (b<0 ? 0 : b));

			dstP[3 * x] = (uchar)b;
			dstP[3 * x + 1] = (uchar)g;
			dstP[3 * x + 2] = (uchar)r;
		}
	}
	imshow("羽化", dst);

	waitKey();
	imwrite("D://羽化.jpg", dst);

}


//-----开始------
void COpenCVLearningDlg::OnBnClickedStartButton()
{
	Feathering();
}
