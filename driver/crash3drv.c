/***************************************************************************/
/*
** PSF-o-Cycle development system
**
** This is an example which demonstrates the basics of how to make a PSF
** driver stub and illustrates the format of the PSF_DRIVER_INFO block.
** It can be customized to create stubs for actual games (whether they use
** the SEQ/VAB library or not).
*/

/*
** Define the location of the PSF driver stub.
** You should define this to somewhere safe where there's no useful data and
** which will not get overwritten by the BSS clear loop.
*/
#define PSFDRV_LOAD       (0x80100000)
#define PSFDRV_SIZE       (0x00001000)
#define PSFDRV_PARAM      (0x80101000)
#define PSFDRV_PARAM_SIZE (0x00000100)

/*
** You can also define locations of game-specific data here.
*/
#define MY_SEQ      (0x80120000)
#define MY_SEQ_SIZE (0x00010000)
#define MY_VH       (0x80130000)
#define MY_VH_SIZE  (0x00010000)
#define MY_VB       (0x80140000)
#define MY_VB_SIZE  (0x00070000)

/*
** Parameters - you can make up any parameters you want within the
** PSFDRV_PARAM block.
*/
#define PARAM_SEQVOL   (*((unsigned char*)(PSFDRV_PARAM+0x0000)))
#define PARAM_TICKMODE (*((unsigned char*)(PSFDRV_PARAM+0x000C)))
#define PARAM_LOOPOFF  (*((unsigned char*)(PSFDRV_PARAM+0x0010)))
#define PARAM_SEQNUM   (*((unsigned char*)(PSFDRV_PARAM+0x0014)))
#define PARAM_MAXSEQ   (*((unsigned char*)(PSFDRV_PARAM+0x0018)))

/***************************************************************************/
/*
** Entry point
*/
int psfdrv(void);
int psfdrv_entry(void) {
  /*
  ** Read the entire driver area, to ensure it doesn't get thrown out by
  ** PSFLab's optimizer
  */
  int *a = ((int*)(PSFDRV_LOAD));
  int *b = ((int*)(PSFDRV_LOAD+PSFDRV_SIZE));
  int c = 0;
  while(a < b) { c += (*a++); }
  /* This return value is completely ignored. */
  return c + psfdrv();
}

/***************************************************************************/

#define ASCSIG(a,b,c,d) ( \
  ((((unsigned long)(a)) & 0xFF) <<  0) | \
  ((((unsigned long)(b)) & 0xFF) <<  8) | \
  ((((unsigned long)(c)) & 0xFF) << 16) | \
  ((((unsigned long)(d)) & 0xFF) << 24)   \
  )

/***************************************************************************/
/*
** PSF_DRIVER_INFO block.
*/
unsigned long driverinfo[] = {
  /*
  ** Signature
  */
  ASCSIG('P','S','F','_'),
  ASCSIG('D','R','I','V'),
  ASCSIG('E','R','_','I'),
  ASCSIG('N','F','O',':'),
  /*
  ** Driver load address (was #defined earlier)
  */
  PSFDRV_LOAD,
  /*
  ** Driver entry point
  */
  (int)psfdrv_entry,
  /*
  ** Driver text string.  This should include the name of the game.
  */
  (int)"Crash Bandicoot 3 (NTSC-J) PSF Driver v1.0",
  /*
  ** Original EXE filename and CRC - ignore if zero
  **
  ** You may not want to use the exact original EXE here.  Sometimes you may
  ** want to patch the BSS clearing loop first, to ensure that it doesn't
  ** overwrite your driver stub, SEQ data, or other data that you added after
  ** the fact.  In this case I usually use a different name for the patched
  ** EXE, i.e. "ff8patch.exe" for Final Fantasy 8, and redo the CRC
  ** accordingly.
  */
  (int)"SCPS_100.73", 0x4E3986B5,
  /*
  ** Jump patch address
  ** You should change this to point to the address of the "jal main"
  ** instruction in the game's original EXE.
  */
  0x80048E48,
  /*
  ** List of song-specific areas we DO NOT upgrade.
  ** This is a 0-terminated list of addresses and byte lengths.
  ** Mark the areas containing SEQ, VAB, or other song-specific data here.
  ** Marking the psfdrv parameter area here might also be a good idea.
  */
  MY_SEQ, MY_SEQ_SIZE,
  MY_VH, MY_VH_SIZE,
  MY_VB, MY_VB_SIZE,
  PSFDRV_PARAM, PSFDRV_PARAM_SIZE,
  0,
  /*
  ** List of parameters (name,address,bytesize)
  ** This is a 0-terminated list.
  */
  (int)"seqvol",   (int)(&PARAM_SEQVOL),   1,
  (int)"tickmode", (int)(&PARAM_TICKMODE), 1,
  (int)"loop_off", (int)(&PARAM_LOOPOFF ), 1,
  (int)"seqnum",   (int)(&PARAM_SEQNUM)  , 1,
  (int)"maxseq",   (int)(&PARAM_MAXSEQ)  , 1,
  0
};

/***************************************************************************/
/*
** Handy definitions
*/
#define NULL (0)

#define F0(x) (*((func0)(x)))
#define F1(x) (*((func1)(x)))
#define F2(x) (*((func2)(x)))
#define F3(x) (*((func3)(x)))
#define F4(x) (*((func4)(x)))
#define F5(x) (*((func5)(x)))
typedef int (*func0)(void);
typedef int (*func1)(int);
typedef int (*func2)(int,int);
typedef int (*func3)(int,int,int);
typedef int (*func4)(int,int,int,int);
typedef long (*func5)(long);

/*
** die() function - emits a break instruction.
** This isn't emulated in Highly Experimental, so it will cause the emulation
** to halt (this is a desired effect).
*/
unsigned long die_data[] = {0x4D};
#define die F0(die_data)

/*
** loopforever() - emits a simple branch and nop.
** Guaranteed to be detected as idle in H.E. no matter what the compiler
** does.
*/
unsigned long loopforever_data[] = {0x1000FFFF,0};
#define loopforever F0(loopforever_data)

#define ASSERT(x) { if(!(x)) { die(); } }

/***************************************************************************/
/*
** Library call addresses.
**
** You'll want to fill in the proper addresses for these based on what you
** found in IDA Pro or similar.
*/  
  #define ResetCallback                          F0(0x8004B850)

  #define SsInit                                 F0(0x8005377C)
  #define SsSetMVol(a,b)                         F2(0x800332D4) ((int)(a),(int)(b))
  #define SsStart                                F0(0x80055834)
  #define SsSetTableSize(a,b,c)                  F3(0x80056D30) ((int)(a),(int)(b),(int)(c))
  #define SsSetTickMode(a)                       F1(0x80056F08) ((int)(a))
  #define SsVabOpenHeadSticky(a,b,c)   ((short)( F3(0x8005B2C0) ((int)(a),(int)(b),(int)(c)) ))
  #define SpuIsTransferCompleted(a)    ((short)( F5(0x80051D78) ((long)(a)) ))
  
  // SEP functions
  #define SsSepOpen(a,b,c)             ((short)( F3(0x800537CC) ((int)(a),(int)(b),(int)(c)) ))
  #define SsSepPlay(a,b,c,d)                     F4(0x80055388) ((int)(a),(int)(b),(int)(c),(int)(d))
  #define SsSepSetVol(a,b,c,d)                   F4(0x80057160) ((int)(a),(int)(b),(int)(c),(int)(d))

  // Not found in the game itself
  short SsVabTransBody(unsigned char*, short);

  // Dependency of SsVabTransBody
  #define SpuWrite(a,b)                          F2(0x80051C88) ((int)(a),(int)(b))
  #define SpuSetTransferStartAddr(a)             F1(0x80051CE8) ((int)(a))
  #define SpuSetTransferMode(a)                  F1(0x80051D48) ((int)(a))
  #define _spu_setInTransfer(a)                  F1(0x800581D4) ((int)(a))
  #define _svm_vab_used            ((unsigned char*)0x8006ECE8)
  #define _svm_vab_total           ((unsigned long*)0x8006ED00)
  #define _svm_vab_start           ((unsigned long*)0x8006ED48)

/***************************************************************************/
/*
** PSF driver main() replacement
*/
int psfdrv(void) {
  void *seq, *vh, *vb;
  int vabid, seqid;
  int mvol;
  int seqvol;
  int tickmode;
  int loop_off;
  int seqnum;
  int maxseq;
  int r;

  seq = (void*)(MY_SEQ);
  vh  = (void*)(MY_VH);
  vb  = (void*)(MY_VB);

  mvol = 0x7F;  // Constant

  /*
  ** Retrieve parameters and set useful defaults if they're zero
  */
  seqvol   = PARAM_SEQVOL;    // Set 0x7F(127d) for Crash 1 and 2, 0x5F(95d) for Crash 3
  tickmode = PARAM_TICKMODE;
  loop_off = PARAM_LOOPOFF;
  seqnum   = PARAM_SEQNUM;
  maxseq   = PARAM_MAXSEQ;
  if(!seqvol)   seqvol    = 0x5F;
  if(!seqnum)   seqnum    = 0;
  if(!tickmode) tickmode  = 1;
  if(!maxseq)   maxseq    = 1;

  /*
  ** Initialize stuff
  */
  ResetCallback();

  SsInit();
  
  /* If the game originally used a predefined address for the SEQ table,
  ** you might want to set it here */
#define SSTABLE (0x8006D448)

  SsSetTableSize(SSTABLE, 2, maxseq);
  SsSetTickMode(tickmode);
  
  SsSetMVol(mvol, mvol);


  /*
  ** Start sound engine
  */
  SsStart();

  /*
  ** Open/transfer the VAB data
  */
  vabid = SsVabOpenHeadSticky(vh, 0, 0x1010);
  ASSERT(vabid >= 0);

  r = SsVabTransBody(vb, vabid);
  ASSERT(r == vabid);

  do {
    r = SpuIsTransferCompleted(1);
  } while (r == 0);
  ASSERT(r == 1);

  // Open the SEP
  seqid = SsSepOpen(seq, vabid, maxseq);
  ASSERT(seqid >= 0);

  //Play the seq
  SsSepPlay(seqid, seqnum, 1, loop_off);

  // Set its volume
  SsSepSetVol(seqid, seqnum, seqvol, seqvol);

  // Loop a while.
  loopforever();

  return 0;
}

/***************************************************************************/
/*
** Alternative to the PsyQ library function
*/
short SsVabTransBody(unsigned char *addr, short vabid) {
  if (((unsigned short)vabid < 0x11) && (_svm_vab_used[vabid] == 2)) {
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(_svm_vab_start[vabid]);
    SpuWrite(addr, _svm_vab_total[vabid]);
    _svm_vab_used[vabid] = 1;
    return vabid;
  }
  else {
    _spu_setInTransfer(0);
    return -1;
  }
}

/***************************************************************************/
