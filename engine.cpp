#include <windows.h>
#include <cmath>
#include "engine.h"

//GetPixel, SetPixel bez optymalizacji
HBITMAP Rotate(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p){
  
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  HDC InDC,LargeDC,ResDC;
  HBITMAP LargeHbm,ResHbm;
  HBITMAP InOldHbm,LargeOldHbm,ResOldHbm;
  
  InDC = CreateCompatibleDC(hdc);
  ResDC = CreateCompatibleDC(hdc);
  
  InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
  
  int w,h;
  
  w = abs(cos(a)*bm.bmWidth+0.5) + abs(sin(a)*bm.bmHeight+0.5)+1;
  h = abs(cos(a)*bm.bmHeight+0.5) + abs(sin(a)*bm.bmWidth+0.5)+1;
  
  ResHbm = CreateCompatibleBitmap(hdc,w,h);
  ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
   
  double dx,dy;
  dx = 0;
    if ((a >= 0) && (a <= 3.14/2.0)){
      dy = 0;
      dx = -sin(a)*bm.bmHeight;       
    } else if ((a > 3.14/2.0) && (a <= 3.14)){
      dy = cos(a)*bm.bmHeight;
      dx = +cos(a)*bm.bmWidth - sin(a)*bm.bmHeight;     
    } else if ((a > 3.14) && (a <= 3.14/2.0*3.0)){
      dx = cos(a)*bm.bmWidth;
      dy = cos(a)*bm.bmHeight+sin(a)*bm.bmWidth;     
    } else {
      dy = sin(a)*bm.bmWidth;     
    }
    
     
               
  if (AF > 1){
    LargeDC = CreateCompatibleDC(hdc);
    LargeHbm = CreateCompatibleBitmap(hdc,AF*w,AF*h);
    LargeOldHbm = (HBITMAP)SelectObject(LargeDC,LargeHbm);
    
    for (double x = 0;x < w*AF;x++) for (double y = 0;y < h*AF;y++){
      int x2,y2;
      x2 = (x/AF+dx)*cos(a)+(y/AF+dy)*sin(a);
      y2 = -double(x/AF+dx)*sin(a)+(y/AF+dy)*cos(a);
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        SetPixel(LargeDC,x,y,GetPixel(InDC,x2,y2));        
      } else SetPixel(LargeDC,x,y,Background);    
    }
    
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
    COLORREF res;
    
    int r,g,b;
    r = 0;
    g = 0;
    b = 0;
    
    for (int X = 0;X < AF;X++) for (int Y = 0;Y < AF;Y++){
      res = GetPixel(LargeDC,x*AF+X,y*AF+Y);
      r += (res % 256);
      res /= 256;
      g += (res % 256);
      res /= 256;
      b += (res % 256);  
    }
    
    r /= AF*AF;
    g /= AF*AF;
    b /= AF*AF;
    SetPixel(ResDC,x,y,RGB(r,g,b));  
    }
    
    SelectObject(LargeDC,LargeOldHbm);
    DeleteObject(LargeHbm);
    DeleteDC(LargeDC);      
  } else {
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
      int x2,y2;
      x2 = (x+dx)*cos(a)+(y+dy)*sin(a);
      y2 = -double(x+dx)*sin(a)+(y+dy)*cos(a);
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        SetPixel(ResDC,x,y,GetPixel(InDC,x2,y2));        
      } else SetPixel(ResDC,x,y,Background);    
    }     
  }            
  
  SelectObject(InDC,InOldHbm);
  SelectObject(ResDC,ResOldHbm);
  DeleteDC(InDC);
  DeleteDC(ResDC);
  DeleteObject(&bm);
  
  return ResHbm;       
}

//GetDIBits, SetDIBits bez optymalizacji
HBITMAP Rotate2(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p){
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  HDC InDC,LargeDC,ResDC;
  HBITMAP LargeHbm,ResHbm;
  HBITMAP InOldHbm,LargeOldHbm,ResOldHbm;
  
  InDC = CreateCompatibleDC(hdc);
  ResDC = CreateCompatibleDC(hdc);
  
  InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  int x2,y2;
  
  int X = bm.bmWidth;
  int Y = 0;
  x2 = X*cos(a) + Y*sin(a);
  y2 = Y*cos(a) - X*sin(a);
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = bm.bmWidth;
  Y = bm.bmHeight;
  x2 = X*cos(a) + Y*sin(a);
  y2 = Y*cos(a) - X*sin(a);
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = 0;
  Y = bm.bmHeight;
  x2 = X*cos(a) + Y*sin(a);
  y2 = Y*cos(a) - X*sin(a);
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  int W,H;
  W = maxx-minx;
  H = maxy-miny;
  
  int w,h;
  
  w = W+2;
  h = H+2;
  
  //w = int(abs(cos(a)*bm.bmWidth) + abs(sin(a)*bm.bmHeight)+0.5);
  //h = int(abs(cos(a)*bm.bmHeight) + abs(sin(a)*bm.bmWidth)+0.5);
  
  BITMAPINFOHEADER   bi;
     
  bi.biSize = sizeof(BITMAPINFOHEADER);    
  bi.biWidth = bm.bmWidth;    
  bi.biHeight = bm.bmHeight;  
  bi.biPlanes = 1;    
  bi.biBitCount = 32;    
  bi.biCompression = BI_RGB;    
  bi.biSizeImage = 0;  
  bi.biXPelsPerMeter = 0;    
  bi.biYPelsPerMeter = 0;    
  bi.biClrUsed = 0;    
  bi.biClrImportant = 0;
  
  int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
  DWORD dwBmpSize = pitch * bi.biHeight;

  HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
  unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

  GetDIBits(InDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
  BITMAPINFOHEADER bi2 = bi;
  bi2.biWidth = w;
  bi2.biHeight = h;
  
  int pitch2 = ((bi2.biWidth * bi2.biBitCount + 31)/ 32) * 4;
  DWORD dwBmpSize2 = pitch2 * bi2.biHeight;
  
  HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
  unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
  
  ResHbm = CreateCompatibleBitmap(hdc,w,h);
  ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
  
  GetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);
   
  double dx,dy;
  dx = 0;
    if ((a >= 0) && (a <= 3.14/2.0)){
      dy = 0;
      dx = -sin(a)*bm.bmHeight;       
    } else if ((a > 3.14/2.0) && (a <= 3.14)){
      dy = cos(a)*bm.bmHeight;
      dx = +cos(a)*bm.bmWidth - sin(a)*bm.bmHeight;     
    } else if ((a > 3.14) && (a <= 3.14/2.0*3.0)){
      dx = cos(a)*bm.bmWidth;
      dy = cos(a)*bm.bmHeight+sin(a)*bm.bmWidth;     
    } else {
      dy = sin(a)*bm.bmWidth;     
    }
    
    int R = Background % 256;
    Background /= 256;
    int G = Background % 256;
    Background /= 256;
    int B = Background % 256;
    
  if (AF > 1){
    LargeDC = CreateCompatibleDC(hdc);
    LargeHbm = CreateCompatibleBitmap(hdc,AF*w,AF*h);
    LargeOldHbm = (HBITMAP)SelectObject(LargeDC,LargeHbm);
    
    BITMAPINFOHEADER bi3 = bi;
    bi3.biWidth = w*AF;
    bi3.biHeight = h*AF;
  
    int pitch3 = ((bi3.biWidth * bi3.biBitCount + 31)/ 32) * 4;
    DWORD dwBmpSize3 = pitch3 * bi3.biHeight;
  
    HANDLE hDIB3 = GlobalAlloc(GHND,dwBmpSize3);
    unsigned char *lpbitmap3 = (unsigned char *)GlobalLock(hDIB3);
  
    GetDIBits(ResDC, ResHbm, 0,(UINT)(bi3.biHeight),lpbitmap3,(BITMAPINFO *)&bi3, DIB_RGB_COLORS);
    
    for (double x = 0;x < w*AF;x++) for (double y = 0;y < h*AF;y++){
      int x2,y2;
      x2 = (x/AF+dx)*cos(a)+(y/AF+dy)*sin(a);
      y2 = -double(x/AF+dx)*sin(a)+(y/AF+dy)*cos(a);
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = b;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = g;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = r;        
      } else {
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = B;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = G;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = R;     
      }    
    }
    
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
    COLORREF res;
    
    int r,g,b;
    r = 0;
    g = 0;
    b = 0;
    
    for (int X = 0;X < AF;X++) for (int Y = 0;Y < AF;Y++){
      b += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X));
      g += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+1);
      r += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+2); 
    }
    
    r /= AF*AF;
    g /= AF*AF;
    b /= AF*AF;
    *(lpbitmap2+pitch2*y+4*x) = b;
    *(lpbitmap2+pitch2*y+4*x+1) = g;
    *(lpbitmap2+pitch2*y+4*x+2) = r;  
    }
    
    SelectObject(LargeDC,LargeOldHbm);
    DeleteObject(LargeHbm);
    DeleteDC(LargeDC);
    GlobalFree(hDIB3);      
  } else {
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
      int x2,y2;
      x2 = (x+dx)*cos(a)+(y+dy)*sin(a);
      y2 = -double(x+dx)*sin(a)+(y+dy)*cos(a);
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap2+y*pitch2+x*4) = b;
        *(lpbitmap2+y*pitch2+x*4+1) = g;
        *(lpbitmap2+y*pitch2+x*4+2) = r;
      } else {
        *(lpbitmap2+y*pitch2+x*4) = B;
        *(lpbitmap2+y*pitch2+x*4+1) = G;
        *(lpbitmap2+y*pitch2+x*4+2) = R;     
      } 
    }     
  }  
  
  SetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);          
  
  SelectObject(InDC,InOldHbm);
  SelectObject(ResDC,ResOldHbm);
  DeleteDC(InDC);
  DeleteDC(ResDC);
  
  GlobalFree(hDIB);
  GlobalFree(hDIB2);
  
  
  return ResHbm;  
          
}

//GetDIBits, SetDIBits wyliczanie sinusa i cosinusa raz
HBITMAP Rotate3(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p){
  
  double Cos = cos(a);
  double Sin = sin(a);
  
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  HDC InDC,LargeDC,ResDC;
  HBITMAP LargeHbm,ResHbm;
  HBITMAP InOldHbm,LargeOldHbm,ResOldHbm;
  
  InDC = CreateCompatibleDC(hdc);
  ResDC = CreateCompatibleDC(hdc);
  
  InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  int x2,y2;
  
  int X = bm.bmWidth;
  int Y = 0;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = bm.bmWidth;
  Y = bm.bmHeight;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = 0;
  Y = bm.bmHeight;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  int W,H;
  W = maxx-minx;
  H = maxy-miny;
  
  int w,h;
  
  w = W+2;
  h = H+2;
  
  //w = int(abs(Cos*bm.bmWidth) + abs(Sin*bm.bmHeight)+0.5);
  //h = int(abs(Cos*bm.bmHeight) + abs(Sin*bm.bmWidth)+0.5);
  
  BITMAPINFOHEADER   bi;
     
  bi.biSize = sizeof(BITMAPINFOHEADER);    
  bi.biWidth = bm.bmWidth;    
  bi.biHeight = bm.bmHeight;  
  bi.biPlanes = 1;    
  bi.biBitCount = 32;    
  bi.biCompression = BI_RGB;    
  bi.biSizeImage = 0;  
  bi.biXPelsPerMeter = 0;    
  bi.biYPelsPerMeter = 0;    
  bi.biClrUsed = 0;    
  bi.biClrImportant = 0;
  
  int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
  DWORD dwBmpSize = pitch * bi.biHeight;

  HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
  unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

  GetDIBits(InDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
  BITMAPINFOHEADER bi2 = bi;
  bi2.biWidth = w;
  bi2.biHeight = h;
  
  int pitch2 = ((bi2.biWidth * bi2.biBitCount + 31)/ 32) * 4;
  DWORD dwBmpSize2 = pitch2 * bi2.biHeight;
  
  HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
  unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
  
  ResHbm = CreateCompatibleBitmap(hdc,w,h);
  ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
  
  GetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);
   
  double dx,dy;
  dx = 0;
    if ((a >= 0) && (a <= 3.14/2.0)){
      dy = 0;
      dx = -Sin*bm.bmHeight;       
    } else if ((a > 3.14/2.0) && (a <= 3.14)){
      dy = Cos*bm.bmHeight;
      dx = +Cos*bm.bmWidth - Sin*bm.bmHeight;     
    } else if ((a > 3.14) && (a <= 3.14/2.0*3.0)){
      dx = Cos*bm.bmWidth;
      dy = Cos*bm.bmHeight+Sin*bm.bmWidth;     
    } else {
      dy = Sin*bm.bmWidth;     
    }
    
    int R = Background % 256;
    Background /= 256;
    int G = Background % 256;
    Background /= 256;
    int B = Background % 256;
    
  if (AF > 1){
    LargeDC = CreateCompatibleDC(hdc);
    LargeHbm = CreateCompatibleBitmap(hdc,AF*w,AF*h);
    LargeOldHbm = (HBITMAP)SelectObject(LargeDC,LargeHbm);
    
    BITMAPINFOHEADER bi3 = bi;
    bi3.biWidth = w*AF;
    bi3.biHeight = h*AF;
  
    int pitch3 = ((bi3.biWidth * bi3.biBitCount + 31)/ 32) * 4;
    DWORD dwBmpSize3 = pitch3 * bi3.biHeight;
  
    HANDLE hDIB3 = GlobalAlloc(GHND,dwBmpSize3);
    unsigned char *lpbitmap3 = (unsigned char *)GlobalLock(hDIB3);
  
    GetDIBits(ResDC, ResHbm, 0,(UINT)(bi3.biHeight),lpbitmap3,(BITMAPINFO *)&bi3, DIB_RGB_COLORS);
    
    for (double x = 0;x < w*AF;x++) for (double y = 0;y < h*AF;y++){
      int x2,y2;
      x2 = (x/AF+dx)*Cos+(y/AF+dy)*Sin;
      y2 = -double(x/AF+dx)*Sin+(y/AF+dy)*Cos;
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = b;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = g;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = r;        
      } else {
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = B;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = G;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = R;     
      }    
    }
    
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
    COLORREF res;
    
    int r,g,b;
    r = 0;
    g = 0;
    b = 0;
    
    for (int X = 0;X < AF;X++) for (int Y = 0;Y < AF;Y++){
      b += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X));
      g += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+1);
      r += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+2); 
    }
    
    r /= AF*AF;
    g /= AF*AF;
    b /= AF*AF;
    *(lpbitmap2+pitch2*y+4*x) = b;
    *(lpbitmap2+pitch2*y+4*x+1) = g;
    *(lpbitmap2+pitch2*y+4*x+2) = r;  
    }
    
    SelectObject(LargeDC,LargeOldHbm);
    DeleteObject(LargeHbm);
    DeleteDC(LargeDC);
    GlobalFree(hDIB3);      
  } else {
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
      int x2,y2;
      x2 = (x+dx)*Cos+(y+dy)*Sin;
      y2 = -double(x+dx)*Sin+(y+dy)*Cos;
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap2+y*pitch2+x*4) = b;
        *(lpbitmap2+y*pitch2+x*4+1) = g;
        *(lpbitmap2+y*pitch2+x*4+2) = r;
      } else {
        *(lpbitmap2+y*pitch2+x*4) = B;
        *(lpbitmap2+y*pitch2+x*4+1) = G;
        *(lpbitmap2+y*pitch2+x*4+2) = R;     
      } 
    }     
  }  
  
  SetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);          
  
  SelectObject(InDC,InOldHbm);
  SelectObject(ResDC,ResOldHbm);
  DeleteDC(InDC);
  DeleteDC(ResDC);
  
  GlobalFree(hDIB);
  GlobalFree(hDIB2);
  
  
  return ResHbm;  
}

//GetDIBits, SetDIBits w zewnêtrznej pêtli y zamiast x
HBITMAP Rotate4(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p){
  
  double Cos = cos(a);
  double Sin = sin(a);
  
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  HDC InDC,LargeDC,ResDC;
  HBITMAP LargeHbm,ResHbm;
  HBITMAP InOldHbm,LargeOldHbm,ResOldHbm;
  
  InDC = CreateCompatibleDC(hdc);
  ResDC = CreateCompatibleDC(hdc);
  
  InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  int x2,y2;
  
  int X = bm.bmWidth;
  int Y = 0;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = bm.bmWidth;
  Y = bm.bmHeight;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = 0;
  Y = bm.bmHeight;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  int W,H;
  W = maxx-minx;
  H = maxy-miny;
  
  int w,h;
  
  w = W+2;
  h = H+2;
  
  BITMAPINFOHEADER   bi;
     
  bi.biSize = sizeof(BITMAPINFOHEADER);    
  bi.biWidth = bm.bmWidth;    
  bi.biHeight = bm.bmHeight;  
  bi.biPlanes = 1;    
  bi.biBitCount = 32;    
  bi.biCompression = BI_RGB;    
  bi.biSizeImage = 0;  
  bi.biXPelsPerMeter = 0;    
  bi.biYPelsPerMeter = 0;    
  bi.biClrUsed = 0;    
  bi.biClrImportant = 0;
  
  int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
  DWORD dwBmpSize = pitch * bi.biHeight;

  HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
  unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

  GetDIBits(InDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
  BITMAPINFOHEADER bi2 = bi;
  bi2.biWidth = w;
  bi2.biHeight = h;
  
  int pitch2 = ((bi2.biWidth * bi2.biBitCount + 31)/ 32) * 4;
  DWORD dwBmpSize2 = pitch2 * bi2.biHeight;
  
  HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
  unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
  
  ResHbm = CreateCompatibleBitmap(hdc,w,h);
  ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
  
  GetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);
   
  double dx,dy;
  dx = 0;
    if ((a >= 0) && (a <= 3.14/2.0)){
      dy = 0;
      dx = -Sin*bm.bmHeight;       
    } else if ((a > 3.14/2.0) && (a <= 3.14)){
      dy = Cos*bm.bmHeight;
      dx = +Cos*bm.bmWidth - Sin*bm.bmHeight;     
    } else if ((a > 3.14) && (a <= 3.14/2.0*3.0)){
      dx = Cos*bm.bmWidth;
      dy = Cos*bm.bmHeight+Sin*bm.bmWidth;     
    } else {
      dy = Sin*bm.bmWidth;     
    }
    
    int R = Background % 256;
    Background /= 256;
    int G = Background % 256;
    Background /= 256;
    int B = Background % 256;
    
  if (AF > 1){
    LargeDC = CreateCompatibleDC(hdc);
    LargeHbm = CreateCompatibleBitmap(hdc,AF*w,AF*h);
    LargeOldHbm = (HBITMAP)SelectObject(LargeDC,LargeHbm);
    
    BITMAPINFOHEADER bi3 = bi;
    bi3.biWidth = w*AF;
    bi3.biHeight = h*AF;
  
    int pitch3 = ((bi3.biWidth * bi3.biBitCount + 31)/ 32) * 4;
    DWORD dwBmpSize3 = pitch3 * bi3.biHeight;
  
    HANDLE hDIB3 = GlobalAlloc(GHND,dwBmpSize3);
    unsigned char *lpbitmap3 = (unsigned char *)GlobalLock(hDIB3);
  
    GetDIBits(ResDC, ResHbm, 0,(UINT)(bi3.biHeight),lpbitmap3,(BITMAPINFO *)&bi3, DIB_RGB_COLORS);
    
    for (double y = 0;y < h*AF;y++) for (double x = 0;x < w*AF;x++){
      int x2,y2;
      x2 = (x/AF+dx)*Cos+(y/AF+dy)*Sin;
      y2 = -double(x/AF+dx)*Sin+(y/AF+dy)*Cos;
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = b;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = g;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = r;        
      } else {
        *(lpbitmap3+int(y)*pitch3+int(x)*4) = B;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = G;
        *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = R;     
      }    
    }
    
    for (int x = 0;x < w;x++) for (int y = 0;y < h;y++){
    COLORREF res;
    
    int r,g,b;
    r = 0;
    g = 0;
    b = 0;
    
    for (int X = 0;X < AF;X++) for (int Y = 0;Y < AF;Y++){
      b += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X));
      g += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+1);
      r += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+2); 
    }
    
    r /= AF*AF;
    g /= AF*AF;
    b /= AF*AF;
    *(lpbitmap2+pitch2*y+4*x) = b;
    *(lpbitmap2+pitch2*y+4*x+1) = g;
    *(lpbitmap2+pitch2*y+4*x+2) = r;  
    }
    
    SelectObject(LargeDC,LargeOldHbm);
    DeleteObject(LargeHbm);
    DeleteDC(LargeDC);
    GlobalFree(hDIB3);      
  } else {
    for (int y = 0;y < h;y++) for (int x = 0;x < w;x++){
      int x2,y2;
      x2 = (x+dx)*Cos+(y+dy)*Sin;
      y2 = -double(x+dx)*Sin+(y+dy)*Cos;
      if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
        int b = *(lpbitmap+y2*pitch+x2*4);
        int g = *(lpbitmap+y2*pitch+x2*4+1);
        int r = *(lpbitmap+y2*pitch+x2*4+2);
        *(lpbitmap2+y*pitch2+x*4) = b;
        *(lpbitmap2+y*pitch2+x*4+1) = g;
        *(lpbitmap2+y*pitch2+x*4+2) = r;
      } else {
        *(lpbitmap2+y*pitch2+x*4) = B;
        *(lpbitmap2+y*pitch2+x*4+1) = G;
        *(lpbitmap2+y*pitch2+x*4+2) = R;     
      } 
    }     
  }  
  
  SetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);          
  
  SelectObject(InDC,InOldHbm);
  SelectObject(ResDC,ResOldHbm);
  DeleteDC(InDC);
  DeleteDC(ResDC);
  
  GlobalFree(hDIB);
  GlobalFree(hDIB2);
  
  
  return ResHbm;  
}

//GetDIBits, SetDIBits z pe³n¹ optymalizacj¹
HBITMAP Rotate5(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p,TPoint *outp){
  if ((a < 0.001) || (a > 2.0*double(PI)-0.001)){
    BITMAP bm;
    GetObject(Hbm,sizeof(BITMAP),&bm);
    
    HDC ResDC,InDC;
    HBITMAP ResHbm,ResOldHbm,InOldHbm;
    ResDC = CreateCompatibleDC(hdc);
    InDC = CreateCompatibleDC(hdc);
    ResHbm = CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
    ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
    InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
    
    BitBlt(ResDC,0,0,bm.bmWidth,bm.bmHeight,InDC,0,0,SRCCOPY);
    
    SelectObject(InDC,InOldHbm);
    DeleteDC(InDC);
    SelectObject(ResDC,ResOldHbm);
    DeleteDC(ResDC);
    
    if (outp != NULL){
      outp->x = p.x;
      outp->y = p.y;         
    }
    
    return ResHbm;       
  }
  
  if ((Abs(a-double(PI)/2.0) < 0.001) || (Abs(a-double(PI)) < 0.001) || (Abs(a-double(PI)*3.0/2.0) < 0.001)){              
  
    BITMAP bm;
    GetObject(Hbm,sizeof(BITMAP),&bm);
    
    HDC InDC,ResDC;
    HBITMAP ResHbm;
    HBITMAP InOldHbm,ResOldHbm;
    
    InDC = CreateCompatibleDC(hdc);
    ResDC = CreateCompatibleDC(hdc);
    
    InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
    
    BITMAPINFOHEADER   bi;
     
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bm.bmWidth;    
    bi.biHeight = bm.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;
  
    int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
    DWORD dwBmpSize = pitch * bi.biHeight;

    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
    unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

    GetDIBits(InDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
    BITMAPINFOHEADER bi2 = bi;
    
    if (Abs(a-PI/2.0) < 0.001){
                      
      BITMAPINFOHEADER bi2 = bi;
      bi2.biWidth = bm.bmHeight;
      bi2.biHeight = bm.bmWidth;
      
      int pitch2 = ((bi2.biWidth*bi2.biBitCount + 31)/32)*4;
      
      DWORD dwBmpSize2 = pitch2*bi2.biHeight;
      
      HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
      unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
      
      ResHbm = CreateCompatibleBitmap(hdc,bm.bmHeight,bm.bmWidth);
      ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);

      for (int y = 0;y < bm.bmHeight;y++) for (int x = 0;x < bm.bmWidth;x++){
        *(lpbitmap2+x*pitch2+(bm.bmHeight-1-y)*4) = *(lpbitmap+y*pitch+x*4);
        *(lpbitmap2+x*pitch2+(bm.bmHeight-1-y)*4+1) = *(lpbitmap+y*pitch+x*4+1);
        *(lpbitmap2+x*pitch2+(bm.bmHeight-1-y)*4+2) = *(lpbitmap+y*pitch+x*4+2);    
      }
      
      SetDIBits(ResDC,ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);
      
      if (outp != NULL){
        outp->x = p.y;
        outp->y = bm.bmWidth-1-p.x;         
      }
                   
    } else if (Abs(a-PI) < 0.001){
      HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize);
      unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
      
      ResHbm = CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
      ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
      
      for (int y = 0;y < bm.bmHeight;y++) for (int x = 0;x < bm.bmWidth;x++){
        *(lpbitmap2+(bm.bmHeight-1-y)*pitch+(bm.bmWidth-1-x)*4) = *(lpbitmap+y*pitch+x*4);
        *(lpbitmap2+(bm.bmHeight-1-y)*pitch+(bm.bmWidth-1-x)*4+1) = *(lpbitmap+y*pitch+x*4+1);
        *(lpbitmap2+(bm.bmHeight-1-y)*pitch+(bm.bmWidth-1-x)*4+2) = *(lpbitmap+y*pitch+x*4+2);    
      }
      
      SetDIBits(ResDC,ResHbm, 0,(UINT)(bi.biHeight),lpbitmap2,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
      
      if (outp != NULL){
        outp->x = bm.bmWidth-1-p.x;
        outp->y = bm.bmHeight-1-p.y;         
      }
      
    } else {
                      
      BITMAPINFOHEADER bi2 = bi;
      bi2.biWidth = bm.bmHeight;
      bi2.biHeight = bm.bmWidth;
      
      int pitch2 = ((bi2.biWidth*bi2.biBitCount + 31)/32)*4;
      
      DWORD dwBmpSize2 = pitch2*bi2.biHeight;
      
      HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
      unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);
      
      ResHbm = CreateCompatibleBitmap(hdc,bm.bmHeight,bm.bmWidth);
      ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
      
      for (int y = 0;y < bm.bmHeight;y++) for (int x = 0;x < bm.bmWidth;x++){
        *(lpbitmap2+(bm.bmWidth-1-x)*pitch2+y*4) = *(lpbitmap+y*pitch+x*4);
        *(lpbitmap2+(bm.bmWidth-1-x)*pitch2+y*4+1) = *(lpbitmap+y*pitch+x*4+1);
        *(lpbitmap2+(bm.bmWidth-1-x)*pitch2+y*4+2) = *(lpbitmap+y*pitch+x*4+2);    
      }
      
      SetDIBits(ResDC,ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);
      
      if (outp != NULL){
        outp->x = bm.bmHeight-1-p.y;
        outp->y = p.x;         
      }
      
    }
    
    SelectObject(InDC,InOldHbm);
    DeleteDC(InDC);
    SelectObject(ResDC,ResOldHbm);
    DeleteDC(ResDC);
  
    return ResHbm;                 
  } 
  
  double Cos = cos(2*PI-a);
  double Sin = sin(2*PI-a);
  
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  HDC InDC,LargeDC,ResDC;
  HBITMAP LargeHbm,ResHbm;
  HBITMAP InOldHbm,LargeOldHbm,ResOldHbm;
  
  InDC = CreateCompatibleDC(hdc);
  ResDC = CreateCompatibleDC(hdc);
  
  InOldHbm = (HBITMAP)SelectObject(InDC,Hbm);
  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  int x2,y2;

  x2 = (bm.bmWidth-p.x)*Cos + (-p.y)*Sin;
  y2 = (-p.y)*Cos - (bm.bmWidth-p.x)*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  x2 = (bm.bmWidth-p.x)*Cos + (bm.bmHeight-p.y)*Sin;
  y2 = (bm.bmHeight-p.y)*Cos - (bm.bmWidth-p.x)*Sin;            
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  x2 = (-p.x)*Cos + (bm.bmHeight-p.y)*Sin;
  y2 = (bm.bmHeight-p.y)*Cos - (-p.x)*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  x2 = (-p.x)*Cos + (-p.y)*Sin;
  y2 = (-p.y)*Cos - (-p.x)*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;

  int w,h;
  
  w = maxx-minx+2;
  h = maxy-miny+2;
  
  BITMAPINFOHEADER   bi;
     
  bi.biSize = sizeof(BITMAPINFOHEADER);    
  bi.biWidth = bm.bmWidth;    
  bi.biHeight = bm.bmHeight;  
  bi.biPlanes = 1;    
  bi.biBitCount = 32;    
  bi.biCompression = BI_RGB;    
  bi.biSizeImage = 0;  
  bi.biXPelsPerMeter = 0;    
  bi.biYPelsPerMeter = 0;    
  bi.biClrUsed = 0;    
  bi.biClrImportant = 0;
  
  int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
  DWORD dwBmpSize = pitch * bi.biHeight;

  HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
  unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

  GetDIBits(InDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
  ResHbm = CreateCompatibleBitmap(hdc,w,h);
  ResOldHbm = (HBITMAP)SelectObject(ResDC,ResHbm);
  
  BITMAPINFOHEADER bi2 = bi;
    bi2.biWidth = w;
    bi2.biHeight = h;
    
    int pitch2 = ((bi2.biWidth * bi2.biBitCount + 31)/ 32) * 4;
    DWORD dwBmpSize2 = pitch2 * bi2.biHeight;
   
  Sin = sin(a);
  Cos = cos(a);    
    
    if (outp != NULL){
      outp->x = -minx;
      outp->y = -miny;
    }            
    
    int R = Background % 256;
    Background /= 256;
    int G = Background % 256;
    Background /= 256;
    int B = Background % 256;
    
  if (AF > 1){
    
    BITMAPINFOHEADER bi3 = bi;
    bi3.biWidth = w*AF;
    bi3.biHeight = h*AF;
  
    int pitch3 = ((bi3.biWidth * bi3.biBitCount + 31)/ 32) * 4;
    DWORD dwBmpSize3 = pitch3 * bi3.biHeight;
  
    HANDLE hDIB3 = GlobalAlloc(GHND,dwBmpSize3);
    unsigned char *lpbitmap3 = (unsigned char *)GlobalLock(hDIB3);
  
    double x2,y2;
    double cosaf = Cos/AF;
    double sinaf = Sin/AF;
    int H,W;
    H = h*AF;
    W = w*AF;
    x2 = minx*Cos+miny*Sin-cosaf-sinaf+p.x;
    y2 = miny*Cos-Sin*minx+sinaf-cosaf+p.y;
    for (double y = 0;y < H;y++){ 
      x2 += sinaf;
      y2 += cosaf;
      for (double x = 0;x < W;x++){
        x2 += cosaf;
        y2 -= sinaf;
        if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
          int b = *(lpbitmap+int(y2)*pitch+int(x2)*4);
          int g = *(lpbitmap+int(y2)*pitch+int(x2)*4+1);
          int r = *(lpbitmap+int(y2)*pitch+int(x2)*4+2);
          *(lpbitmap3+int(y)*pitch3+int(x)*4) = b;
          *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = g;
          *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = r;        
        } else {
          *(lpbitmap3+int(y)*pitch3+int(x)*4) = B;
          *(lpbitmap3+int(y)*pitch3+int(x)*4+1) = G;
          *(lpbitmap3+int(y)*pitch3+int(x)*4+2) = R;     
        }   
      }
      x2 -= cosaf*w*AF;
      y2 += sinaf*w*AF;
    }    
    
    for (int y = 0;y < h;y++) for (int x = 0;x < w;x++){
    COLORREF res;
    
    int r,g,b;
    r = 0;
    g = 0;
    b = 0;
    
    for (int Y = 0;Y < AF;Y++) for (int X = 0;X < AF;X++){
      b += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X));
      g += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+1);
      r += *(lpbitmap3+pitch3*(y*AF+Y)+4*(x*AF+X)+2); 
    }
    
    r /= AF*AF;
    g /= AF*AF;
    b /= AF*AF;
    *(lpbitmap3+pitch2*y+4*x) = b;
    *(lpbitmap3+pitch2*y+4*x+1) = g;
    *(lpbitmap3+pitch2*y+4*x+2) = r;  
    }
    
    SetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap3,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);         
  
    SelectObject(InDC,InOldHbm);
    SelectObject(ResDC,ResOldHbm);
    DeleteDC(InDC);
    DeleteDC(ResDC);
  
    GlobalFree(hDIB);
    GlobalFree(hDIB3);
  
  
    return ResHbm;  
          
  } else {
  
    HANDLE hDIB2 = GlobalAlloc(GHND,dwBmpSize2);
    unsigned char *lpbitmap2 = (unsigned char *)GlobalLock(hDIB2);    
         
    double x2,y2;
    x2 = (minx-1)*Cos+(miny-1)*Sin+p.x;
    y2 = -(minx-1)*Sin+(miny-1)*Cos+p.y;
    for (int y = 0;y < h;y++){
      x2 += Sin;
      y2 += Cos;
      for (int x = 0;x < w;x++){
        x2 += Cos;
        y2 -= Sin;                              
        if (((x2 >= 0) && (x2 < bm.bmWidth)) && ((y2 >= 0) && (y2 < bm.bmHeight))){
          int b = *(lpbitmap+int(y2)*pitch+int(x2)*4);
          int g = *(lpbitmap+int(y2)*pitch+int(x2)*4+1);
          int r = *(lpbitmap+int(y2)*pitch+int(x2)*4+2);
          *(lpbitmap2+y*pitch2+x*4) = b;
          *(lpbitmap2+y*pitch2+x*4+1) = g;
          *(lpbitmap2+y*pitch2+x*4+2) = r;
        } else {
          *(lpbitmap2+y*pitch2+x*4) = B;
          *(lpbitmap2+y*pitch2+x*4+1) = G;
          *(lpbitmap2+y*pitch2+x*4+2) = R;     
        }   
      }
      x2 -= Cos*w;
      y2 += Sin*w;    
    }
    
    SetDIBits(ResDC, ResHbm, 0,(UINT)(bi2.biHeight),lpbitmap2,(BITMAPINFO *)&bi2, DIB_RGB_COLORS);          
  
  SelectObject(InDC,InOldHbm);
  SelectObject(ResDC,ResOldHbm);
  DeleteDC(InDC);
  DeleteDC(ResDC);
  
  GlobalFree(hDIB);
  GlobalFree(hDIB2);
  
  
  return ResHbm;        
  }  
  
    
}

bool CheckForColor(HDC hdc,HBITMAP Hbm,COLORREF Color){
     
  HDC TempDC = CreateCompatibleDC(hdc);
  HBITMAP OldBm = (HBITMAP)SelectObject(TempDC,Hbm);
  
  BITMAP bm;
  GetObject(Hbm,sizeof(BITMAP),&bm);
  
  BITMAPINFOHEADER   bi;
     
  bi.biSize = sizeof(BITMAPINFOHEADER);    
  bi.biWidth = bm.bmWidth;    
  bi.biHeight = bm.bmHeight;  
  bi.biPlanes = 1;    
  bi.biBitCount = 32;    
  bi.biCompression = BI_RGB;    
  bi.biSizeImage = 0;  
  bi.biXPelsPerMeter = 0;    
  
  bi.biYPelsPerMeter = 0;    
  bi.biClrUsed = 0;    
  bi.biClrImportant = 0;
  
  int pitch = ((bi.biWidth *bi.biBitCount + 31)/ 32) *4;
            
  DWORD dwBmpSize = pitch * bi.biHeight;

  HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
  unsigned char *lpbitmap = (unsigned char *)GlobalLock(hDIB); 

  GetDIBits(TempDC, Hbm, 0,(UINT)(bi.biHeight),lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);
  
  bool found = false;
  
  unsigned char R = Color%256;
  Color /= 256;
  unsigned char G = Color%256;
  Color /= 256;
  unsigned char B = Color%256;
  
  for (int x = 0;x < bi.biWidth;x++) for (int y = 0;y < bi.biHeight;y++){
    unsigned char b = *(lpbitmap+pitch*y+x*4);
    unsigned char g = *(lpbitmap+pitch*y+x*4+1);
    unsigned char r = *(lpbitmap+pitch*y+x*4+2);
    if ((R == r) && (G == g) && (B == b)){
      found = true;
      break;
    }    
  }
  
  GlobalFree(hDIB);
  
  SelectObject(TempDC,OldBm);
  DeleteDC(TempDC);
  
  return found;
       
}

bool once;

TPoint RotateOnBitmap(int x,int y,int Width,int Height,double a){

  int dx,dy;                          
  
  if ((a >= 0) && (a <= PI/2.0)){
      dy = 0;
      dx = -sin(a)*Height;       
    } else if ((a > PI/2.0) && (a <= PI)){
      dy = cos(a)*Height;
      dx = +cos(a)*Width - sin(a)*Height;     
    } else if ((a > PI) && (a <= PI/2.0*3.0)){
      dx = cos(a)*Width;
      dy = cos(a)*Height+sin(a)*Width;     
    } else {
      dx = 0;
      dy = sin(a)*Width;     
    }             
    
    int x2,y2;
    x2 = x*cos(a) - y*sin(a) - dx;
    y2 = y*cos(a) + x*sin(a) - dy;
      
  TPoint p;
  p.x = x2;
  p.y = y2;
  
  return p;
       
}

TPoint RotateOnBitmap2(int Width,int Height,double a,TPoint p){

  double Cos = cos(2*PI-a);
  double Sin = sin(2*PI-a);
  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
  
  int x2,y2;
  
  int X = Width-p.x;
  int Y = -p.y;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = Width-p.x;
  Y = Height-p.y;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;            
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = -p.x;
  Y = Height-p.y;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;
  
  X = -p.x;
  Y = -p.y;
  x2 = X*Cos + Y*Sin;
  y2 = Y*Cos - X*Sin;
  if (x2 < minx) minx = x2;
  if (x2 > maxx) maxx = x2;
  if (y2 < miny) miny = y2;
  if (y2 > maxy) maxy = y2;

  int dx,dy;
  
  dx = minx;
  dy = miny;                                       
  
      
  TPoint r;
  r.x = -dx;
  r.y = -dy;
  
  return r;
       
}

double DegToRad(double Deg){
  return (Deg/360.0)*2.0*PI;       
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent){
 HDC hdcMem, hdcMem2;
 HBITMAP hbmMask, hbmOld, hbmOld2;
 BITMAP bm;

 GetObject(hbmColour, sizeof(BITMAP), &bm);
 hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

 hdcMem = CreateCompatibleDC(NULL);
 hdcMem2 = CreateCompatibleDC(NULL);

 hbmOld = (HBITMAP)SelectObject(hdcMem, hbmColour);
 hbmOld2 = (HBITMAP)SelectObject(hdcMem2, hbmMask);

 SetBkColor(hdcMem, crTransparent);

 BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
 BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

 SelectObject(hdcMem, hbmOld);
 SelectObject(hdcMem2, hbmOld2);
 DeleteDC(hdcMem);
 DeleteDC(hdcMem2);

 return hbmMask;
}

bool IsLeap(int year){
   return (!(year % 4)) && !((!(year%100)) && (year%400));    
}

char* inttostr(int x){
  char* res = new char[10];
  if (x == 0){
    res[0] = '0';
    res[1] = 0;
    return res;
  }
  int i =9;
  while (x != 0){
    res[i] = 48+(x%10);
    x /= 10;
    i--;    
  }   
  for (int j = i+1;j <= 9;j++) res[j-i-1] = res[j];
  res[9-i] = 0;
  return res;
}

int strtoint(char* x){
  bool pos;
  int i = 0;
  if (x[0] == '-'){
    pos = false;
    i++;
  } else if (x[0] == '+'){
    pos = true;
    i++;
  }
  int res = 0;
  while (x[i] != 0){
    res *= 10;
    res += x[i] - '0';
    i++;
  }
  if (!pos) res = -res;
  return res;
}

void ShowMessage(char* msg){
  MessageBox(NULL,msg,"Message",MB_OK);   
}

bool Exist(const char *file){
  HANDLE hFile = CreateFileA(file,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
    OPEN_EXISTING,0,NULL);
  
  if (hFile == INVALID_HANDLE_VALUE) return false;
  
  CloseHandle(hFile);
  return true;
  
}

bool ValidDate(int y,int m,int d){
  bool res = true;
  res = res && (y >= 0) && (m > 0) && (m <= 12) && (d > 0);
  switch (m){
    case 1:{
      if (d > 31) return false;   
    } break;
    case 2:{
      if (d > 28+IsLeap(y)) return false;
    } break;
    case 3:{
      if (d > 31) return false;
    } break;
    case 4:{
      if (d > 30) return false;
    } break;
    case 5:{
      if (d > 31) return false;
    } break;
    case 6:{
      if (d > 30) return false;
    } break;
    case 7:{
      if (d > 31) return false;
    } break;
    case 8:{
      if (d > 31) return false;
    } break;
    case 9:{
      if (d > 30) return false;
    } break;
    case 10:{
      if (d > 31) return false;
    } break;
    case 11:{
      if (d > 30) return false;
    } break;
    case 12:{
      if (d > 31) return false;
    } break;       
  }
  return res;
}

int DayInTheYear(TDate Date){
  switch (Date.m){
    case 1:{
      return Date.d;   
    } break;
    case 2:{
      return Date.d+31;
    } break;
    case 3:{
      return Date.d+59+IsLeap(Date.y);
    } break;
    case 4:{
      return Date.d+90+IsLeap(Date.y);
    } break;
    case 5:{
      return Date.d+120+IsLeap(Date.y);
    } break;
    case 6:{
      return Date.d+151+IsLeap(Date.y);
    } break;
    case 7:{
      return Date.d+181+IsLeap(Date.y);
    } break;
    case 8:{
      return Date.d+212+IsLeap(Date.y);
    } break;
    case 9:{
      return Date.d+243+IsLeap(Date.y);
    } break;
    case 10:{
      return Date.d+273+IsLeap(Date.y);
    } break;
    case 11:{
      return Date.d+304+IsLeap(Date.y);
    } break;
    case 12:{
      return Date.d+334+IsLeap(Date.y);
    } break;       
  }    
}

bool OnlyNumbers(char* x){
  bool res = true;
  for (int i = 0;x[i] != 0;i++) res = res && (x[i] >= '0') && (x[i] <= '9'); 
  return res;    
}

LPSTR GetWindowTextFast(HWND hwnd){
  DWORD l = GetWindowTextLength(hwnd);
  LPSTR Buffer = (LPSTR)GlobalAlloc(GPTR,l+1);
  GetWindowText(hwnd,Buffer,l+1);
  return Buffer;      
}

bool GetWindowTextFast(HWND hwnd,LPSTR Buffer){
  DWORD l = GetWindowTextLength(hwnd);
  if (!l) return false;
  else {
    Buffer = (LPSTR)GlobalAlloc(GPTR,l+1);
    GetWindowText(hwnd,Buffer,l+1);
  }
  return true;     
}

void WritelnInConsole(char* x){
     
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
 
    DWORD X;

    WriteConsole(hOut,x,strlen(x),&X,NULL);   
    WriteConsole(hOut,"\n",1,&X,NULL);
      
}

void WriteInConsole(char* x){
     
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
 
    DWORD X;

    WriteConsole(hOut,x,strlen(x),&X,NULL);   
      
}

int DaysAfter22(int y){
  int a = y/100;
  int b = y % 100;
  int c = (3*(a+25))/4;
  int d = (3*(a+25))%4;
  int e = (8*(a+11))/25;
  int f = (5*a+b)%19;
  int g = (19*f + c - e) % 30;
  int h = (f + 11*g)/319;
  int j = (60*(5-d)+b)/4;
  int k = (60*(5-d)+b)%4;
  int m = (2*j - k - g + h)%7;
  int n = (g-h+m+114)/31;
  int p = (g-h+m+114)%31;
  int dy = p+1;
  int mth = n;
  return g - h + m;  
}

TDate Easter(int y){
  TDate Res;
  Res.d = 22+DaysAfter22(y);
  if (Res.d > 31){
    Res.m = 4;
    Res.d -= 31;          
  } else Res.m = 3;
  Res.y = y;
  return Res;
}

TDate BozeCialo(int y){
  TDate Res = Easter(y);
  Res.d += 60;
  if ((Res.m == 3) && (Res.d > 31)){
    Res.m = 4;
    Res.d -= 31;           
  }   
  if ((Res.m == 4) && (Res.d > 30)){
    Res.m = 5;
    Res.d -= 30;           
  }
  if ((Res.m == 5) && (Res.d > 31)){
    Res.m = 6;
    Res.d -= 31;           
  }
  return Res;
}

int DayOfTheWeek(TDate Date){
  int code;
  switch (Date.m){
    case 1: code = 6-IsLeap(Date.y); break;
    case 2: code = 2-IsLeap(Date.y); break;
    case 3: code = 2; break;
    case 4: code = 5; break;
    case 5: code = 0; break;
    case 6: code = 3; break;
    case 7: code = 5; break;
    case 8: code = 1; break;
    case 9: code = 4; break;
    case 10: code = 6; break;
    case 11: code = 2; break;
    case 12: code = 4; break;       
  }
  code += Date.d;
  code += int((Date.y - int(Date.y/100)*100)*5/4);
  switch (int((Date.y-int(Date.y/400)*400)/100)){
    case 1: code += 5; break;
    case 2: code += 3; break;
    case 3: code += 1; break;       
  }
  return code%7;
}

TDate DayToDate(int year,int day){
  TDate res;
  for (;;){
  res.m = 1;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 28+IsLeap(year)) day -= 28+IsLeap(year);
  else break;
  res.m++;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 30) day -= 30;
  else break;
  res.m++;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 30) day -= 30;
  else break;
  res.m++;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 30) day -= 30;
  else break;
  res.m++;
  if (day > 31) day -= 31;
  else break;
  res.m++;
  if (day > 30) day -= 30;
  else break;
  res.m++;
  break;
  }
  res.y = year;
  res.d = day;
  return res;   
}

void ClearRect(RECT &rc){
  rc.left = 0;
  rc.right = 0;
  rc.top = 0;
  rc.bottom = 0;     
}

int Max(int x,int y){
  if (x > y) return x;
  else return y;   
}

int Min(int x,int y){
  if (x < y) return x;
  else return y;   
}

COLORREF Fade(COLORREF Color){
  int r,g,b;
  r = (Color % 256);
  Color /= 256;
  g = (Color % 256);
  Color /= 256;
  b = (Color % 256);
  int mid = (r+g+b)/3;
  r = r + (mid-r)/1.2;
  g = g + (mid-g)/1.2;
  b = b + (mid-b)/1.2;
  COLORREF Res;
  Res = b*256*256+g*256+r;
  return Res;
}

COLORREF Darken(COLORREF Color){
  int r,g,b;
  r = (Color % 256);
  Color /= 256;
  g = (Color % 256);
  Color /= 256;
  b = (Color % 256);
  r = Max(r-32,0);
  g = Max(g-32,0);
  b = Max(b-32,0);
  COLORREF Res;
  Res = b*256*256+g*256+r;
  return Res;      
}

void ActivateFullscreen(){    
    
  DEVMODE Temp;
  Temp.dmSize = sizeof(DEVMODE);
  Temp.dmDriverExtra = 0;
  FS.dmSize = sizeof(DEVMODE);
  FS.dmDriverExtra = 0;
  EnumDisplaySettings(NULL,0,&FS);
  int i = 1;
  while (EnumDisplaySettings(NULL,i,&Temp)){
    if (Temp.dmPelsWidth > FS.dmPelsWidth) FS = Temp;
    else if (Temp.dmPelsWidth ==  FS.dmPelsWidth){
      if (Temp.dmPelsHeight > FS.dmPelsHeight) FS = Temp;
      else if (Temp.dmPelsHeight == FS.dmPelsHeight){
        if (Temp.dmBitsPerPel > FS.dmBitsPerPel) FS = Temp;
        else if (Temp.dmBitsPerPel == FS.dmBitsPerPel){
          if (Temp.dmDisplayFrequency > FS.dmDisplayFrequency) FS = Temp;     
        }    
      }     
    }
    i++;
  }   
  WNDW = FS.dmPelsWidth;
  WNDH = FS.dmPelsHeight;
  
}

double RadToDeg(double Rad){
  return Rad/2/PI*360.0;       
}

double Abs(double x){
  if (x < 0) return -x;
  else return x;       
}

double sqr(double a){
  return a*a;
}
