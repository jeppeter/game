//*****************************************************************************************
//
//��Visual C++����Ϸ�����ʼ�ϵ������Դ�� ��ʮ�� ǳīDirectX�̳�֮ʮ ��Ϸ����������� : DirectInputר��
//		 VS2010��
// 2012�� 1��27��  Create by ǳī 
//ͼ�꼰ͼƬ�زģ� ���ɽ���������ǰ���� �
//Դ�����ײ������ӣ�  http://blog.csdn.net/zhmxy555/article/details/8547531
//��������������ҵĲ��ͣ� http://blog.csdn.net/zhmxy555 
//�˿����飺��Ȼѡ����Զ������ֻ�˷�����
//
//***************************************************************************************** 


//*****************************************************************************************
// Desc: �궨�岿��   
//*****************************************************************************************
#define SCREEN_WIDTH	800						//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define SCREEN_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	_T("��Visual C++��Ϸ�����ʼǡ���������demo֮��ʮ�� ǳīDirectX�̳�֮ʮ ��Ϸ����������� : DirectInputר��") //Ϊ���ڱ��ⶨ��ĺ�
#define DIRECTINPUT_VERSION 0x0800  //ָ��DirectInput�汾����ֹDIRECTINPUT_VERSION undefined����
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }      //�Զ���һ��SAFE_RELEASE()��,����COM��Դ���ͷ�
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }


//*****************************************************************************************
// Desc: ͷ�ļ����岿��  
//*****************************************************************************************                                                                                       
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <time.h> 
#include <dinput.h>                 // ʹ��DirectInput���������ͷ�ļ���ע������û��8




//*****************************************************************************************
// Desc: ���ļ����岿��  
//***************************************************************************************** 
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dinput8.lib")     // ʹ��DirectInput���������ͷ�ļ���ע��������8
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "winmm.lib") 





//*****************************************************************************************
// Desc: ȫ�ֱ�����������  
//*****************************************************************************************
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; //Direct3D�豸����
LPD3DXFONT				  g_pTextFPS				=NULL;    //����COM�ӿ�
LPD3DXFONT              g_pTextAdaperName           = NULL;  // �Կ���Ϣ��2D�ı�
LPD3DXFONT              g_pTextHelper          = NULL;  // ������Ϣ��2D�ı�
LPD3DXFONT              g_pTextInfor           = NULL;  // ������Ϣ��2D�ı�
float					g_FPS								= 0.0f;       //һ�������͵ı���������֡����
wchar_t					g_strFPS[50]={0};    //����֡���ʵ��ַ�����
wchar_t				g_strAdapterName[60]={0};    //�����Կ����Ƶ��ַ�����

LPDIRECTINPUT8          g_pDirectInput      = NULL; //
LPDIRECTINPUTDEVICE8    g_pMouseDevice      = NULL;
DIMOUSESTATE            g_diMouseState      = {0};
LPDIRECTINPUTDEVICE8    g_pKeyboardDevice   = NULL;
char                    g_pKeyStateBuffer[256] = {0};
D3DXMATRIX      g_matWorld;   //�������

LPD3DXMESH          g_pMesh     = NULL; // ����Ķ���
D3DMATERIAL9*       g_pMaterials    = NULL; // ����Ĳ�����Ϣ
LPDIRECT3DTEXTURE9* g_pTextures     = NULL; // �����������Ϣ
DWORD               g_dwNumMtrls    = 0;    // ���ʵ���Ŀ

//*****************************************************************************************
// Desc: ȫ�ֺ����������� 
//***************************************************************************************** 
LRESULT CALLBACK	WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
HRESULT				Direct3D_Init(HWND hwnd,HINSTANCE hInstance);
HRESULT				Objects_Init();
void				Direct3D_Render( HWND hwnd);
void				Direct3D_Update( HWND hwnd);
void				Direct3D_CleanUp( );
float				Get_FPS();
void				Matrix_Set();
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize) ;


//*****************************************************************************************
// Name: WinMain( )
// Desc: WindowsӦ�ó�����ں���
//*****************************************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{

	//��ʼ���һ�������Ĵ�����
	WNDCLASSEX wndClass = { 0 };				//��WINDCLASSEX������һ�������࣬����wndClassʵ������WINDCLASSEX������֮�󴰿ڵĸ����ʼ��    
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;	//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;				//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance = hInstance;				//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon=(HICON)::LoadImage(NULL,_T("icon.ico"),IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE); //��ȫ�ֵ�::LoadImage�����ӱ��ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    //ָ��������Ĺ������
	wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = _T("ForTheDreamOfGameDevelop");		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	if( !RegisterClassEx( &wndClass ) )				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;		

	HWND hwnd = CreateWindow( _T("ForTheDreamOfGameDevelop"),WINDOW_TITLE,			//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH,
		SCREEN_HEIGHT, NULL, NULL, hInstance, NULL );


	//Direct3D��Դ�ĳ�ʼ��������ʧ����messagebox������ʾ
	if (!(S_OK==Direct3D_Init (hwnd,hInstance)))
	{
		MessageBox(hwnd, _T("Direct3D��ʼ��ʧ��~��"), _T("ǳī����Ϣ����"), 0); //ʹ��MessageBox����������һ����Ϣ���� 
	}



	MoveWindow(hwnd,200,50,SCREEN_WIDTH,SCREEN_HEIGHT,true);   //����������ʾʱ��λ�ã��������Ͻ�λ����Ļ���꣨200��50����
	ShowWindow( hwnd, nShowCmd );    //����Win32����ShowWindow����ʾ����
	UpdateWindow(hwnd);  //�Դ��ڽ��и��£��������������·���Ҫװ��һ��


	//��Ϣѭ������
	MSG msg = { 0 };  //��ʼ��msg
	while( msg.message != WM_QUIT )			//ʹ��whileѭ��
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage( &msg );		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage( &msg );		//�ú����ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			Direct3D_Update(hwnd);         //���ø��º��������л���ĸ���
			Direct3D_Render(hwnd);			//������Ⱦ���������л������Ⱦ			
		}
	}

	UnregisterClass(_T("ForTheDreamOfGameDevelop"), wndClass.hInstance);
	return 0;  
}



//*****************************************************************************************
// Name: WndProc()
// Desc: �Դ�����Ϣ���д���
//*****************************************************************************************
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )   //���ڹ��̺���WndProc
{
	switch( message )				//switch��俪ʼ
	{
	case WM_PAINT:					 // �ͻ����ػ���Ϣ
		Direct3D_Render(hwnd);          //����Direct3D_Render���������л���Ļ���
		ValidateRect(hwnd, NULL);   // ���¿ͻ�������ʾ
		break;									//������switch���

	case WM_KEYDOWN:                // ���̰�����Ϣ
		if (wParam == VK_ESCAPE)    // ESC��
			DestroyWindow(hwnd);    // ���ٴ���, ������һ��WM_DESTROY��Ϣ
		break;
	case WM_DESTROY:				//����������Ϣ
		Direct3D_CleanUp();     //����Direct3D_CleanUp����������COM�ӿڶ���
		PostQuitMessage( 0 );		//��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;						//������switch���

	default:						//������case�����������ϣ���ִ�и�default���
		return DefWindowProc( hwnd, message, wParam, lParam );		//����ȱʡ�Ĵ��ڹ�����ΪӦ�ó���û�д���Ĵ�����Ϣ�ṩȱʡ�Ĵ���
	}

	return 0;					//�����˳�
}


//*****************************************************************************************
// Name: Direct3D_Init( )
// Desc: ��ʼ��Direct3D
// Point:��Direct3D��ʼ���Ĳ�����
//		1.��ʼ���Ĳ���֮һ������Direct3D�ӿڶ���
//		2.��ʼ���Ĳ���֮������ȡӲ���豸��Ϣ
//		3.��ʼ���Ĳ���֮�������ṹ��
//		4.��ʼ���Ĳ���֮�ģ�����Direct3D�豸�ӿ�
//*****************************************************************************************

HRESULT Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
{

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮һ�����ӿڡ�������Direct3D�ӿڶ���, �Ա��ø�Direct3D���󴴽�Direct3D�豸����
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D�ӿڶ���Ĵ���
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) //��ʼ��Direct3D�ӿڶ��󣬲�����DirectX�汾Э��
 			return E_FAIL;

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮��,ȡ��Ϣ������ȡӲ���豸��Ϣ
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
		{
			return E_FAIL;
		}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //֧��Ӳ���������㣬���ǾͲ���Ӳ���������㣬���׵�
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //��֧��Ӳ���������㣬����ֻ�ò��������������

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮���������ݡ������D3DPRESENT_PARAMETERS�ṹ��
	//--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = SCREEN_WIDTH;
	d3dpp.BackBufferHeight           = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 2;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮�ģ����豸��������Direct3D�豸�ӿ�
	//--------------------------------------------------------------------------------------
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;


	//��ȡ�Կ���Ϣ��g_strAdapterName�У������Կ�����֮ǰ���ϡ���ǰ�Կ��ͺţ����ַ���
	 wchar_t TempName[60]=L"��ǰ�Կ��ͺţ�";   //����һ����ʱ�ַ������ҷ����˰�"��ǰ�Կ��ͺţ�"�ַ����������ǵ�Ŀ���ַ�����
	 D3DADAPTER_IDENTIFIER9 Adapter;  //����һ��D3DADAPTER_IDENTIFIER9�ṹ�壬���ڴ洢�Կ���Ϣ
	 pD3D->GetAdapterIdentifier(0,0,&Adapter);//����GetAdapterIdentifier����ȡ�Կ���Ϣ
	 int len = MultiByteToWideChar(CP_ACP,0, Adapter.Description, -1, NULL, 0);//�Կ����������Ѿ���Adapter.Description���ˣ�������Ϊchar���ͣ�����Ҫ����תΪwchar_t����
	 MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_strAdapterName, len);//�ⲽ������ɺ�g_strAdapterName�о�Ϊ��ǰ���ǵ��Կ���������wchar_t���ַ�����
	 wcscat_s(TempName,g_strAdapterName);//�ѵ�ǰ���ǵ��Կ����ӵ�����ǰ�Կ��ͺţ����ַ������棬�������TempName��
	 wcscpy_s(g_strAdapterName,TempName);//��TempName�еĽ��������ȫ�ֱ���g_strAdapterName�У��󹦸��~



	 //--------------------------------------------------------------------------------------
	 // ��DirectInputʹ���岽����ǰ�����������豸�����ʽ����Ȩ������Ϊ����豸��ʼ��
	 //--------------------------------------------------------------------------------------
	// ����DirectInput�ӿں��豸
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	//g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pMouseDevice, NULL);

	// �������ݸ�ʽ��Э������
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pMouseDevice, NULL);
	g_pMouseDevice->SetDataFormat(&c_dfDIMouse);

	//��ȡ�豸����Ȩ
	g_pMouseDevice->Acquire();

	//--------------------------------------------------------------------------------------
	// ��DirectInputʹ���岽����ǰ�����������豸�����ʽ����Ȩ������Ϊ�����豸��ʼ��
	//--------------------------------------------------------------------------------------
	// ����DirectInput�ӿں��豸
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pKeyboardDevice, NULL);

	// �������ݸ�ʽ��Э������
	g_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	g_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//��ȡ�豸����Ȩ
	g_pKeyboardDevice->Acquire();


	if(!(S_OK==Objects_Init())) return E_FAIL;

	SAFE_RELEASE(pD3D) //LPDIRECT3D9�ӿڶ����ʹ����ɣ����ǽ����ͷŵ�

	return S_OK;
}


HRESULT Objects_Init()
{
	//��������
	D3DXCreateFont(g_pd3dDevice, 36, 0, 0, 1000, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pTextFPS);
	D3DXCreateFont(g_pd3dDevice, 20, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"��������", &g_pTextAdaperName); 
	D3DXCreateFont(g_pd3dDevice, 23, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"΢���ź�", &g_pTextHelper); 
	D3DXCreateFont(g_pd3dDevice, 26, 0, 1000, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"����", &g_pTextInfor); 

	// ��X�ļ��м�����������
	LPD3DXBUFFER pAdjBuffer  = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pd3dDevice, 
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);

	// ��ȡ���ʺ���������
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];

	for (DWORD i=0; i<g_dwNumMtrls; i++) 
	{
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		g_pTextures[i]  = NULL;
		D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();


		// ������Ⱦ״̬
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������
		g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û�����
		
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//����Ϊ�����������
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);


	return S_OK;
}


//*****************************************************************************************
// Name:Matrix_Set()
// Desc: �����������
// Point:��Direct3D�Ĵ�任��
//		1.���Ĵ�任֮һ��������任���������
//		2.���Ĵ�任֮������ȡ���任���������
//		3.���Ĵ�任֮������ͶӰ�任���������
//		4.���Ĵ�任֮�ġ����ӿڱ任������
//*****************************************************************************************
void Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮һ��������任���������
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ȡ���任���������
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //����һ������
	D3DXVECTOR3 vEye(0.0f, 0.0f, -250.0f);  //�������λ��
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ͶӰ�任���������
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj; //����һ������
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f,(float)((double)SCREEN_WIDTH/SCREEN_HEIGHT),1.0f, 1000.0f); //����ͶӰ�任����
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮�ġ����ӿڱ任������
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	vp.X      = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	vp.Y      = 0;		//�ӿ���ԶԴ��ڵ�Y����
	vp.Width  = SCREEN_WIDTH;	//�ӿڵĿ��
	vp.Height = SCREEN_HEIGHT; //�ӿڵĸ߶�
	vp.MinZ   = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	vp.MaxZ   = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	g_pd3dDevice->SetViewport(&vp); //�ӿڵ�����

}


void				Direct3D_Update( HWND hwnd)
{
	// ��ȡ������Ϣ������������Ӧ�����ģʽ  
	if (g_pKeyStateBuffer[DIK_1]    & 0x80)         // �����ּ�1�����£�����ʵ�����  
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	if (g_pKeyStateBuffer[DIK_2]    & 0x80)         // �����ּ�2�����£������߿����  
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);  

	// ��ȡ�������
	::ZeroMemory(&g_diMouseState, sizeof(g_diMouseState));
	Device_Read(g_pMouseDevice, (LPVOID)&g_diMouseState, sizeof(g_diMouseState));

	// ��ȡ��������
	::ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pKeyboardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));


	// ��ס���������϶���Ϊƽ�Ʋ���
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if (g_diMouseState.rgbButtons[0] & 0x80) 
	{
		fPosX += g_diMouseState.lX *  0.08f;
		fPosY += g_diMouseState.lY * -0.08f;
	}

	//�����֣�Ϊ�۲����������
	fPosZ += g_diMouseState.lZ * 0.02f;

	// ƽ������
	if (g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.005f;
	if (g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.005f;
	if (g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.005f;


	D3DXMatrixTranslation(&g_matWorld, fPosX, fPosY, fPosZ);


	// ��ס����Ҽ����϶���Ϊ��ת����
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI ;
	if (g_diMouseState.rgbButtons[1] & 0x80) 
	{
		fAngleX += g_diMouseState.lY * -0.01f;
		fAngleY += g_diMouseState.lX * -0.01f;
	}
	// ��ת����
	if (g_pKeyStateBuffer[DIK_UP]    & 0x80) fAngleX += 0.005f;
	if (g_pKeyStateBuffer[DIK_DOWN]  & 0x80) fAngleX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_LEFT]  & 0x80) fAngleY -= 0.005f;
	if (g_pKeyStateBuffer[DIK_RIGHT] & 0x80) fAngleY += 0.005f;


	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	g_matWorld = Rx * Ry * g_matWorld;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);
	Matrix_Set();
}



//*****************************************************************************************
// Name: Direct3D_Render()
// Desc: ����ͼ�ε���Ⱦ����
// Point:��Direct3D��Ⱦ�岽����
//		1.��Ⱦ�岽��֮һ����������
//		2.��Ⱦ�岽��֮������ʼ����
//		3.��Ⱦ�岽��֮������ʽ����
//		4.��Ⱦ�岽��֮�ģ���������
//		5.��Ⱦ�岽��֮�壬��ת��ʾ
//*****************************************************************************************

void Direct3D_Render(HWND hwnd)
{

	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʼ����
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->BeginScene();                     // ��ʼ����
	
			//--------------------------------------------------------------------------------------
			// ��Direct3D��Ⱦ�岽��֮��������ʽ���ƣ����ö��㻺�����ͼ��
			//--------------------------------------------------------------------------------------

			// ��������
			for (DWORD i = 0; i < g_dwNumMtrls; i++)
			{
				g_pd3dDevice->SetMaterial(&g_pMaterials[i]);
				g_pd3dDevice->SetTexture(0, g_pTextures[i]);
				g_pMesh->DrawSubset(i);
			}

			//�ڴ������ϽǴ�����ʾÿ��֡��
			int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), Get_FPS() );
			g_pTextFPS->DrawText(NULL, g_strFPS, charCount , &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_RGBA(0,239,136,255));

			//��ʾ�Կ�������
			g_pTextAdaperName->DrawText(NULL,g_strAdapterName, -1, &formatRect, 
				DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));

			// ���������Ϣ
			 formatRect.top = 30;
			static wchar_t strInfo[256] = {0};
			swprintf_s(strInfo,-1, L"ģ������: (%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
			g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135,239,136,255));

			// ���������Ϣ
			formatRect.left = 0,formatRect.top = 380;
			g_pTextInfor->DrawText(NULL, L"����˵��:", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(235,123,230,255));
			formatRect.top += 35;
			g_pTextHelper->DrawText(NULL, L"    ��ס���������϶���ƽ��ģ��", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    ��ס����Ҽ����϶�����תģ��", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    ���������֣�����ģ��", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    W��S��A��D����ƽ��ģ�� ", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    �ϡ��¡����ҷ��������תģ�� ", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    ���ּ�1��2����ʵ��������߿����֮���л�", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));
			formatRect.top += 25;
			g_pTextHelper->DrawText(NULL, L"    ESC�� : �˳�����", -1, &formatRect, 
				DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(255,200,0,255));


	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�ġ�����������
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->EndScene();                       // ��������
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�塿����ʾ��ת
	//--------------------------------------------------------------------------------------
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
	 
}




//*****************************************************************************************
// Name��Get_FPS��������
// Desc: ���ڼ���֡����
//*****************************************************************************************
float Get_FPS()
{

	//�����ĸ���̬����
	static float  fps = 0; //������Ҫ�����FPSֵ
	static int    frameCount = 0;//֡��
	static float  currentTime =0.0f;//��ǰʱ��
	static float  lastTime = 0.0f;//����ʱ��

	frameCount++;//ÿ����һ��Get_FPS()������֡������1
	currentTime = timeGetTime()*0.001f;//��ȡϵͳʱ�䣬����timeGetTime�������ص����Ժ���Ϊ��λ��ϵͳʱ�䣬������Ҫ����0.001���õ���λΪ���ʱ��

	//�����ǰʱ���ȥ����ʱ�������1���ӣ��ͽ���һ��FPS�ļ���ͳ���ʱ��ĸ��£�����֡��ֵ����
	if(currentTime - lastTime > 1.0f) //��ʱ�������1����
	{
		fps = (float)frameCount /(currentTime - lastTime);//������1���ӵ�FPSֵ
		lastTime = currentTime; //����ǰʱ��currentTime��������ʱ��lastTime����Ϊ��һ��Ļ�׼ʱ��
		frameCount    = 0;//������֡��frameCountֵ����
	}

	return fps;
}



//*****************************************************************************************
// Name: Device_Read();
// Desc: ���ܶ�ȡ�豸����������
//*****************************************************************************************
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize) 
{
	HRESULT hr;
	while (true) 
	{
		pDIDevice->Poll();              // ��ѯ�豸
		pDIDevice->Acquire();           // ��ȡ�豸�Ŀ���Ȩ
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if (FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}

//*****************************************************************************************
// Name: Direct3D_CleanUp()
// Desc: ��Direct3D����Դ���������ͷ�COM�ӿڶ���
//*****************************************************************************************
void Direct3D_CleanUp()
{
	//�ͷ�COM�ӿڶ���
	g_pMouseDevice->Unacquire();
	g_pKeyboardDevice->Unacquire();
	for (DWORD i = 0; i<g_dwNumMtrls; i++) 
		SAFE_RELEASE(g_pTextures[i]);
	SAFE_DELETE(g_pTextures)
	SAFE_DELETE(g_pMaterials)
	SAFE_RELEASE(g_pMesh)
	SAFE_RELEASE(g_pTextFPS)
	SAFE_RELEASE(g_pd3dDevice)	
	SAFE_RELEASE(g_pMouseDevice)
	SAFE_RELEASE(g_pKeyboardDevice)
}

