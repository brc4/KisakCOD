#ifndef __BINKH__
#define __BINKH__

#define BINKMAJORVERSION 1
#define BINKMINORVERSION 9
#define BINKSUBVERSION 9
#define BINKVERSION "1.9i"
#define BINKDATE    "2008-10-31"

#ifndef __RADRES__

#ifndef __RADBASEH__
#include "radbase.h"
#endif

RADDEFSTART

typedef struct BINK PTR4* HBINK;

struct BINKIO;
typedef S32  (RADLINK PTR4* BINKIOOPEN)         (struct BINKIO PTR4* Bnkio, const char PTR4 *name, U32 flags);
typedef U32  (RADLINK PTR4* BINKIOREADHEADER)   (struct BINKIO PTR4* Bnkio, S32 Offset, void PTR4* Dest,U32 Size);
typedef U32  (RADLINK PTR4* BINKIOREADFRAME)    (struct BINKIO PTR4* Bnkio, U32 Framenum,S32 origofs,void PTR4* dest,U32 size);
typedef U32  (RADLINK PTR4* BINKIOGETBUFFERSIZE)(struct BINKIO PTR4* Bnkio, U32 Size);
typedef void (RADLINK PTR4* BINKIOSETINFO)      (struct BINKIO PTR4* Bnkio, void PTR4* Buf,U32 Size,U32 FileSize,U32 simulate);
typedef U32  (RADLINK PTR4* BINKIOIDLE)         (struct BINKIO PTR4* Bnkio);
typedef void (RADLINK PTR4* BINKIOCLOSE)        (struct BINKIO PTR4* Bnkio);
typedef S32  (RADLINK PTR4* BINKIOBGCONTROL)    (struct BINKIO PTR4* Bnkio, U32 Control);

typedef void (RADLINK PTR4* BINKCBSUSPEND)    (struct BINKIO PTR4* Bnkio);
typedef S32  (RADLINK PTR4* BINKCBTRYSUSPEND) (struct BINKIO PTR4* Bnkio);
typedef void (RADLINK PTR4* BINKCBRESUME)     (struct BINKIO PTR4* Bnkio);
typedef void (RADLINK PTR4* BINKCBIDLE)       (struct BINKIO PTR4* Bnkio);

typedef struct BINKIO {
  BINKIOREADHEADER ReadHeader;
  BINKIOREADFRAME  ReadFrame;
  BINKIOGETBUFFERSIZE GetBufferSize;
  BINKIOSETINFO SetInfo;
  BINKIOIDLE Idle;
  BINKIOCLOSE Close;
  BINKIOBGCONTROL BGControl;
  HBINK bink;
  volatile U32 ReadError;
  volatile U32 DoingARead;
  volatile U32 BytesRead;
  volatile U32 Working;
  volatile U32 TotalTime;
  volatile U32 ForegroundTime;
  volatile U32 IdleTime;
  volatile U32 ThreadTime;
  volatile U32 BufSize;
  volatile U32 BufHighUsed;
  volatile U32 CurBufSize;
  volatile U32 CurBufUsed;
  volatile U32 Suspended;
  volatile U8 iodata[128+32];

  // filled in by the caller
  BINKCBSUSPEND suspend_callback;
  BINKCBTRYSUSPEND try_suspend_callback;
  BINKCBRESUME resume_callback;
  BINKCBIDLE idle_on_callback;
  volatile U32 callback_control[16];  // buffer for background IO callback
} BINKIO;

struct BINKSND;
typedef S32  (RADLINK PTR4* BINKSNDOPEN)       (struct BINKSND PTR4* BnkSnd, U32 freq, S32 bits, S32 chans, U32 flags, HBINK bink);
typedef S32  (RADLINK PTR4* BINKSNDREADY)      (struct BINKSND PTR4* BnkSnd);
typedef S32  (RADLINK PTR4* BINKSNDLOCK)       (struct BINKSND PTR4* BnkSnd, U8 PTR4* PTR4* addr, U32 PTR4* len);
typedef S32  (RADLINK PTR4* BINKSNDUNLOCK)     (struct BINKSND PTR4* BnkSnd, U32 filled);
typedef void (RADLINK PTR4* BINKSNDVOLUME)     (struct BINKSND PTR4* BnkSnd, S32 volume);
typedef void (RADLINK PTR4* BINKSNDPAN)        (struct BINKSND PTR4* BnkSnd, S32 pan);
typedef void (RADLINK PTR4* BINKSNDMIXBINS)    (struct BINKSND PTR4* BnkSnd, U32 PTR4* mix_bins, U32 total);
typedef void (RADLINK PTR4* BINKSNDMIXBINVOLS) (struct BINKSND PTR4* BnkSnd, U32 PTR4* vol_mix_bins, S32 PTR4* volumes, U32 total );
typedef S32  (RADLINK PTR4* BINKSNDONOFF)      (struct BINKSND PTR4* BnkSnd, S32 status);
typedef S32  (RADLINK PTR4* BINKSNDPAUSE)      (struct BINKSND PTR4* BnkSnd, S32 status);
typedef void (RADLINK PTR4* BINKSNDCLOSE)      (struct BINKSND PTR4* BnkSnd);

typedef BINKSNDOPEN  (RADLINK PTR4* BINKSNDSYSOPEN) (UINTa param);

typedef struct BINKSND {
  // for the spu, we *send* through sndreadpos, but we only return sndwritepos
  U8 PTR4* sndwritepos;       // current write position

  U32 audiodecompsize;

  U32 sndbufsize;             // sound buffer size
  U8 PTR4* sndbuf;            // sound buffer
  U8 PTR4* sndend;            // end of the sound buffer
  UINTa sndcomp;              // sound compression handle

  U8 PTR4* sndreadpos;        // current read position

#if defined( BINK_SPU_PROCESS )
  struct binksnd_hide  // put variables that we don't want to accidentally
  {                    //   use on the spu into a structure
#endif
  U32 orig_freq;
  U32 freq;
  S32 bits,chans;

  S32 BestSizeIn16;
  U32 BestSizeMask;
  S32 OnOff;
  U32 Latency;
  U32 VideoScale;

  U32 sndendframe;            // frame number that the sound ends on

  U32 sndpad;                 // padded this much audio
  S32 sndprime;               // amount of data to prime the playahead
  S32 NoThreadService;
  U32 SoundDroppedOut;
  U32 sndconvert8;            // convert back to 8-bit sound at runtime
  U8 snddata[256];

  BINKSNDREADY Ready;
  BINKSNDLOCK Lock;
  BINKSNDUNLOCK Unlock;
  BINKSNDVOLUME Volume;
  BINKSNDPAN Pan;
  BINKSNDPAUSE Pause;
  BINKSNDONOFF SetOnOff;
  BINKSNDCLOSE Close;
  BINKSNDMIXBINS MixBins;
  BINKSNDMIXBINVOLS MixBinVols;

#if defined( BINK_SPU_PROCESS )
  } spu_hide;
#endif

} BINKSND;

typedef struct BINKRECT {
  S32 Left,Top,Width,Height;
} BINKRECT;

#define BINKMAXDIRTYRECTS 8

typedef struct BUNDLEPOINTERS {
  void* typeptr;
  void* type16ptr;
  void* colorptr;
  void* bits2ptr;
  void* motionXptr;
  void* motionYptr;
  void* dctptr;
  void* mdctptr;
  void* patptr;
} BUNDLEPOINTERS;

typedef struct BINKPLANE
{
  S32 Allocate;
  void * Buffer;
  U32 BufferPitch;
} BINKPLANE;

typedef struct BINKFRAMEPLANESET
{
  BINKPLANE YPlane;
  BINKPLANE cRPlane;
  BINKPLANE cBPlane;
  BINKPLANE APlane;
} BINKFRAMEPLANESET;

#define BINKMAXFRAMEBUFFERS 2

typedef struct BINKFRAMEBUFFERS
{
  S32 TotalFrames;
  U32 YABufferWidth;
  U32 YABufferHeight;
  U32 cRcBBufferWidth;
  U32 cRcBBufferHeight;

  U32 FrameNum;
  BINKFRAMEPLANESET Frames[ BINKMAXFRAMEBUFFERS ];
} BINKFRAMEBUFFERS;

typedef struct BINK {
  U32 Width;             // Width (1 based, 640 for example)
  U32 Height;            // Height (1 based, 480 for example)
  U32 Frames;            // Number of frames (1 based, 100 = 100 frames)
  U32 FrameNum;          // Frame to *be* displayed (1 based)
  U32 LastFrameNum;      // Last frame decompressed or skipped (1 based)

  U32 FrameRate;         // Frame Rate Numerator
  U32 FrameRateDiv;      // Frame Rate Divisor (frame rate=numerator/divisor)

  U32 ReadError;         // Non-zero if a read error has ocurred
  U32 OpenFlags;         // flags used on open
  U32 BinkType;          // Bink flags

  U32 Size;              // size of file
  U32 FrameSize;         // The current frame's size in bytes
  U32 SndSize;           // The current frame sound tracks' size in bytes

  U32 FrameChangePercent; // very rough percentage of the frame that changed

  BINKRECT FrameRects[BINKMAXDIRTYRECTS];// Dirty rects from BinkGetRects
  S32 NumRects;

  BINKFRAMEBUFFERS * FrameBuffers; // Bink frame buffers that we decompress to

  void PTR4* MaskPlane;  // pointer to the mask plane (Ywidth/16*Yheight/16)
  U32 MaskPitch;         // Mask Pitch
  U32 MaskLength;        // total length of the mask plane
  void PTR4* AsyncMaskPlane;  // pointer to the mask plane for async data
  void PTR4* InUseMaskPlane;  // pointer to the mask plane in use
  void PTR4* LastMaskPlane;   // pointer to the last mask plane

  U32 LargestFrameSize;  // Largest frame size
  U32 InternalFrames;    // how many frames were potentially compressed

  S32 NumTracks;         // how many tracks

  U32 Highest1SecRate;   // Highest 1 sec data rate
  U32 Highest1SecFrame;  // Highest 1 sec data rate starting frame

  S32 Paused;            // is the bink movie paused?

  // everything below is for internal Bink use

  S32 async_in_progress[2];   // is an async decompression in progress

  U32 soundon;                // sound turned on?
  U32 videoon;                // video turned on?

  void PTR4* compframe;       // compressed frame data
  U32 compframesize;          // compressed frame size
  U32 compframeoffset;        // compressed frame offset
  U32 compframekey;           // if this frame a key frame
  
  U32 skippedlastblit;        // skipped last frame?

  U32 playingtracks;          // how many tracks are playing
  BINKSND PTR4* bsnd;         // SND structures
  S32 PTR4* trackindexes;     // track indexes

  BUNDLEPOINTERS bunp;        // pointers to internal temporary memory

  U32 changepercent;          // how much roughly did the current frame change?

#if defined( BINK_SPU_PROCESS )
  struct bink_hide  // put variables that we don't want to accidentally
  {                 //   use on the spu into a structure
#endif

  void PTR4* preloadptr;      // preloaded compressed frame data
  U32* frameoffsets;          // offsets of each of the frames

  BINKIO bio;                 // IO structure (should be the last element)
  U8 PTR4* ioptr;             // io buffer ptr
  U32 iosize;                 // io buffer size
  U32 decompwidth;            // width not include scaling
  U32 decompheight;           // height not include scaling

  U32 PTR4* tracksizes;       // largest single frame of track
  U32 PTR4* tracktypes;       // type of each sound track
  S32 PTR4* trackIDs;         // external track numbers

  U32 numrects;               // number of rects from BinkGetRects

  U32 playedframes;           // how many frames have we played
  U32 firstframetime;         // very first frame start
  U32 startblittime;          // start of blit period
  U32 startsynctime;          // start of synched time
  U32 startsyncframe;         // frame of startsynctime
  U32 twoframestime;          // two frames worth of time

  U32 slowestframetime;       // slowest frame in ms
  U32 slowestframe;           // slowest frame number
  U32 slowest2frametime;      // second slowest frame in ms
  U32 slowest2frame;          // second slowest frame

  U32 totalmem;               // total memory used
  U32 timevdecomp;            // total time decompressing video
  U32 timeadecomp;            // total time decompressing audio
  U32 timeblit;               // total time blitting
  U32 timeopen;               // total open time

  U32 fileframerate;          // frame rate originally in the file
  U32 fileframeratediv;

  U32 runtimeframes;          // max frames for runtime analysis
  S32 rtindex;                // index of where we are in the runtime frames
  U32 PTR4* rtframetimes;     // start times for runtime frames
  U32 PTR4* rtadecomptimes;   // decompress times for runtime frames
  U32 PTR4* rtvdecomptimes;   // decompress times for runtime frames
  U32 PTR4* rtblittimes;      // blit times for runtime frames
  U32 PTR4* rtreadtimes;      // read times for runtime frames
  U32 PTR4* rtidlereadtimes;  // idle read times for runtime frames
  U32 PTR4* rtthreadreadtimes;// thread read times for runtime frames

  U32 lastblitflags;          // flags used on last blit
  U32 lastdecompframe;        // last frame number decompressed
  U32 lastfinisheddoframe;    // time that the last do frame finished

  U32 lastresynctime;         // last loop point that we did a resync on
  U32 doresync;               // should we do a resync in the next doframe?

  U32 soundskips;             // number of sound stops
  U32 skipped_status_this_frame;//0=not checked this frame, 1=no skip, 2=skip
  U32 very_delayed;           // is this frame more than 725 ms late?
  U32 skippedblits;           // how many blits were skipped

  U32 skipped_in_a_row;       // how many frames have we skipped in a row
  U32 paused_sync_diff;       // sync delta at the time of a pause
  U32 last_time_almost_empty; // time of last almost empty IO buffer
  U32 last_read_count;        // counter to keep track of the last bink IO
  U32 last_sound_count;       // counter to keep track of the last bink sound
  U32 snd_callback_buffer[16]; // buffer for background sound callback
  S32 allkeys;                // are all frames keyframes?
  BINKFRAMEBUFFERS * allocatedframebuffers; // pointer to internally allocated buffers

#if defined( BINK_SPU_PROCESS )
  } spu_hide;
#endif
} BINK;


typedef struct BINKSUMMARY {
  U32 Width;                  // Width of frames
  U32 Height;                 // Height of frames
  U32 TotalTime;              // total time (ms)
  U32 FileFrameRate;          // frame rate
  U32 FileFrameRateDiv;       // frame rate divisor
  U32 FrameRate;              // frame rate
  U32 FrameRateDiv;           // frame rate divisor
  U32 TotalOpenTime;          // Time to open and prepare for decompression
  U32 TotalFrames;            // Total Frames
  U32 TotalPlayedFrames;      // Total Frames played
  U32 SkippedFrames;          // Total number of skipped frames
  U32 SkippedBlits;           // Total number of skipped blits
  U32 SoundSkips;             // Total number of sound skips
  U32 TotalBlitTime;          // Total time spent blitting
  U32 TotalReadTime;          // Total time spent reading
  U32 TotalVideoDecompTime;   // Total time spent decompressing video
  U32 TotalAudioDecompTime;   // Total time spent decompressing audio
  U32 TotalIdleReadTime;      // Total time spent reading while idle
  U32 TotalBackReadTime;      // Total time spent reading in background
  U32 TotalReadSpeed;         // Total io speed (bytes/second)
  U32 SlowestFrameTime;       // Slowest single frame time (ms)
  U32 Slowest2FrameTime;      // Second slowest single frame time (ms)
  U32 SlowestFrameNum;        // Slowest single frame number
  U32 Slowest2FrameNum;       // Second slowest single frame number
  U32 AverageDataRate;        // Average data rate of the movie
  U32 AverageFrameSize;       // Average size of the frame
  U32 HighestMemAmount;       // Highest amount of memory allocated
  U32 TotalIOMemory;          // Total extra memory allocated
  U32 HighestIOUsed;          // Highest extra memory actually used
  U32 Highest1SecRate;        // Highest 1 second rate
  U32 Highest1SecFrame;       // Highest 1 second start frame
} BINKSUMMARY;


typedef struct BINKREALTIME {
  U32 FrameNum;               // Current frame number
  U32 FrameRate;              // frame rate
  U32 FrameRateDiv;           // frame rate divisor
  U32 Frames;                 // frames in this sample period
  U32 FramesTime;             // time is ms for these frames
  U32 FramesVideoDecompTime;  // time decompressing these frames
  U32 FramesAudioDecompTime;  // time decompressing these frames
  U32 FramesReadTime;         // time reading these frames
  U32 FramesIdleReadTime;     // time reading these frames at idle
  U32 FramesThreadReadTime;   // time reading these frames in background
  U32 FramesBlitTime;         // time blitting these frames
  U32 ReadBufferSize;         // size of read buffer
  U32 ReadBufferUsed;         // amount of read buffer currently used
  U32 FramesDataRate;         // data rate for these frames
} BINKREALTIME;

#define BINKMARKER1 'fKIB'
#define BINKMARKER2 'gKIB'    // new Bink files use this tag
#define BINKMARKER3 'hKIB'    // newer Bink files use this tag
#define BINKMARKER4 'iKIB'    // even newer Bink files use this tag

typedef struct BINKHDR {
  U32 Marker;                 // Bink marker
  U32 Size;                   // size of the file-8
  U32 Frames;                 // Number of frames (1 based, 100 = 100 frames)
  U32 LargestFrameSize;       // Size in bytes of largest frame
  U32 InternalFrames;         // Number of internal frames

  U32 Width;                  // Width (1 based, 640 for example)
  U32 Height;                 // Height (1 based, 480 for example)
  U32 FrameRate;              // frame rate
  U32 FrameRateDiv;           // frame rate divisor (framerate/frameratediv=fps)

  U32 Flags;                  // height compression options
  U32 NumTracks;              // number of tracks
} BINKHDR;


//=======================================================================
#define BINKYAINVERT          0x00000800L // Reverse Y and A planes when blitting (for debugging)
#define BINKFRAMERATE         0x00001000L // Override fr (call BinkFrameRate first)
#define BINKPRELOADALL        0x00002000L // Preload the entire animation
#define BINKSNDTRACK          0x00004000L // Set the track number to play
#define BINKOLDFRAMEFORMAT    0x00008000L // using the old Bink frame format (internal use only)
#define BINKRBINVERT          0x00010000L // use reversed R and B planes (internal use only)
#define BINKGRAYSCALE         0x00020000L // Force Bink to use grayscale
//#define BINKNOMMX             0x00040000L // Don't use MMX
#define BINKNOSKIP            0x00080000L // Don't skip frames if falling behind
#define BINKALPHA             0x00100000L // Decompress alpha plane (if present)
#define BINKNOFILLIOBUF       0x00200000L // Don't Fill the IO buffer (in BinkOpen and BinkCopyTo)
#define BINKSIMULATE          0x00400000L // Simulate the speed (call BinkSim first)
#define BINKFILEHANDLE        0x00800000L // Use when passing in a file handle
#define BINKIOSIZE            0x01000000L // Set an io size (call BinkIOSize first)
#define BINKIOPROCESSOR       0x02000000L // Set an io processor (call BinkIO first)
#define BINKFROMMEMORY        0x04000000L // Use when passing in a pointer to the file
#define BINKNOTHREADEDIO      0x08000000L // Don't use a background thread for IO
#define BINKNOFRAMEBUFFERS    0x00000400L // Don't allocate internal frame buffers - application must call BinkRegisterFrameBuffers
#define BINKNOYPLANE          0x00000200L // Don't decompress the Y plane (internal flag)
#define BINKRUNNINGASYNC      0x00000100L // This frame is decompressing asynchronously

#define BINKSURFACEFAST       0x00000000L
#define BINKSURFACESLOW       0x08000000L
#define BINKSURFACEDIRECT     0x04000000L

#define BINKCOPYALL           0x80000000L // copy all pixels (not just changed)
#define BINKCOPY2XH           0x10000000L // Force doubling height scaling
#define BINKCOPY2XHI          0x20000000L // Force interleaving height scaling
#define BINKCOPY2XW           0x30000000L // copy the width zoomed by two
#define BINKCOPY2XWH          0x40000000L // copy the width and height zoomed by two
#define BINKCOPY2XWHI         0x50000000L // copy the width and height zoomed by two
#define BINKCOPY1XI           0x60000000L // copy the width and height zoomed by two
#define BINKCOPYNOSCALING     0x70000000L // Force scaling off

//#define BINKNOFILLIOBUF     0x00200000L  // Don't Fill the IO buffer (in BinkOpen and BinkCopyTo)
//#define BINKALPHA           0x00100000L // Decompress alpha plane (if present)
//#define BINKNOSKIP          0x00080000L // don't skip the blit if behind in sound
//#define BINKNOMMX           0x00040000L // No MMX
//#define BINKGRAYSCALE       0x00020000L // force Bink to use grayscale
//#define BINKRBINVERT        0x00010000L // use reversed R and B planes

#define BINKSURFACEP8          0
#define BINKSURFACE24          1
#define BINKSURFACE24R         2
#define BINKSURFACE32          3
#define BINKSURFACE32R         4
#define BINKSURFACE32A         5
#define BINKSURFACE32RA        6
#define BINKSURFACE4444        7
#define BINKSURFACE5551        8
#define BINKSURFACE555         9
#define BINKSURFACE565        10
#define BINKSURFACE655        11
#define BINKSURFACE664        12
#define BINKSURFACEYUY2       13
#define BINKSURFACEUYVY       14
#define BINKSURFACEYV12       15
#define BINKSURFACEMASK       15

#ifdef __RADXBOX__

#define BINKSURFACESALL        32
#define BINKCONVERTERSMONO     64
#define BINKCONVERTERS2X      256

#define BINKCONVERTERSALL (BINKSURFACESALL|BINKCONVERTERSMONO|BINKCONVERTERS2X)

#define BinkLoad() BinkLoadUnload(1)
#define BinkUnload() BinkLoadUnload(0)

#define BinkLoadConverter(val) BinkLoadUnloadConverter(val,1)
#define BinkUnloadConverter(val) BinkLoadUnloadConverter(val,0)

RADEXPFUNC void RADEXPLINK BinkLoadUnload( S32 inout );
RADEXPFUNC void RADEXPLINK BinkLoadUnloadConverter( U32 surfaces, S32 inout );

#endif

#define BINKGOTOQUICK          1
#define BINKGOTOQUICKSOUND     2

#define BINKGETKEYPREVIOUS     0
#define BINKGETKEYNEXT         1
#define BINKGETKEYCLOSEST      2
#define BINKGETKEYNOTEQUAL   128

#define BINKDOFRAMEY           1
#define BINKDOFRAMEA           2
#define BINKDOFRAMECRCB        4
#define BINKDOFRAMESTART       256
#define BINKDOFRAMEEND         512


//=======================================================================

#ifdef __RADMAC__

#if !defined(__RADMACH__) && defined(__RADINDLL__)
#pragma export on
#endif

  RADEXPFUNC HBINK RADEXPLINK BinkMacOpen(void /*FSSpec*/ * fsp,U32 flags);
#endif

#ifdef __RADNDS__
  RADEXPFUNC HBINK RADEXPLINK BinkNDSOpen(void /*FSFileID*/ * fid,U32 flags);
#endif

RADEXPFUNC void PTR4* RADEXPLINK BinkLogoAddress(void);

RADEXPFUNC void RADEXPLINK BinkSetError(const char PTR4* err);
RADEXPFUNC char PTR4* RADEXPLINK BinkGetError(void);

RADEXPFUNC HBINK RADEXPLINK BinkOpen(const char PTR4* name,U32 flags);

RADEXPFUNC void RADEXPLINK BinkGetFrameBuffersInfo( HBINK bink, BINKFRAMEBUFFERS * fbset );
RADEXPFUNC void RADEXPLINK BinkRegisterFrameBuffers( HBINK bink, BINKFRAMEBUFFERS * fbset );
RADEXPFUNC S32  RADEXPLINK BinkDoFrame(HBINK bnk);
RADEXPFUNC S32  RADEXPLINK BinkDoFramePlane(HBINK bnk,U32 which_planes);
RADEXPFUNC void RADEXPLINK BinkNextFrame(HBINK bnk);
RADEXPFUNC S32  RADEXPLINK BinkWait(HBINK bnk);
RADEXPFUNC void RADEXPLINK BinkClose(HBINK bnk);
RADEXPFUNC S32  RADEXPLINK BinkPause(HBINK bnk,S32 pause);
RADEXPFUNC S32  RADEXPLINK BinkCopyToBuffer(HBINK bnk,void* dest,S32 destpitch,U32 destheight,U32 destx,U32 desty,U32 flags);
RADEXPFUNC S32  RADEXPLINK BinkCopyToBufferRect(HBINK bnk,void* dest,S32 destpitch,U32 destheight,U32 destx,U32 desty,U32 srcx, U32 srcy, U32 srcw, U32 srch, U32 flags);
RADEXPFUNC S32  RADEXPLINK BinkGetRects(HBINK bnk,U32 flags);
RADEXPFUNC void RADEXPLINK BinkGoto(HBINK bnk,U32 frame,S32 flags);  // use 1 for the first frame
RADEXPFUNC U32  RADEXPLINK BinkGetKeyFrame(HBINK bnk,U32 frame,S32 flags);

RADEXPFUNC S32  RADEXPLINK BinkSetVideoOnOff(HBINK bnk,S32 onoff);
RADEXPFUNC S32  RADEXPLINK BinkSetSoundOnOff(HBINK bnk,S32 onoff);
RADEXPFUNC void RADEXPLINK BinkSetVolume(HBINK bnk, U32 trackid, S32 volume);
RADEXPFUNC void RADEXPLINK BinkSetPan(HBINK bnk,U32 trackid, S32 pan);
RADEXPFUNC void RADEXPLINK BinkSetMixBins(HBINK bnk,U32 trackid, U32 PTR4* mix_bins, U32 total);
RADEXPFUNC void RADEXPLINK BinkSetMixBinVolumes(HBINK bnk,U32 trackid, U32 PTR4* vol_mix_bins, S32 PTR4* volumes, U32 total);
RADEXPFUNC void RADEXPLINK BinkService(HBINK bink);

RADEXPFUNC S32  RADEXPLINK BinkShouldSkip(HBINK bink);

RADEXPFUNC void RADEXPLINK BinkGetPalette( void * out_pal );

#define BINKBGIOSUSPEND 1
#define BINKBGIORESUME  2
#define BINKBGIOWAIT    0x80000000

RADEXPFUNC S32  RADEXPLINK BinkControlBackgroundIO(HBINK bink,U32 control);


#if defined( __RADWIN__ ) || defined( __RADXENON__ ) || defined( __RADPS3__ )

RADEXPFUNC S32 RADEXPLINK BinkStartAsyncThread( S32 thread_num, void const * param );
RADEXPFUNC S32  RADEXPLINK BinkDoFrameAsync(HBINK bink, U32 yplane_thread_num, U32 other_work_thread_num );
RADEXPFUNC S32  RADEXPLINK BinkDoFrameAsyncWait(HBINK bink, S32 us);
RADEXPFUNC S32 RADEXPLINK BinkRequestStopAsyncThread( S32 thread_num );
RADEXPFUNC S32 RADEXPLINK BinkWaitStopAsyncThread( S32 thread_num );

#endif


typedef struct BINKTRACK PTR4* HBINKTRACK;

typedef struct BINKTRACK
{
  U32 Frequency;
  U32 Bits;
  U32 Channels;
  U32 MaxSize;

  HBINK bink;
  UINTa sndcomp;
  S32 trackindex;
} BINKTRACK;


RADEXPFUNC HBINKTRACK RADEXPLINK BinkOpenTrack(HBINK bnk,U32 trackindex);
RADEXPFUNC void RADEXPLINK BinkCloseTrack(HBINKTRACK bnkt);
RADEXPFUNC U32  RADEXPLINK BinkGetTrackData(HBINKTRACK bnkt,void PTR4* dest);

RADEXPFUNC U32  RADEXPLINK BinkGetTrackType(HBINK bnk,U32 trackindex);
RADEXPFUNC U32  RADEXPLINK BinkGetTrackMaxSize(HBINK bnk,U32 trackindex);
RADEXPFUNC U32  RADEXPLINK BinkGetTrackID(HBINK bnk,U32 trackindex);

RADEXPFUNC void RADEXPLINK BinkGetSummary(HBINK bnk,BINKSUMMARY PTR4* sum);
RADEXPFUNC void RADEXPLINK BinkGetRealtime(HBINK bink,BINKREALTIME PTR4* run,U32 frames);

RADEXPFUNC void RADEXPLINK BinkSetSoundTrack(U32 total_tracks, U32 PTR4* tracks);
RADEXPFUNC void RADEXPLINK BinkSetIO(BINKIOOPEN io);
RADEXPFUNC void RADEXPLINK BinkSetFrameRate(U32 forcerate,U32 forceratediv);
RADEXPFUNC void RADEXPLINK BinkSetSimulate(U32 sim);
RADEXPFUNC void RADEXPLINK BinkSetIOSize(U32 iosize);

RADEXPFUNC S32  RADEXPLINK BinkSetSoundSystem(BINKSNDSYSOPEN open, UINTa param);

#ifdef __RADX86__
#define BINK_CPU_MMX   1
#define BINK_CPU_3DNOW 2
#define BINK_CPU_SSE   4
#define BINK_CPU_SSE2  8
#elif defined( __RADMAC__ )
#define BINK_CPU_ALTIVEC 1
#endif

RADEXPFUNC S32 RADEXPLINK BinkControlPlatformFeatures( S32 use, S32 dont_use );

#ifdef __RADWIN__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenDirectSound(UINTa param); // don't call directly
  #define BinkSoundUseDirectSound(lpDS) BinkSetSoundSystem(BinkOpenDirectSound,(UINTa)lpDS)

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenWaveOut(UINTa param); // don't call directly
  #define BinkSoundUseWaveOut() BinkSetSoundSystem(BinkOpenWaveOut,0)

#endif

#ifdef __RADXENON__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenXAudio(UINTa param); // don't call directly
  #define BinkSoundUseXAudio() BinkSetSoundSystem(BinkOpenXAudio,0)

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenXAudio2(UINTa param); // don't call directly
  #define BinkSoundUseXAudio2(IXAudio2_ptr) BinkSetSoundSystem(BinkOpenXAudio2,(UINTa)IXAudio2_ptr)


#endif


#ifndef __RADMAC__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenMiles(UINTa param); // don't call directly
  #define BinkSoundUseMiles(hdigdriver) BinkSetSoundSystem(BinkOpenMiles,(UINTa)hdigdriver)

#endif


#ifdef __RADMAC__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenSoundManager(UINTa param); // don't call directly
  #define BinkSoundUseSoundManager() BinkSetSoundSystem(BinkOpenSoundManager,0)

#endif

#ifdef __RADLINUX__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenSDLMixer(UINTa param); // don't call directly
  #define BinkSoundUseSDLMixer() BinkSetSoundSystem(BinkOpenSDLMixer,0)

#endif

#if defined( __RADNGC__ ) || defined( __RADWII__ )

  typedef void PTR4 * (RADLINK PTR4* RADARAMALLOC) ( U32 num_bytes );
  typedef void (RADLINK PTR4* RADARAMFREE)   ( void PTR4 * ptr );

  typedef struct RADARAMCALLBACKS
  {
    RADARAMALLOC aram_malloc;
    RADARAMFREE aram_free;
  } RADARAMCALLBACKS;

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenAX(U32 param); // don't call directly
  #define BinkSoundUseAX( functions ) BinkSetSoundSystem(BinkOpenAX,(U32)functions) // takes a pointer to RADARAMCALLBACKS

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenMusyXSound(U32 param); // don't call directly
  #define BinkSoundUseMusyX( ) BinkSetSoundSystem(BinkOpenMusyXSound,0)

#endif

#ifdef __RADPS2__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenRAD_IOP(U32 param); // don't call directly
  #define BinkSoundUseRAD_IOP( which_sound_core ) BinkSetSoundSystem(BinkOpenRAD_IOP,which_sound_core)

#endif

#ifdef __RADPS3__

  RADEXPFUNC void RADEXPLINK BinkFreeGlobals( void );

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenLibAudio( U32 param );
  #define BinkSoundUseLibAudio( number_of_speakers ) BinkSetSoundSystem( BinkOpenLibAudio, number_of_speakers )

#endif

#ifdef __RADPSP__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenPSPSound( U32 param );
  #define BinkSoundUsePSPSound( which_pcm_channel ) BinkSetSoundSystem( BinkOpenPSPSound, ((U32)(S32)which_pcm_channel) )

#endif

#ifdef __RADNDS__

  RADEXPFUNC BINKSNDOPEN RADEXPLINK BinkOpenNDSSound(U32 param); // don't call directly
  #define BinkSoundUseNDSSound( param ) BinkSetSoundSystem(BinkOpenNDSSound, param)

  // Packs channel bitmask into loword and alarm bitmask into lobyte of hiword
  #define BINK_PACK_CHANNEL( ch )    ((U32)(1 << ((ch) & 0xFFFF)))
  #define BINK_PACK_ALARM( al )      ((U32)(1 << (16 + ((al) & 0xFF))))

#endif

#if defined(__RADXBOX__) || defined(__RADWIN__)

RADEXPFUNC S32 RADEXPLINK BinkDX8SurfaceType(void* lpD3Ds);

#endif

#if defined(__RADXENON__) || defined(__RADWIN__)

RADEXPFUNC S32 RADEXPLINK BinkDX9SurfaceType(void* lpD3Ds);

#endif


// The BinkBuffer API isn't implemented on DOS, or any of the consoles
#if !defined(__RADDOS__) && !defined(__RADXBOX__) && !defined(__RADNGC__) && !defined(__RADNDS__)

//=========================================================================
typedef struct BINKBUFFER * HBINKBUFFER;

#define BINKBUFFERSTRETCHXINT    0x80000000
#define BINKBUFFERSTRETCHX       0x40000000
#define BINKBUFFERSHRINKXINT     0x20000000
#define BINKBUFFERSHRINKX        0x10000000
#define BINKBUFFERSTRETCHYINT    0x08000000
#define BINKBUFFERSTRETCHY       0x04000000
#define BINKBUFFERSHRINKYINT     0x02000000
#define BINKBUFFERSHRINKY        0x01000000
#define BINKBUFFERSCALES         0xff000000
#define BINKBUFFERRESOLUTION     0x00800000

#ifdef __RADMAC__

//#include <windows.h>
//#include <palettes.h>
//#include <qdoffscreen.h>

typedef struct BINKBUFFER {
  U32 Width;
  U32 Height;
  U32 WindowWidth;
  U32 WindowHeight;
  U32 SurfaceType;
  void* Buffer;
  S32 BufferPitch;
  U32 ScreenWidth;
  U32 ScreenHeight;
  U32 ScreenDepth;
  U32 ScaleFlags;

  S32 destx,desty;
  S32 wndx,wndy;
  U32 wnd;

  S32 noclipping;
  U32 type;
  S32 issoftcur;
  U32 cursorcount;

} BINKBUFFER;


#define BINKBUFFERAUTO                0
#define BINKBUFFERDIRECT              1
#define BINKBUFFERGWORLD              2
#define BINKBUFFERTYPEMASK           31

RADEXPFUNC HBINKBUFFER RADEXPLINK BinkBufferOpen( void* /*WindowPtr*/ wnd, U32 width, U32 height, U32 bufferflags);
RADEXPFUNC S32 RADEXPLINK BinkGDSurfaceType( void* /*GDHandle*/ gd );
RADEXPFUNC S32 RADEXPLINK BinkIsSoftwareCursor(void* /*GDHandle*/ gd);
RADEXPFUNC S32 RADEXPLINK BinkCheckCursor(void* /*WindowPtr*/ wp,S32 x,S32 y,S32 w,S32 h);

#else

typedef struct BINKBUFFER {
  U32 Width;
  U32 Height;
  U32 WindowWidth;
  U32 WindowHeight;
  U32 SurfaceType;
  void* Buffer;
  S32 BufferPitch;
  S32 ClientOffsetX;
  S32 ClientOffsetY;
  U32 ScreenWidth;
  U32 ScreenHeight;
  U32 ScreenDepth;
  U32 ExtraWindowWidth;
  U32 ExtraWindowHeight;
  U32 ScaleFlags;
  U32 StretchWidth;
  U32 StretchHeight;

  S32 surface;
  void* ddsurface;
  void* ddclipper;
  S32 destx,desty;
  S32 wndx,wndy;
  void* wnd;
  S32 minimized;
  S32 ddoverlay;
  S32 ddoffscreen;
  S32 lastovershow;

  S32 issoftcur;
  U32 cursorcount;
  void* buffertop;
  U32 type;
  S32 noclipping;

  S32 loadeddd;
  S32 loadedwin;

  void* dibh;
  void* dibbuffer;
  S32 dibpitch;
  void* dibinfo;
  void* dibdc;
  void* diboldbitmap;

} BINKBUFFER;


#define BINKBUFFERAUTO                0
#define BINKBUFFERPRIMARY             1
#define BINKBUFFERDIBSECTION          2
#define BINKBUFFERYV12OVERLAY         3
#define BINKBUFFERYUY2OVERLAY         4
#define BINKBUFFERUYVYOVERLAY         5
#define BINKBUFFERYV12OFFSCREEN       6
#define BINKBUFFERYUY2OFFSCREEN       7
#define BINKBUFFERUYVYOFFSCREEN       8
#define BINKBUFFERRGBOFFSCREENVIDEO   9
#define BINKBUFFERRGBOFFSCREENSYSTEM 10
#define BINKBUFFERLAST               10
#define BINKBUFFERTYPEMASK           31

RADEXPFUNC HBINKBUFFER RADEXPLINK BinkBufferOpen( void* /*HWND*/ wnd, U32 width, U32 height, U32 bufferflags);
RADEXPFUNC S32 RADEXPLINK BinkBufferSetHWND( HBINKBUFFER buf, void* /*HWND*/ newwnd);
RADEXPFUNC S32 RADEXPLINK BinkDDSurfaceType(void PTR4* lpDDS);
RADEXPFUNC S32 RADEXPLINK BinkIsSoftwareCursor(void PTR4* lpDDSP, void* /*HCURSOR*/ cur);
RADEXPFUNC S32 RADEXPLINK BinkCheckCursor(void* /*HWND*/ wnd,S32 x,S32 y,S32 w,S32 h);
RADEXPFUNC S32 RADEXPLINK BinkBufferSetDirectDraw(void PTR4* lpDirectDraw, void PTR4* lpPrimary);

#endif

RADEXPFUNC void RADEXPLINK BinkBufferClose( HBINKBUFFER buf);
RADEXPFUNC S32 RADEXPLINK BinkBufferLock( HBINKBUFFER buf);
RADEXPFUNC S32 RADEXPLINK BinkBufferUnlock( HBINKBUFFER buf);
RADEXPFUNC void RADEXPLINK BinkBufferSetResolution( S32 w, S32 h, S32 bits);
RADEXPFUNC void RADEXPLINK BinkBufferCheckWinPos( HBINKBUFFER buf, S32 PTR4* NewWindowX, S32 PTR4* NewWindowY);
RADEXPFUNC S32 RADEXPLINK BinkBufferSetOffset( HBINKBUFFER buf, S32 destx, S32 desty);
RADEXPFUNC void RADEXPLINK BinkBufferBlit( HBINKBUFFER buf, BINKRECT PTR4* rects, U32 numrects );
RADEXPFUNC S32 RADEXPLINK BinkBufferSetScale( HBINKBUFFER buf, U32 w, U32 h);
RADEXPFUNC char PTR4* RADEXPLINK BinkBufferGetDescription( HBINKBUFFER buf);
RADEXPFUNC char PTR4* RADEXPLINK BinkBufferGetError();
RADEXPFUNC void RADEXPLINK BinkRestoreCursor(S32 checkcount);
RADEXPFUNC S32 RADEXPLINK BinkBufferClear(HBINKBUFFER buf, U32 RGB);

#endif

typedef void PTR4* (RADLINK PTR4* BINKMEMALLOC) (U32 bytes);
typedef void       (RADLINK PTR4* BINKMEMFREE)  (void PTR4* ptr);

RADEXPFUNC void RADEXPLINK BinkSetMemory(BINKMEMALLOC a,BINKMEMFREE f);

#ifdef __RADMAC__

#if !defined(__RADMACH__) && defined(__RADINDLL__)
#pragma export off
#endif

#endif

RADDEFEND

#endif

// @cdep pre $set(INCs,$INCs -I$clipfilename($file)) $ignore(TakeCPP)

#endif

