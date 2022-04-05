
// camer_demoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "camer_demo.h"
#include "camer_demoDlg.h"
#include "afxdialogex.h"
#include "MVImage.h"

#include "stdafx.h"
#include "camer_demoDoc.h"

#include <stack>
#include <vector>


#ifdef _DEBUG
#define new DEBUG_NEW

#endif

#define DISTANCE(x0,y0,x1,y1) sqrt((float)((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)))
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


#define BACKGROUND 0
#define BEAN 128
#define THRESHOLD_GREEN 15//消除绿豆噪点的阈值


int q[1500010], queue_front = 0, queue_rear = -1;//队列、队首、队尾
int q_xy[1500010][2], q_xy_num = 0;//队列里进入的点的xy坐标
int green_xy[1000][2], green_num = 0;
int yellow_xy[1000][2], yellow_num = 0;

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


// CcamerdemoDlg 对话框



CcamerdemoDlg::CcamerdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAMER_DEMO_DIALOG, pParent)
	, m_bRun(FALSE)
	, m_hCam(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcamerdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcamerdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OpenCam, &CcamerdemoDlg::OnBnClickedOpencam)
	ON_BN_CLICKED(IDC_StartGrab, &CcamerdemoDlg::OnBnClickedStartgrab)
	ON_BN_CLICKED(IDC_CloseCam, &CcamerdemoDlg::OnBnClickedClosecam)
	ON_BN_CLICKED(IDC_load_picture, &CcamerdemoDlg::OnBnClickedloadpicture)
	ON_BN_CLICKED(IDC_sort, &CcamerdemoDlg::OnBnClickedsort)


END_MESSAGE_MAP()


#ifdef _DEBUG
//camer_demoDoc* CcamerdemoDlg::GetDocument() const // 非调试版本是内联的
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(camer_demoDoc)));
//	return (camer_demoDoc*)m_pDocument;
//}
#endif //_DEBUG



// CcamerdemoDlg 消息处理程序

BOOL CcamerdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("函数库初始化失败"), _T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
	{
		MessageBox(_T("查找连接计算机的相机失败"),_T("提示"), MB_ICONWARNING);
		return TRUE;
	}
	GetDlgItem(IDC_OpenCam)->EnableWindow(true);
	GetDlgItem(IDC_load_picture)->EnableWindow(true);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CcamerdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CcamerdemoDlg::OnPaint()
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
HCURSOR CcamerdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CcamerdemoDlg::LoadBmp()
{
	
	return true;
}


void CcamerdemoDlg::OnBnClickedOpencam()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCams = 0;
	MVGetNumOfCameras(&nCams);
	if (nCams == 0)
	{
		MessageBox(_T("没有找到相机,请确认连接和相机IP设置"),_T("提示"),MB_ICONWARNING);
			return;
	}
	MVSTATUS_CODES r = MVOpenCamByIndex(0, &m_hCam);
	if (m_hCam == NULL)
	{
		if (r == MVST_ACCESS_DENIED)
			MessageBox(_T("无法打开相机，可能正被别的软件控制111"), _T("提示"),MB_ICONWARNING);
		else if (r == MVST_INVALID_PARAMETER)
			MessageBox(_T("相机id错误222"), _T("提示"), MB_ICONWARNING);
		else if (r == MVST_SUCCESS)
			MessageBox(_T("返回值为success 333"), _T("提示"), MB_ICONWARNING);
		else
		{
			CString msg;
			msg.Format(_T("error_code:%d\n"), r);
			MessageBox(msg);
			//MessageBox(_T("无法打开相机444") % r, _T("提示"), MB_ICONWARNING);
		}
		return;
	}
	else
	{
		CString msg;
		msg.Format(_T("m_hCam:%d\n"), &m_hCam);
		MessageBox(msg);
	}

	int width, h;

	MVGetWidth(m_hCam, &width);
	MVGetHeight(m_hCam, &h);
	MVGetPixelFormat(m_hCam, &m_PixelFormat);

	m_image.CreateByPixelFormat(width, h, m_PixelFormat);
	m_image_last.CreateByPixelFormat(width, h, m_PixelFormat);
	m_image_show.CreateByPixelFormat(width, h, m_PixelFormat);

	m_grey_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	m_shrink_temp_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	m_shrink_end_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	m_dilation_temp_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	m_dilation_end_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	m_shrink_temp_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	yellow_beans_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	green_beans_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	copy_yellow_beans_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	copy_green_beans_image.CreateByPixelFormat(width, h, PixelFormat_Mono8);
	

	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(true);
	GetDlgItem(IDC_sort)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CcamerdemoDlg::OnBnClickedloadpicture()
{
	// TODO: 在此添加控件通知处理程序代码

	m_image.Load("C:\\Users\\HUINAIZUI\\Desktop\\manybeans.bmp");
	m_grey_image.Load("C:\\Users\\HUINAIZUI\\Desktop\\bean8bit.bmp");
	//图像信息的获取
	image_width = m_image.GetWidth();
	image_height = m_image.GetHeight();
	image_line_byte = m_image.GetPitch();
	image_bit_count = m_image.GetBPP();
	image_pointer = (unsigned char*)m_image.GetBits();
	image_memory_size = image_height * image_line_byte;
	start_point.x = 0;
	end_point.x = image_width - 1;
	start_point.y = 0;
	end_point.y = image_height - 1;
	//以上都是CcamerdemoDlg类成员变量
	m_grey_image.CreateByPixelFormat(image_width, image_height, PixelFormat_Mono8);


	CString msg;
	msg.Format(TEXT("image_width=%d,image_height=%d,image_line_byte=%d,image_memory_size=%d,image_bit_count=%d"), image_width, image_height, image_line_byte, image_memory_size, image_bit_count);
	MessageBox(msg);

	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(true);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CcamerdemoDlg::Draw_my_Image(MVImage* img, int draw_pic)
{
	CRect rct;
	GetDlgItem(draw_pic)->GetClientRect(&rct);
	int dstW = rct.Width();
	int dstH = rct.Height();
	CDC* pDC = GetDlgItem(draw_pic)->GetDC();
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		img->Draw(pDC->GetSafeHdc(), 0, 0, dstW, dstH);
	}
	ReleaseDC(pDC);
}

int CcamerdemoDlg::image_compare(MVImage* image_input1, MVImage* image_input2)
{
	int width, height;
	width = image_input1->GetWidth();
	height = image_input1->GetHeight();

	unsigned char* lp_src1 = NULL;
	unsigned char* lp_src2 = NULL;
	lp_src1 = (unsigned char*)image_input1->GetBits();
	lp_src2 = (unsigned char*)image_input2->GetBits();

	int count_same_point = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (abs((*lp_src1 - *lp_src2)) <= SAME_THRESHOLD &&
				abs(*(lp_src1 + 1) - *(lp_src2 + 1)) <= SAME_THRESHOLD  &&
				abs(*(lp_src1 + 2) - *(lp_src2 + 2)) <= SAME_THRESHOLD)
			{
				count_same_point++; //不变的点
			}
			lp_src1 += 3; //24bit位深的RGB图
			lp_src2 += 3;
		}
	}

	if (count_same_point >= COMPARE_THRESHOLD) 
		return 1; 
	else 
		return 0;

}

void CcamerdemoDlg::rgb_image_copy(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;
	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < 3 * width; j++)
		{
			unsigned char temp;
			temp = *lp_src;
			*lp_dst = temp;
			lp_src++;
			lp_dst++;
		}
	}
}



int CcamerdemoDlg::OnStreamCB(MV_IMAGE_INFO* pInfo)
{
	MVInfo2Image(m_hCam, pInfo, &m_image);

	int compare_result = image_compare(&m_image, &m_image_last);
	//返回1说明图像与前一刻相比没变，0说明变了

	if (camera_frame == 0) //第0帧直接复制完成初始化即可
	{
		camera_frame++;
		rgb_image_copy(&m_image, &m_image_last);
		compare_flag = 0;
	}
	else if (compare_result == 0)
	//图像变了，则将compare_flag置零，意味着在下次进入不变状态会触发图像处理
	{
		camera_frame++;
		if (camera_frame == 30)
			camera_frame = 1;

		rgb_image_copy(&m_image, &m_image_last);

		compare_flag = 0;
	}
	else //图像没变
	{
		camera_frame++;
		if (camera_frame == 30)
			camera_frame = 1;

		rgb_image_copy(&m_image, &m_image_last);
		if (compare_flag == 0)
		{
			OnBnClickedsort();
		}
		compare_flag = 1; 
		//图像处理完后记得置1，即只有“从变到不变时”才会触发图像处理，持续不变时不会触发
	}
	
	rgb_image_copy(&m_image, &m_image_show);
	//get_final(&m_image_show);

	paint_color(&m_image_show);

	Draw_my_Image(&m_image_show, pic);


	//DrawImage(pic);
	return 0;
}


//StreamCB的主要作用是调用MVInfo2Image()和DrawImage()两个函数来将此前获得的图像信息转化为图像并显示
//创建的图像是一个m_image类的实例
int __stdcall StreamCB(MV_IMAGE_INFO* pInfo, ULONG_PTR nUserVal)
{
	CcamerdemoDlg* pDlg = (CcamerdemoDlg*)nUserVal;
	return (pDlg->OnStreamCB(pInfo));
}

void CcamerdemoDlg::OnBnClickedStartgrab()
{
	// TODO: 在此添加控件通知处理程序代码
	TriggerModeEnums enumMode;
	MVGetTriggerMode(m_hCam, &enumMode);
	if (enumMode != TriggerMode_Off) //不为连续采集，触发模式开，相机等待软触发或外触发信号再采集图像
	{
		MessageBox(_T("不为连续采集，触发模式开"), _T("提示"), MB_ICONWARNING);
		MVSetTriggerMode(m_hCam, TriggerMode_Off);  //强行设置为连续采集模式
		Sleep(100);
	}
	MVStartGrab(m_hCam, (MVStreamCB)StreamCB, (ULONG_PTR)this);
	m_bRun = true;

	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);
	//图像信息的获取
	image_width = m_image.GetWidth(); 
	image_height = m_image.GetHeight();
	image_line_byte = m_image.GetPitch();
	image_bit_count = m_image.GetBPP();
	image_pointer = (unsigned char*)m_image.GetBits();
	image_memory_size = image_height * image_line_byte;
	start_point.x = 0;
	end_point.x = image_width - 1;
	start_point.y = 0;
	end_point.y = image_height - 1;

	CString msg;
	msg.Format(TEXT("image_width=%d,image_height=%d,image_line_byte=%d,image_memory_size=%d,image_bit_count=%d"), image_width, image_height, image_line_byte, image_memory_size, image_bit_count);
	MessageBox(msg);

	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(true);
	GetDlgItem(IDC_CloseCam)->EnableWindow(true);
}

void CcamerdemoDlg::shrink(MVImage* image_input)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	lp_src = (unsigned char*)image_input->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// 去掉边界!
			if (*lp_src & EDGEPOINT) //条件为真说明灰度值为240，即为豆子边界
				(*lp_src) &= NO_MARK;//灰度值改为0，纯黑，即去掉边界
				
			lp_src++;
		}
	}
}

//豆子内部值为128不变，边界为240(白色)
void CcamerdemoDlg::GenEdge(MVImage* image_input)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	lp_src = (unsigned char*)image_input->GetBits();

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			*lp_src &= NO_EDGE_POINT;//not edge
			if (*lp_src & MARKED)//条件为真说明*lp_src的最高位是1，即该点灰度值≥128（事实上只能为128或240），即为豆子内部或边界
			{
				if (j == start_point.y || i == start_point.x || j == end_point.y || i == end_point.x)
					*lp_src |= EDGEPOINT;//*lp_src=240

				else if (!((*(lp_src - image_line_byte - 1) & MARKED)
					&& (*(lp_src - image_line_byte) & MARKED)
					&& (*(lp_src - image_line_byte + 1) & MARKED)
					&& (*(lp_src - 1) & MARKED)
					&& (*(lp_src + 1) & MARKED)
					&& (*(lp_src + image_line_byte - 1) & MARKED)
					&& (*(lp_src + image_line_byte) & MARKED)
					&& (*(lp_src + image_line_byte + 1) & MARKED)
					))//该点为豆子的边界(即不满足“八邻域皆为128”)
					*lp_src |= EDGEPOINT;
			}
			lp_src++;
		}
	}

}


//判断条件改成四邻域
void CcamerdemoDlg::GenEdge4(MVImage* image_input)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	lp_src = (unsigned char*)image_input->GetBits();

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			*lp_src &= NO_EDGE_POINT;//not edge
			if (*lp_src & MARKED)//条件为真说明*lp_src的最高位是1，即该点灰度值≥128（事实上只能为128或240），即为豆子内部或边界
			{
				if (j == start_point.y || i == start_point.x || j == end_point.y || i == end_point.x)
					*lp_src |= EDGEPOINT;//*lp_src=240

				else if(!((*(lp_src - image_line_byte) & MARKED)
						&& (*(lp_src - 1) & MARKED)
						&& (*(lp_src + 1) & MARKED)
						&& (*(lp_src + image_line_byte) & MARKED))
						)
					*lp_src |= EDGEPOINT;
			}
			lp_src++;
		}
	}
}

//计算该坨坨点的质点与最大半径并去除其中心点标志
void CcamerdemoDlg::calculate_center_area(int i, int j)
{
	unsigned char* lp_src;
	lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + i;

	if (j == start_point.y || j == end_point.y || i == start_point.x || i == end_point.x) // 最边上的不用处理
	{
		return;
	}

	total_area++; //点个数代表面积
	total_x += i;
	total_y += j;
	*lp_src &= NO_CENTER;

	for (unsigned int n = 0; n < points_temp.size(); n++)
	{
		if (points_temp.at(n).x == i && points_temp.at(n).y == j)
		{
			if (points_temp.at(n).radius > max_radius)
				max_radius = points_temp.at(n).radius;
			break; //(i,j)点只有一个，找到后直接break就行
		}
	}

	if (*(lp_src - 1) & CENTERED)
	{
		calculate_center_area(i - 1, j);
	}
	if (*(lp_src + 1) & CENTERED)
	{
		calculate_center_area(i + 1, j);
	}
	if (*(lp_src + image_line_byte) & CENTERED)
	{
		calculate_center_area(i, j - 1);
	}
	if (*(lp_src - image_line_byte) & CENTERED)
	{
		calculate_center_area(i, j + 1);
	}

	if (*(lp_src - image_line_byte + 1) & CENTERED)
	{
		calculate_center_area(i + 1, j + 1);
	}
	if (*(lp_src - image_line_byte - 1) & CENTERED)
	{
		calculate_center_area(i - 1, j + 1);
	}
	if (*(lp_src + image_line_byte + 1) & CENTERED)
	{
		calculate_center_area(i + 1, j - 1);
	}
	if (*(lp_src + image_line_byte - 1) & CENTERED)
	{
		calculate_center_area(i - 1, j - 1);
	}
}

void CcamerdemoDlg::binary_threshold(MVImage* image_input, int threshold)
{
	int width, height;
	unsigned char* lp_src = NULL;
	
	width = image_input->GetWidth(); 
	height = image_input->GetHeight(); 
	lp_src = (unsigned char*)image_input->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (*(lp_src) <= threshold)
			{
				*lp_src = 128; //内部灰色
			}
			else
			{
				*lp_src = 0; //背景黑色
			}
			lp_src++;
		}
	}

}

void CcamerdemoDlg::another_shrink(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;

	width = image_input->GetWidth();
	height = image_input->GetHeight();

	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < 1 || i >= height - 1 || j < 1 || j >= width - 1)
			{
				*lp_dst = BLACK;
				lp_src++;
				lp_dst++;
				continue;
			}
			if (*lp_src == BLACK) //背景维持不变
				*lp_dst = BLACK;
			else if (*(lp_src - 1) == BLACK || 
				*(lp_src + 1) == BLACK ||
				*(lp_src - width - 1) == BLACK || 
				*(lp_src - width) == BLACK || 
				*(lp_src - width + 1) == BLACK ||
				*(lp_src + width - 1) == BLACK || 
				*(lp_src + width) == BLACK || 
				*(lp_src + width + 1) == BLACK )
					*lp_dst = BLACK;
			else *lp_dst = GREY;
			lp_src++;
			lp_dst++;
		}
	}
}

void CcamerdemoDlg::another_shrink_4(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;

	width = image_input->GetWidth();
	height = image_input->GetHeight();

	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < 1 || i >= height - 1 || j < 1 || j >= width - 1)
			{
				*lp_dst = BLACK;
				lp_src++;
				lp_dst++;
				continue;
			}
			if (*lp_src == BLACK) //背景维持不变
				*lp_dst = BLACK;
			else if (*(lp_src - 1) == BLACK ||
				*(lp_src + 1) == BLACK ||
				*(lp_src - width) == BLACK ||
				*(lp_src + width) == BLACK )
				*lp_dst = BLACK;
			else *lp_dst = GREY;
			lp_src++;
			lp_dst++;
		}
	}
}

void CcamerdemoDlg::image_move(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_output->GetHeight();
	unsigned char temp;
	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;
	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			
			temp = *lp_src;
			*lp_dst = temp;
			lp_src++;
			lp_dst++;
		}
	}

}

void CcamerdemoDlg::dilation(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;
	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < 1 || i >= height - 1 || j < 1 || j >= width - 1)
			{
				*lp_dst = BLACK;
				lp_src++;
				lp_dst++;
				continue;
			}
			if (*lp_src == GREY)
				*lp_dst = GREY;

			else if (*(lp_src - 1) == GREY || 
				*(lp_src + 1) == GREY ||
				*(lp_src - width - 1) == GREY || 
				*(lp_src - width) == GREY || 
				*(lp_src - width + 1) == GREY ||
				*(lp_src + width - 1) == GREY || 
				*(lp_src + width) == GREY || 
				*(lp_src + width + 1) == GREY )
					*lp_dst = GREY;
			else *lp_dst = BLACK;
			lp_src++;
			lp_dst++;
		}
	}

}

void CcamerdemoDlg::dilation_4(MVImage* image_input, MVImage* image_output)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	unsigned char* lp_dst = NULL;
	lp_src = (unsigned char*)image_input->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < 1 || i >= height - 1 || j < 1 || j >= width - 1)
			{
				*lp_dst = BLACK;
				lp_src++;
				lp_dst++;
				continue;
			}
			if (*lp_src == GREY)
				*lp_dst = GREY;

			else if (*(lp_src - 1) == GREY ||
				*(lp_src + 1) == GREY ||
				*(lp_src - width) == GREY ||
				*(lp_src + width) == GREY )
				*lp_dst = GREY;
			else *lp_dst = BLACK;
			lp_src++;
			lp_dst++;
		}
	}

}

void CcamerdemoDlg::image_sub(MVImage* image_input1, MVImage* image_input2, MVImage* image_output)
{
	int width, height;
	width = image_input1->GetWidth();
	height = image_input1->GetHeight();

	unsigned char* lp_src1 = NULL;
	unsigned char* lp_src2 = NULL;
	unsigned char* lp_dst = NULL;

	lp_src1 = (unsigned char*)image_input1->GetBits();
	lp_src2 = (unsigned char*)image_input2->GetBits();
	lp_dst = (unsigned char*)image_output->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (((*lp_src1) + (*lp_src2)) == 128)
			{
				*lp_dst = 128; //找出绿豆
			}
			else
			{
				*lp_dst = 0;
			}
			lp_src1++;
			lp_src2++;
			lp_dst++;
		}
	}

}

int CcamerdemoDlg::Pythagorean(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void CcamerdemoDlg::count_beans(MVImage* image_input, int mode)
{
	queue_front = 0;//队首
	queue_rear = -1;//队尾初值

	//模式0数绿豆，模式1数黄豆
	if (mode == 0) 
		green_num = 0;
	else if (mode == 1)
		yellow_num = 0;

	int dir[4][2] = { {-1,0},{0,-1},{1,0},{0,1} }; //BFS搜索方向为左、上、右、下

	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	lp_src = (unsigned char*)image_input->GetBits();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if ((*lp_src) == BEAN)
			{
				//*lp_src  = BACKGROUND;
				int bit_serial_num = i * width + j;
				q[++queue_rear] = bit_serial_num; //搜到豆子点就将其坐标入队，队尾指针加1
				q_xy_num = 0;

				//队列未空时进入while，出while时已将该连通域内的点坐标全记录q_xy[][]中，并将该连通域涂黑了
				while (queue_front <= queue_rear)
				{
					int q_serial_num = q[queue_front];
					int x = q_serial_num / width;
					int y = q_serial_num % width;

					queue_front++; //坐标被记录完后就可以出队了

					q_xy[q_xy_num][0] = x;
					q_xy[q_xy_num][1] = y;
					q_xy_num++;

					for (int k = 0; k < 4; k++)
					{
						//一个比较巧妙的地方是：用int型的xy坐标来表示下一个点，进行BFS，而不是让图像指针移动
						int next_x = x + dir[k][0];
						int next_y = y + dir[k][1];
						int next_serial_num = next_x * width + next_y;

						//遇背景点和边界点则停止，换下一个方向继续搜
						if (next_x < 0 || next_x >= height || next_y < 0 || next_y >= width)
							continue;
						if (*(lp_src - bit_serial_num + next_serial_num) == BACKGROUND)
							continue; 

						//若找到豆子点，则直接将其变为背景点（比打VISITED标签要简单）
						*(lp_src - bit_serial_num + next_serial_num) = BACKGROUND;
						q[++queue_rear] = next_serial_num;
					}

				}

				if (mode == 0 && q_xy_num >= THRESHOLD_GREEN)
				{
					double sum_x = 0;
					double sum_y = 0;
					for (int i = 0; i < q_xy_num; i++)
					{
						sum_x += q_xy[i][0];
						sum_y += q_xy[i][1];
					}
					sum_x /= q_xy_num;
					sum_y /= q_xy_num;

					/*int flag = 0;
					for (int i = 0; i < green_num; i++)
					{
						if (Pythagorean(int(sum_x), int(sum_y), green_xy[i][0], green_xy[i][1]) <= THRESHOLD_GREEN)
						{
							flag = 1;
							break;
						}
					}
					if (flag)continue;*/

					green_xy[green_num][0] = int(sum_x);
					green_xy[green_num][1] = int(sum_y);
					green_num++;
				}
				else if (mode == 1)
				{
					double sum_x = 0;
					double sum_y = 0;
					for (int i = 0; i < q_xy_num; i++)
					{
						sum_x += q_xy[i][0];
						sum_y += q_xy[i][1];
					}
					sum_x /= q_xy_num;
					sum_y /= q_xy_num;
					yellow_xy[yellow_num][0] = int(sum_x);
					yellow_xy[yellow_num][1] = int(sum_y);
					yellow_num++;
				}
			}
			lp_src++;
		}
	}
}

void CcamerdemoDlg::paint_circle(int x, int y, int r, int w, int h, unsigned char* lp_src, int mode)
{
	//绿豆的mode为1，黄豆为2
	int ym;
	int xymax = int(r / 1.414); //45°线与圆的交点横坐标(取整)
	int sp = 0;

	for (int i = 0; i <= xymax; i++)
	{	//i从0到0.707r，代表选取45°到90°的1/8圆
		sp = 0;
		int j = 0;

		while (i * i + j * j - r * r <= 0)
		{//找到圆的边界
			j++;
		}
		
		/*
		判断边界内外侧哪一个更接近圆的坐标：
		其与半径的差值分别为d1=|i²+(ym-1)²-r²|和d2=|i²+ym²-r²|，且已知绝对值符号内d1为负，d2为正。(因为分别位于圆的内外两侧)
		我们需要他们中的较小者作为有效点坐标，因此我们可以判断他们差值的符号d2-d1=2i²+2ym²-2ym+1-2r²如果为负就选取(i，ym)，反之同理。
		由于一般横纵坐标都是远大于1的，因此上式顺理成章简化为D2-D1=i²+ym²-ym-r²。同理判断符号即可
		*/
		ym = (i * i + j * j - r * r - j < 0) ? j : (j - 1);

		//4个for实现按列填充
		for (int k = -i; k <= i; k++)
		{
			sp = ((x + k) * w + y - ym) * 3; //rgb图像，记得乘3
			if (mode == 1)
			{
				*(lp_src + sp) = 0;
				*(lp_src + sp + 1) = 255;
				*(lp_src + sp + 2) = 255;
			}
			else if (mode == 2)
			{
				*(lp_src + sp) = 255;
				*(lp_src + sp + 1) = 0;
				*(lp_src + sp + 2) = 255;
			}	
		}

		for (int k = -i; k <= i; k++)
		{
			sp = ((x + k) * w + y + ym) * 3;
			if (mode == 1)
			{
				*(lp_src + sp) = 0;
				*(lp_src + sp + 1) = 255;
				*(lp_src + sp + 2) = 255;
			}
			else if (mode == 2)
			{
				*(lp_src + sp) = 255;
				*(lp_src + sp + 1) = 0;
				*(lp_src + sp + 2) = 255;
			}
		}

		for (int k = -ym; k <= ym; k++)
		{
			sp = ((x + k) * w + y - i) * 3;
			if (mode == 1)
			{
				*(lp_src + sp) = 0;
				*(lp_src + sp + 1) = 255;
				*(lp_src + sp + 2) = 255;
			}
			else if (mode == 2)
			{
				*(lp_src + sp) = 255;
				*(lp_src + sp + 1) = 0;
				*(lp_src + sp + 2) = 255;
			}
		}

		for (int k = -ym; k <= ym; k++)
		{
			sp = ((x + k) * w + y + i) * 3;
			if (mode == 1)
			{
				*(lp_src + sp) = 0;
				*(lp_src + sp + 1) = 255;
				*(lp_src + sp + 2) = 255;
			}
			else if (mode == 2)
			{
				*(lp_src + sp) = 255;
				*(lp_src + sp + 1) = 0;
				*(lp_src + sp + 2) = 255;
			}
		}
	}
}

void CcamerdemoDlg::paint_color(MVImage* image_input)
{
	int width, height;
	width = image_input->GetWidth();
	height = image_input->GetHeight();

	unsigned char* lp_src = NULL;
	lp_src = (unsigned char*)image_input->GetBits();

	for (int i = 0; i < green_num; i++)
	{
		int x = green_xy[i][0];
		int y = green_xy[i][1];

		paint_circle(x, y, 15, width, height, lp_src, 1);
	}

	for (int i = 0; i < yellow_num; i++)
	{
		int x = yellow_xy[i][0];
		int y = yellow_xy[i][1];

		paint_circle(x, y, 25, width, height, lp_src, 2);
	}
}

void CcamerdemoDlg::OnBnClickedsort()
{
	// 灰度图转换与阈值二值化
	MVImageBGRToGray(m_hCam, &m_image, &m_grey_image);
	binary_threshold(&m_grey_image, my_binary_threshold);

	
	//把小的绿豆腐蚀掉，最后得到的m_shrink_end_image为被腐蚀过的纯黄豆图
	another_shrink(&m_grey_image, &m_shrink_temp_image);
	for (int i = 1; i <= shrink_times; i++)
	{ //交替使用四邻域和八邻域腐蚀，以使腐蚀后的形状更接近于圆形，经验证效果很好，下方的膨胀同理
		if (i % 2 == 0)
		{
			another_shrink(&m_shrink_temp_image, &m_shrink_end_image);
			image_move(&m_shrink_end_image, &m_shrink_temp_image);
		}
		else
		{
			another_shrink_4(&m_shrink_temp_image, &m_shrink_end_image);
			image_move(&m_shrink_end_image, &m_shrink_temp_image);
		}
	}



	/*
	数黄豆个数：
	为啥要在这儿数呢？
	因为现在是经腐蚀的黄豆图，连通域更小，数起来性能消耗更少，没必要把黄豆膨胀为原图后再数
	*/
	image_move(&m_shrink_end_image, &copy_yellow_beans_image);
	count_beans(&copy_yellow_beans_image, 1);


	//大的黄豆膨胀恢复，最后得到的yellow_beans_image为复原的纯黄豆图
	dilation(&m_shrink_end_image, &m_dilation_temp_image);
	for (int i = 1; i <= dilation_times; i++)
	{
		if (i % 2 == 0)
		{
			dilation(&m_dilation_temp_image, &m_dilation_end_image);
			image_move(&m_dilation_end_image, &m_dilation_temp_image);
		}
		else
		{
			dilation_4(&m_dilation_temp_image, &m_dilation_end_image);
			image_move(&m_dilation_end_image, &m_dilation_temp_image);
		}
	}

	image_move(&m_dilation_end_image, &yellow_beans_image);




	//原图与纯黄豆图相减，得绿豆图
	image_sub(&m_grey_image, &m_dilation_end_image, &green_beans_image);

	Draw_my_Image(&green_beans_image, pic2);

	//膨胀得到的黄豆不可能与原图完全吻合，会有一些残余边，所以还要再腐蚀几次将其清理
	another_shrink(&green_beans_image, &m_shrink_temp_image);
	for (int i = 1; i <= clean_times; i++)
	{
		if (i % 2 == 0)
		{
			another_shrink(&m_shrink_temp_image, &green_beans_image);
			image_move(&green_beans_image, &m_shrink_temp_image);
		}
		else
		{
			another_shrink_4(&m_shrink_temp_image, &green_beans_image);
			image_move(&green_beans_image, &m_shrink_temp_image);
		}
	}

	//Draw_my_Image(&green_beans_image, pic2);
	
	//在数豆子之前先预腐蚀几次，以减少数豆程序的性能消耗
	//ps.事实上经验证，豆子数目小于200时无明显性能瓶颈，green_pre_shrink_times暂设为0
	for (int k = 0; k < green_pre_shrink_times; k++)
	{
		if (k%2 ==0)
			GenEdge(&green_beans_image);
		else
			GenEdge4(&green_beans_image);

		shrink(&green_beans_image);
	}

	//Draw_my_Image(&m_shrink_temp_image, pic2);
	//Draw_my_Image(&green_beans_image, pic2);

	count_beans(&green_beans_image, 0);

	CString str[3];
	str[0].Format(_T("豆子总数：%d"), yellow_num+ green_num);
	GetDlgItem(text3)->SetWindowText((str[0]));

	str[1].Format(_T("黄豆：%d"), yellow_num);
	GetDlgItem(text2)->SetWindowText((str[1]));

	str[2].Format(_T("绿豆：%d"), green_num);
	GetDlgItem(text1)->SetWindowText((str[2]));

}


void CcamerdemoDlg::OnBnClickedClosecam()
{
	// TODO: 在此添加控件通知处理程序代码
	MVStopGrab(m_hCam);
	MVCloseCam(m_hCam);
	m_bRun = false;

	GetDlgItem(IDC_OpenCam)->EnableWindow(true);
	GetDlgItem(IDC_load_picture)->EnableWindow(true);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}

void CcamerdemoDlg::OnClose()
{
	if (m_bRun != false)
	{
		MVStopGrab(m_hCam);
	}
	MVTerminateLib();
	CDialog::OnClose();
}




