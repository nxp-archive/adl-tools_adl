// Software Read Register
void boot(void) __attribute__ ((noreturn)) ;
void boot(void)

{
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;

int  unsigned long dummy_flash_read;
int  unsigned short   delay_time;

// note not all global defines access during boot compile
#define CTIMODE_B       M0_CONTROL_REGS.m0_control_reg5.f.ctimode
#define READ_IDLE       0x38
#define COREHOLD_F  	M0_CONTROL_REGS.m0_control_reg0.f.corehld

#define FW_BASE_ADDRESS     0x08000000
#define FW_START            (*(volatile uint32_t *)(FW_BASE_ADDRESS))

#define FW_PRESENT_IFR 0x01200202;
#define  BME_AND_OFF    0x04000000
#define  BME_OR_OFF     0x08000000
#define  BME_XOR_OFF    0x0C000000


// **** hard delay 2us ****
for (delay_time = 1;delay_time <= 10;delay_time++)
{
  __asm__ ("nop");
}

//CORE_ALL_CORE_CFG0 = 0x00000000; // write all cfg bits to zero after recover state past
CORE_ALL_CORE_TST0_W = 0x00000000;   // Clear CGS, SEGS, YZS, SGS, etc.
CORE_ALL_CORE_TST4_W = 0x00000000;   // Clear CGPS and any other CORE TST bits
EVENT_CTL = READ_RDY_EN_M | TIMER_EN_M;
STATE = READ_IDLE; // read state
__asm__ ("WFE");   // wait for ftf_read_ready

//   did we time out with no read_ready
if (TIMER0_W == 0x0000) CCOBD0_W = 0xEEEE;


MFBADR0_W = 0x00000000; // RESET MFBADR register is not reset in RTL
TIMER0_W = 0x00000000;  // reset timer

if (CTIMODE_B) CLK_CTL = 0x00;       // select ipg_clk for bistmode

// agu code code check non calibrated
// **********************************

// enable agu event when sleep is activated will force agu trigger
EVENT_CTL = AGU_EN_M;    // enable AGU event
__asm__ ("SEV");
__asm__ ("WFE");
__asm__ ("WFE");

// get fail count if more than three bits exit
if (FCNT0_W > 3) goto boot_exit;


__asm__  (" MOV   R8,R0");   // preserve lower registers in high reg space
__asm__  (" MOV   R9,R1");   // since ram is not initiated
__asm__  (" MOV   R10,R2");
__asm__  (" MOV   R11,R3");
__asm__  (" MOV   R12,R4");
// write analog registers with cal data
__asm__  (" LDR   R0,=0x09001020");    // start of analog config data in IFRN
__asm__  (" LDMIA R0!,{R3,R4,R5,R6} ");

__asm__  (" LDR   R0,=0x4002A10F");    // Define clk_ctl
__asm__  (" MOVS  R1,#1");
__asm__  (" STRB  R1,[R0,#0]");        // select ipg_clk

__asm__  (" LDR   R1,=0x4002B000");    // analog register base address
__asm__  (" STMIA R1!,{R3-r6} ");

__asm__  (" LDR   R0,=0x4002A10F");    // Define clk_ctl
__asm__  (" MOVS  R1,#17");
__asm__  (" STRB  R1,[R0,#0]");        // select osc

// write core0 registers with cal data
__asm__  (" LDR   R0,=0x09001040");    // start of core0 config data in IFRN
__asm__  (" LDR   R1,=0x4002B500");    // core0 register base address
__asm__  (" LDMIA R0!,{R3,R4,R5,R6} ");
__asm__  (" STMIA R1!,{R3-r6} ");

__asm__  (" LDR   R0,=0x4002A10D");    // Set up address for rd_ctl register
__asm__  (" MOVS  R1,#2");
__asm__  (" STRB  R1,[R0,#0]");        // write read ctl register to 1 = one wait states

__asm__  (" LDR   R0,=0x09001050");    // start of core1 config data in IFRN
// write core1 registers with cal data
__asm__  (" LDR   R1,=0x4002B600");    // core 1 register base address
__asm__  (" LDMIA R0!,{R3,R4,R5,R6} ");
__asm__  (" STMIA R1!,{R3-r6} ");
// write core2 registers with cal data
__asm__  (" LDR   R1,=0x4002B700");    // core 2 register base address
__asm__  (" LDMIA R0!,{R3,R4,R5,R6} ");
__asm__  (" STMIA R1!,{R3-r6} ");


__asm__  (" MOV   R0,R8");
__asm__  (" MOV   R1,R9");
__asm__  (" MOV   R2,R10");
__asm__  (" MOV   R3,R11");
__asm__  (" MOV   R4,R12");


AGUSTART0_W = FW_PRESENT_IFR;
AGUBOUND0_W = FW_PRESENT_IFR;


FCNT0_W = 0x00000000; // clear fail count
EVENT_CTL = 0x00;     // clear ready for re-trigger
__asm__ ("SEV");
__asm__ ("WFE");
EVENT_CTL = AGU_EN_M; // enable AGU event
__asm__ ("WFE");      // wait for agu to complete

// Clear IFRSEL
MMISC_H = 0;

// get fail count if more than three bits exit
if (FCNT0_W > 3) goto boot_exit;


EVENT_CTL = CCIF_EN_M;
//    do a send event followed by wait for event
//    flushes out any registered / latched event
__asm__  ("SEV");
__asm__  ("WFE");

*((volatile uint32_t *) (0xE000ED08)) = 0x1C000000; // vector base to boot space
*((volatile uint32_t *) (0xE000E100)) = 0xC0;       // enable SWWRREG/SWRDREG interrupts
__asm__  ("  LDR     R0,=0x08000001");
__asm__  ("  BLX     R0");


boot_exit:
// Clear IFRSEL
MCTL_H  =  0x70; // clr mbsel
MMISC_H = 0;

NO_FW_LOOP:

// Switch to ipg_clk before clearing core hold
// Retain reset value for eccen=CLK_CTL[0]
CLK_CTL  = 0x01; // ctl_osc_h=CLK_CTL[4], 0=ipg_clk, 1=osc_clk
COREHOLD_F = 0; //clear core hold
volatile uint32_t bme_or_pointer_s_0;
bme_or_pointer_s_0 = (uint32_t)(&FSTAT) + BME_OR_OFF;
*(volatile unsigned short *)(bme_or_pointer_s_0) = CCIF_M;

//FSTAT = CCIF_M;
EVENT_CTL = CCIF_EN_M;
*((volatile uint32_t *) (0xE000ED08)) = 0x1C000000; // vector base to boot space
*((volatile uint32_t *) (0xE000E100)) = 0x1C0;      // enable test_int/SWWRREG/SWRDREG interrupts
__asm__ ("SEV");
__asm__ ("WFE");
__asm__ ("WFE"); // wait for CCIF go low
// if command not a valid non sw/rd wr reg or test command we have
//if (~((CMD_CCOB == 0x82) || (CMD_CCOB == 0x83) ||
//       ((CMD_CCOB & (uint8_t)0xF0) == 0x20) || ((CMD_CCOB & (uint8_t)0xF0) == 0x30)))
//{
//FSTAT = ACCERR_M;
//}

goto NO_FW_LOOP;

}

