/**************************************************************************
*                                                                         *
*   PROJECT     : MIPS port for uC/OS-II                                  *
*                                                                         *
*   MODULE      : link.xn                                                 *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : MIPS 4Kc (32 bit RISC) - ATLAS board                    *
*                                                                         *
*   TOOL-CHAIN  : SDE & Cygnus                                            *
*                                                                         *
*   DESCRIPTION :                                                         *
*   Linker script file                                                    *
*                                                                         *
**************************************************************************/

OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)

/**** Start point ****/
ENTRY(_start)			/* Entry point of application		*/

MEMORY
{
    /* ram (wx) : ORIGIN = 0x80000000, LENGTH = 64M */
    ram (wx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  /**** Code and read-only data ****/

  /* .text 0x80008000 : */
  .text 0x80008000 :  
  {
    _ftext = ABSOLUTE(.) ;	/* Start of code and read-only data	*/
    *(.init) 		/* Entry point				*/
	*(.text)
    _ecode = ABSOLUTE(.) ;	/* End of code				*/
    *(.rodata)
    . = ALIGN(8);
    _etext = ABSOLUTE(.);	/* End of code and read-only data	*/
  } > ram

  /**** Initialised data ****/
  .gcc_except_table ALIGN (0x4) : { . = .; KEEP(*(.gcc_except_table)) *(.gcc_except_table.*) } > ram
  .ctors ALIGN (0x4) : { . = .; KEEP (*crtbegin.o(.ctors)) __CTOR_LIST__ = .; PROVIDE (__CTOR_LIST__ = .); KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors)) KEEP (*(SORT(.ctors.*))) KEEP (*(.ctors)) __CTOR_END__ = .; PROVIDE (__CTOR_END__ = .); } > ram
  .dtors ALIGN (0x4) : { . = .; KEEP (*crtbegin.o(.dtors)) __DTOR_LIST__ = .; PROVIDE (__DTOR_LIST__ = .); KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors)) KEEP (*(SORT(.dtors.*))) KEEP (*(.dtors)) __DTOR_END__ = .; PROVIDE (__DTOR_END__ = .); } > ram

  .data :
  {
    _fdata = ABSOLUTE(.);	/* Start of initialised data		*/
    *(.data)
    *(.data_section) 
    *(.data._ZN*) 
    *(.gnu.linkonce.d*)
    SORT(CONSTRUCTORS)
    . = ALIGN(8);
    *(.rel.*)

    __gp = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/

    *(.lit8) 
    *(.lit4) 
    *(.rel.*)
    *(.sdata) 
    
    . = ALIGN(8);
    _edata  = ABSOLUTE(.);	/* End of initialised data		*/
  } > ram

  /**** Uninitialised data ****/
. = ALIGN(64);
  _fbss = .;			/* Start of uninitialised data		*/

  .sbss (NOLOAD): 
  { 
    . = . + 0x400;
    *(.sbss) 
    *(.sbss_section) 
    *(.scommon)
    *(.scommon_section) 
    .   =  ALIGN(8) ;    
  }
  .bss (NOLOAD):
  {
     . = . + 0x400;
     *(.av_bss) 
    . = . + 0x400;
     *(.av_bss_section) 
     
     . = ALIGN(32);
     . = . + 128;
     
    *(.bss)
    *(.bss_section)
    *(.bss.*)
    *(COMMON)
    *(.COMMON_section) 
    .   =  ALIGN(8) ;    

    _freemem = .;
    _ap_stack_base = .;
    . = . + 30000;
    . = . + 00000;		/*	qbq*/
    .   =  ALIGN(8) ;    
    _ap_stack_top = .;

    . = ALIGN(8);
  }

  _end = . ;			/* End of unitialised data		*/

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }

  /DISCARD/ :
  {
    *(.reginfo)
  }
 
  PROVIDE(etext = _etext);
  PROVIDE (edata = .);
  PROVIDE (end = .);
}


