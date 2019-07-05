/*****************************************************************************

                                   main.cpp

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/Roam/main.cpp#5 $

	$NoKeywords: $

*****************************************************************************/

#include "RoamTerrain.h"
#include "RoamTree/RoamTree.h"
#include "RoamTree/DataTree.h"
#include "RoamTree/GridTriangle.h"

#include "Misc/Etc.h"
#include "Math/Vector3.h"

#include "Glx/Glx.h"
#include "Wglx/Wglx.h"
#include "Wx/Wx.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cmath>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
static void InitializeRendering();
static void Display();
static void Reshape( int w, int h );
static void ReportGlErrors( GLenum error );
static bool Update( HWND hWnd );
static void UpdateFrameRateDisplay();

static void DrawTerrain();
static void DrawVertexNormals();
static void DrawFaceNormals();

static void DrawRoamNode( RoamTree::Node const & node, float tolerance );
static void DrawRoamTree( RoamTree const & tree, float tolerance );
static void DrawRoamTerrain( float tolerance );
static void DrawRoamTree( RoamTree const & tree );


static char						s_AppName[]	 = "Roam Terrain";
static char						s_TitleBar[] = "Roam Terrain";

static Glx::Lighting *			s_pLighting;
static Glx::DirectionalLight *	s_pDirectionalLight;

static HeightField *			s_pHeightField;
static Glx::Mesh *				s_pTerrainMesh;
static Glx::MipMappedTexture *	s_pTerrainTexture;
static Glx::Material *			s_pTerrainMaterial;
static Glx::TLVertex *			s_paVertexGrid;

#if defined( VIEWING_NORMALS )

static Glx::Mesh *				s_pVertexNormals;
static Glx::Mesh *				s_pFaceNormals;
static Glx::Material *			s_pVertexNormalsMaterial;
static Glx::Material *			s_pFaceNormalsMaterial;

#endif // defined( VIEWING_NORMALS )

static Glx::Viewport *			s_pViewport;
static TerrainCamera *			s_pCamera;
static float					s_CameraSpeed	=	2.f;
static WGlx::Font *				s_pFont;

static DataTree *				s_pLowerLeftData;
static DataTree *				s_pUpperRightData;

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow )
{
	if ( Wx::RegisterWindowClass(	CS_OWNDC,
									( WNDPROC )WindowProc,
									hInstance,
									s_AppName,
									LoadCursor( NULL, IDC_ARROW ) ) == NULL )
	{
		MessageBox( NULL, "Wx::RegisterWindowClass() failed.", "Error", MB_OK );
		exit( 1 );
	}

	HWND hWnd = CreateWindowEx( 0,
								s_AppName,
								s_TitleBar,
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, 500, 500,
								NULL,
								NULL,
								hInstance,
								NULL );

	if ( hWnd == NULL )
	{
		MessageBox( NULL, "CreateWindowEx() failed.", "Error", MB_OK );
		exit( 1 );
	}

	try
	{
		// Compute the terrain vertices

		s_paVertexGrid = new Glx::TLVertex[ s_pHeightField->GetSizeI() * s_pHeightField->GetSizeJ() ];
		if ( !s_paVertexGrid ) throw std::bad_alloc();

		int const					sx		= s_pHeightField->GetSizeI();
		int const					sy		= s_pHeightField->GetSizeJ();
		HeightField::Vertex const *	pData	= s_pHeightField->GetData();
		float const					s		= s_pHeightField->GetScale();
		float const					x0		= -( sx - 1 ) * .5f * s;
		float const					y0		= -( sy - 1 ) * .5f * s;

		for ( int i = 0; i < sy; i ++ )
		{
			for ( int j = 0; j < sx; j ++ )
			{
				Glx::TLVertex * const	p	= &s_paVertexGrid[ i * s_pHeightField->GetSizeI() + j ];

				p->m_Position.m_X			= x0 + j * s;
				p->m_Position.m_Y			= y0 + i * s;
				p->m_Position.m_Z			= s_pHeightField->GetData( j, i )->m_Z;
				p->m_Normal					= ComputeGridNormal( *s_pHeightField, j, i );
				p->m_TexturePosition.m_X	= j * .125f;
				p->m_TexturePosition.m_Y	= i * .125f;
			}
		}

		assert( s_pHeightField->GetSizeI() == s_pHeightField->GetSizeJ() );

		s_pLowerLeftData = new DataTree( GridTriangle( 0, 0, GridTriangle::O_UPRIGHT, s_pHeightField->GetSizeI()-1 ),
										s_paVertexGrid,
										s_pHeightField->GetSizeI() );
		if ( !s_pLowerLeftData ) throw std::bad_alloc();

		s_pUpperRightData = new DataTree( GridTriangle( s_pHeightField->GetSizeI()-1, s_pHeightField->GetSizeI()-1,
														GridTriangle::O_DOWNLEFT, s_pHeightField->GetSizeI()-1 ),
										s_paVertexGrid,
										s_pHeightField->GetSizeI() );
		if ( !s_pUpperRightData ) throw std::bad_alloc();

		HDC hDC	= GetDC( hWnd );
		int	rv;

		WGlx::SetPixelFormat( hDC, 0, true );

		WGlx::CurrentRenderingContext	rc( hDC );	// Current rendering context

		InitializeRendering();

		s_pCamera	= new TerrainCamera( 60.f, 1.f, 1000.f,
										 Vector3( 0.f, -s_pHeightField->GetSizeJ() * XY_SCALE * .5f, Z_SCALE ),
										 -90.f, 90.f, 90.f );
		if ( !s_pCamera ) throw std::bad_alloc();

		s_pViewport	= new Glx::Viewport( 0, 0, 500, 500, s_pCamera );
		if ( !s_pViewport ) throw std::bad_alloc();

		s_pLighting			= new Glx::Lighting( Glx::Rgba( .2f, .2f, .3f ) );
		if ( !s_pLighting ) throw std::bad_alloc();
		s_pDirectionalLight	= new Glx::DirectionalLight( GL_LIGHT0, Vector3( 1.f, -1.f, 1.f ), Glx::Rgba( .8f, .8f, .7f ) );
		if ( !s_pDirectionalLight ) throw std::bad_alloc();

		int const	terrainTextureSize	= 256;

		assert( Glx::Extension::IsSupported( "GL_EXT_bgra" ) );

		s_pTerrainTexture	= new Glx::MipMappedTexture( terrainTextureSize, terrainTextureSize, GL_BGR_EXT, GL_UNSIGNED_BYTE );
		if ( !s_pTerrainTexture ) throw std::bad_alloc();

//		TgaFile	file( "RoamTex.tga" );
		TgaFile	file( "drock011.tga" );
		bool	ok;
		
		assert( file.m_ImageType == TgaFile::IMAGE_TRUECOLOR );
		assert( file.m_Height == terrainTextureSize && file.m_Width == terrainTextureSize );
		
		unsigned char *	s_pTextureData	= new unsigned char[ terrainTextureSize * terrainTextureSize * 4 ];
		if ( !s_pTextureData ) throw std::bad_alloc();
		
		ok = file.Read( s_pTextureData );
		assert( ok );
		
		s_pTerrainTexture->BuildAllMipMaps( s_pTextureData );
		
		delete[] s_pTextureData;

		s_pTerrainMaterial	= new Glx::Material( s_pTerrainTexture/*,
												 GL_MODULATE,
												 Glx::Rgba::WHITE,
												 Glx::Rgba::WHITE, 0.f,
												 Glx::Rgba::BLACK,
												 GL_SMOOTH,
												 GL_FRONT_AND_BACK*/ );
		if ( !s_pTerrainMaterial ) throw std::bad_alloc();

//		s_pTerrainMesh			= new Glx::Mesh( s_pTerrainMaterial );
//		s_pTerrainMesh->Begin();
//		DrawTerrain();
//		s_pTerrainMesh->End();
//
//		s_pVertexNormalsMaterial	= new Glx::Material( Glx::Rgba::BLACK,
//												 Glx::Rgba::BLACK, 0.f,
//												 Glx::Rgba::RED/*,
//												 GL_FLAT,
//												 GL_FRONT_AND_BACK*/ );
//
//		s_pVertexNormals			= new Glx::Mesh( s_pVertexNormalsMaterial );
//		s_pVertexNormals->Begin();
//		DrawVertexNormals();
//		s_pVertexNormals->End();
//
//		s_pFaceNormalsMaterial	= new Glx::Material( Glx::Rgba::BLACK,
//												 Glx::Rgba::BLACK, 0.f,
//												 Glx::Rgba::GREEN/*,
//												 GL_FLAT,
//												 GL_FRONT_AND_BACK*/ );
//
//		s_pFaceNormals			= new Glx::Mesh( s_pFaceNormalsMaterial );
//		s_pFaceNormals->Begin();
//		DrawFaceNormals();
//		s_pFaceNormals->End();

		LOGFONT	lf =
		{
			16, 0,
			0, 0,
			FW_BOLD, FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE,
			"Courier New"
		};

		s_pFont = new WGlx::Font( hDC, lf );
		if ( !s_pFont ) throw std::bad_alloc();

		// Create the skybox

			s_pSkyBox = new SkyBox( "skybox", SkyBox::FACE_TOP_FACES );
			if ( !s_pSkyBox ) throw std::bad_alloc();

		ShowWindow( hWnd, nCmdShow );

		rv = Wx::MessageLoop( hWnd, Update );

		delete s_pSkyBox;
		delete s_pFont;
		delete s_pTerrainMaterial;
		delete s_pTerrainTexture;
		delete s_pDirectionalLight;
		delete s_pLighting;
		delete s_pViewport;
		delete s_pCamera;

		delete s_pUpperRightData;
		delete s_pLowerLeftData;
		delete s_pHeightField;

		ReleaseDC( hWnd, hDC );
		DestroyWindow( hWnd );

		return rv;
	}
	catch ( ... )
	{
		MessageBox( NULL, "Unable to initialize.", "Roam Terrain", MB_OK );
		exit(1);
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static bool Update( HWND hWnd )
{
	InvalidateRect( hWnd, NULL, FALSE );

	return true;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{ 
	switch( uMsg )
	{
	case WM_PAINT:
	{
		static PAINTSTRUCT ps;

		Display();
		BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		return 0;
	}

	case WM_SIZE:
		Reshape( LOWORD( lParam ), HIWORD( lParam ) );
		PostMessage( hWnd, WM_PAINT, 0, 0 );
		return 0;

	case WM_CHAR:
	{
		switch ( wParam )
		{
		case VK_ESCAPE:	// Quit
			PostQuitMessage( 0 );
			break;

		case ' ':	// Forward
			s_pCamera->Move( s_pCamera->GetDirection() * s_CameraSpeed );
			break;

		case 's':	// Backwards
			s_pCamera->Move( -s_pCamera->GetDirection() * s_CameraSpeed );
			break;

		case 'd':	// Strafe right
			s_pCamera->Move( s_pCamera->GetRight() * s_CameraSpeed );
			break;

		case 'a':	// Strafe left
			s_pCamera->Move( -s_pCamera->GetRight() * s_CameraSpeed );
			break;

		case 'w':	// Strafe up
			s_pCamera->Move( s_pCamera->GetUp() * s_CameraSpeed );
			break;

		case 'x':	// Strafe down
			s_pCamera->Move( -s_pCamera->GetUp() * s_CameraSpeed );
			break;
		}
		return 0;
	}

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_UP:
			s_pCamera->Pitch( 5.f );
			break;

		case VK_DOWN:
			s_pCamera->Pitch( -5.f );
			break;

		case VK_LEFT:
			s_pCamera->Yaw( 5.f );
			break;

		case VK_RIGHT:
			s_pCamera->Yaw( -5.f );
			break;

		case VK_PRIOR:
			s_pCamera->Roll( 5.f );
			break;

		case VK_INSERT:
			s_pCamera->Roll( -5.f );
			break;
		}
		return 0;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam ); 
} 


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void InitializeRendering()
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static Vector3 ComputeGridNormal( HeightField const & hf, int x, int y )
{
//	The 4 adjacent points in a uniform grid: A, B, C, D
//
//	   B
//	   |
//	C--0--A
//	   |
//	   D
//
//	
//	The ratio of XY-scale to Z-scale: s = Sxy / Sz
//	The desired normal: N = cross(A,B) + cross(B,C) + cross(C,D) + cross(D,A), (then normalize)
//	
//	Nx = 2 * s * (Cz - Az)
//	Ny = 2 * s * (Dz - Bz)
//	Nz = 4 * s^2
//	N = normalize( N )
//	
//	Since N is normalized in the end, it can be divided by 2 * s:
//	
//	Nx = Cz - Az
//	Ny = Dz - Bz
//	Nz = 2 * s
//	N = normalize( N )
//

	HeightField::Vertex const * const	paV		= hf.GetData( x, y );
	int const							sx		= hf.GetSizeI();
	int const							sy		= hf.GetSizeJ();

	float const	z0	= paV[ 0 ].m_Z;

	float const	z1 = ( x + 1 < sx ) ? ( paV[   1 ].m_Z ) : z0;
	float const	z2 = ( y + 1 < sy ) ? ( paV[  sx ].m_Z ) : z0;
	float const	z3 = ( x - 1 >= 0 ) ? ( paV[  -1 ].m_Z ) : z0;
	float const	z4 = ( y - 1 >= 0 ) ? ( paV[ -sx ].m_Z ) : z0;

	Vector3	normal( z3 - z1, z4 - z2, 2 * hf.GetScale() );// / hf.GetZScale() );

	return normal.Normalize();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawTerrain()
{
	assert( s_pHeightField->GetSizeI() == s_pHeightField->GetSizeJ() );

	RoamTerrain	terrain( s_pLowerLeftData, s_pUpperRightData,
						 RoamTree::SplitParameters( s_pCamera->GetPosition(),
													s_pCamera->GetDirection(),
													s_pCamera->GetAngleOfView() * s_pViewport->GetAspectRatio(),
													.005f ) );

	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );

	s_pTerrainMaterial->Apply();

	DrawRoamTree( *terrain.m_pLowerLeft );
	DrawRoamTree( *terrain.m_pUpperRight );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static int	s_TriangleCount;
static int	s_VertexCount;

static void DrawRoamTree( RoamTree const & tree )
{
	s_TriangleCount		= 0;
	s_VertexCount		= 0;

	glBegin( GL_TRIANGLES );
	
	for ( RoamTree::Node const * pNode = tree.FirstLeaf(); pNode != 0; pNode = tree.NextLeaf( pNode ) )
	{
		pNode->GetV0()->Apply();
		pNode->GetV1()->Apply();
		pNode->GetV2()->Apply();

//		glTexCoord2f( 0.f, 0.f );
//		glNormal3fv( pNode->GetV0()->m_Normal.m_V );
//		glVertex3fv( pNode->GetV0()->m_Position.m_V );
//
//		glTexCoord2f( 1.f, 0.f );
//		glNormal3fv( pNode->GetV1()->m_Normal.m_V );
//		glVertex3fv( pNode->GetV1()->m_Position.m_V );
//
//		glTexCoord2f( 0.f, 1.f );
//		glNormal3fv( pNode->GetV2()->m_Normal.m_V );
//		glVertex3fv( pNode->GetV2()->m_Position.m_V );

		s_VertexCount += 3;

//		{
//			char buffer[ 256 ];
//			sprintf( buffer, "%d: %5.1f %5.1f %5.1f, %5.1f %5.1f %5.1f, %5.1f %5.1f %5.1f\n" ,
//					 nodeCount,
//					 pNode->GetV0()->m_Position.m_X, pNode->GetV0()->m_Position.m_Y, pNode->GetV0()->m_Position.m_Z,
//					 pNode->GetV1()->m_Position.m_X, pNode->GetV1()->m_Position.m_Y, pNode->GetV1()->m_Position.m_Z,
//					 pNode->GetV2()->m_Position.m_X, pNode->GetV2()->m_Position.m_Y, pNode->GetV2()->m_Position.m_Z );
//			OutputDebugString( buffer );
//			
//			sprintf( buffer, "%d: %5.3f %5.3f %5.3f %5.3f, %5.3f %5.3f %5.3f %5.3f, %5.3f %5.3f %5.3f %5.3f\n" ,
//					 nodeCount,
//					 pNode->GetV0()->m_Normal.m_X, pNode->GetV0()->m_Normal.m_Y, pNode->GetV0()->m_Normal.m_Z, pNode->GetV0()->m_Normal.Length(),
//					 pNode->GetV1()->m_Normal.m_X, pNode->GetV1()->m_Normal.m_Y, pNode->GetV1()->m_Normal.m_Z, pNode->GetV1()->m_Normal.Length(),
//					 pNode->GetV2()->m_Normal.m_X, pNode->GetV2()->m_Normal.m_Y, pNode->GetV2()->m_Normal.m_Z, pNode->GetV2()->m_Normal.Length() );
//			OutputDebugString( buffer );
//
//			sprintf( buffer, "%d: 0x%08x 0x%08x\n" ,
//					 nodeCount,
//					 pNode->LeftChild(), pNode->RightChild() );
//			OutputDebugString( buffer );
//		}
//
//		++nodeCount;

		s_TriangleCount += 1;
	}

	glEnd();

//	{
//		char buffer[ 256 ];
//		sprintf( buffer, "node count = %d\n" , nodeCount );
//		OutputDebugString( buffer );
//	}
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawHud()
{
	// Switch to ortho projection for 2d

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0., 1., 0., 1., -1., 1. );

	// Reset view for hud

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	s_pLighting->Disable();
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );

	static DWORD	oldTime	= timeGetTime();

	DWORD const		newTime	= timeGetTime();
	int const		dt		= newTime - oldTime;

	if ( dt > 0 )
	{
		char buffer [ 256 ];

		sprintf( buffer, "Triangles: %-08d Vertices: %-08d Frame rate: %5.2f fps", s_TriangleCount, s_VertexCount, 1. / ( dt * .001 ) );

		glColor3f( 1.f, 1.f, 0.f );
		glRasterPos2f( .01f, .01f );
		s_pFont->DrawString( buffer );

		oldTime = newTime;
	}

	// Switch back to perspective projection

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Display()
{
	glClear( GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Place the camera

	s_pCamera->Look();

	// Draw the sky

	s_pLighting->Disable();
	s_pSkyBox->Apply( *s_pCamera );

	// Lights

	s_pLighting->Enable();
	s_pDirectionalLight->Apply();

	// Draw the terrain

	DrawTerrain();

//	s_pTerrain->Apply();

//	// Draw the vertex normals
//
//	s_pVertexNormals->Apply();
//
//	// Draw the face normals
//
//	s_pFaceNormals->Apply();

	// Draw the HUD

	DrawHud();

	// Display the scene

	glFlush();
	SwapBuffers( wglGetCurrentDC() );

	// Report any GL errors

	Glx::ReportAnyErrors();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Reshape( int w, int h )
{
	s_pViewport->Reshape( w, h );
}
