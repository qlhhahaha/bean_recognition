
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

	MVImage m_image_last;

	MVImage m_image_show;

	//定义灰度图（8位深度）处理对象
	MVImage m_grey_image;

	MVImage m_shrink_temp_image;

	MVImage m_shrink_end_image;

	MVImage m_dilation_temp_image;

	MVImage m_dilation_end_image;

	MVImage yellow_beans_image;

	MVImage green_beans_image;

	MVImage copy_yellow_beans_image;

	MVImage copy_green_beans_image;


	//定义像素格式对象
	MV_PixelFormatEnums m_PixelFormat;
	//采集标识
	bool m_bRun;
	//通过回调函数获取图像数据信息
	int OnStreamCB(MV_IMAGE_INFO* pInfo);

	void Draw_my_Image(MVImage* img,int draw_pic);

	void OnClose();
	//关闭相机、释放资源

	bool LoadBmp();

public:
	afx_msg void OnBnClickedOpencam();
	afx_msg void OnBnClickedStartgrab();
	afx_msg void OnBnClickedClosecam();
	afx_msg void OnBnClickedloadpicture();
	afx_msg void OnBnClickedsort();

	void calculate_center_area(int i, int j);

	void GenEdge(MVImage* image_input);
	void GenEdge4(MVImage* image_input);
	void shrink(MVImage* image_input);

	void binary_threshold(MVImage* image_input, int threshold);
	void another_shrink(MVImage* image_input, MVImage* image_output);
	void another_shrink_4(MVImage* image_input, MVImage* image_output);
	void image_move(MVImage* image_input, MVImage* image_output);
	void dilation(MVImage* image_input, MVImage* image_output);
	void dilation_4(MVImage* image_input, MVImage* image_output);
	void image_sub(MVImage* image_input1, MVImage* image_input2, MVImage* image_output);

	int image_compare(MVImage* image_input1, MVImage* image_input2);
	void rgb_image_copy(MVImage* image_input, MVImage* image_output);

	void count_beans(MVImage* image_input1, int mode);
	int Pythagorean(int x1, int y1, int x2, int y2);
	void paint_color(MVImage* image_input);
	void paint_circle(int x, int y, int r, int w, int h, unsigned char* p, int mode);

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
	const int NO_MARK = 0x7f;//& set 0
	const int NO_EDGE_POINT = 0x8f;//&= noedge
	const int CENTERED = 0x2;//|= set 1 ,& center
	const int NO_CENTER = 0xfd;//&= nocenter
	const double Pi = 3.14159;
	const unsigned char TWOVALUE_L = 0x0;
	const int NO_VISITED = 0xfe;//&= novisited 
	bool center_points_flag;
	long total_area, total_x, total_y, max_radius;		// 用于递归

	const int  my_binary_threshold = 40;
	const int  WHITE = 255;
	const int  BLACK = 0;
	const int  GREY = 128;
	const int  shrink_times = 25;//全图腐蚀，去除绿豆，得到黄豆图
	const int  dilation_times = 23;//腐蚀后的黄豆图，膨胀，为了去和原图相减，但是还有残留边
	const int  clean_times = 13;
	const int yellow_pre_shrink_times = 9;  //生成边界的那种收缩算法  
	const int green_pre_shrink_times = 0;
	const int COMPARE_THRESHOLD = 1200000;
	const int SAME_THRESHOLD = 10;
	int camera_frame = 0;
	int compare_flag = 0;

	vector<CENTER_POINT>	points_temp;// 用于临时存储CENTER_POINT
	vector<CENTER_POINT>	final_center_points;
};
