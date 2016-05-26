#define PI 3.1415926
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long				LONG;
typedef BYTE* PBYTE;
typedef void *PVOID;
typedef PVOID HANDLE;
typedef void *LPVOID;


// 调色板项
typedef struct tag_RGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} _RGBQUAD;

// bmp头信息
typedef struct tag_BITMAPINFOHEADER{
	DWORD      biSize;	//指出本数据结构所需要的字节数
	LONG       biWidth;	//宽度
	LONG       biHeight;	//高度
	WORD       biPlanes;	//输出设备的位平面数，为1
	WORD       biBitCount;//给出每个像素的位数，1 4，8，24，32
	DWORD      biCompression;//位图的压缩类型
	DWORD      biSizeImage;	//图像字节数的多少
	LONG       biXPelsPerMeter;//图像水平分辨率
	LONG       biYPelsPerMeter;//图像垂直分辨率
	DWORD      biClrUsed;	//调色板中图像实际使用的颜色数
	DWORD      biClrImportant;//给出重要颜色的索引值
} _BITMAPINFOHEADER;

// bmp信息
typedef struct tag_BITMAPINFO {
	_BITMAPINFOHEADER    bmiHeader;
	_RGBQUAD             bmiColors[1];
} _BITMAPINFO;

typedef struct RESULT {
	int width;
	int height;
	PBYTE data;
} RESULT;

// 裁剪的矩形
typedef struct tag_RECT {

	int X;

	int Y;

	int width;

	int height;

}_RECT;
typedef struct POX {
	int x;
	int y;
}POX;
#include <pshpack2.h>
//bmp文件头
typedef struct tag_BITMAPFILEHEADER {
	WORD    bfType;			//位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
	DWORD   bfSize;			//BMP图像文件的大小
	WORD    bfReserved1;	//0
	WORD    bfReserved2;	//0
	DWORD   bfOffBits;	//BMP图像数据的地址
} _BITMAPFILEHEADER;

// 完整的BMP图信息
typedef struct tagBMPINFOMATION {
	_BITMAPINFOHEADER bih;		// bmp头信息
	BYTE *data;				// 图像信息
	BYTE *pData24;				// 图像信息
	int nComponent;				//3 彩色/1 灰度
	int nAdjust; // 用于字节对齐
	int nAdjust24; // 用于字节对齐
}BMPINFOMATION;



/***********************************************
*存储bmp格式文件 方式1
**************************************************/
void saveAsBmp(BMPINFOMATION bmp, char *strDestFileName){
	_BITMAPFILEHEADER bfh;		// bmp文件头
	_BITMAPINFOHEADER bih;		// bmp头信息
	//FILE *fp2;
	//fp2 = fopen("f:\\2.txt","w");
	//fputs("1\n",fp2);
	bih = bmp.bih;
	int nAdjust; // 用于字节对齐
	BYTE *data= NULL;//new BYTE[bih.biWidth*bih.biHeight];
	data = bmp.pData24;
	int nComponent = 3;
	FILE *f;
	nAdjust = bmp.bih.biWidth*3%4;
	if (nAdjust) 
		nAdjust = 4-nAdjust;
	//printf("new:%d\n",nAdjust);
	f=fopen(strDestFileName,"wb");
	if (f==NULL){
		delete [] data;
		//delete [] pDataConv;
		return;
	}
	// 写文件头
	memset(&bfh,0,sizeof(bfh));
	bfh.bfSize = sizeof(bfh)+sizeof(bih);
	bfh.bfOffBits = sizeof(bfh)+sizeof(bih);

	bfh.bfSize += (bih.biWidth*3+nAdjust)*bih.biHeight;
	bfh.bfType = 0x4d42;
	//fprintf(fp2,"w:%d,h:%d\n",bih.biWidth,bih.biHeight);
	fwrite(&bfh,sizeof(bfh),1,f);
	fwrite(&bih,sizeof(bih),1,f);
	fwrite(data,(bih.biWidth*3+nAdjust)*bih.biHeight,1,f);
	fclose(f);
	//fclose(fp2);
	delete [] data;
}

//保存成一张jpg压缩图
void saveAsJpg(char *strDestFileName,BMPINFOMATION bmp) {
	_BITMAPINFOHEADER bih;		// bmp头信息
	_RGBQUAD rq[256];			// 调色板
	int i=0,j=0;
	int withSize;//每行宽度
	int nAdjust; // 用于字节对齐
	nAdjust = bmp.nAdjust;
	BYTE *pData24 = NULL;//new BYTE[bih.biWidth*bih.biHeight];
	int nComponent = 0;
	bih = bmp.bih;
	pData24 = bmp.pData24;
	nComponent = bmp.nComponent;

	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr jem;
	FILE *f;
	jcs.err = jpeg_std_error(&jem);

	jpeg_create_compress(&jcs);

	f=fopen(strDestFileName,"wb");
	if (f==NULL) {
		delete [] pData24;
		return;
	}

	jpeg_stdio_dest(&jcs, f);
	jcs.image_width = bih.biWidth ; 			// 为图的宽和高，单位为像素 
	jcs.image_height = bih.biHeight;
	jcs.input_components = nComponent;			// 1,表示灰度图， 如果是彩色位图，则为3 

	if (nComponent==1)
		jcs.in_color_space = JCS_GRAYSCALE; //JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像 
	else 
		jcs.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jcs);	

	// 设置 density_unit { may be 0 for unknown, 1 for dots/inch, or 2 for dots/cm.}
	jcs.density_unit = 1;
	// 设置水平/垂直分辨率
	jcs.X_density = 300;
	jcs.Y_density = 300;
	jpeg_set_quality (&jcs,60,true);

	jpeg_start_compress(&jcs, TRUE);
	JSAMPROW row_pointer[1];			// 一行位图
	int row_stride;						// 每一行的字节数 
	row_stride = (bih.biWidth)*nComponent;		// 如果不是索引图,此处需要乘以3

	// 对每一行进行压缩
	while (jcs.next_scanline < jcs.image_height) {
		row_pointer[0] = & pData24[(jcs.image_height-jcs.next_scanline-1) * (row_stride+nAdjust)];
		jpeg_write_scanlines(&jcs, row_pointer, 1);
	}
	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);

	fclose(f);
	delete [] pData24;
}


/***********************************************
*旋转算法+（自适应二值化方法）找出边缘
**************************************************/
BYTE * imRotate_BinaryzationFindEdge (BYTE *data,int w,int h,float beta,LONG *biWidth,LONG *biHeight,char *strDestFileName,BYTE *obm) {

	int iw;
	int ih;
	iw = w;
	ih = h;
	// 1.计算旋转后的新图像最大最小包围盒宽高
	LONG owh = (LONG)sqrt(iw * iw + ih * ih + 0.5);
	double t = beta / 180;
	BYTE * pData24 = data;
	int nAdjust24 = w*3%4;
	if (nAdjust24) nAdjust24 = 4-nAdjust24;
	// 得到的新图像
	//FILE *fp2;
	//fp2 = fopen("f:\\data.txt","w");
	
	obm = new BYTE[owh * owh * 4];
	float cos_beta = (float)cos(t * PI);
	float sin_beta = (float)sin (t * PI);

	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			int pox_new = j*(owh*3+nAdjust24)+i*3;
			obm[pox_new] = 0;
			obm[pox_new + 1] = 0;
			obm[pox_new + 2] = 0;
		}
	}
	//fprintf(fp2,"1!\n");
	// 2.逆旋转变换，计算输出图像点p[i*(bih.biWidth*3+nAdjust24) + j * 3]所对应的原图像的坐标(x,y);
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){

			//旋转变换的逆变换
			float u = (i - owh /2) * cos_beta + (j - owh / 2) * sin_beta;
			float v = (j - owh / 2) * cos_beta - (i - owh /2 ) * sin_beta;

			// 换成相对于原图像的绝对坐标
			u += iw / 2;
			//后面的200是权宜之计,为了使图像在中间
			v += ih / 2;
			//v += + ih * 0.1543;

			int x = (int )u;
			int y = (int )v;

			//printf("i =  %d,j = %d\n",i,j);
			//3.检验条件，对满足条件的点（x,y），赋值F(i,j) = f(x,y)
			if ((x >= 0) && (x < iw) && (y >= 0) && (y < ih)) {

				// 从x,y 取值
				int pox_ = y*(w*3+nAdjust24)+x*3;
				int b = pData24[pox_];
				int g = pData24[pox_ + 1];
				int r = pData24[pox_ + 2];
				//printf("%d,%d,%d\n",b,g,r);
				//赋值到点(i,j)
				int pox_new = j*(owh*3+nAdjust24)+i*3;
				obm[pox_new] = b;
				obm[pox_new + 1] = g;
				obm[pox_new + 2] = r;
			}

		}
	}


	//二值化
	BYTE * obm_gray = new BYTE[(owh+nAdjust24)*owh];
	double p[257]; //计算每个灰度值的概率
	//设定一个最大概率
	p[256] = 1;
	for (int i = 0;i < 256;i++) {
		p[i] = 0;
	}
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			int b = obm[j*(owh*3+nAdjust24)+i*3];	//B
			int g = obm[j*(owh*3+nAdjust24)+i*3+1]; //G 
			int r = obm[j*(owh*3+nAdjust24)+i*3+2]; //R 
			// 根据公式算出灰度化数值，存入数组
			int pox = j*(owh+nAdjust24)+i;
			//obm_gray[pox]  = (r + b + g)/ 3.0;//得到灰度值
			//obm_gray[pox]=pow(( pow(static_cast<double>(r),2.2)* 0.2973 + pow(static_cast<double>(g),2.2)* 0.6274 + pow(static_cast<double>(b),2.2) * 0.0753),(1/2.2));
					obm_gray[pox] = (r*19595 + g*38469 + b*7472) >> 16;
			//obm_gray[pox]=r*0.299 + g*0.587 +b*0.114;
			p[obm_gray[pox]]++;//给对应的灰度值增加1;
		}
	}

	
	//分段线性变换
	int bMap[257]; //灰度映射表
	int BX1 = 20;
	int BY1 = 4;
	int BX2 = 230;
	int BY2 = 214;
	int i;
	for (i = 0;i < BX1;i++){ //(0-x1)
		//判断bx1是否大于0
		if (BX1 > 0) {
			bMap[i] = (int)BY1 * i / BX1;
		} else {
			bMap[i] = 0;		
		}
	}
	for (;i < BX2;i++) {//(x1-x2)
		if(BX2 != BX1){
			bMap[i] = BY1 + (int) ((BY2 - BY1) * (i - BX1) / (BX2 - BX1));
		} else {
			//直接赋值为BY1
			bMap[i] =BY1;
		}
	}
	for (;i < 256;i++) {//(x2-256)
		//判断bx2是否等于255（防止分母为0）
		if(BX2 !=255) {
			//线性变换
			bMap[i] = BY2 +(int)((255-BY2)*(i - BX2)/(255 - BX2));
		} else {
			//直接 赋值255
			bMap[i] = 255;
		}
	}
	//对各像素进行灰度转换
	for (i = 0;i < owh;i++) {
		for (int j = 0;j < owh;j++) {
			//对像素进行灰度映射处理
			int pox = i*(owh+nAdjust24)+j;
			int T = obm_gray[pox];
			obm_gray[pox] = bMap[T];
		}
	}

	//输出增强结果
	if (1) {
		_BITMAPINFOHEADER bih = {0,0,0,0,0,0,0,0,0,0,0};		// bmp头信息
		int nAdjust; // 用于字节对齐
		//*****************************
		bih.biSize = 40;
		bih.biWidth = owh;
		bih.biHeight = owh;
		bih.biPlanes = 1;
		bih.biBitCount = 24;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = ((bih.biWidth%4 == 0 ) ? bih.biWidth : (bih.biWidth + bih.biWidth % 4) ) * bih.biHeight;
		bih.biXPelsPerMeter = 300;
		bih.biYPelsPerMeter = 300;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		//*******************************************************************
		BMPINFOMATION bmp_t;
	
		BYTE * g_T = new BYTE[(owh * 3+nAdjust24)*owh];
		for (int j=0;j<owh;j++) {
			for (int i=0;i<owh;i++){
			g_T[j*(owh*3+nAdjust24)+i*3] = obm_gray[j*(owh+nAdjust24)+i];	//B
			g_T[j*(owh*3+nAdjust24)+i*3+1] = obm_gray[j*(owh+nAdjust24)+i]; //G 
			g_T[j*(owh*3+nAdjust24)+i*3+2] = obm_gray[j*(owh+nAdjust24)+i]; //R
			}
		}
		bmp_t.bih = bih;
		bmp_t.nComponent = 3;
		bmp_t.data = g_T;
		bmp_t.pData24 = g_T;
		bmp_t.nAdjust24 = nAdjust24;
		bmp_t.nAdjust = nAdjust24;
		// 以上图像读取完毕
		saveAsBmp(bmp_t,"增强_canny_中值滤波后结果.bmp");	
	}
	// end


	//fprintf(fp2,"2!\n");
	
	//算出概率
	for (int i = 0;i < 256;i++) {
		p[i] /= (owh * owh * 1.0);
		//fprintf(fp2,"%d	%f\n",i,p[i]);
	}
	//fclose(fp2);
	//求出灰度值在10~127间的极小值
	
	int max = 255;
	int mymin=0;
	//求出直方图右端点
	for (int i = 255;i >= 0;i--) {
		if (p[i] > 0.001) {
			max = i;
			break;
		}
	}

	//求出直方图左端点
	for (int i = 0;i <255;i++) {
		if (p[i] > 0.001) {
			mymin = i;
			break;
		}
	}
	int min_pox = mymin;
//定义搜索灰度区间
	int t1=mymin+(max-mymin)/4;
	int t0=mymin+(max-mymin)/2;
    int t2=max-(max-mymin)/4;
	for (int i = t2;i >=t0;i--) {
		if (p[i] <= p[min_pox]){
			min_pox = i;
		}
	}
	
	if(min_pox>=t2) {
		min_pox=40;
	}
	//if(min_pox<t0*1.2)
	//	min_pox=0.8*t2;

	// 得到的新图像
	/*FILE *fp2;
	fp2 = fopen("D:\\b.txt","w+");
	fprintf(fp2,"min_pox= %d,t0=%d,t2=%d",min_pox,t0,t2);*/
	
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			int pox = j*(owh+nAdjust24)+i;
			if (obm_gray[pox] < min_pox) {
				obm_gray[pox] = 255;//设置为背景
			}else{
				obm_gray[pox] =0;//设置为图像
			}
		}
	}
	SIZE szz;
	szz.cx = owh;
	szz.cy = owh;
	//printf("进行图像滤波开始\n");
	MedianFilter(obm_gray,szz,13 * 13);
	//fprintf(fp2,"滤波结束!\n");
	
	BYTE * obm_gray_new =  new BYTE[(owh+nAdjust24)*owh];

	for (int i = 0;i < (owh+nAdjust24)*owh;i++) {
		obm_gray_new[i] = obm_gray[i];
	}

	BMPINFOMATION bmp_t;
	bmp_t.pData24 = obm_gray_new;
	bmp_t.bih.biHeight = owh;
	bmp_t.bih.biWidth = owh;
	bmp_t.nComponent = 1;
	//输出二值化结果
	int debug = 1;
	if (debug) {
		
		_BITMAPINFOHEADER bih = {0,0,0,0,0,0,0,0,0,0,0};		// bmp头信息
		int nAdjust; // 用于字节对齐
		//*****************************
		bih.biSize = 40;
		bih.biWidth = owh;
		bih.biHeight = owh;
		bih.biPlanes = 1;
		bih.biBitCount = 24;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = ((bih.biWidth%4 == 0 ) ? bih.biWidth : (bih.biWidth + bih.biWidth % 4) ) * bih.biHeight;
		bih.biXPelsPerMeter = 300;
		bih.biYPelsPerMeter = 300;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;


		//*******************************************************************
		BMPINFOMATION bmp_t;
	
		BYTE * g_T = new BYTE[(owh * 3+nAdjust24)*owh];
		for (int j=0;j<owh;j++) {
			for (int i=0;i<owh;i++){
			g_T[j*(owh*3+nAdjust24)+i*3] = obm_gray[j*(owh+nAdjust24)+i];	//B
			g_T[j*(owh*3+nAdjust24)+i*3+1] = obm_gray[j*(owh+nAdjust24)+i]; //G 
			g_T[j*(owh*3+nAdjust24)+i*3+2] = obm_gray[j*(owh+nAdjust24)+i]; //R
			}
		}
		bmp_t.bih = bih;
		bmp_t.nComponent = 3;
		bmp_t.data = g_T;
		bmp_t.pData24 = g_T;
		bmp_t.nAdjust24 = nAdjust24;
		bmp_t.nAdjust = nAdjust24;
		// 以上图像读取完毕
		saveAsBmp(bmp_t,"mybinary.bmp");	
	}
	/****************************************/
	//查找最大外界矩形，并画出最大边界
	POX pox[4];
	int _w = owh;
	int _h = owh;
	int x_min,y_min,x_max,y_max;
	int go_on = 1;
	//找到的边界点
	//  左上（x_min,y_min） 右上（x_max,y_min） 左下 (x_max,y_min) 右下 (x_max,y_max)

	//找y_min
	for (int i = 0; i < _h;i++) {
		go_on = 1;
		for (int j = 0; j < _w;j++) {
			if (obm_gray[i*(owh+nAdjust24)+j] == 0) { // 255是边界
				y_min = i;
				go_on = 0;
				pox[0].x = j;
				pox[0].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//找x_min
	for (int j = 0; j < _w;j++) {
		go_on = 1;
		for (int i = 0;i < _h;i++) {
			if (obm_gray[i*(owh+nAdjust24)+j] == 0) { // 255是边界
				x_min = j;
				pox[1].x = j;
				pox[1].y = i;
				go_on = 0;
				break;
			}
		}
		if (!go_on) break;
	}
	//找y_max
	for (int i = _h - 1;i >= 0;i--) {
		go_on = 1;
		for (int j = 0;j < _w;j++){
			if (obm_gray[i*(owh+nAdjust24)+j] == 0) { // 255是边界
				y_max = i;
				go_on = 0;
				pox[2].x = j;
				pox[2].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//找x_max
	for (int j = _w - 1; j >= 0;j--) {
		go_on = 1;
		for (int i = _h - 1;i >= 0;i--) {
			if (obm_gray[i*(owh+nAdjust24)+j] == 0) { // 255是边界
				x_max = j;
				go_on = 0;
				pox[3].x = j;
				pox[3].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//fprintf(fp2,"找标记点!\n");

	//printf("%d,%d,%d,%d\n",x_min,y_min,x_max,y_max);
	//对原图中有对应颜色进行改变
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			if (
				obm[j*(owh*3+nAdjust24)+i*3] == 123 &&
				obm[j*(owh*3+nAdjust24)+i*3+1] == 123 &&
				obm[j*(owh*3+nAdjust24)+i*3+2] == 123 
				) {
					obm[j*(owh*3+nAdjust24)+i*3] = 123;
					obm[j*(owh*3+nAdjust24)+i*3+1] = 123;
					obm[j*(owh*3+nAdjust24)+i*3+2] = 123-1;
			}
		}

	}
	//fprintf(fp2,"对原图修改!\n");
	//fclose(fp2);
	//对四个点进行标记
	for (int i = 0;i < 4;i++){
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3] = 123;	//B
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3+1] = 123; //G 
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3+2] = 123; //R
		
	}

	/*******************************************************************/
	/*******************************/

	*biWidth = owh;
	*biHeight = owh;
	//obmde = obm;
	delete []obm_gray;
	//delete []obm;
	return obm;
}




/***********************************************
*旋转算法+（canny边缘查找+小窗口中值滤波）方法找出边缘
**************************************************/
BYTE * imRotate_CannyFindEdge (BYTE *data,int w,int h,float beta,LONG *biWidth,LONG *biHeight,char *strDestFileName,BYTE *obm) {

	int iw;
	int ih;
	iw = w;
	ih = h;
	// 1.计算旋转后的新图像最大最小包围盒宽高
	LONG owh = (LONG)sqrt(iw * iw + ih * ih + 0.5);
	double t = beta / 180;
	BYTE * pData24 = data;
	int nAdjust24 = w*3%4;
	if (nAdjust24) nAdjust24 = 4-nAdjust24;
	// 得到的新图像
	//FILE *fp2;
	//fp2 = fopen("f:\\b.txt","w");
	
	obm = new BYTE[owh * owh * 4];
	float cos_beta = (float)cos(t * PI);
	float sin_beta = (float)sin (t * PI);

	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			int pox_new = j*(owh*3+nAdjust24)+i*3;
			obm[pox_new] = 0;
			obm[pox_new + 1] = 0;
			obm[pox_new + 2] = 0;
		}
	}
	//fprintf(fp2,"1!\n");
	// 2.逆旋转变换，计算输出图像点p[i*(bih.biWidth*3+nAdjust24) + j * 3]所对应的原图像的坐标(x,y);
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){

			//旋转变换的逆变换
			float u = (i - owh /2) * cos_beta + (j - owh / 2) * sin_beta;
			float v = (j - owh / 2) * cos_beta - (i - owh /2 ) * sin_beta;

			// 换成相对于原图像的绝对坐标
			u += iw / 2;
			//后面的200是权宜之计,为了使图像在中间
			v += ih / 2;
			//v += + ih * 0.1543;

			int x = (int )u;
			int y = (int )v;

			//printf("i =  %d,j = %d\n",i,j);
			//3.检验条件，对满足条件的点（x,y），赋值F(i,j) = f(x,y)
			if ((x >= 0) && (x < iw) && (y >= 0) && (y < ih)) {

				// 从x,y 取值
				int pox_ = y*(w*3+nAdjust24)+x*3;
				int b = pData24[pox_];
				int g = pData24[pox_ + 1];
				int r = pData24[pox_ + 2];
				//printf("%d,%d,%d\n",b,g,r);
				//赋值到点(i,j)
				int pox_new = j*(owh*3+nAdjust24)+i*3;
				obm[pox_new] = b;
				obm[pox_new + 1] = g;
				obm[pox_new + 2] = r;
			}

		}
	}


	//二值化
	BYTE * obm_gray = new BYTE[(owh+nAdjust24)*owh];
	double p[257]; //计算每个灰度值的概率
	//设定一个最大概率
	p[256] = 1;
	for (int i = 0;i < 256;i++) {
		p[i] = 0;
	}
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			int b = obm[j*(owh*3+nAdjust24)+i*3];	//B
			int g = obm[j*(owh*3+nAdjust24)+i*3+1]; //G 
			int r = obm[j*(owh*3+nAdjust24)+i*3+2]; //R 
			// 根据公式算出灰度化数值，存入数组
			int pox = j*(owh+nAdjust24)+i;
			//obm_gray[pox]  = (r + b + g)/ 3.0;//得到灰度值
			//obm_gray[pox]=pow(( pow(static_cast<double>(r),2.2)* 0.2973 + pow(static_cast<double>(g),2.2)* 0.6274 + pow(static_cast<double>(b),2.2) * 0.0753),(1/2.2));
					obm_gray[pox] = (r*19595 + g*38469 + b*7472) >> 16;
			//obm_gray[pox]=r*0.299 + g*0.587 +b*0.114;
			p[obm_gray[pox]]++;//给对应的灰度值增加1;
		}
	}

	
	//分段线性变换
	int bMap[257]; //灰度映射表
	int BX1 = 20;
	int BY1 = 4;
	int BX2 = 230;
	int BY2 = 214;
	int i;
	for (i = 0;i < BX1;i++){ //(0-x1)
		//判断bx1是否大于0
		if (BX1 > 0) {
			bMap[i] = (int)BY1 * i / BX1;
		} else {
			bMap[i] = 0;		
		}
	}
	for (;i < BX2;i++) {//(x1-x2)
		if(BX2 != BX1){
			bMap[i] = BY1 + (int) ((BY2 - BY1) * (i - BX1) / (BX2 - BX1));
		} else {
			//直接赋值为BY1
			bMap[i] =BY1;
		}
	}
	for (;i < 256;i++) {//(x2-256)
		//判断bx2是否等于255（防止分母为0）
		if(BX2 !=255) {
			//线性变换
			bMap[i] = BY2 +(int)((255-BY2)*(i - BX2)/(255 - BX2));
		} else {
			//直接 赋值255
			bMap[i] = 255;
		}
	}
	//对各像素进行灰度转换
	for (i = 0;i < owh;i++) {
		for (int j = 0;j < owh;j++) {
			//对像素进行灰度映射处理
			int pox = i*(owh+nAdjust24)+j;
			int T = obm_gray[pox];
			obm_gray[pox] = bMap[T];
		}
	}
	//end

	BYTE * obm_gray_old =  new BYTE[(owh+nAdjust24)*owh];
	for (int i = 0;i < (owh+nAdjust24)*owh;i++) {
		obm_gray_old[i] = obm_gray[i];
	}
	//canny
	SIZE sz;
	sz.cx = owh;
	sz.cy = owh;
	Canny(obm_gray_old,sz,2,obm_gray);
	//中值滤波
	MedianFilter(obm_gray,sz,5 * 5);
	//输出增强结果
	int test = 1;
	if (test) {
		_BITMAPINFOHEADER bih = {0,0,0,0,0,0,0,0,0,0,0};		// bmp头信息
		int nAdjust; // 用于字节对齐
		//*****************************
		bih.biSize = 40;
		bih.biWidth = owh;
		bih.biHeight = owh;
		bih.biPlanes = 1;
		bih.biBitCount = 24;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = ((bih.biWidth%4 == 0 ) ? bih.biWidth : (bih.biWidth + bih.biWidth % 4) ) * bih.biHeight;
		bih.biXPelsPerMeter = 300;
		bih.biYPelsPerMeter = 300;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		//*******************************************************************
		BMPINFOMATION bmp_t;
	
		BYTE * g_T = new BYTE[(owh * 3+nAdjust24)*owh];
		for (int j=0;j<owh;j++) {
			for (int i=0;i<owh;i++){
			g_T[j*(owh*3+nAdjust24)+i*3] = obm_gray[j*(owh+nAdjust24)+i];	//B
			g_T[j*(owh*3+nAdjust24)+i*3+1] = obm_gray[j*(owh+nAdjust24)+i]; //G 
			g_T[j*(owh*3+nAdjust24)+i*3+2] = obm_gray[j*(owh+nAdjust24)+i]; //R
			}
		}
		bmp_t.bih = bih;
		bmp_t.nComponent = 3;
		bmp_t.data = g_T;
		bmp_t.pData24 = g_T;
		bmp_t.nAdjust24 = nAdjust24;
		bmp_t.nAdjust = nAdjust24;
		// 以上图像读取完毕
		saveAsBmp(bmp_t,"增强_canny_中值滤波后结果1.bmp");	
	}
	// end
	
	/****************************************/
	//查找最大外界矩形，并画出最大边界
	POX pox[4];
	int border = 255;

	int _w = owh;
	int _h = owh;
	int x_min,y_min,x_max,y_max;
	int go_on = 1;
	//找到的边界点
	//  左上（x_min,y_min） 右上（x_max,y_min） 左下 (x_max,y_min) 右下 (x_max,y_max)

	//找y_min
	for (int i = 0; i < _h;i++) {
		go_on = 1;
		for (int j = 0; j < _w;j++) {
			if (obm_gray[i*(owh+nAdjust24)+j] == border) { // 255是边界
				y_min = i;
				go_on = 0;
				pox[0].x = j;
				pox[0].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//找x_min
	for (int j = 0; j < _w;j++) {
		go_on = 1;
		for (int i = 0;i < _h;i++) {
			if (obm_gray[i*(owh+nAdjust24)+j] == border) { // 255是边界
				x_min = j;
				pox[1].x = j;
				pox[1].y = i;
				go_on = 0;
				break;
			}
		}
		if (!go_on) break;
	}
	//找y_max
	for (int i = _h - 1;i >= 0;i--) {
		go_on = 1;
		for (int j = 0;j < _w;j++){
			if (obm_gray[i*(owh+nAdjust24)+j] == border) { // 255是边界
				y_max = i;
				go_on = 0;
				pox[2].x = j;
				pox[2].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//找x_max
	for (int j = _w - 1; j >= 0;j--) {
		go_on = 1;
		for (int i = _h - 1;i >= 0;i--) {
			if (obm_gray[i*(owh+nAdjust24)+j] == border) { // 255是边界
				x_max = j;
				go_on = 0;
				pox[3].x = j;
				pox[3].y = i;
				break;
			}
		}
		if (!go_on) break;
	}
	//fprintf(fp2,"找标记点!\n");

	//printf("%d,%d,%d,%d\n",x_min,y_min,x_max,y_max);
	//对原图中有对应颜色进行改变
	for (int j=0;j<owh;j++) {
		for (int i=0;i<owh;i++){
			if (
				obm[j*(owh*3+nAdjust24)+i*3] == 123 &&
				obm[j*(owh*3+nAdjust24)+i*3+1] == 123 &&
				obm[j*(owh*3+nAdjust24)+i*3+2] == 123 
				) {
					obm[j*(owh*3+nAdjust24)+i*3] = 123;
					obm[j*(owh*3+nAdjust24)+i*3+1] = 123;
					obm[j*(owh*3+nAdjust24)+i*3+2] = 123-1;
			}
		}

	}
	//fprintf(fp2,"对原图修改!\n");
	//fclose(fp2);
	//对四个点进行标记
	for (int i = 0;i < 4;i++){
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3] = 123;	//B
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3+1] = 123; //G 
		obm[(pox[i].y)*(owh*3+nAdjust24)+(pox[i].x) * 3+2] = 123; //R
		
	}

	/*******************************************************************/
	/*******************************/

	*biWidth = owh;
	*biHeight = owh;
	//obmde = obm;
	delete []obm_gray;
	//delete []obm;
	return obm;
}
