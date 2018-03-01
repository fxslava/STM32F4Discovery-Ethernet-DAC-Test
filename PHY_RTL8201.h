/* Basic Registers */
#define REG_CNTRL           0x00        /* Control Register                  */
#define REG_STATS           0x01        /* Status Register                   */
#define REG_PHYID1          0x02        /* PHY Identifier register Hi        */
#define REG_PHYID2          0x03        /* PHY Identifier register Lo        */
#define REG_LDADV           0x04        /* Auto-Negotiation Advertisement    */
#define REG_LPADV           0x05        /* Auto-Neg. Link Partner Abitily    */
#define REG_ANEGX           0x06        /* Auto-Neg. Expansion register      */

/* Extended Registers */
#define REG_NSR							0x10				/* NWay Setup Register               */
#define REG_LBREMR					0x11				/* Loopback, Bypass, Receiver Error 
																					 Mask Register                     */
#define REG_REC							0x12				/* RX_ER Counter                     */
#define REG_SNRDISP					0x13				/* SNR Display Register              */
#define REG_TEST						0x19				/* Test Register                     */

/* Control Register */
#define CNTRL_RESET         0x8000      /* Software Reset                    */
#define CNTRL_LOOPBACK      0x4000      /* Loopback mode                     */
#define CNTRL_SPEED_SEL     0x2000      /* Speed Select (1=100Mb/s)          */
#define CNTRL_ANEG_EN       0x1000      /* Auto Negotiation Enable           */
#define CNTRL_POWER_DOWN    0x0800      /* Power Down                        */
#define CNTRL_REST_ANEG     0x0200      /* Restart Auto Negotiation          */
#define CNTRL_DUPLEX        0x0100      /* Duplex Mode (1=Full duplex)       */

/* Status Register */
#define STATS_100B_T4       0x8000      /* 100BASE-T4 Capable                */
#define STATS_100B_TX_FD    0x4000      /* 100BASE-TX Full Duplex Capable    */
#define STATS_100B_TX_HD    0x2000      /* 100BASE-TX Half Duplex Capable    */
#define STATS_10B_T_FD      0x1000      /* 10BASE-T Full Duplex Capable      */
#define STATS_10B_T_HD      0x0800      /* 10BASE-T Half Duplex Capable      */
#define STATS_MF_PRE_SUP    0x0040      /* Preamble suppression Capable      */
#define STATS_ANEG_COMPL    0x0020      /* Auto Negotiation Complete         */
#define STATS_REM_FAULT     0x0010      /* Remote Fault                      */
#define STATS_ANEG_ABIL     0x0008      /* Auto Negotiation Ability          */
#define STATS_LINK_STAT     0x0004      /* Link Status (1=established)       */
#define STATS_JABBER_DET    0x0002      /* Jaber Detect                      */
#define STATS_EXT_CAPAB     0x0001      /* Extended Capability               */

/* PHY Identifier Registers */
#define PHY_ID1             0x0000      /* ST802RT1 Device Identifier MSB    */
#define PHY_ID2             0x8201      /* ST802RT1 Device Identifier LSB    */

/* NWay Setup Register (REG_NSR)*/
#define NSR_FLAGLSC					0x0001			/* ANE experienced link status state */
#define NSR_FLAGPDF					0x0002			// ANE experienced parallel detection fault state
#define NSR_FLAGABD					0x0004			// ANE experienced ability detect state
#define NSR_NWLPBK					0x0200			// Set NWay to lookup mode
#define NSR_TESTFUN					0x0400			// ANE speeds up internal timer
#define NSR_ENNWLE					0x0800			// LED4 Pin indicates linkpulse

/*Loopback, Bypass, Receiver Error Mask Register (REG_LBREMR) */
#define NSR_RMIIMODE				0x0001			/* RMII mode is Enabled              */
#define NSR_FXMODE					0x0002			/* Fiber Mode mode is Enabled        */
#define NSR_PKT_err					0x0004			// detection of packet error due to 722 ms time-out
#define NSR_LINK_err				0x0008			/* Link error                        */
#define NSR_PME_err					0x0010			/* Pre-mature error                  */
#define NSR_CODE_err				0x0020			/* Code error                        */
#define NSR_JBEN						0x0040			// Enable Jabber Function in 10Base-T
#define NSR_F_Link_100			0x0080			// 100Mbps for diagnostic
#define NSR_F_Link_10				0x0100			// 10Mbps for diagnostic
#define NSR_LB							0x0200			/* Enable DSP loopback               */
#define NSR_AnalogOFF				0x0800			/* Power down analog RX/TX           */
#define NSR_LDPS						0x1000			/* Enable Link Down Power Saving mode*/
#define NSR_SCR							0x2000			// Allows bypassing of the scrambler/descrambler
#define NSR_4B5B						0x4000			// Allows bypassing of the 4B/5B & 5B/4B encoder
#define NSR_RPTR						0x8000			/* Repeater mode                     */

/* Test Register */
#define TEST_LINK100				0x0001			// 
#define TEST_LINK10					0x0002			// 
#define TEST_PHYAD0					0x0080			// 

