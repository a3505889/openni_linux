#include "stdafx.h"

#define QQQVGA_WIDTH 80
#define QQQVGA_HEIGHT 60
#define QQVGA_WIDTH 160
#define QQVGA_HEIGHT 120
#define QVGA_WIDTH 320
#define QVGA_HEIGHT 240
#define VGA_WIDTH 640
#define VGA_HEIGHT 480

#define DEFAULT_RECORDING_FRAME 200
#define DEFAULT_RECORDING_WIDTH QVGA_WIDTH
#define DEFAULT_RECORDING_HEIGHT QVGA_HEIGHT
#define DEFAULT_RATIO (0.7)
#define DEFAULT_BLOCK_SIZE 1

#define RECORDING_FPS 30
#define LOG_PATH "testLog.csv"

using namespace std;
using namespace xn;

XnUInt32 g_xRes, g_yRes;
int g_recordingFrame, g_blockSize;
double g_ratio;

struct Point
{
    unsigned int x;
    unsigned int y;
};

struct Points
{
    Point LT;
    Point T;
    Point RT;
    Point L;
    Point C;
    Point R;
    Point LB;
    Point B;
    Point RB;
};

double calcMean( unsigned int *dataSet )
{
    double sum = 0;
    double mean = 0;
    for ( int i = 1; i <= g_recordingFrame; i++ )
    {
        sum += *( dataSet + i );
    }
    mean = sum / g_recordingFrame;
    return mean;
}

double calcSD( unsigned int *dataSet )
{
    double mean = 0;
    double dev = 0;
    double sdev = 0;
    double var = 0;
    double sd = 0;
    int i = 0;

    mean = calcMean( dataSet );
    for ( i = 1; i <= g_recordingFrame; i++ )
    {
        dev = ( *( dataSet + i ) - mean ) * ( *( dataSet + i ) - mean );
        sdev += dev;
    }
    var = sdev / ( g_recordingFrame - 1 );
    sd = sqrt( var );
    return sd;
}

void setResolution( char *res )
{
    if ( strcmp( res, "vga" ) == 0 || strcmp( res, "VGA" ) == 0 )
    {
        g_xRes = VGA_WIDTH;
        g_yRes = VGA_HEIGHT;
    }
    else if ( strcmp( res, "qvga" ) == 0 || strcmp( res, "QVGA" ) == 0 )
    {
        g_xRes = QVGA_WIDTH;
        g_yRes = QVGA_HEIGHT;
    }
    else if ( strcmp( res, "qqvga" ) == 0 || strcmp( res, "QQVGA" ) == 0 )
    {
        g_xRes = QQVGA_WIDTH;
        g_yRes = QQVGA_HEIGHT;
    }
    else if ( strcmp( res, "qqqvga" ) == 0 || strcmp( res, "QQQVGA" ) == 0 )
    {
        g_xRes = QQQVGA_WIDTH;
        g_yRes = QQQVGA_HEIGHT;
    }
    else
    {
        printf( "The resolution input is unknown.\nSet default resolution: QVGA.\n" );
    }

    return;
}

void setRecordingFrame( char *res )
{
    int frameCount;
    char* error = "The recording frame amount is not an integer.\n";
    try
    {
        frameCount = atoi( res );
        if ( frameCount >= 10 )
        {
            g_recordingFrame = frameCount;
        }
        else
        {
            error = "The recording frame amount should be equal or bigger than 10\n";
            throw error;
        }
    }
    catch ( char* e )
    {
        printf( "%sSet default recording frame amount: 200.\n", e );
    }
}

void setRatio( char *res )
{
    double r;
    char* error = "The ratio is not a number.\n";
    try
    {
        r = atof( res );
        if ( r > 0 && r < 1 )
        {
            g_ratio = r;
        }
        else
        {
            error = "The ratio should be in the range between 0 and 1.\n";
            throw error;
        }
    }
    catch ( char* e )
    {
        printf( "%sSet default ratio: 0.7.\n", e );
    }
}

void setBlockSize( char *res )
{
    int blockSize;
    char* error = "The block size is not an odd integer.\n";
    try
    {
        blockSize = atoi( res );
        if ( blockSize >= 1 && blockSize <= 15 && ( ( blockSize % 2 ) == 1 ) )
        {
            g_blockSize = blockSize;
        }
        else
        {
            error = "The block size should be an odd number between 1 and 15.\n";
            throw error;
        }
    }
    catch ( char* e )
    {
        printf( "%sSet default block size: 1.\n", e );
    }
}

unsigned int calcAvgData( DepthMetaData &metaData, int x, int y, int blockSize )
{
    unsigned int avgData = 0;

    if ( blockSize == 1 )
    {
        avgData = metaData( x, y );
    }
    else
    {
        int range = ( blockSize - 1 ) / 2;
        for ( int i = ( x - range ) ; i <= ( x + range ) ; i++ )
        {
            for ( int j = ( y - range ) ; j <= ( y + range ) ; j++ )
            {
                avgData += metaData( i, j );
            }
        }
        avgData /= ( blockSize * blockSize );
    }

    return avgData;
}

int main( int argc, char* argv[] )
{
    g_xRes = DEFAULT_RECORDING_WIDTH;
    g_yRes = DEFAULT_RECORDING_HEIGHT;
    g_recordingFrame = DEFAULT_RECORDING_FRAME;
    g_ratio = DEFAULT_RATIO;
    g_blockSize = DEFAULT_BLOCK_SIZE;

    if ( argc > 1 )
    {
        for ( int i = 1 ; i < argc ; i++ )
        {
            if ( strcmp( argv[i], "-r" ) == 0 )
            {
                if ( argc == i + 1 )
                {
                    printf( "\nThe parameter of -r is missing.\n" );
                    break;
                }
                setResolution( argv[i + 1] );
                i++;
            }
            else if ( strcmp( argv[i], "-f" ) == 0 )
            {
                if ( argc == i + 1 )
                {
                    printf( "\nThe parameter of -f is missing.\n" );
                    break;
                }
                setRatio( argv[i + 1] );
                i++;
            }
            else if ( strcmp( argv[i], "-K" ) == 0 )
            {
                if ( argc == i + 1 )
                {
                    printf( "\nThe parameter of -K is missing.\n" );
                    break;
                }
                setRecordingFrame( argv[i + 1] );
                i++;
            }
            else if ( strcmp( argv[i], "-b" ) == 0 )
            {
                if ( argc == i + 1 )
                {
                    printf( "\nThe parameter of -b is missing.\n" );
                    break;
                }
                setBlockSize( argv[i + 1] );
                i++;
            }
            else
            {
                printf( "\nNot valid parameters.\n" );
                break;
            }
        }
    }

    Context mContext;
    mContext.Init();

    XnMapOutputMode mapMode;
    mapMode.nXRes = g_xRes;
    mapMode.nYRes = g_yRes;
    mapMode.nFPS = RECORDING_FPS;

    DepthGenerator mDepthGen;
    mDepthGen.Create( mContext );
    mDepthGen.SetMapOutputMode( mapMode );

    DepthMetaData mDepthMD;
    unsigned int *mDataSet_LT = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_T = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_RT = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_L = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_C = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_R = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_LB = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_B = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    unsigned int *mDataSet_RB = ( unsigned int* ) calloc ( g_recordingFrame + 1, sizeof( unsigned int ) );
    Points mPoints;
    // cos(@) = 4/5
    // sin(@) = 3/5
    double hypotenuse = sqrt(pow(mapMode.nXRes * 0.5, 2) + pow(mapMode.nYRes * 0.5, 2)) * g_ratio;
    double a_side = hypotenuse * 4 / 5; //hypotenuse * cos(@)
    double b_side = hypotenuse * 3 / 5; //hypotenuse * sin(@)

    mPoints.C.x = ( unsigned )( mapMode.nXRes * 0.5 );
    mPoints.C.y = ( unsigned )( mapMode.nYRes * 0.5 );
    mPoints.LT.x = ( unsigned )( mPoints.C.x - a_side );
    mPoints.LT.y = ( unsigned )( mPoints.C.y - b_side );
    mPoints.T.x = ( unsigned )( mPoints.C.x );
    mPoints.T.y = ( unsigned )( mPoints.C.y - b_side );
    mPoints.RT.x = ( unsigned )( mPoints.C.x + a_side );
    mPoints.RT.y = ( unsigned )( mPoints.C.y - b_side);
    mPoints.L.x = ( unsigned )( mPoints.C.x - a_side );
    mPoints.L.y = ( unsigned )( mPoints.C.y );
    mPoints.R.x = ( unsigned )( mPoints.C.x + a_side );
    mPoints.R.y = ( unsigned )( mPoints.C.y );
    mPoints.LB.x = ( unsigned )( mPoints.C.x - a_side );
    mPoints.LB.y = ( unsigned )( mPoints.C.y + b_side );
    mPoints.B.x = ( unsigned )( mPoints.C.x );
    mPoints.B.y = ( unsigned )( mPoints.C.y + b_side );
    mPoints.RB.x = ( unsigned )( mPoints.C.x + a_side );
    mPoints.RB.y = ( unsigned )( mPoints.C.y + b_side );

    cout << "LT (" << mPoints.LT.x << ", " << mPoints.LT.y << ")" << endl;
    cout << "T (" << mPoints.T.x << ", " << mPoints.T.y << ")" << endl;
    cout << "RT (" << mPoints.RT.x << ", " << mPoints.RT.y << ")" << endl;
    cout << "L (" << mPoints.L.x << ", " << mPoints.L.y << ")" << endl;
    cout << "C (" << mPoints.C.x << ", " << mPoints.C.y << ")" << endl;
    cout << "R (" << mPoints.R.x << ", " << mPoints.R.y << ")" << endl;
    cout << "LB (" << mPoints.LB.x << ", " << mPoints.LB.y << ")" << endl;
    cout << "B (" << mPoints.B.x << ", " << mPoints.B.y << ")" << endl;
    cout << "RB (" << mPoints.RB.x << ", " << mPoints.RB.y << ")" << endl;

    mContext.StartGeneratingAll();

    cout << "\n";
    cout << "==================================" << endl;
    cout << " LIPS Noise Test program         " << endl;
    cout << "==================================" << endl;

    cout << "\n";
	cout << "Test start ( " << g_xRes << "x" << g_yRes << " f=" << g_ratio << " K=" << g_recordingFrame << " b=" << g_blockSize << " )" << endl;
    cout << "Progress: ";

    int i = 0;
    unsigned int percent = 10;
    while ( i < ( g_recordingFrame + 1 ) )
    {
        mContext.WaitOneUpdateAll( mDepthGen );
        mDepthGen.GetMetaData( mDepthMD );
#if 0
        mDataSet_LT[i] = mDepthMD( mPoints.LT.x, mPoints.LT.y );
        mDataSet_T[i] = mDepthMD( mPoints.T.x, mPoints.T.y );
        mDataSet_RT[i] = mDepthMD( mPoints.RT.x, mPoints.RT.y );
        mDataSet_L[i] = mDepthMD( mPoints.L.x, mPoints.L.y );
        mDataSet_C[i] = mDepthMD( mPoints.C.x, mPoints.C.y );
        mDataSet_R[i] = mDepthMD( mPoints.R.x, mPoints.R.y );
        mDataSet_LB[i] = mDepthMD( mPoints.LB.x, mPoints.LB.y );
        mDataSet_B[i] = mDepthMD( mPoints.B.x, mPoints.B.y );
        mDataSet_RB[i] = mDepthMD( mPoints.RB.x, mPoints.RB.y );
#endif
        mDataSet_LT[i] = calcAvgData( mDepthMD, mPoints.LT.x, mPoints.LT.y, g_blockSize );
        mDataSet_T[i] = calcAvgData( mDepthMD, mPoints.T.x, mPoints.T.y, g_blockSize );
        mDataSet_RT[i] = calcAvgData( mDepthMD, mPoints.RT.x, mPoints.RT.y, g_blockSize );
        mDataSet_L[i] = calcAvgData( mDepthMD, mPoints.L.x, mPoints.L.y, g_blockSize );
        mDataSet_C[i] = calcAvgData( mDepthMD, mPoints.C.x, mPoints.C.y, g_blockSize );
        mDataSet_R[i] = calcAvgData( mDepthMD, mPoints.R.x, mPoints.R.y, g_blockSize );
        mDataSet_LB[i] = calcAvgData( mDepthMD, mPoints.LB.x, mPoints.LB.y, g_blockSize );
        mDataSet_B[i] = calcAvgData( mDepthMD, mPoints.B.x, mPoints.B.y, g_blockSize );
        mDataSet_RB[i] = calcAvgData( mDepthMD, mPoints.RB.x, mPoints.RB.y, g_blockSize );
        i++;
        if ( 0 == i % ( unsigned int )( g_recordingFrame / 10 ) )
        {
            cout << percent << "%";
            percent += 10;
        }
        else
        {
            cout << ".";
        }
    }
    cout << "\n";
    cout << "Test finish! generating test result" << endl;
    ofstream newfile( LOG_PATH );
    // row 1
    newfile << "point,LT,T,RT,L,C,R,LB,B,RB\n";

    // Print mean for each block
    newfile << "mean,";
    // LT
    newfile << calcMean( mDataSet_LT ) << ",";
    // T
    newfile << calcMean( mDataSet_T ) << ",";
    // RT
    newfile << calcMean( mDataSet_RT ) << ",";
    // L
    newfile << calcMean( mDataSet_L ) << ",";
    // C
    newfile << calcMean( mDataSet_C ) << ",";
    // R
    newfile << calcMean( mDataSet_R ) << ",";
    // LB
    newfile << calcMean( mDataSet_LB ) << ",";
    // B
    newfile << calcMean( mDataSet_B ) << ",";
    // RB
    newfile << calcMean( mDataSet_RB ) << "\n";

    // Print deviation for each block
    newfile << "deviation,";
    // LT
    newfile << calcSD( mDataSet_LT ) << ",";
    // T
    newfile << calcSD( mDataSet_T ) << ",";
    // RT
    newfile << calcSD( mDataSet_RT ) << ",";
    // L
    newfile << calcSD( mDataSet_L ) << ",";
    // C
    newfile << calcSD( mDataSet_C ) << ",";
    // R
    newfile << calcSD( mDataSet_R ) << ",";
    // LB
    newfile << calcSD( mDataSet_LB ) << ",";
    // B
    newfile << calcSD( mDataSet_B ) << ",";
    // RB
    newfile << calcSD( mDataSet_RB ) << "\n";

    for ( i = 1; i <= g_recordingFrame; i++ )
    {
        newfile << "DATA" << i << ","
                << mDataSet_LT[i] << "," << mDataSet_T[i] << "," << mDataSet_RT[i] << ","
                << mDataSet_L[i] << "," << mDataSet_C[i] << "," << mDataSet_R[i] << ","
                << mDataSet_LB[i] << "," << mDataSet_B[i] << "," << mDataSet_RB[i] << "\n";
    }

    newfile.close();
    cout << "Test result is generated" << endl;
    cout << "\n";
    mContext.StopGeneratingAll();

    mContext.Release();
    return 0;
}
