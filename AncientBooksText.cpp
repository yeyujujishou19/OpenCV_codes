
#include "LayoutAnalysis.h"

//===============全局变量==========================================================================
string sReadPath = "";          //读取 图像路径

bool bAuxiliaryCode = true;                   //辅助代码
//===============全局变量==========================================================================


//获取子文件夹路径
void get_dirs(CString strPath, vector <CString>& dirs)
{
	vector<CString> vecFiles = {};
	//获取文件夹下所有子文件夹名
	CString strFilePath;
	//int64    dwDirSize = 0;
	strFilePath += strPath;
	strFilePath += "//*.*";
	CFileFind finder;
	BOOL bFind = finder.FindFile(strFilePath);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (!finder.IsDots())
		{
			CString strTempPath = finder.GetFilePath();
			if (finder.IsDirectory())
			{
				dirs.push_back(strTempPath);// 遍历子文件夹

											//TraverseDir(strTempPath, dirs);
			}
			else
			{
				continue;
			}
		}
	}
	finder.Close();
}

//垂直方向投影
Mat VerticalProjection(Mat srcImageBin)//垂直积分投影  
{
	//	Mat lineImage(1, srcImageBin.cols, CV_8UC1, cv::Scalar(0, 0, 0));
	Mat lineImage(1, srcImageBin.cols, CV_16SC1, cv::Scalar(0, 0, 0));
	int value;
	for (int i = 0; i < srcImageBin.cols; i++)
	{
		int icount = 0;
		for (int j = 0; j < srcImageBin.rows; j++)
		{
			value = srcImageBin.at<uchar>(j, i);
			if (value == 0)
			{
				icount++; //统计每列的白色像素点   
			}
		}
		lineImage.at<ushort>(0, i) = icount;
	}
	return lineImage;
}

//水平方向投影
Mat HorProjection(Mat srcImageBin)
{
	//Mat lineImage(srcImageBin.rows,1, CV_8UC1, cv::Scalar(0, 0, 0));
	Mat lineImage(srcImageBin.rows, 1, CV_16SC1, cv::Scalar(0, 0, 0));
	int value;
	for (int i = 0; i < srcImageBin.rows; i++)
	{
		int icount = 0;
		for (int j = 0; j < srcImageBin.cols; j++)
		{
			value = srcImageBin.at<uchar>(i, j);
			if (value == 0)
			{
				icount++; //统计每列的白色像素点   
			}
		}
		lineImage.at<ushort>(i, 0) = icount;
	}
	return lineImage;
}

//水平方向投影 重载函数
void HorProjection(Mat srcImageBin, Mat &lineImage)
{
	int value;
	for (int i = 0; i < srcImageBin.rows; i++)
	{
		if (i == 385)
		{
			int a = 0;
		}
		int icount = 0;
		for (int j = 0; j < srcImageBin.cols; j++)
		{
			value = srcImageBin.at<uchar>(i, j);
			if (value == 0)
			{
				icount++; //统计每列的白色像素点   
			}
		}
		lineImage.at<ushort>(i, 0) = icount;

	}

}

//按指定字符切分字符串
string split2(string str, char del) //忽略“空位”（::192:168:ABC::416）->（192 168 ABC 416）
{
	stringstream ss(str);
	string tok;
	vector<string> ret;
	while (getline(ss, tok, del))
	{
		if (tok > "")
			ret.push_back(tok);
	}
	return ret[ret.size() - 1];
}

//分割字符
vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}


//=========================================================================================
//===========古文-文字区域定位=============================================================
//=========================================================================================


//古文文字矩形结构体
struct stAncientBooksRect
{
	int x;
	int y;
	int width;
	int height;

	int iColID = 0;  //列序号

	int iRowID = 0;  //行序号

};


//按x正序
bool MySort_x2(Rect rect1, Rect rect2)
{
	return rect1.x < rect2.x;
}

//按x正序
bool MySort_x3(stAncientBooksRect rect1, stAncientBooksRect rect2)
{
	return rect1.iColID < rect2.iColID;
}

//计算黑点比例
float ComputerblackRate(Mat imgrect)
{
	//计算黑点占矩形比例
	float fCount = 0;
	float fRate = 0;

	for (int icol = 0; icol < imgrect.cols; icol++)
	{
		for (int irow = 0; irow < imgrect.rows; irow++)
		{
			int value = imgrect.at<uchar>(irow, icol);
			if (value == 0)
			{
				fCount++;
			}
		}
	}
	fRate = fCount / (imgrect.cols*imgrect.rows);

	return fRate;
}

//精确边界
void PreciseBoundary(vector<stAncientBooksRect> &iVecRectWorld, Mat BinImage)
{
	//转换成彩色图
	Mat rgbImg5;
	cvtColor(BinImage, rgbImg5, COLOR_GRAY2RGB);

	Rect MyRect;
	for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	{
		MyRect.x = iVecRectWorld[ia].x;
		MyRect.y = iVecRectWorld[ia].y;
		MyRect.width = iVecRectWorld[ia].width;
		MyRect.height = iVecRectWorld[ia].height;

		Mat imgrect = BinImage(Rect(MyRect));

		//垂直投影，水平投影
		Mat VerImg = VerticalProjection(imgrect);
		Mat HorImg = HorProjection(imgrect);


		bool bLeft = false;
		bool bRight = false;
		bool bUp = false;
		bool bDown = false;
		int iLeft = 0;
		int iRight = 0;
		int iUp = 0;
		int iDown = 0;

		//左右边界
		for (int icol = 0; icol < VerImg.cols; icol++)
		{
			if (VerImg.at<ushort>(0, icol) != 0 && bLeft == false)
			{
				iLeft = icol;
				bLeft = true;
			}
			if (VerImg.at<ushort>(0, VerImg.cols - 1 - icol) != 0 && bRight == false)
			{
				iRight = VerImg.cols - 1 - icol;
				bRight = true;
			}
			if (bLeft == true && bRight == true)
			{
				break;
			}
		}

		//上下边界
		for (int irow = 0; irow < HorImg.rows; irow++)
		{
			if (HorImg.at<ushort>(irow, 0) != 0 && bUp == false)
			{
				iUp = irow;
				bUp = true;
			}
			if (HorImg.at<ushort>(HorImg.rows - 1 - irow, 0) != 0 && bDown == false)
			{
				iDown = HorImg.rows - 1 - irow;
				bDown = true;
			}
			if (bUp == true && bDown == true)
			{
				break;
			}
		}

		iVecRectWorld[ia].x = iVecRectWorld[ia].x + iLeft;
		iVecRectWorld[ia].y = iVecRectWorld[ia].y + iUp;
		iVecRectWorld[ia].width = iRight - iLeft;
		iVecRectWorld[ia].height = iDown - iUp;

		//画出轮廓外接矩形
		if (bAuxiliaryCode)
		{

			Rect rect;
			rect.x = iVecRectWorld[ia].x;
			rect.y = iVecRectWorld[ia].y;
			rect.width = iVecRectWorld[ia].width;
			rect.height = iVecRectWorld[ia].height;

			rectangle(rgbImg5, rect, Scalar(0, 0, 255), 2, 1);//用矩形画矩形窗
		}

	}
	int a = 0;

}

//定位文字区域
string AncientBooksText(Mat img0, string imageName)
{
	////-----------------裁剪周边-------------------------------------------------------
	//Rect Myrect;
	//Myrect.x = 50;
	//Myrect.y = 200;
	//Myrect.width = img0.cols - 100;
	//Myrect.height = img0.rows - 400;
	//Mat imgrect = img0(Rect(Myrect.x, Myrect.y, Myrect.width, Myrect.height));
	////-----------------裁剪周边-------------------------------------------------------

	//转换成彩色图
	Mat rgbImg;
	cvtColor(img0, rgbImg, COLOR_GRAY2RGB);

	//转换成彩色图
	Mat rgbImg3;
	cvtColor(img0, rgbImg3, COLOR_GRAY2RGB);

	Mat binImage, binImageCopy;
	threshold(img0, binImage, 0, 255, CV_THRESH_OTSU);
	binImageCopy = binImage.clone();

	//获取自定义核
	//Mat element = getStructuringElement(MORPH_RECT, Size(2, 2)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的														   
	//dilate(binImageCopy, binImageCopy, element);//膨胀操作

	//-----------------形态学提取直线并用矩形加强-------------------------------------
	Mat imageF1, imageF2, imageF3, imageF4, imageF5, imageF6, imageF7;

	Mat element1 = getStructuringElement(MORPH_RECT, Size(190, 1)); //横直线
	Mat element3 = getStructuringElement(MORPH_RECT, Size(1, 190)); //竖直线

																	//--------横线-------------
	morphologyEx(img0, imageF1, MORPH_CLOSE, element1);
	threshold(imageF1, imageF2, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);//自适应阈值

	vector< vector<Point> > contours2; //轮廓数组
	std::vector<cv::Vec4i> hierarchy2;
	findContours(imageF2, contours2, hierarchy2, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //寻找轮廓，图片是黑底白字

	vector< vector<Point> >::iterator itr2;  //轮廓迭代器
	itr2 = contours2.begin();
	int iThreshold2 = 5;
	while (itr2 != contours2.end())
	{
		//绘制轮廓的最小外接矩形  
		Rect rect2 = boundingRect(*itr2);

		if (rect2.width > img0.cols * 4 / 5)
		{
			rect2.x = rect2.x - iThreshold2;
			rect2.y = rect2.y - iThreshold2;
			rect2.width = rect2.width + 2 * iThreshold2;
			rect2.height = rect2.height + 2 * iThreshold2;
			rectangle(binImageCopy, rect2, Scalar(255), CV_FILLED, 1);//用矩形画矩形窗   黑色
		}
		else if (rect2.width > img0.cols * 1 / 10)
		{
			rect2.x = rect2.x - iThreshold2;
			rect2.y = rect2.y - iThreshold2;
			rect2.width = rect2.width + 1.5 * iThreshold2;
			rect2.height = rect2.height + 1.5 * iThreshold2;
			rectangle(binImageCopy, rect2, Scalar(255), CV_FILLED, 1);//用矩形画矩形窗   黑色
		}

		itr2++;
	} //while (itr != contours.end())

	  //--------竖线-------------
	morphologyEx(img0, imageF4, MORPH_CLOSE, element3);
	threshold(imageF4, imageF5, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);

	vector< vector<Point> > contours5; //轮廓数组
	std::vector<cv::Vec4i> hierarchy5;
	findContours(imageF5, contours5, hierarchy5, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //寻找轮廓，图片是黑底白字

	vector< vector<Point> >::iterator itr5;  //轮廓迭代器
	itr5 = contours5.begin();
	int iThreshold5 = 16;
	while (itr5 != contours5.end())
	{
		//绘制轮廓的最小外接矩形  
		Rect rect5 = boundingRect(*itr5);

		if (rect5.height > img0.rows * 4 / 5)
		{
			rect5.x = rect5.x - iThreshold5;
			rect5.y = rect5.y - iThreshold5;
			rect5.width = rect5.width + 2 * iThreshold5;
			rect5.height = rect5.height + 2 * iThreshold5;
			rectangle(binImageCopy, rect5, Scalar(255), CV_FILLED, 1);//用矩形画矩形窗   黑色
		}
		else if (rect5.height > img0.rows * 2 / 5)
		{
			rect5.x = rect5.x - iThreshold5 / 3;
			rect5.y = rect5.y - iThreshold5 / 3;
			rect5.width = rect5.width + 2 * iThreshold5 / 3;
			rect5.height = rect5.height + 2 * iThreshold5 / 3;
			rectangle(binImageCopy, rect5, Scalar(255), CV_FILLED, 1);//用矩形画矩形窗   黑色
		}

		itr5++;
	} //while (itr != contours.end())

	  //bitwise_or(imageF2, imageF5, imageF7);

	  //-----------------垂直投影-------------------------------------------------------
	Mat VerImgCut(1, img0.cols, CV_8UC1, cv::Scalar(0, 0, 0));
	VerImgCut = VerticalProjection(binImageCopy); //垂直投影

												  //创建并绘制垂直投影图像
	cv::Mat projImg(img0.rows, img0.cols, CV_8U, cv::Scalar(255));
	for (int col = 0; col < img0.cols; ++col)
	{
		cv::line(projImg, cv::Point(col, img0.rows - VerImgCut.at<ushort>(0, col)), cv::Point(col, img0.rows - 1), cv::Scalar::all(0));
	}
	//创建并绘制垂直投影图像
	//-----------------垂直投影-------------------------------------------------------

	//---------------确定每列的范围-开始结束位置--------------------------------------------
	vector<Rect> ivecRect;                                               //列矩形信息
	vector<int> ivecBegin;                                               //开始位置
	vector<int> ivecEnd;                                                 //结束位置
	Rect myRect;
	int icount = 0;                                                      //统计峰值数
	int ibegin = 666666;                                                 //开始位置
	int iend = 666666;                                                   //结束位置
	int minVal = 16;                                                     //判断的基准值
	int minRange = 30;                                                    //字符间分割的最小间隔
	int value = 0;
	for (int i = 0; i < VerImgCut.cols; i++)
	{
		value = VerImgCut.at<ushort>(0, i);
		if (value > minVal&&ibegin == 666666)
		{
			ibegin = i;                                                  //开始位置  
		}
		else if (value < minVal&&ibegin != 666666)
		{
			if (i - ibegin >= minRange)
			{
				icount++;                                                    //统计数加1
				iend = i;                                                    //结束位置
				ivecBegin.push_back(ibegin);                                 //加入容器
				ivecEnd.push_back(iend);                                     //加入容器
				myRect.x = ibegin - 2;
				myRect.y = 0;
				myRect.width = iend - ibegin + 4;
				myRect.height = img0.rows;
				ivecRect.push_back(myRect);
				ibegin = 666666;                                             //复位
				iend = 666666;                                               //复位

				if (bAuxiliaryCode)
				{
					rectangle(rgbImg, myRect, Scalar(0, 0, 255), 3, 1);//用矩形画矩形窗   黑色
				}

			}
			else
			{
				ibegin = 666666;                                             //复位
				iend = 666666;                                               //复位
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)
	   //---------------确定每列的范围-开始结束位置--------------------------------------------

	   //------------------精确定位每列位置-------------------------------------------------------
	for (int ia = 0; ia < ivecRect.size(); ia++)
	{
		if (ivecRect[ia].x < 120)
		{
			ivecRect[ia].width = ivecRect[ia].width + ivecRect[ia].x - 92;
			ivecRect[ia].x = 92;

		}
		Mat imgrect = binImageCopy(Rect(ivecRect[ia]));

		Mat HorImg = HorProjection(imgrect); //水平投影

											 //确定段落起始行
		int iStartRow = 0;                  //开始列
		int iEndRow = HorImg.rows - 1;      //结束列
		int iJudgeVal = 2;                  //判断值
		bool bStart = false;
		bool bEnd = false;
		for (int r = 0; r < HorImg.rows; r++)
		{
			int inewStartVal = HorImg.at<ushort>(r, 0); //正向
			if (inewStartVal > iJudgeVal && bStart == false)
			{
				iStartRow = r;
				bStart = true;
			}
			int inewEndVal = HorImg.at<ushort>(HorImg.rows - 1 - r, 0); //反向
			if (inewEndVal > iJudgeVal && bEnd == false)
			{
				iEndRow = HorImg.rows - r;
				bEnd = true;
			}
			if (bStart == true && bEnd == true)
			{
				break;
			}
		}
		//垂直投影，确定段落起始列
		ivecRect[ia].y = ivecRect[ia].y + iStartRow - 3;
		ivecRect[ia].height = iEndRow - iStartRow + 6;
		if (bAuxiliaryCode)
		{
			//画出轮廓外接矩形
			//rectangle(rgbImg3, ivecRect[ia], Scalar(0, 255, 0), 3, 1);//用矩形画矩形窗   黑色
		}
		//------------确定x和width-----------------

		int b = 0;
	}
	//------------------精确定位每列位置-------------------------------------------------------

	//------------------列切割-----------------------------------------------------------------
	Mat LeftImg(img0.size(), CV_8UC1, cv::Scalar(255));
	Mat RightImg(img0.size(), CV_8UC1, cv::Scalar(255));

	for (int ia = 0; ia < ivecRect.size(); ia++)
	{
		Mat imgrect = img0(Rect(ivecRect[ia]));

		//拷贝至左边
		if (ivecRect[ia].x + ivecRect[ia].width < 680)
		{
			Mat imageROI = LeftImg(Rect(ivecRect[ia]));

			imgrect.copyTo(imageROI, imgrect);
		}
		else //拷贝至右边
		{
			Mat imageROI = RightImg(Rect(ivecRect[ia]));

			imgrect.copyTo(imageROI, imgrect);
		}
		int b = 0;
	}

	//------------------列切割-----------------------------------------------------------------

	// 局部二值化 
	Mat binImageLeft, binImageRight;
	int blockSize = 151;
	int constValue = 35;
	cv::adaptiveThreshold(LeftImg, binImageLeft, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);
	cv::adaptiveThreshold(RightImg, binImageRight, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);

	Mat HorImgLeft = HorProjection(binImageLeft); //水平投影

	Mat HorImgRight = HorProjection(binImageRight); //水平投影

	if (bAuxiliaryCode)
	{
		//--------------创建并绘制垂直投影图像----------------------------------
		cv::Mat projImgLeft(binImageLeft.rows, binImageLeft.cols, CV_8U, cv::Scalar(255));

		for (int irow = 0; irow < binImageLeft.rows; ++irow)
		{
			cv::line(projImgLeft, cv::Point(binImageLeft.cols - HorImgLeft.at<ushort>(irow, 0), irow), cv::Point(binImageLeft.cols - 1, irow), cv::Scalar::all(0));
		}


		cv::Mat projImgRight(binImageRight.rows, binImageRight.cols, CV_8U, cv::Scalar(255));

		for (int irow = 0; irow < binImageRight.rows; ++irow)
		{
			cv::line(projImgRight, cv::Point(binImageRight.cols - HorImgRight.at<ushort>(irow, 0), irow), cv::Point(binImageRight.cols - 1, irow), cv::Scalar::all(0));
		}
		//--------------创建并绘制垂直投影图像----------------------------------
	}

	//---------------确定每行的范围-开始结束位置--------------------------------------------
	//----------------左边-----------------------
	vector<int> ivecBeginLeft;                                                //开始位置
	vector<int> ivecEndLeft;                                                 //结束位置
	int icountLeft = 0;                                                      //统计峰值数
	int ibeginLeft = 666666;                                                 //开始位置
	int iendLeft = 666666;                                                   //结束位置
	int minValLeft = 70;                                                     //判断的基准值
	int minRangeLeft = 5;                                                    //字符间分割的最小间隔
	int valueLeft = 0;
	for (int i = 0; i < HorImgLeft.rows; i++)
	{
		if (i > 895 && i < 960)
		{
			minValLeft = 80;
		}
		valueLeft = HorImgLeft.at<ushort>(i, 0);
		if (valueLeft > minValLeft&&ibeginLeft == 666666)
		{
			ibeginLeft = i;                                                  //开始位置  
		}
		else if (valueLeft < minValLeft&&ibeginLeft != 666666)
		{
			if (i - ibeginLeft >= minRangeLeft)
			{
				icountLeft++;                                                    //统计数加1
				iendLeft = i;                                                    //结束位置
				ivecBeginLeft.push_back(ibeginLeft);                                 //加入容器
				ivecEndLeft.push_back(iendLeft);                                     //加入容器
				ibeginLeft = 666666;                                             //复位
				iendLeft = 666666;                                               //复位
			}
			else
			{
				ibeginLeft = 666666;                                             //复位
				iendLeft = 666666;                                               //复位
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)

	vector<int> iVecLeft;
	int iValueLeft = 0;
	for (int iLeft = 0; iLeft < ivecBeginLeft.size(); iLeft++)
	{
		if (iLeft == 0)
		{
			iValueLeft = ivecBeginLeft[iLeft];
			iVecLeft.push_back(iValueLeft);
		}
		else
		{
			iValueLeft = (ivecBeginLeft[iLeft] + ivecEndLeft[iLeft - 1]) / 2;
			iVecLeft.push_back(iValueLeft);
		}
	}
	iValueLeft = ivecEndLeft[ivecEndLeft.size() - 1];
	iVecLeft.push_back(iValueLeft);

	//----------------右边-----------------------
	vector<int> ivecBeginRight;                                               //开始位置
	vector<int> ivecEndRight;                                                 //结束位置
	int icountRight = 0;                                                      //统计峰值数
	int ibeginRight = 666666;                                                 //开始位置
	int iendRight = 666666;                                                   //结束位置
	int minValRight = 62;                                                     //判断的基准值
	int minRangeRight = 10;                                                   //字符间分割的最小间隔
	int valueRight = 0;
	for (int i = 0; i < HorImgRight.rows; i++)
	{
		valueRight = HorImgRight.at<ushort>(i, 0);
		if (valueRight > minValRight&&ibeginRight == 666666)
		{
			ibeginRight = i;                                                  //开始位置  
		}
		else if (valueRight < minValRight&&ibeginRight != 666666)
		{
			if (i - ibeginRight >= minRangeRight)
			{
				icountRight++;                                                    //统计数加1
				iendRight = i;                                                    //结束位置
				ivecBeginRight.push_back(ibeginRight);                                 //加入容器
				ivecEndRight.push_back(iendRight);                                     //加入容器
				ibeginRight = 666666;                                             //复位
				iendRight = 666666;                                               //复位
			}
			else
			{
				ibeginRight = 666666;                                             //复位
				iendRight = 666666;                                               //复位
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)

	vector<int> iVecRight;
	int iValueRight = 0;
	for (int iRight = 0; iRight < ivecBeginRight.size(); iRight++)
	{
		if (iRight == 0)
		{
			iValueRight = ivecBeginRight[iRight];
			iVecRight.push_back(iValueRight);
		}
		else
		{
			iValueRight = (ivecBeginRight[iRight] + ivecEndRight[iRight - 1]) / 2;
			iVecRight.push_back(iValueRight);
		}
	}
	iValueRight = ivecEndRight[ivecEndRight.size() - 1];
	iVecRight.push_back(iValueLeft);
	//---------------确定每行的范围-开始结束位置--------------------------------------------


	//---------------排序-----------------------------------------------------------------
	//对istart到iend的信息按x排序
	sort(ivecRect.begin(), ivecRect.end(), MySort_x2);
	sort(iVecLeft.begin(), iVecLeft.end());
	sort(iVecRight.begin(), iVecRight.end());
	//---------------排序-----------------------------------------------------------------


	//---------------确定每个字符区域-------------------------------------------------------
	vector<stAncientBooksRect> iVecRectWorld;
	stAncientBooksRect MyRectWord;
	for (int ib = 0; ib < ivecRect.size(); ib++)
	{
		MyRectWord.x = ivecRect[ib].x;
		MyRectWord.width = ivecRect[ib].width;
		MyRectWord.iColID = ib;  //列序号

		if (MyRectWord.x + MyRectWord.width < 680)
		{
			for (int ic = 0; ic < iVecLeft.size() - 1; ic++)
			{
				MyRectWord.y = iVecLeft[ic];
				MyRectWord.height = iVecLeft[ic + 1] - iVecLeft[ic];
				MyRectWord.iRowID = ic;  //行序号


				Rect rectLeft;
				rectLeft.x = MyRectWord.x;
				rectLeft.y = MyRectWord.y;
				rectLeft.width = MyRectWord.width + 2;
				rectLeft.height = MyRectWord.height;

				Mat imgrect = binImageCopy(Rect(rectLeft));
				float fRate = ComputerblackRate(imgrect);  //计算黑点比例
				if (fRate > 0.01)
				{
					iVecRectWorld.push_back(MyRectWord);

					//画出轮廓外接矩形
					if (bAuxiliaryCode)
					{
						rectangle(rgbImg3, rectLeft, Scalar(0, 0, 255), 2, 1);//用矩形画矩形窗
					}
				}

			}
		}
		else
		{
			for (int id = 0; id < ivecBeginRight.size(); id++)
			{
				MyRectWord.y = iVecRight[id];
				MyRectWord.height = iVecRight[id + 1] - iVecRight[id];
				MyRectWord.iRowID = id;  //行序号

				Rect rectRight;
				rectRight.x = MyRectWord.x;
				rectRight.y = MyRectWord.y;
				rectRight.width = MyRectWord.width;
				rectRight.height = MyRectWord.height;

				Mat imgrect = binImageCopy(Rect(rectRight));
				float fRate = ComputerblackRate(imgrect);  //计算黑点比例
				if (fRate > 0.05)
				{
					iVecRectWorld.push_back(MyRectWord);

					//画出轮廓外接矩形
					if (bAuxiliaryCode)
					{
						rectangle(rgbImg3, rectRight, Scalar(0, 0, 255), 2, 1);//用矩形画矩形窗
					}
				}


			}
		}
	}
	//---------------确定每个字符区域-------------------------------------------------------

	//精确边界
	PreciseBoundary(iVecRectWorld, binImageCopy);

	//排序
	sort(iVecRectWorld.begin(), iVecRectWorld.end(), MySort_x3);


	//====================转成string====================================================
	string strResult; //结果字符串
	stringstream ssxmlResult;
	ssxmlResult << "<ImageName>" << imageName << "</ImageName>" << "\n";
	ssxmlResult << "<LR_Column>" << 0 << "</LR_Column>" << "\n";
	ssxmlResult << "<All_Txt>" << 1 << "</All_Txt>" << "\n";
	int iColID = iVecRectWorld[0].iColID;
	bool bChange = false;
	int iCount = 0;
	for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	{
		if (iColID == iVecRectWorld[ia].iColID&&iCount == 0)
		{
			ssxmlResult << "<iColID>" << iVecRectWorld[ia].iColID << "</iColID>" << "\n";
			iCount++;

		}
		else if (iColID != iVecRectWorld[ia].iColID)
		{
			iColID = iVecRectWorld[ia].iColID;
			iCount = 0;
			if (iColID == iVecRectWorld[ia].iColID&&iCount == 0)
			{
				ssxmlResult << "<iColID>" << iVecRectWorld[ia].iColID << "</iColID>" << "\n";
				iCount++;
			}

		}
		if (iColID == iVecRectWorld[ia].iColID)
		{
			if (iColID == 17)
			{
				iVecRectWorld[ia].width = iVecRectWorld[ia].width + 3;
			}
			if (iColID == 0 && iVecRectWorld[ia].iRowID == 14)
			{
				iVecRectWorld[ia].y = iVecRectWorld[ia].y - 5;
				iVecRectWorld[ia].height = iVecRectWorld[ia].height + 10;
			}
			if (iColID == 9 || iColID == 10 || iColID == 11 || iColID == 12 || iColID == 13 || iColID == 14)
			{
				if (iVecRectWorld[ia].iRowID == 14)
				{
					iVecRectWorld[ia].height = iVecRectWorld[ia].height + 8;
				}

			}
			ssxmlResult << "<rect id=" << iVecRectWorld[ia].iRowID + 1 << ">";
			ssxmlResult << iVecRectWorld[ia].x << "," << iVecRectWorld[ia].y << ","
				<< iVecRectWorld[ia].width << "," << iVecRectWorld[ia].height << "," << 1 << "</rect>" << "\n";
		}


	}
	strResult = ssxmlResult.str();
	//cout << strResult << endl;
	//====================转成string====================================================




	//-------------------画图------------------------------------------------
	////转换成彩色图
	//Mat rgbImg6;
	//cvtColor(img0, rgbImg6, COLOR_GRAY2RGB);

	//Rect MyRect;
	//for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	//{
	//	//画出轮廓外接矩形
	//	if (bAuxiliaryCode)
	//	{

	//		Rect rect;
	//		rect.x = iVecRectWorld[ia].x;
	//		rect.y = iVecRectWorld[ia].y;
	//		rect.width = iVecRectWorld[ia].width;
	//		rect.height = iVecRectWorld[ia].height;
	//		rectangle(rgbImg6, rect, Scalar(255, 0, 0), 2, 1);//用矩形画矩形窗
	//	}

	//}
	//-------------------画图------------------------------------------------

	////====存图==========================================================================
	//if (bAuxiliaryCode)
	//{
	//	cout << strResult << endl;

	//	Mat dst2;
	//	resize(rgbImg, dst2, Size(), 0.5, 0.5);  //缩放再存图减少存储空间
	//	string str = imageName; //去除末尾四个字符“.png”
	//	for (int i = 0; i < 4; i++)
	//	{
	//		imageName.pop_back();
	//	}
	//	stringstream ss;
	//	ss << "C:\\Users\\cnki\\Desktop\\11\\" << imageName << ".jpg";
	//	imwrite(ss.str(), dst2);
	//}
	////====存图==========================================================================
	//int a1 = 0;
	return strResult;
}

void main()
{
	bool bTotalImage = false;                        //所有图片是否读完

	CString strDir = "D:\\sxl\\处理图片\\11古籍行字切割";

	vector<CString> vFilePathList;
	get_dirs(strDir, vFilePathList);

	while (!bTotalImage)
	{
		for (int ia = 0; ia < vFilePathList.size(); ia++)
		{
			//cout << "ia:" << ia << ",Dirs:" << vFilePathList.size() << endl;
			sReadPath = CW2A(vFilePathList[ia].GetString());
			//#############################################
			//遍历文件夹内图像
			vector<String> fileNames;
			string strImgPath = sReadPath + "\\*.jpg";
			glob(strImgPath, fileNames, false);
			for (int i = 0; i < fileNames.size(); i++)
			{
				cout << i << "\t" << fileNames.size() << endl;
				string imageName = fileNames[i];     //图像名称，带后缀.jpg

				Mat image0, image;
				image0 = imread(imageName, 0);//读取图片 

				if (image0.data == 0)
				{
					return;
				}
				else
				{
					imageName = split2(imageName, '\\');//拆分字符串

					string strResult;
					strResult = AncientBooksText(image0, imageName); //文本定位
					cout << strResult << endl;
				}

			}//for (int i = 0; i < fileNames.size(); i++)
			 //#############################################
		} //for (int ia = 0; ia < vFilePathList.size(); ia++)

		  ////==========输出txt==============
		  //fout.close();                  //关闭文件
		  ////==========输出txt==============


		bTotalImage = true; //表示全部图片已经读完
	} //while (!bTotalImage)
	system("pause");
}

//=========================================================================================
//===========古文-文字区域定位=============================================================
//=========================================================================================



