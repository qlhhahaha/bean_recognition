
// camer_demoDlg.h: 头文件
//

#pragma once
#include "MVGigE.h"
#include "MVImage.h"
#include "camer_demoDoc.h"

#include "cdib.h"
#include <complex>
using namespace std;
#include <vector>
#include "afxwin.h"

struct  RGB
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

struct CENTER_POINT
{
	int x;
	int y;
	int radius;
};

// CcamerdemoDlg 对话框
class CcamerdemoDlg : public CDialogEx
{
// 构造
public:
	CcamerdemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMER_DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 特性
public:
	camer_demoDoc* GetDocument() const;


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	//定义相机句柄对象
	HANDLE m_hCam;
	//定义图像处理对象
	MVImage m_image;

	//定义灰度图（8位深度）处理对象
	MVImage m_grey_image;

	//定义像素格式对象
	MV_PixelFormatEnums m_PixelFormat;
	//采集标识
	bool m_bRun;
	//通过回调函数获取图像数据信息
	int OnStreamCB(MV_IMAGE_INFO* pInfo);
	//画图
	void DrawImage();
	//关闭相机、释放资源
	void OnClose();

	bool LoadBmp();

public:
	afx_msg void OnBnClickedOpencam();
	afx_msg void OnBnClickedStartgrab();
	afx_msg void OnBnClickedClosecam();
	afx_msg void OnBnClickedloadpicture();
	afx_msg void OnBnClickedsort();


	afx_msg void mark();
	afx_msg void two_value();
	afx_msg void fill_holes();
	afx_msg void shrink();
	afx_msg void find_center();
	afx_msg void count_cell();

	void process_fill_holes(int wd, int ht);
	void single_picture_draw();
	void single_grey_picture_draw();
	void GenEdge(void);
	void GenEdge4(void);
	void mark_it(int i, int j);
	void save_it(int i, int j, int radius);
	void calculate_center_area(int i, int j);
public:
	CPoint start_point;
	CPoint end_point;


	int image_width;
	int image_height;
	int image_line_byte;    //图像一行所占字节数  
	int image_bit_count;    //图像每个像素所占位数
	int image_memory_size; //图像所占内存大小
	unsigned char* image_pointer;//读入图片数据后的指针 

	const int TWOVALUE_H = 0x80;	// =TWOVALUE_H: set 1
	const int EDGEPOINT = 0x70;//|= set 1, & edge
	const int MARK_VISITED = 0x81;//& 
	const int MARKED = 0x80;//|=set & mark
	const int VISITED = 0x01;//|= set 1 ,& visited
	const int pre_shrink_count = 3;
	const int NO_MARK = 0x0f;//& set 0
	const int NO_EDGE_POINT = 0x8f;//&= noedge
	const int CENTERED = 0x2;//|= set 1 ,& center
	const int NO_CENTER = 0xfd;//&= nocenter
	const double Pi = 3.14159;
	const unsigned char TWOVALUE_L = 0x0;
	const int NO_VISITED = 0xfe;//&= novisited 
	bool center_points_flag;
	long total_area, total_x, total_y, max_radius;		// 用于递归
	vector<CENTER_POINT>	points_temp;// 用于临时存储CENTER_POINT
	vector<CENTER_POINT>	final_center_points;
};
