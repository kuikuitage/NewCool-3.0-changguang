#ifndef IQIYITVCWRAPPER_H_INCLUDED
#define IQIYITVCWRAPPER_H_INCLUDED



/// /////////////////////////////////////////////////////////////
///                         error code
/// /////////////////////////////////////////////////////////////
#define     IQY_NORMAL                   0      //
#define     IQY_ERROR_PARAM              1      //
#define     IQY_ERROR_SYSTEM             2      //system error
#define     IQY_ERROR_BUSY               3      //system busy
#define     IQY_ERROR_DISABLE_CHG_STATUS 4      //status change disable
#define     IQY_ERROR_DEVICE_ERROR       5
#define     IQY_ERROR_BUFFERING          6
#define     IQY_ERROR_OPERATE_DISABLE    7
#define     IQY_ERROR_PLAY_CHECK_ERROR   8



/// /////////////////////////////////////////////////////////////
///                         define part
/// /////////////////////////////////////////////////////////////
#define     IQY_TAG_MAX                  16     //最大能取得的tag个数        max = 32 ?
#define     IQY_SUBTAG_MAX               40     //最大能取得的subtag个数     max = 32 ?
#define     IQY_SEARCHWORD_MAX           32     //search word最大个数       max = 128
#define     IQY_ALBUM_LIST_MAX           50     //最大能取得的专辑个数         max = 60
#define     IQY_CHANNEL_MAX              20     //最大能取得的channel个数     max = 60
#define     IQY_EPISODE_MAX              20     //最大能取得的episode个数     max = 120
#define     IQY_CHANNEL_LABEL_MAX        20     //label of channel's max
#define     IQY_PLAY_LIST_MAX            200    //play list's max
#define     IQY_VD_MAX                   30     //vd max
#define     IQY_AD_DURATION_MAX          30     //ad duration list max

#define     IQY_LEN_MAX                  512    //字符串最大长度
#define     IQY_LEN_URL                  256    //url长度
#define     IQY_LEN_MSG                  128    //返回msg长度
#define     IQY_LEN_QPID                 32     //qipuId长度
#define     IQY_LEN_VID                  36     //VID长度 = 32
#define     IQY_LEN_CHANNEL_ITEM0        17     //channel结构体中，基本单位中，长度小的item
#define     IQY_LEN_CHANNEL_ITEM1        33     //channel结构体中，基本单位中，长度较小的item
#define     IQY_LEN_CHANNEL_ITEM2        65     //channel结构体中，基本单位中，长度普通的item
#define     IQY_LEN_CAST_ITEM1           33     //cast结构体中，基本单位中，长度较小的item
#define     IQY_LEN_CAST_ITEM2           65     //cast结构体中，基本单位中，长度较大的item
#define     IQY_LEN_ALBUM_INFO_ITEM1     40     //albumInfo结构体中，基本单位中，长度较小的item
#define     IQY_LEN_ALBUM_INFO_ITEM2     65     //albumInfo结构体中，基本单位中，长度普通的item
#define     IQY_LEN_ALBUM_INFO_ITEM3     129    //albumInfo结构体中，基本单位中，长度较大的item
#define     IQY_LEN_EPISODE_ITEM1        33     //episode结构体中，基本单位中，长度较小的item
#define     IQY_LEN_EPISODE_ITEM2        65     //episode结构体中，基本单位中，长度普通的item
#define     IQY_LEN_EPISODE_ITEM3        129    //episode结构体中，基本单位中，长度较大的item
#define     IQY_LEN_SEARCHWORD_ITEM1     33     //SearchWord结构体中，基本单位中，长度较小的item
#define     IQY_LEN_SEARCHWORD_ITEM2     65     //SearchWord结构体中，基本单位中，长度普通的item
#define     IQY_LEN_CHANNEL_LABEL_ITEM1  33     //ChannelLabelInfo结构体中，基本单位中，长度较小的item
#define     IQY_LEN_CHANNEL_LABEL_ITEM2  65     //channelLabelInfo结构体中，基本单位中，长度普通的item
#define     IQY_LEN_RESULOTION_ITEM      33     //resulotion len
#define     IQY_LEN_TIME                 17     //system time len
#define     IQY_LEN_COMPILE_TIME         33     //SDK compile time len



/// /////////////////////////////////////////////////////////////
///                         struct
/// /////////////////////////////////////////////////////////////
typedef struct st_IQY_SubTag_C
{
    char    name[IQY_LEN_CHANNEL_ITEM1];        //tag name
    char    value[IQY_LEN_CHANNEL_ITEM1];       //tag value
} IQY_SubTag;

typedef struct st_IQY_Tag_C
{
    char    name[IQY_LEN_CHANNEL_ITEM0];        //tag name
    int     subTagSize;                         //size
    IQY_SubTag   subtag[IQY_SUBTAG_MAX];        //tag value
} IQY_Tag;

typedef struct st_IQY_Channel_C
{
    char    backImg[IQY_LEN_URL];               //背景图backImage
    int     channelId;                          //频道id
    char    name[IQY_LEN_CHANNEL_ITEM0];        //频道名称
    int     spec;                               //图片规格,picSpec1是横图，2是竖图，3是横竖混排，4是其它
    char    recTag[IQY_LEN_CHANNEL_ITEM1];      //rec tag id
    char    focus[IQY_LEN_CHANNEL_ITEM1];       //recommand id
    IQY_Tag tags[IQY_TAG_MAX];                  //tags
    int     tagSize;                            //tag size
} IQY_Channel;

typedef struct st_IQY_ChannelList_C
{
    int     total;                              //all频道总数
    int     size;                               //取得的频道总数
    IQY_Channel data[IQY_CHANNEL_MAX];          //频道基本信息数组
    int     resultCode;                         //执行结果返回 0：正常；>0:异常；错误code另行定义
} IQY_ChannelList;

typedef struct st_IQY_Cast_C
{
    char    composer[IQY_LEN_CAST_ITEM1];       //作曲
    char    host[IQY_LEN_CAST_ITEM2];           //主持人 例："何炅,吴昕,李维嘉,杜海涛,谢娜"
    char    dubber[IQY_LEN_CAST_ITEM1];         //配音
    char    maker[IQY_LEN_CAST_ITEM1];          //制片人
    char    star[IQY_LEN_CAST_ITEM2];           //明星 例："筷子兄弟,罗志祥,毛俊杰"
    char    producer[IQY_LEN_CAST_ITEM1];       //出品人
    char    songWriter[IQY_LEN_CAST_ITEM1];     //作词
    char    guest[IQY_LEN_CAST_ITEM2];          //嘉宾 例："筷子兄弟,罗志祥,毛俊杰"
    char    mainActor[IQY_LEN_CAST_ITEM2];      //主演/演唱者/主持人/配音 例："何炅,吴昕,李维嘉,杜海涛,谢娜"
    char    actor[IQY_LEN_CAST_ITEM2];          //演员/MV演员/嘉宾/配音角色 例："筷子兄弟,罗志祥,毛俊杰"
    char    writer[IQY_LEN_CAST_ITEM1];         //编剧
    char    director[IQY_LEN_CAST_ITEM1];       //导演
} IQY_Cast;

typedef struct st_IQY_AlbumInfo_C
{
    char    qpId[IQY_LEN_QPID];                 //专辑奇谱（视频/专辑/来源）ID
    char    tvQid[IQY_LEN_QPID];                //剧集奇谱ID
    char    name[IQY_LEN_ALBUM_INFO_ITEM2];     //专辑（视频/专辑/来源）名称 例："同桌的你"
    int     type;                               //专辑类型（0：视频，1：专辑）调用此接口传进来的那个qpId的数据类型，不是返回结果中的qpId的数据类型
    char    pic[IQY_LEN_URL];                   //专辑图 例："http://pic6.qiyipic.com/image/20140808/ce/b7/f1/v_106541155_m_600_m1.jpg"
    char    tvPic[IQY_LEN_URL];                 //海报图 例："http://pic6.qiyipic.com/image/20140808/ce/b7/f1/v_106541155_m_600_m1.jpg"
    float   score;
    char    desc[IQY_LEN_MAX];                  //专辑描述 例："1993年的夏天"
    char    key[IQY_LEN_ALBUM_INFO_ITEM1];      //专辑关键字
    int     isSeries;                           //是否多集；0为单集 1为多集
    char    chnName[IQY_LEN_ALBUM_INFO_ITEM1];  //频道名称 例："电影"
    int     chnId;                              //频道ID 例：1
    char    tag[IQY_LEN_ALBUM_INFO_ITEM2];      //标签列表 例："华语,爱情,剧情,国语”
    IQY_Cast cast;                              //cast信息
    int     exclusive;                          //是否独播，1为是;0为否
    char    sourceCode[IQY_LEN_ALBUM_INFO_ITEM2];   //来源code
    int     is3D;                               //是否3D,1为是;0为否
    int     tvsets;                             //总集数
    int     tvCount;                            //（专辑/来源）总视频数或（视频）当前集数
    char    vid[IQY_LEN_VID];                   //播放码流id
    long    len;                                //播放时长 例：5905
    char    initIssueTime[IQY_LEN_ALBUM_INFO_ITEM1];    //奇艺首次上线时间 例："2014-11-12 14:58:56"
    //pic url
    char    pic360x180[IQY_LEN_URL];            //360x180 pic url
    char    pic260x360[IQY_LEN_URL];            //260x360 pic url
    char    pic195x260[IQY_LEN_URL];            //195x360 pic url
} IQY_AlbumInfo;

typedef struct st_IQY_AlbumList_C
{
    int     total;                              //all专辑总数
    int     size;                               //取得的专辑总数
    IQY_AlbumInfo data[IQY_ALBUM_LIST_MAX];     //专辑基本信息数组
    int     resultCode;                         //执行结果返回 0：正常；>0:异常；错误code另行定义
} IQY_AlbumList;

typedef struct st_IQY_Episode_C
{
    char    tvQid[IQY_LEN_QPID];                //剧集ID 例：1181747
    char    vid[IQY_LEN_VID];                   //播放码流id
    int     type;                               //类型 0,预告片；1，正片
    char    name[IQY_LEN_EPISODE_ITEM2];        //剧集名称 例："爱情公寓4第1集"
    char    albumName[IQY_LEN_EPISODE_ITEM1];   //所属专辑名称
    char    year[IQY_LEN_EPISODE_ITEM1];        //综艺作为第几期使用 例：20130101
    long    len;                                //时长
    int     order;                              //播放次序，第几集如果是来源专辑下的剧集order
    char    pic[IQY_LEN_URL];                   //剧集图片 例："http://pic4.qiyipic.com/image/20140116/v_104637386_m_601_m1.jpg"
    IQY_Cast cast;                              //cast信息
    char    desc[IQY_LEN_MAX];                  //描述
} IQY_Episode;

typedef struct st_IQY_EpisodeList_C
{
    int     total;                              //all专辑总数
    int     size;                               //取得的专辑总数
    IQY_Episode data[IQY_EPISODE_MAX];          //专辑基本信息数组
    int     resultCode;                         //执行结果返回 0：正常；>0:异常；错误code另行定义
} IQY_EpisodeList;

typedef struct st_IQY_Word_C
{
    char word[IQY_LEN_SEARCHWORD_ITEM1];        //word
} IQY_Word;

typedef struct st_IQY_SearchWord_C
{
	char site[IQY_LEN_SEARCHWORD_ITEM1];        //
	char eventId[IQY_LEN_SEARCHWORD_ITEM2];     //
	IQY_Word wordList[IQY_SEARCHWORD_MAX];      //search word's list
	int wordSize;                               //wordlist's size
} IQY_SearchWord;

typedef struct st_IQY_ChannelLabelInfo_C
{
    char name[IQY_LEN_CHANNEL_LABEL_ITEM1];
    char plid[IQY_LEN_CHANNEL_LABEL_ITEM1];
} IQY_ChannelLabelInfo;

typedef struct st_IQY_ChannelLabelList_C
{
    int size;
    IQY_ChannelLabelInfo labelList[IQY_CHANNEL_LABEL_MAX];
} IQY_ChannelLabelList;

typedef struct st_IQY_PlayList_C
{
    int     total;                              //all专辑总数
    int     size;                               //取得的专辑总数
    IQY_AlbumInfo data[IQY_PLAY_LIST_MAX];      //专辑基本信息数组
    int     resultCode;                         //执行结果返回 0：正常；>0:异常；错误code另行定义
} IQY_PlayList;

///callback
typedef void (*error_callback) (int errorCode);
typedef void (*playstate_callback) (int nCase, int param);

typedef struct st_IQY_CollectingInfo_C
{
    char        qpid[IQY_LEN_QPID];                     //qpid
    char        plid[IQY_LEN_CHANNEL_LABEL_ITEM1];      //从播单播放时候，传进来
    int         chnId;
    char        resulotion[IQY_LEN_RESULOTION_ITEM];    //设备分辨率 1920x1080
    int         tvepin;                                 //剧集集数，在剧集列表点击的集数
    int         vvfrom;                                 //0:非连播   2:专辑内连播   3:专辑外连播  (注意)
} IQY_CollectingInfo;

typedef struct st_IQY_PlaySetup_C
{
    char                tvQid[IQY_LEN_QPID];    //剧集ID 例：1181747
    char                vid[IQY_LEN_VID];       //播放码流id
    int                 vd;
    int                 startTime;              //start play time
    IQY_CollectingInfo  info;
    error_callback      error;
    playstate_callback  playstate;
} IQY_PlaySetup;

typedef struct st_IQY_PlayerInit_C
{
    void*               cfg;
    long                size;
} IQY_PlayerInit;

typedef struct st_IQY_Init_C
{
    int                 mode;                   //mode
    int                 firstPwOn;              //1:第一次开机   0:不是第一次开机
    IQY_PlayerInit      playerInit;
} IQY_Init;

typedef struct st_IQY_CurrentPlayingVideoInfo_C
{
    char    vid[IQY_LEN_VID];           //播放码流id
    int     vd;                         //当前播放码流版本，1:流畅，2:高清, 3:超清，4:720P, 5:1080P
    int     duration;                   //视频时长
    int     cid;                        //channel id
    int     currentPos;
    int     videoWidth;
    int     videoHeight;
    int     leftVolume;
    int     rightVolume;
    int     vdlist[IQY_VD_MAX];         //vd list
    int     size;
    long    adDurationList[IQY_AD_DURATION_MAX]; //返回的是每个ad的时长
    int     adDurationListSize;
} IQY_CurrentPlayingVideoInfo;

typedef struct st_IQY_SystemTime_C
{
    char    time[IQY_LEN_TIME];
} IQY_SystemTime;


typedef struct st_IQY_VdList_C
{
    int     vdlist[IQY_VD_MAX];         //vd list
    int     size;
} IQY_VdList;

typedef struct st_IQY_SDK_Version_C
{
    char    compileTime[IQY_LEN_COMPILE_TIME];
} IQY_SDK_Version;


/// /////////////////////////////////////////////////////////////
///                         enum
/// /////////////////////////////////////////////////////////////
enum en_IdType_C
{
    TYPE_PLAYLIST            = 0,
    TYPE_RECOMMAND,
    TYPE_MAX
};

enum en_vvfrom_C
{
    TYPE_VVFROM_CLICK        = 0,          //非连播
//    TYPE_VVFROM_CONT_REC,                //
    TYPE_VVFROM_CONT_EPISODE = 2,          //连播
    TYPE_VVFROM_CONT_ALBUM,                //专辑外连播
    TYPE_VVFROM_MAX
};

enum en_CURRENT_STATUS_C
{
    TYPE_STATUS_NORMAL       = 0,        //普通状态
    TYPE_STATUS_ADVERTISING,             //正在播放广告
    TYPE_STATUS_VIDEO,                   //正在播放正片
    TYPE_STATUS_MAX
};

enum en_vdType_C
{
    TYPE_VD_300              = 1,        //1: 流畅
    TYPE_VD_600 ,                        //2: 高清
    TYPE_VD_1000,                        //3: 超清
    TYPE_VD_720P,                        //4: 720p
    TYPE_VD_1080P,                       //5: 1080p
    TYPE_VD_FAST_DRM,                    //6: 极速DRM
    TYPE_VD_AUTO_DRM,                    //7: 流畅DRM
    TYPE_VD_HD_DRM,                      //8: 高清DRM
    TYPE_VD_SUPER_HD_DRM,                //9: 超清DRM
    TYPE_VD_4K,                          //10: 4K
    TYPE_VD_5M,                          //11: 5M
    TYPE_VD_8M,                          //12: 8M
    TYPE_VD_1000_DOLBY,                  //13: 1000杜比
    TYPE_VD_720P_DOLBY,                  //14: 720p杜比
    TYPE_VD_1080P_DOLBY,                 //15: 1080p杜比
    TYPE_VD_4K_DOLBY,                    //16: 4k杜比
    TYPE_VD_720P_265,                    //17: 720P_265
    TYPE_VD_1080P_265,                   //18: 1080P_265
    TYPE_VD_4K_265,                      //19: 4K_265
    TYPE_VD_600_DOLBY,                   //20: 600_dolby
    TYPE_VD_FAST            = 96,        //96: 极速
    TYPE_VD_MAX
};



/// /////////////////////////////////////////////////////////////
///                        interface api
/// /////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int iqy_libInit(IQY_Init* pInit);
int iqy_libUninit(void);

///epg
int iqy_getChannelList(const char* versionId, int pageNo, int pageSize, IQY_ChannelList* pChnlList);
int iqy_getAlbumInfo(char* qpId, IQY_AlbumInfo* pAlbumInfo);
int iqy_getAlbumList(int chnId, int fromChnId, const char* tagValues, int pageNo, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getAlbumVideo(char* qpId, const char* sourceCode, int videoType, int pageNo, int pageSize, IQY_EpisodeList* pEpisodeLst);
int iqy_getRealTimeSearchWord(const char* keyWord, IQY_SearchWord* pSearchWord);
int iqy_albumSearch(const char* keyword, int chnId, int pageNo, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getChannelLabelList(int isFree, const char* chnRecTagId, IQY_ChannelLabelList* pLabelList);
int iqy_getPlayListById(int isFree, const char* id, int idType, IQY_PlayList* pPlayList);
int iqy_getRelatedRecommendAlbumList(int isFree, int chnId, const char* qpid, const char* tvQid, int pageSize, IQY_AlbumList* pAlbumList);
int iqy_getVdList(char* tvQid, char* vid, IQY_VdList* pVdlist);

///player
int iqy_start(const IQY_PlaySetup* pPlaySetup);
int iqy_pause();
int iqy_resume();
int iqy_stop();
int iqy_seek(int jumpTime);
int iqy_getCurrentPlayingVideoInfo( IQY_CurrentPlayingVideoInfo* pCurrentPlayingVideoInfo );
int iqy_switchVd(int vd, int currentTime);
int iqy_getCurrentStatus();
int iqy_releasePlayer();

///time
int iqy_getSystemTime(IQY_SystemTime* pTime);

///compile time
int iqy_getSDKVersion(IQY_SDK_Version* pSdkVersion);

#ifdef __cplusplus
}
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //IQIYITVCWRAPPER_H_INCLUDED
