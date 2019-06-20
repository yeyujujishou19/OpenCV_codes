
#include "LayoutAnalysis.h"


//=========================================================================================
//===========test==========================================================================
//=========================================================================================

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

//去除大区域 
void RemoveBigRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;
	//新建一幅标签图像初始化为0像素点，为了记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查   
	//初始化的图像全部为0，未检查  
	Mat PointLabel = Mat::zeros(Src.size(), CV_8UC1);
	if (CheckMode == 1)//去除小连通区域的白色点  
	{
		//cout << "去除小连通域.";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) < 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//将背景黑色点标记为合格，像素为3  
				}
			}
		}
	}
	else//去除孔洞，黑色点像素  
	{
		//cout << "去除孔洞";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) > 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//如果原图是白色区域，标记为合格，像素为3  
				}
			}
		}
	}


	vector<Point2i>NeihborPos;//将邻域压进容器  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else int a = 0;//cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; i++)
	{
		for (int j = 0; j < Src.cols; j++)
		{
			if (PointLabel.at<uchar>(i, j) == 0)//标签图像像素点为0，表示还未检查的不合格点  
			{   //开始检查  
				vector<Point2i>GrowBuffer;//记录检查像素点的个数  
				GrowBuffer.push_back(Point2i(j, i));
				PointLabel.at<uchar>(i, j) = 1;//标记为正在检查  
				int CheckResult = 0;

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界    
						{
							if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer    
								PointLabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查    
							}
						}
					}
				}
				if (GrowBuffer.size()<AreaLimit) //判断结果（是否超出限定的大小），1为未超出，2为超出    
					CheckResult = 2;
				else
				{
					CheckResult = 1;
					RemoveCount++;//记录有多少区域被去除  
				}

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//标记不合格的像素点，像素值为2  
				}
				//********结束该点处的检查**********    
			}
		}
	}


	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域    
	for (int i = 0; i < Src.rows; ++i)
	{
		for (int j = 0; j < Src.cols; ++j)
		{
			if (PointLabel.at<uchar>(i, j) == 2)
			{
				Dst.at<uchar>(i, j) = CheckMode;
			}
			else if (PointLabel.at<uchar>(i, j) == 3)
			{
				Dst.at<uchar>(i, j) = Src.at<uchar>(i, j);

			}
		}
	}
	//cout << RemoveCount << " objects removed." << endl;
}
//定位文字区域
string ExtractText(Mat img0, string imageName)
{

	Mat binImage,binImageCopy;
	threshold(img0, binImage, 0, 255, CV_THRESH_OTSU);
	binImageCopy = binImage.clone();

	//转换成彩色图
	//Mat rgbImg(img0.size(), CV_8UC3);

	 //获取自定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(36, 2)); //第一个参数MORPH_RECT表示矩形的卷积核，当然还可以选择椭圆形的、交叉型的
	erode(binImage, binImage, element);//腐蚀操作，对白色部分腐蚀，对黑色部分膨胀

	RemoveBigRegion(binImage, binImage, 100000, 0, 1);    //去除较大的连通域

	binImage = 255 - binImage; //反色，黑底白字
	vector< vector<Point> > contours; //轮廓数组
	std::vector<cv::Vec4i> hierarchy;
	findContours(binImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //寻找轮廓，图片是黑底白字


	////转换成彩色图
	//Mat dstImage(img0.size(), CV_8UC3);
	//int index = 0;
	//for (; index >= 0; index = hierarchy[index][0]) {
	//	cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	cv::drawContours(dstImage, contours, index, color, 8, 8, hierarchy);
	//}
	
	//-----------------二值化处理并提取轮廓----------------------------------
	//--------存储到容器中-------------------
	vector< vector<Point> >::iterator itr;  //轮廓迭代器
	itr = contours.begin();

	vector <Rect> iRectVec; //矩形容器

	//if (bAuxiliaryCode)
	//{
	//	cvtColor(img0, rgbImg, COLOR_GRAY2BGR);
	//}
	while (itr != contours.end())
	{
		//绘制轮廓的最小外接矩形  
		Rect rect = boundingRect(*itr);

		if (rect.height > 42 && rect.height < 90 && rect.width>30 && rect.width<3000)//粗筛选
		{
			//计算黑点占矩形比例
			float fCount = 0;
			float fRate = 0;
			Mat imgrect = binImageCopy(rect);
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
			if (fRate > 0.15) //细筛选
			{
				if (rect.width / rect.height>3&&rect.width > 200 && ( (rect.x+rect.width)<img0.cols/8
						                || ((rect.x + rect.width)>img0.cols * 5 / 6 && (rect.y + rect.height)>img0.rows * 3 / 5)
						                || (rect.y + rect.height)>img0.rows * 3 / 5) ) //再筛选
				{
					if (rect.x > img0.cols * 8.5 / 10 && rect.y > img0.rows * 8.5 / 10)
					{

					}
					else
					{
						//if (bAuxiliaryCode)
						//{
						//	//画出轮廓外接矩形
						//	rectangle(rgbImg, rect, Scalar(0, 255, 0), 8, 1);//用矩形画矩形窗   黑色
						//}

						iRectVec.push_back(rect);
					}
				
				}
				else if(rect.width / rect.height>1.5 && rect.width <= 200 && ( (rect.x + rect.width)<img0.cols / 8
						                    || ((rect.x + rect.width)>img0.cols*5 / 6 && (rect.y + rect.height)>img0.rows * 3 / 5 )
						                    || (rect.y + rect.height)>img0.rows * 3 / 5 ))//再筛选
				{
					//if (bAuxiliaryCode)
					//{
					//	//画出轮廓外接矩形
					//	rectangle(rgbImg, rect, Scalar(0, 255, 0), 8, 1);//用矩形画矩形窗   黑色
					//}

					iRectVec.push_back(rect);
				}

			} //if (fRate > 0.15)

		} //if (rect.height > 30 && rect.height < 100 && rect.width>100 && rect.width<2500

		itr++;
	} //while (itr != contours.end())

	
	//====================转成string====================================================
	string strResult; //结果字符串
	stringstream ssxmlResult;
	ssxmlResult << "<ImageName>" << imageName << "</ImageName>" << "\n";
	ssxmlResult << "<LR_Column>" << 0 << "</LR_Column>" << "\n";
	ssxmlResult << "<All_Txt>" << 1 << "</All_Txt>" << "\n";
	for (int ia = 0; ia < iRectVec.size(); ia++)
	{
		ssxmlResult << "<rect id=" << ia+1 << ">";
		ssxmlResult << iRectVec[ia].x << "," << iRectVec[ia].y << ","
			<< iRectVec[ia].width << "," << iRectVec[ia].height << "," << 1 << "</rect>" << "\n";
	}
	strResult = ssxmlResult.str();
	cout << strResult << endl;
	//====================转成string====================================================


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
	//////====存图==========================================================================
	////int a1 = 0;

	return strResult;
}

void main()
{
	string sReadPath = "";          //读取 图像路径
	bool bTotalImage = false;                        //所有图片是否读完

	CString strDir = "D:\\sxl\\处理图片\\水校正新";

	vector<CString> vFilePathList;
	get_dirs(strDir, vFilePathList);

	while (!bTotalImage)
	{
		for (int ia = 0; ia < vFilePathList.size(); ia++)
		{
			sReadPath = CW2A(vFilePathList[ia].GetString());
			//#############################################
			//遍历文件夹内图像
			vector<String> fileNames;
			string strImgPath = sReadPath + "\\*.tif";
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
					strResult = ExtractText(image0, imageName); //文本定位，输入校正好的图像
				}

			}//for (int i = 0; i < fileNames.size(); i++)
			 //#############################################
		} //for (int ia = 0; ia < vFilePathList.size(); ia++)



		bTotalImage = true; //表示全部图片已经读完
	} //while (!bTotalImage)
	system("pause");
}