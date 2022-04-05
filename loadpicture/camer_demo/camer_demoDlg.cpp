
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

	int w, h;
	MVGetWidth(m_hCam, &w);
	MVGetHeight(m_hCam, &h);


	MVGetPixelFormat(m_hCam, &m_PixelFormat);
	m_image.CreateByPixelFormat(w, h, m_PixelFormat);
	m_grey_image.CreateByPixelFormat(w, h, PixelFormat_Mono8);


	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(true);
	GetDlgItem(IDC_sort)->EnableWindow(false);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CcamerdemoDlg::OnBnClickedloadpicture()
{
	// TODO: 在此添加控件通知处理程序代码

	m_image.Load("D:\\HUST_COURSE\\模式识别与图像处理综合实验\\bean_recognition\\pictures\\manybeans.bmp");
	m_grey_image.Load("D:\\HUST_COURSE\\模式识别与图像处理综合实验\\bean_recognition\\pictures\\bean8bit.bmp");
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

	single_picture_draw();

	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(true);
	GetDlgItem(IDC_CloseCam)->EnableWindow(false);
}


void CcamerdemoDlg::single_picture_draw()
{
	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);

	CDC* pDC = GetDlgItem(pic)->GetDC();
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		m_image.Draw(pDC->GetSafeHdc(), 0, 0, 0.5 * image_width, 0.5 * image_height);
		//m_image.Draw(pDC->GetSafeHdc(), 0, 0, image_width, image_height);
	}
	ReleaseDC(pDC);
}

void CcamerdemoDlg::single_grey_picture_draw()
{
	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);

	CDC* pDC = GetDlgItem(pic)->GetDC();
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		m_grey_image.Draw(pDC->GetSafeHdc(), 0, 0, 0.5 * image_width, 0.5 * image_height);
		//m_grey_image.Draw(pDC->GetSafeHdc(), 0, 0,image_width,  image_height);
	}
	ReleaseDC(pDC);
}

void CcamerdemoDlg::DrawImage()
{
	CRect rct;
	GetDlgItem(pic)->GetClientRect(&rct);
	int dstW = rct.Width();
	int dstH = rct.Height();
	CDC* pDC = GetDlgItem(pic)->GetDC();
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		m_image.Draw(pDC->GetSafeHdc(), 0, 0, dstW, dstH);
	}
	ReleaseDC(pDC);
}

int CcamerdemoDlg::OnStreamCB(MV_IMAGE_INFO* pInfo)
{
	MVInfo2Image(m_hCam, pInfo, &m_image);
	DrawImage();
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
	GetDlgItem(IDC_OpenCam)->EnableWindow(false);
	GetDlgItem(IDC_load_picture)->EnableWindow(false);
	GetDlgItem(IDC_StartGrab)->EnableWindow(false);
	GetDlgItem(IDC_sort)->EnableWindow(true);
	GetDlgItem(IDC_CloseCam)->EnableWindow(true);
}

void CcamerdemoDlg::mark()
{
	unsigned char* lp_src;

	// 指向目标图像对应象素的指针
	unsigned char* lp_dst;

	unsigned char* lp_new_bits;

	lp_new_bits = new unsigned char[image_memory_size];
	if (lp_new_bits == NULL)
	{
		// 分配内存失败
		return;
	}

	lp_src = (unsigned char*)image_pointer;
	memcpy(lp_new_bits, lp_src, image_memory_size);

	// 针对图像每行进行操作
	for (int i = start_point.y; i < end_point.y; i++)
	{
		// 针对每行图像每列进行操作
		for (int j = start_point.x; j < end_point.x; j++)
		{
			// 指向image第i行，第j个象素的指针
			lp_src = image_pointer + image_line_byte * (image_height - 1 - i) + j * 3;
			RGB rgb;
			rgb.b = *lp_src; rgb.g = *(lp_src + 1); rgb.r = *(lp_src + 2);
		
			double x1 = rgb.r;//0—255
		
			if (x1 < 40) 
			{//内部
				*lp_src = 0; *(lp_src + 1) = 0; *(lp_src + 2) = 255;//red
			}
			else if (x1 < 100 && x1>40) 
			{//边界
				*lp_src = 255; *(lp_src + 1) = 0; *(lp_src + 2) = 0;//Blue
			}
			else 
			{//为了避开后面判断时对背景误判，将其值略作改变
				if (*lp_src == 0)	*lp_src = 1;//Mark判断
				else if (*lp_src == 255)	*lp_src = 254;//maybe mark判断
				else if (*(lp_src + 2) == 128)	*(lp_src+2) = 127;
				if (*(lp_src + 1) == 255)	*(lp_src + 1) = 254; //edge判断
			}
		}
	}

		//MessageBox(TEXT("Mark(green) & maybe Mark(Blue) "));
		//single_picture_draw();

		const int M = 5;  //5*5窗口滤波
		bool bdelete;
		//filter 
		for (int i = start_point.y + M; i < end_point.y - M; i++)//
		{
			// 针对每行图像每列进行操作
			for (int j = start_point.x + M; j < end_point.x - M; j++)
			{
				lp_dst = image_pointer + (long)image_line_byte * ((long)image_height - 1 - i) + (long)j * 3;
				if (*(lp_dst + 1) == 255)//edge 
				{
					bdelete = true;
					for (int m = -M; m <= M; m++)
						for (int n = -M; n <= M; n++)
						{
							if (m == -M || m == M || n == -M || n == M) {
								if (*(lp_dst + (long)image_line_byte * m + (long)n * 3) || (*(lp_dst + (long)image_line_byte * m + (long)n * 3 + 1) == 255))//noMark && no Edge
								{
									bdelete = false;
									m = M + 1; 
									n = M + 1;//out
								}
							}
						}
					if (bdelete)
						*(lp_dst + 1) = 0;//delete edge
				}
			}
		}

		delete[] lp_new_bits;
}

void CcamerdemoDlg::two_value()
{
	unsigned char* lp_src;

	unsigned char* lp_dst;

	unsigned char* lp_grey_dst;

	long int image_new_line_bits = (image_width + 3) / 4 * 4;  //保证每一行字节数都是4的倍数

	unsigned char* lp_new_bits;

	lp_new_bits = new unsigned char[image_height * image_new_line_bits];
	if (lp_new_bits == NULL)
	{
		// 分配内存失败
		return;
	}

	for (int i = 0; i < image_height; i++) 
	{
		for (int j = 0; j < image_width; j++) 
		{
			lp_src = image_pointer + image_line_byte * (image_height - 1 - i) + j * 3;
			lp_dst = lp_new_bits + image_new_line_bits * (image_height - 1 - i) + j; 
			//lp_dst会用来处理8bit位深的灰度图，所以j不用乘3

			unsigned char v = 0;
			if (*(lp_src) == 0)//Mark
			{
				v = TWOVALUE_H;
			}
			*lp_dst = (unsigned char)v;
		}
	}
	image_line_byte = image_new_line_bits;

	camer_demoDoc* pDoc = new camer_demoDoc();
	pDoc->m_pDib = new CDib(CSize(image_new_line_bits, image_height), 8);
	image_pointer = pDoc->m_pDib->m_lpImage;
	image_height = pDoc->m_pDib->m_lpBMIH->biHeight;
	image_width = pDoc->m_pDib->m_lpBMIH->biWidth;
	image_bit_count = pDoc->m_pDib->m_lpBMIH->biBitCount;
	lp_src = (unsigned char*)pDoc->m_pDib->m_lpvColorTable;


	//CString msg;
	//msg.Format(TEXT("image_width=%d,image_height=%d,image_line_byte=%d,image_memory_size=%d,image_bit_count=%d"), image_width, image_height, image_new_line_bits, image_memory_size, image_bit_count);
	//MessageBox(msg);

	image_pointer = (unsigned char*)m_grey_image.GetBits();
	memcpy(image_pointer, lp_new_bits, image_new_line_bits * image_height);

	//single_grey_picture_draw();


	delete pDoc;
	delete[]lp_new_bits;
}


void CcamerdemoDlg::shrink()
{
	CString msg;
	const int pre_shrink_count = 13;    // 先去掉pre_shrink_count层皮
	unsigned char* lp_src;

	GenEdge(); //生成豆子的边界(白色，240)
	//MessageBox("GenEdge finished");
	//single_grey_picture_draw();

	for (int k = 0; k < pre_shrink_count; k++)
	{
		for (int i = start_point.y; i <= end_point.y; i++)
		{
			lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - i) + start_point.x - 1;
			for (int j = start_point.x; j <= end_point.x; j++)
			{
				lp_src++;

				// 去掉边界!
				if (*lp_src & EDGEPOINT) //条件为真说明灰度值为240，即为豆子边界
					(*lp_src) &= NO_MARK;//灰度值改为0，纯黑，即去掉边界
			}
		}

		//不管是GenEdge还是GenEdge4，送进去的都是豆子无边界的图
		if (k % 2 == 0)
			GenEdge();
		else
			GenEdge4();
	}

	//MessageBox("shrinking finished");
	//single_grey_picture_draw();
}

//豆子内部值为128不变，边界为240(白色)
void CcamerdemoDlg::GenEdge()
{
	unsigned char* lp_src;

	for (int j = start_point.y; j <= end_point.y; j++)
	{
		lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x -1;
		for (int i = start_point.x; i <= end_point.x; i++)
		{
			lp_src++;
			*lp_src &= NO_EDGE_POINT;//not edge
			if (*lp_src & MARKED)//条件为真说明*lp_src的最高位是1，即该点灰度值≥128（事实上只能为128或240），即为豆子内部或边界
			{
				if (j == start_point.y || i == start_point.x || j == end_point.y || i == end_point.x)	
					*lp_src |= EDGEPOINT;//*lp_src=240

				else if ( !((*(lp_src - image_line_byte - 1) & MARKED)
							&& (*(lp_src - image_line_byte) & MARKED)
							&& (*(lp_src - image_line_byte + 1) & MARKED)
							&& (*(lp_src - 1) & MARKED)
							&& (*(lp_src + 1) & MARKED)
							&& (*(lp_src + image_line_byte - 1) & MARKED)
							&& (*(lp_src + image_line_byte) & MARKED)
							&& (*(lp_src + image_line_byte + 1) & MARKED)
							) )//该点为豆子的边界(即不满足“八邻域皆为128”)
					*lp_src |= EDGEPOINT;
			}

		}
	}
}

//判断条件改成四邻域
void CcamerdemoDlg::GenEdge4()
{
	unsigned char* lp_src;

	for (int j = start_point.y; j <= end_point.y; j++)
	{
		lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
		for (int i = start_point.x; i <= end_point.x; i++)
		{
			lp_src++;
			*lp_src &= NO_EDGE_POINT;
			if (*lp_src & MARKED)
			{
				if (j == start_point.y || i == start_point.x || j == end_point.y || i == end_point.x)
				{
					*lp_src |= EDGEPOINT;
				}

				else if       
					( !  ((*(lp_src - image_line_byte) & MARKED)
							&& (*(lp_src - 1) & MARKED)
							&& (*(lp_src + 1) & MARKED)
							&& (*(lp_src + image_line_byte) & MARKED))
						) 
					*lp_src |= EDGEPOINT;
			}

		}
	}
}

/*******************************************************************************************************************
寻找中心算法的思想：
在for循环内不断进行边缘腐蚀，每次收缩一圈,期间不断用mark_it()对边界进行检验：
若为普通的边界点，则center_points_flag在DFS的一通猛如虎的递归操作中，总会变为false，无法进一步触发save_it()；
而若该边界点是经收缩得到的中心点(灰度值也为240，可看作“只有边界的点”)，则center_points_flag为true，触发save_it()

这种算法有一个明显的问题：
实际上触发save_it()的不一定是单个像素的中心点，也有可能为一坨240边界点（只要其中没包含灰色内部点就行）

解决方案：
在save_it()中也用了DFS，若为一坨240边界点，则会将其全部标记为中心点
之后再对所有的中心点作平均操作，把这种坨坨点给去掉
**********************************************************************************************************************/
void CcamerdemoDlg::find_center()
{
	CString msg;

	CENTER_POINT pt;

	points_temp.clear();
	bool changed;
	unsigned char* lp_src;
	for (int k = 0; k < 100; k++) // 标志中心点的腐蚀	
	{
		changed = false;

		// 清除visited标志
		for (int j = start_point.y; j <= end_point.y; j++)
		{
			lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
			for (int i = start_point.x; i <= end_point.x; i++)
			{
				lp_src++;
				*lp_src &= NO_VISITED;
			}
		}

		for (int j = start_point.y; j <= end_point.y; j++)
		{
			lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
			for (int i = start_point.x; i <= end_point.x; i++)
			{
				lp_src++;
				if (j > start_point.y && j<end_point.y && i>start_point.x && i < end_point.x) // 最边上的不用处理
				{
					center_points_flag = true;
					if (*lp_src & EDGEPOINT && !(*lp_src & VISITED))	// 没有访问过的边界
					{
						if (!(*(lp_src - 1) & MARKED) &&
							!(*(lp_src + 1) & MARKED) &&
							!(*(lp_src + image_line_byte) & MARKED) &&
							!(*(lp_src - image_line_byte) & MARKED)) //四邻域都为背景黑点，即这是个孤立点
						{//这个孤立点判断主要是为了鲁棒性，但在豆子题中几乎没用，注释掉也不影响
							if (k <= 2) //噪音
							{
								continue;
							}

							*lp_src |= CENTERED; 
							// 保存一下CENTER_POINT信息，边界点的灰度值从240变为了242
							pt.x = i;
							pt.y = j;
							pt.radius = k + pre_shrink_count + 4;//circle adjust 
							points_temp.push_back(pt); //将该点加入容器

							continue;
						}
						else
							mark_it(i, j); 

						if (center_points_flag)
							save_it(i, j, k + pre_shrink_count + 4);
						//用像素值表示半径，每收缩一圈认为半径减1，所以要加上k
					}
				}
			}
		}

		for (int j = start_point.y; j <= end_point.y; j++)
		{
			lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
			for (int i = start_point.x; i <= end_point.x; i++)
			{
				lp_src++;
	
				if (*lp_src & EDGEPOINT)
				{
					changed = true;
					*lp_src &= NO_MARK; // 去掉边界
				}
			}
		}
		if (k % 2 == 0)
			GenEdge4();
		else
			GenEdge();

		//如果changed为false，则说明图像被腐蚀得一个边界点都不剩了，提前退出循环
		//经检验，一般20次循环内就能把图像全腐蚀完，k=100其实完全用不上
		if (!changed)
			break;

		//msg.Format(TEXT("shrink=%d"), k);
		//MessageBox(msg);
		//single_grey_picture_draw();
	}

	//msg.Format(TEXT("获得的中心点数目= %d"), points_temp.size());
	//MessageBox(msg);
	//single_grey_picture_draw();


	// 取平均值,获得中心点
	vector<CENTER_POINT> points;

	for (int j = start_point.y; j <= end_point.y; j++)
	{
		lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
		for (int i = start_point.x; i <= end_point.x; i++)
		{
			lp_src++;
			if (j > start_point.y && j<end_point.y && i>start_point.x && i < end_point.x) // 最边上的不用处理
			{
				if (*lp_src & CENTERED)
				{	// 孤立的中心点直接入栈
					if (!(*(lp_src - 1) & CENTERED) &&
						!(*(lp_src + 1) & CENTERED) &&
						!(*(lp_src + image_line_byte) & CENTERED) &&
						!(*(lp_src - image_line_byte) & CENTERED) &&
						!(*(lp_src + image_line_byte - 1) & CENTERED) &&
						!(*(lp_src + image_line_byte + 1) & CENTERED) &&
						!(*(lp_src - image_line_byte - 1) & CENTERED) &&
						!(*(lp_src - image_line_byte + 1) & CENTERED))
					{
						pt.x = i;
						pt.y = j;
						for (unsigned int n = 0; n < points_temp.size(); n++)
						{
							if (points_temp.at(n).x == i && points_temp.at(n).y == j)
							{
								pt.radius = points_temp.at(n).radius;
								break;
							}
						}
						points.push_back(pt);
						continue;
					}
					else
					{   //多个中心点求其质点
						total_area = 0;
						max_radius = 0;
						total_x = 0;
						total_y = 0;
						calculate_center_area(i, j);   //认为最大半径才是这颗豆子真正的半径值   
						pt.x = total_x / total_area;
						pt.y = total_y / total_area;
						pt.radius = max_radius;
						*(lp_src - (pt.y - j) * image_line_byte + pt.x - i) |= CENTERED;
						points.push_back(pt);
					}
				}
			}
		}

	}


	//msg.Format(TEXT("取平均值,获得中心点数目= %d"), points.size());
	//MessageBox(msg);
	//single_grey_picture_draw();



	CDC* pdc = GetDlgItem(pic)->GetDC();

	CPen Redpen;
	Redpen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));

	CPen Redpen1;
	Redpen1.CreatePen(PS_DOT, 3, RGB(255, 0, 0));

	CPen Greenpen;
	Greenpen.CreatePen(PS_DOT, 1, RGB(0, 255, 0));

	CPen Bluepen;
	Bluepen.CreatePen(PS_DOT, 1, RGB(0, 0, 255));

	CPen Bluepen1;
	Bluepen1.CreatePen(PS_DOT, 3, RGB(0, 0, 255));

	final_center_points.clear();
	int x0, y0;
	bool adj;

	// 清除center标志
	for (int j = start_point.y; j <= end_point.y; j++)
	{
		lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + start_point.x - 1;
		for (int i = start_point.x; i <= end_point.x; i++)
		{
			lp_src++;
			*lp_src &= NO_CENTER;
		}
	}

	for (unsigned int i = 0; i < points.size(); i++)
	{
		x0 = points.at(i).x;
		y0 = points.at(i).y;
		pt = points.at(i);

		adj = false;

		//相近
		pdc->SelectObject(Redpen);
		for (unsigned int j = i + 1; j < points.size() - 1; j++)
		{
			int x = points.at(j).x;
			int y = points.at(j).y;
			if (abs(x0 - x) + abs(y0 - y) < 15) // 圆心距离相近
			{
				points.at(i).x = (x + x0) / 2;
				points.at(i).y = (y + y0) / 2;
				points.at(i).radius = (points.at(i).radius + points.at(j).radius) / 2;

				pt = points.at(j);

				//用红笔画出与之相近的那个点
				//Arc(pdc->m_hDC,
				//	pt.x - pt.radius + 3,
				//	pt.y - pt.radius + 3,
				//	pt.x + pt.radius - 3,
				//	pt.y + pt.radius - 3,
				//	pt.x + pt.radius - 3,
				//	pt.y - 3,
				//	pt.x + pt.radius - 3,
				//	pt.y - 3);

				points.erase(points.begin() + j);//&points.at(j));
				i--;
				adj = true;
				break;
			}
		}
		if (!adj) // 非相近
		{
			if (points.at(i).radius > 3)
			{
				final_center_points.push_back(points.at(i));
				*(image_pointer + image_line_byte * (image_height - 1 - points.at(i).y) + points.at(i).x) |= CENTERED;
				if (i % 5 == 0 && i)	TRACE("\n");
				TRACE("%3d:(%3d %3d)--%2d\t", i, points.at(i).x, points.at(i).y, points.at(i).radius);
				//display position
				pdc->SelectObject(Greenpen);

				//Arc(pdc->m_hDC,
				//	pt.x - pt.radius,
				//	pt.y - pt.radius,
				//	pt.x + pt.radius,
				//	pt.y + pt.radius,
				//	pt.x + pt.radius,
				//	pt.y,
				//	pt.x + pt.radius,
				//	pt.y
				//);

			}
		}
	}
	//msg.Format(TEXT("平均化相近的中心点后数目= %d"), final_center_points.size());
	//MessageBox(msg);
	//single_grey_picture_draw();

	DeleteObject(Redpen);
	DeleteObject(Greenpen);
	DeleteObject(Bluepen);
}

// 能够进入这个函数的全是非孤立的、没有访问过的边界点，其灰度值为240
void CcamerdemoDlg::mark_it(int i, int j)
{
	unsigned char* lp_src;

	lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + i;

	*(lp_src) |= VISITED; //灰度值从240变为241，标记为访问过

	if (j == start_point.y || j == end_point.y || i == start_point.x || i == end_point.x) // 最边上的不用处理
	{
		return;
	}

	//对8邻域进行DFS搜索
	if (!(*(lp_src - 1) & VISITED) &&	// 没有访问过
		*(lp_src - 1) & MARKED)		// 标志了
	{
		if (*(lp_src - 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i - 1, j); // 左边
		else
			center_points_flag = false;
	}

	if (!(*(lp_src + 1) & VISITED) &&	// 没有访问过
		*(lp_src + 1) & MARKED)		// 标志了
	{
		if (*(lp_src + 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i + 1, j); // 右边
		else
			center_points_flag = false;
	}

	if (!(*(lp_src + image_line_byte) & VISITED) &&	// 没有访问过
		*(lp_src + image_line_byte) & MARKED)	// 标志了
	{
		if (*(lp_src + image_line_byte) & EDGEPOINT)		// 并且是边缘
			mark_it(i, j - 1); // 上面
		else
			center_points_flag = false;
	}

	if (!(*(lp_src - image_line_byte) & VISITED) &&	// 没有访问过
		*(lp_src - image_line_byte) & MARKED)	// 标志了
	{
		if (*(lp_src - image_line_byte) & EDGEPOINT)		// 并且是边缘
			mark_it(i, j + 1); // 下面
		else
			center_points_flag = false;
	}

	if (!(*(lp_src + image_line_byte - 1) & VISITED) &&	// 没有访问过
		*(lp_src + image_line_byte - 1) & MARKED)		// 标志了
	{
		if (*(lp_src + image_line_byte - 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i - 1, j - 1); // 左上
		else
			center_points_flag = false;
	}

	if (!(*(lp_src - image_line_byte - 1) & VISITED) &&	// 没有访问过
		*(lp_src - image_line_byte - 1) & MARKED)		// 标志了
	{
		if (*(lp_src - image_line_byte - 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i - 1, j + 1); // 左下
		else
			center_points_flag = false;
	}

	if (!(*(lp_src + image_line_byte + 1) & VISITED) &&	// 没有访问过
		*(lp_src + image_line_byte + 1) & MARKED)		// 标志了
	{
		if (*(lp_src + image_line_byte + 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i + 1, j - 1); // 右上
		else
			center_points_flag = false;
	}

	if (!(*(lp_src - image_line_byte + 1) & VISITED) &&	// 没有访问过
		*(lp_src - image_line_byte + 1) & MARKED)		// 标志了
	{
		if (*(lp_src - image_line_byte + 1) & EDGEPOINT)		// 并且是边缘
			mark_it(i + 1, j + 1); // 右下
		else
			center_points_flag = false;
	}
}

void CcamerdemoDlg::save_it(int i, int j, int radius)
{
	unsigned char* lp_src;
	lp_src = (unsigned char*)image_pointer + image_line_byte * (image_height - 1 - j) + i;

	if (j == start_point.y || j == end_point.y || i == start_point.x || i == end_point.x) // 最边上的不用处理
		return;

	if (!(*lp_src & CENTERED)) //若未被标记为中心点
	{
		CENTER_POINT pt;
		pt.x = i;
		pt.y = j;
		pt.radius = radius;
		points_temp.push_back(pt); //入栈

		*lp_src |= CENTERED; //标记为中心点
	}

	*lp_src &= NO_VISITED;
	//访问过才进栈！去掉该语句会导致多次进栈
	//ps：只有边界点才有VISITED状态

	if (*(lp_src - 1) & VISITED)
	{
		save_it(i - 1, j, radius);
	}
	if (*(lp_src + 1) & VISITED)
	{
		save_it(i + 1, j, radius);
	}
	if (*(lp_src + image_line_byte) & VISITED)
	{
		save_it(i, j - 1, radius);
	}
	if (*(lp_src - image_line_byte) & VISITED)
	{
		save_it(i, j + 1, radius);
	}

	if (*(lp_src - image_line_byte + 1) & VISITED)
	{
		save_it(i + 1, j + 1, radius);
	}
	if (*(lp_src + image_line_byte - 1) & VISITED)
	{
		save_it(i - 1, j + 1, radius);
	}
	if (*(lp_src - image_line_byte + 1) & VISITED)
	{
		save_it(i + 1, j - 1, radius);
	}
	if (*(lp_src - image_line_byte - 1) & VISITED)
	{
		save_it(i - 1, j - 1, radius);
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

void CcamerdemoDlg::count_cell()
{
	CString msg;

	double total_area = 0, total_radius = 0, aver_area = 0, aver_radius = 0;
	int yellow_beans = 0, green_beans = 0;

	const float accuracy_coefficient = 1.1; //乘个倍数，让结果更精确，经验值

	for (unsigned int i = 0; i < final_center_points.size(); i++)
	{
		//final_center_points.at(i).x *= 0.5;
		//final_center_points.at(i).y *= 0.5;
		//final_center_points.at(i).radius *= 0.5;
		total_area += (double)final_center_points.at(i).radius * (double)final_center_points.at(i).radius; 
		//反正也不是真的要知道面积，double类型的常数pai就不乘了，少点运算量

		total_radius += final_center_points.at(i).radius;
	}
	aver_area = (int)(total_area / final_center_points.size() + .5);
	aver_radius = (int)(total_radius / final_center_points.size() + .5);

	//single_picture_draw();
	//Invalidate(true);
	//OnBnClickedloadpicture();


	CDC* pdc = GetDlgItem(pic)->GetDC();

	CPen yellow_pen;
	yellow_pen.CreatePen(PS_DOT, 1, RGB(0, 0, 255));
	CPen green_pen;
	green_pen.CreatePen(PS_DOT, 1, RGB(0, 255, 0));

	for (unsigned int i = 0; i < final_center_points.size(); i++) 
	{
		if (final_center_points.at(i).radius > aver_radius * accuracy_coefficient) 
		{
			yellow_beans += 1;
			pdc->SelectObject(yellow_pen);
			//八个几何参数中，前四个参数为外接矩形的左上、右下坐标，后四个参数为弧线的起点终点坐标（逆时针画弧线）
			Arc(pdc->m_hDC,
				0.5 * (final_center_points.at(i).x - final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y) - final_center_points.at(i).radius),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y) + final_center_points.at(i).radius),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y)),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y)));
			/*
			按道理来说，这里的纵坐标应该就是final_center_points.at(i).y，但出于某种神秘原因，画出来的圈上下全是颠倒的。。
			所以用end_point.y与之相减，以毒攻毒。。
			反正不是啥大问题，先这样，能work就是王道
			*/
		}
		else
		{
			green_beans += 1;
			pdc->SelectObject(green_pen);
			//八个几何参数中，前四个参数为外接矩形的左上、右下坐标，后四个参数为弧线的起点终点坐标（逆时针画弧线）
			//Arc(pdc->m_hDC,
			//	final_center_points.at(i).x - final_center_points.at(i).radius,
			//	(end_point.y - final_center_points.at(i).y) - final_center_points.at(i).radius,
			//	final_center_points.at(i).x + final_center_points.at(i).radius,
			//	(end_point.y - final_center_points.at(i).y) + final_center_points.at(i).radius,
			//	final_center_points.at(i).x + final_center_points.at(i).radius,
			//	(end_point.y - final_center_points.at(i).y),
			//	final_center_points.at(i).x + final_center_points.at(i).radius,
			//	(end_point.y - final_center_points.at(i).y));
			Arc(pdc->m_hDC,
				0.5 * (final_center_points.at(i).x - final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y) - final_center_points.at(i).radius),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y) + final_center_points.at(i).radius),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y)),
				0.5 * (final_center_points.at(i).x + final_center_points.at(i).radius),
				0.5 * ((end_point.y - final_center_points.at(i).y)));
		}
	}

	CString str[5];
	str[0].Format(_T("豆子总数：%d"), final_center_points.size());
	GetDlgItem(total_number)->SetWindowText((str[0]));

	str[1].Format(_T("黄豆：%d"), yellow_beans);
	GetDlgItem(yellow_beans_number)->SetWindowText((str[1]));

	str[2].Format(_T("绿豆：%d"), green_beans);
	GetDlgItem(green_beans_number)->SetWindowText((str[2]));

	str[3].Format(_T("平均半径：%d"), (int)aver_radius);
	GetDlgItem(average_radius)->SetWindowText((str[3]));

	str[4].Format(_T("平均面积：%d"), (int)aver_area);
	GetDlgItem(average_area)->SetWindowText((str[4]));


	DeleteObject(yellow_pen);
	DeleteObject(green_pen);
}

void CcamerdemoDlg::OnBnClickedsort()
{
	// TODO: 在此添加控件通知处理程序代码
	mark();
	two_value();
	//fill_holes();
	shrink();
	find_center();
	count_cell();
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




