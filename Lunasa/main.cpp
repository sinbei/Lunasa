//	画像透過表示ソフト「Lunasa」Version 1.20

#define _WIN32_WINNT 0x0500
#define WINDOW_NAME TEXT("Lunasa")

#include <windows.h>
#include <wchar.h>
#include <cstdlib>
#include "png.h"
#include "pnginfo.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE,int);

HBITMAP LoadPngImage(HWND hWnd, FILE *fp, unsigned &uWidth,unsigned &uHeight);

static int bmpw,bmph,bmpw2,bmph2;
static HBITMAP hBmp, hBmp2;
static HDC hdc_mem,hdc_temp;
BITMAP bmp_info1, bmp_info2;
bool loadPng = FALSE;


LPCWSTR lpszClassName = TEXT("rgn03"); //ウィンドウクラス
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hCurInst,HINSTANCE hPrevInst,LPSTR lpsCmdLine,int nCmdShow){
	MSG msg;
	BOOL bRet;

	hInst = hCurInst;

	if(!InitApp(hCurInst))
		return FALSE;
	if(!InitInstance(hCurInst,nCmdShow))
		return FALSE;
	while((bRet = GetMessage(&msg,NULL,0,0)) != 0){
		if(bRet == -1){
			break;
		}
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

ATOM InitApp(HINSTANCE hInst){
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = (HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL,MAKEINTRESOURCE(IDC_ARROW),IMAGE_CURSOR,0,0,LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszClassName;
	wc.hIconSm = (HICON)LoadImage(NULL,MAKEINTRESOURCE(IDI_APPLICATION),IMAGE_ICON,0,0,LR_DEFAULTSIZE | LR_SHARED);

	return (RegisterClassEx(&wc));
}


//ウィンドウの生成
BOOL InitInstance(HINSTANCE hInst,int nCmdShow){
	hBmp = (HBITMAP)LoadImage(hInst,TEXT("bitmap1.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hBmp == NULL){
		loadPng = true;
	}
	if(!loadPng){
		GetObject(hBmp, (int)sizeof(bmp_info1),&bmp_info1);
		bmpw = bmp_info1.bmWidth;
		bmph = bmp_info1.bmHeight;
		HWND hWnd;
		hWnd = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_LAYERED,
			lpszClassName,
			WINDOW_NAME,
			WS_POPUP,
			0,
			0,
			bmpw,
			bmph,
			NULL,
			NULL,
			hInst,
			NULL);
		if(!hWnd)
			return FALSE;
		SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 255, LWA_ALPHA);
		ShowWindow(hWnd,nCmdShow);
		UpdateWindow(hWnd);
		return TRUE;
	}else{
		HWND hWnd;
		hWnd = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_LAYERED,
			lpszClassName,
			WINDOW_NAME,
			WS_POPUP,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			NULL,
			NULL,
			hInst,
			NULL);
		if(!hWnd) return FALSE;
		ShowWindow(hWnd,nCmdShow);
		UpdateWindow(hWnd);
		return TRUE;
	}
}



LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
	int id, ix, iy;
	HRGN hRgn, hRgn1, hRgn2, hRgnTemp;
	HDC hdc;
	PAINTSTRUCT ps;
	COLORREF invcolor = RGB(0,0,0);
	switch(msg){
		case WM_CREATE:
			if(!loadPng){
				MessageBox(hWnd,  TEXT("bmpモードでの読み込みを開始します"), TEXT("読み込み開始"),MB_OK);
				hBmp2 = (HBITMAP)LoadImage(hInst,TEXT("bitmap2.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
				if(hBmp == NULL || hBmp2 == NULL){
					MessageBox(hWnd, TEXT("bitmap2.bmpの読み込みに失敗しました敗\nbitmap2.bmpが存在しているか確認してください"), TEXT("NG"), MB_OK);
					PostQuitMessage(0);
					break;
				}
				GetObject(hBmp2, (int)sizeof(bmp_info2),&bmp_info2);
				bmpw2 = bmp_info2.bmWidth;
				bmph2 = bmp_info2.bmHeight;
				hdc_mem = CreateCompatibleDC(NULL);
				SelectObject(hdc_mem, hBmp);

				hdc_temp = CreateCompatibleDC(NULL);
				SelectObject(hdc_temp, hBmp2);

						
				hRgn = CreateRectRgn(0, 0, 1, 1);
				hRgn1 = CreateRectRgn(0, 0, bmpw, bmph);
				hRgn2 = CreateRectRgn(0, 0, 0, 0);
				for(iy = 0;iy < bmph2;iy++){
					for(ix= 0; ix < bmpw2;ix++){
						if(GetPixel(hdc_temp,ix,iy) == invcolor){
							hRgnTemp = CreateRectRgn(ix,iy,ix+1,iy+1);
							CombineRgn(hRgn2,hRgn2,hRgnTemp,RGN_OR);
							DeleteObject(hRgnTemp);
						}
					}
				}


				
				CombineRgn(hRgn, hRgn1, hRgn2, RGN_XOR);
				DeleteObject(hRgn1);
				DeleteObject(hRgn2);
			
				if(hBmp != NULL && hBmp2 != NULL)MessageBox(hWnd,  TEXT("読み込み完了"), TEXT("OK"),MB_OK);
				else{
					MessageBox(hWnd, TEXT("bmpモードでの読み込みに失敗しました\n画像ファイルを確認してください"), TEXT("NG"), MB_OK);
					PostQuitMessage(0);
					break;
				}
			
				SetWindowRgn(hWnd, hRgn, TRUE);
			}else{
				MessageBox(hWnd,  TEXT("pngモードでの読み込みを開始します"), TEXT("読み込み開始"),MB_OK);
				FILE *fp = NULL;
				errno_t err;
				unsigned int width,height;
				//if((err = _wfopen_s(&fp,TEXT("./source.png"),TEXT("rb"))) != 0){
				if((err = fopen_s(&fp,"./source.png","rb")) != 0){
					MessageBox(hWnd, TEXT("pngモードでの読み込みに失敗しました\nsource.pngが適切か確認してください"), TEXT("NG"), MB_OK);
					PostQuitMessage(0);
					break;
				}
				hBmp = LoadPngImage(hWnd,fp,width,height);
				if(hBmp == NULL){
					fclose(fp);
					MessageBox(hWnd, TEXT("読み込み失敗\n画像ファイルを確認してください"), TEXT("NG"), MB_OK);
					PostQuitMessage(0);
					break;
				}

				fclose(fp);
				SetWindowPos(hWnd,0,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
				HDC hmemdc, hdc, hsdc;
				hsdc = GetDC(0);
				hdc = GetDC(hWnd);
				hmemdc = CreateCompatibleDC(hdc);

				POINT wndPos;
				SIZE wndSize;
				RECT rc;

				GetWindowRect(hWnd, &rc);
				wndPos.x = rc.left;
				wndPos.y = rc.top;
				wndSize.cx = width;
				wndSize.cy = height;

				POINT po;
				po.x = po.y = 0;

				BLENDFUNCTION blend;
				blend.BlendOp = AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.SourceConstantAlpha = 255;
				blend.AlphaFormat = AC_SRC_ALPHA;

				HGDIOBJ hOldObj = SelectObject(hmemdc, hBmp);
				BitBlt(hdc,0,0,width,height,hmemdc,0,0,SRCCOPY|CAPTUREBLT);

				if(0 == UpdateLayeredWindow(hWnd,hsdc,&wndPos,&wndSize,hmemdc,&po,0,&blend,ULW_ALPHA)){
					TCHAR strErrMes[80];
					DWORD err = GetLastError();
					wsprintf(strErrMes,TEXT("UpdateLayerdWindow失敗:エラーコード=[%d]"),err);
					MessageBox(hWnd,strErrMes,TEXT("エラー"),MB_OK|MB_ICONSTOP);
					PostQuitMessage(0);
					break;
				}
				SelectObject(hmemdc,hOldObj);
				DeleteDC(hmemdc);
				ReleaseDC(hWnd,hdc);
				ReleaseDC(0,hsdc);
			}
			break;
		case WM_PAINT:
			if(!loadPng){
				hdc = BeginPaint(hWnd, &ps);
				BitBlt(hdc, 0, 0, bmpw, bmph, hdc_mem, 0, 0, SRCCOPY);
				EndPaint(hWnd, &ps);
			}
			else{
			}
			break;
		case WM_RBUTTONDOWN:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case WM_LBUTTONDOWN:
			PostMessage(hWnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
			break;
		case WM_KEYDOWN:
			switch(wp){
				case VK_F5:
					if(loadPng)break;
					SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 63, LWA_ALPHA );
					break;
				case VK_F6:
					if(loadPng)break;
			 		SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 127, LWA_ALPHA);
					break;
				case VK_F7:
					if(loadPng)break;
					SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 191, LWA_ALPHA);
					break;
				case VK_F8:
					if(loadPng)break;
					SetLayeredWindowAttributes(hWnd, RGB(0,0,0), 255, LWA_ALPHA);
					break;
				case VK_F12:
					if(GetWindowLong(hWnd, GWL_EXSTYLE) == (WS_EX_LAYERED | WS_EX_TOPMOST))
						SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					else if(GetWindowLong(hWnd, GWL_EXSTYLE) == WS_EX_LAYERED)
						SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					break;
				default:
					break;
			}
			break;
		case WM_SYSKEYDOWN:
			switch(wp){
				case VK_F10:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			break;
		case WM_CLOSE:
			id = MessageBox(hWnd,TEXT("終了しますか？"),TEXT("終了確認"),MB_YESNO | MB_ICONINFORMATION);
			if(id == IDYES)
				DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DeleteObject(hBmp);
			DeleteDC(hdc_mem);
			PostQuitMessage(0);
			break;
		default:
			return(DefWindowProc(hWnd,msg,wp,lp));
	}
	return 0;
}

HBITMAP LoadPngImage(HWND hWnd, FILE *fp, unsigned &uWidth,unsigned &uHeight){
	if(!fp) return NULL;

	png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(!png_ptr) return NULL;

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return NULL;
	}

	png_info *end_info = png_create_info_struct(png_ptr);
	if(!end_info){
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		return NULL;
	}

	png_init_io(png_ptr, fp);

	png_uint_32 nWidth,nHeight;
	png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);

	nWidth = info_ptr->width;
	nHeight = info_ptr->height;

	BITMAPV5HEADER bi;
	void *lpBits;

	ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
	bi.bV5Size			= sizeof(BITMAPV5HEADER);
	bi.bV5Width			= nWidth;
	bi.bV5Height		= nHeight;
	bi.bV5Planes		= 1;
	bi.bV5BitCount		= 32;
	bi.bV5Compression	= BI_BITFIELDS;
	bi.bV5RedMask		= 0x00FF0000;
	bi.bV5GreenMask		= 0x0000FF00;
	bi.bV5BlueMask		= 0x000000FF;
	bi.bV5AlphaMask		= 0xFF000000;

	HDC hdc = GetDC(hWnd);
	HBITMAP hbmp = CreateDIBSection(hdc,(BITMAPINFO *)&bi, DIB_RGB_COLORS,(void **)&lpBits, NULL, (DWORD)0);
	ReleaseDC(hWnd,hdc);
	DWORD *lpdwPixel = (DWORD *)lpBits;
	DWORD x, y;
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for(x = 0; x < nWidth; x++){
		for(y = 0; y < nHeight; y++){
			DWORD r = row_pointers[y][4*x];
			DWORD g = row_pointers[y][4*x+1];
			DWORD b = row_pointers[y][4*x+2];
			DWORD alpha = row_pointers[y][4*x+3];

			if(alpha == 255) lpdwPixel[(nHeight-y-1)*nWidth+x] = (r << 16) | (g << 8) | (b << 0) | (alpha << 24);
			else if(alpha == 0) lpdwPixel[(nHeight-y-1)*nWidth+x] = 0;
			else{
				r = r * alpha / 255;
				g = g * alpha / 255;
				b = b * alpha / 255;
				lpdwPixel[(nHeight-y-1)*nWidth+x] = (r << 16) | (g << 8) | (b << 0) | (alpha << 24);
			}
		}
	}

	png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);

	uWidth = nWidth;
	uHeight = nHeight;
	return hbmp;
}