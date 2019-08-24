#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void TakeScreenShot(char* filename);

int main()
{
	TakeScreenShot("Screenshot.bmp");
	return 0;
}

void TakeScreenShot(char* filename)
{
	keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
	keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	HBITMAP h;

	OpenClipboard(NULL);
	h = (HBITMAP)GetClipboardData(CF_BITMAP);
	CloseClipboard();
	HDC hdc=NULL;
    FILE* fp=NULL;
    LPVOID pBuf=NULL;
    BITMAPINFO bmpInfo;
    BITMAPFILEHEADER bmpFileHeader;
    do
    {
        hdc=GetDC(NULL);
        ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
        bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        GetDIBits(hdc,h,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);
        if(bmpInfo.bmiHeader.biSizeImage<=0)
            bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;
        if((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
        {
            MessageBox( NULL, "Unable to Allocate Bitmap Memory", "Error", MB_OK|MB_ICONERROR);
            break;
        }
        bmpInfo.bmiHeader.biCompression=BI_RGB;
        GetDIBits(hdc,h,0,bmpInfo.bmiHeader.biHeight,pBuf, &bmpInfo, DIB_RGB_COLORS);
        if((fp = fopen(filename,"wb"))==NULL)
        {
            MessageBox( NULL, "Unable to Create Bitmap File", "Error", MB_OK|MB_ICONERROR);
            break;
        }
        bmpFileHeader.bfReserved1=0;
        bmpFileHeader.bfReserved2=0;
        bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
        bmpFileHeader.bfType='MB';
        bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
        fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
        fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
        fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);
    }

    while(false);
    if(hdc)ReleaseDC(NULL,hdc);
    if(pBuf) free(pBuf);
    if(fp)fclose(fp);
}


/*int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;


    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          256,
                          256,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    EnableOpenGL(hwnd, &hDC, &hRC);

    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            glRotatef(theta, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);

                glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
                glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
                glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);

            glEnd();

            glPopMatrix();

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

