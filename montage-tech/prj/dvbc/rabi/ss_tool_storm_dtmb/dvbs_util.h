#ifndef __DVBS_UTIL_H_
#define __DVBS_UTIL_H_


/*!
  Satellite receiving parameter
  */
typedef struct
{
  /*!
    lnb high frequency
    */
  u32 lnb_high            : 16;
  /*!
    lnb low frequency
    */
  u32 lnb_low             : 16;
  /*! 
    LNB power option, 0: 13/18v 1: 13v 2: 18v 
    */
  u32 lnb_power           : 2;
  /*! 
    LNB types, 0: Standard, 1: user, 2: universal 
    */
  u32 lnb_type            : 2;
  /*!
    0/12v option on/off, 0: off, 1: on
    */
  u32 v12                 : 1;
  /*!
    22k on off state
    */
  u32 k22                 : 1;
 /*!
    user band for unicable
   */
  u32 user_band         :4;
 /*!
    the center freq for unicable
  */
  u32 band_freq           :12;
 /*! 
    unicable type. 0: unicable A; 1: unicable B 
  */
  u32 unicable_type      : 1;
  /*!
    Reserved
    */
  u32 reserve             : 9;
}sat_rcv_para_t;

/*!
  TP receiving parameter
  */
typedef struct
{
  /*! 
    Symbol rate of TP 
    */
  u32 sym;
  /*! 
    Frequency of TP 
    */
  u32 freq;
  /*! 
    Polarity types, 0: horizontal, 1: vertical 
    */
  u32 polarity    : 2;
  /*! 
    Flag on 22k or not at cur tp, 0: 22k off, 1: 22k on 
    */
  u32 is_on22k    : 1;
  /*!
    Reserved
    */
 u32 nim_modulate      : 4;
  /*!
    NIM type, see enum nim_type_t
    */
  u32 nim_type :4;
  /*!
    Reserved
    */
 u32 reserve      : 21;
}tp_rcv_para_t;

/*!
  DVBS lock info
  */
typedef struct
{
  /*!
    Nim ctrl disepc data
    */
  nc_diseqc_info_t disepc_rcv;
  /*!
    Satellite data
    */
  sat_rcv_para_t sat_rcv;
  /*!
    TP data
    */
  tp_rcv_para_t tp_rcv;
} dvbs_lock_info_t;

/*!
  DVBS program info
  */
typedef struct
{
  /*!
    Service ID(DVB concept, program number)
    */
  u16 s_id;
  /*!
    TV mode
    */
  u16 tv_mode;
  /*!
    Video pid
    */
  u16 v_pid;
  /*!
    Audio pid
    */
  u16 a_pid;
  /*!
    PCR pid
    */
  u16 pcr_pid;
  /*!
    PMT pid
    */
  u16 pmt_pid;
  /*!
    Audio track
    */
  u16 audio_track;
  /*!
    Audio volumn;
    */
  u16 audio_volume;
  /*!
    Audio type
    */
  u16 audio_type;
  /*!
    scrambled?
    */
  BOOL is_scrambled;
  /*!
    Video type
    */
  u8 video_type;
  /*!
    reserved
    */
  u16 reserved;
  /*!
    Aspect ratio
    */
  u32 aspect_ratio;
  /*!
    context field 1
    */
  u32 context1;
  /*!
    context field 2
    */
  u32 context2;  
}dvbs_program_t;


#endif // End for __DVBS_UTIL_H_
