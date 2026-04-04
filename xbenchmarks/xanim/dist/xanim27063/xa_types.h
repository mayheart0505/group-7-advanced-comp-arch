
#include <stdio.h>

typedef int             xaLONG;
typedef unsigned int    xaULONG;
typedef short           xaSHORT;
typedef unsigned short  xaUSHORT;
typedef char            xaBYTE;
typedef unsigned char   xaUBYTE;

#define xaFALSE  0
#define xaTRUE   1
#define xaNOFILE 2
#define xaERROR  3

#define xaMIN(x,y)   ( ((x)>(y))?(y):(x) )
#define xaMAX(x,y)   ( ((x)>(y))?(x):(y) )


typedef struct
{
  xaUSHORT red,green,blue,gray;
} ColorReg;

typedef struct XA_ACTION_STRUCT
{
 xaLONG type;           /* type of action */
 xaLONG cmap_rev;          /* rev of cmap */
 xaUBYTE *data;         /* data ptr */
 struct XA_ACTION_STRUCT *next;
 struct XA_CHDR_STRUCT *chdr;
 ColorReg *h_cmap;      /* For IFF HAM images */
 xaULONG *map;
 struct XA_ACTION_STRUCT *next_same_chdr; /*ptr to next action with same cmap*/
} XA_ACTION;

typedef struct XA_CHDR_STRUCT
{
 xaLONG rev;
 ColorReg *cmap;
 xaULONG csize,coff;
 xaULONG *map;
 xaULONG msize,moff;
 struct XA_CHDR_STRUCT *next;
 XA_ACTION *acts;
 struct XA_CHDR_STRUCT *new_chdr;
} XA_CHDR;

typedef struct
{
  xaUBYTE *Ybuf;
  xaUBYTE *Ubuf;
  xaUBYTE *Vbuf;
  xaUBYTE *the_buf;
  xaULONG  the_buf_size;
  xaUSHORT y_w,y_h;
  xaUSHORT uv_w,uv_h;
} YUVBufs;

typedef struct
{
  xaULONG Uskip_mask;
  xaLONG *YUV_Y_tab;
  xaLONG *YUV_UB_tab;
  xaLONG *YUV_VR_tab;
  xaLONG *YUV_UG_tab;
  xaLONG *YUV_VG_tab;
} YUVTabs;


#define DEBUG_LEVEL1   if (xa_debug >= 1)
#define DEBUG_LEVEL2   if (xa_debug >= 2)
#define DEBUG_LEVEL3   if (xa_debug >= 3)
#define DEBUG_LEVEL4   if (xa_debug >= 4)
#define DEBUG_LEVEL5   if (xa_debug >= 5)

/* DELTA Return VALUES */
#define ACT_DLTA_NORM   0x00000000   /* nothing special */
#define ACT_DLTA_BODY   0x00000001   /* IFF BODY - used for dbl buffer */
#define ACT_DLTA_XOR    0x00000002   /* delta work in both directions */
#define ACT_DLTA_NOP    0x00000004   /* delta didn't change anything */
#define ACT_DLTA_MAPD   0x00000008   /* delta was able to map image */
#define ACT_DLTA_BAD    0x80000000   /* uninitialize value if needed */
#define ACT_DLTA_DROP   0x00000010   /* drop this one */
