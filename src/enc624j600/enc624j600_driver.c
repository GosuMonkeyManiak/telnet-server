/*
 *  The I/O interface is hardware selected on power-up using
 *  the SPISEL function on the INT/SPISEL pin.
 *      - latched at a logic high state (SPI)
 *      - latched at a logic low state (PSP)
 * 
 *  SPI
 *      - when selected it is recommended PSPCFG2 and PSPCFG3 pins
 *          to be tied to either Vss or any logic high voltage, and not
 *          be left floating.
 *      - there is an internal pull-up resistor on CS
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "enc624j600/enc624j600_driver_hal.h"
#include "enc624j600/enc624j600_driver.h"

/**
 *  @defgroup Unbanked_SFR_Address Unbanked SFR addresses
 *  @brief Unbanked special function register (SFR) addresses for the ENC624J600.
 * 
 *  @{
 */

#define ETXST       0x00U   /**< Transmit Data Start Pointer */
#define ETXLEN      0x02U   /**< Transmit Buffer Length Pointer */
#define ERXST       0x04U   /**< Receive Buffer Start Address */
#define ERXTAIL     0x06U   /**< Receive Tail Pointer */
#define ERXHEAD     0x08U   /**< Receive Head Pointer */
#define EDMAST      0x0AU   /**< DMA Start Address */
#define EDMALEN     0x0CU   /**< DMA Length */
#define EDMADST     0x0EU   /**< DMA Destination Address */
#define EDMACS      0x10U   /**< DMA Checksum */
#define ETXSTAT     0x12U   /**< Ethernet transmit status register */
#define ETXWIRE     0x14U   /**< Transmit Byte Count on Wire (including collision bytes) */
#define EUDAST      0x16U   /**< User-Defined Area Start Pointer */
#define EUDAND      0x18U   /**< User-Defined Area End Pointer */
#define ESTAT       0x1AU   /**< Ethernet Status Register */
#define EIR         0x1CU   /**< Ethernet Interrupt Flag Register */
#define ECON1       0x1EU   /**< Ethernet control register 1 */
#define EHT1        0x20U   /**< Hash Table Filter */
#define EHT2        0x22U   /**< Hash Table Filter */
#define EHT3        0x24U   /**< Hash Table Filter */
#define EHT4        0x26U   /**< Hash Table Filter */
#define EPMM1       0x28U   /**< Pattern Match Filter Mask */
#define EPMM2       0x2AU   /**< Pattern Match Filter Mask */
#define EPMM3       0x2CU   /**< Pattern Match Filter Mask */
#define EPMM4       0x2EU   /**< Pattern Match Filter Mask */
#define EPMCS       0x30U   /**< Pattern Match Filter Checksum */
#define EPMO        0x32U   /**< Pattern Match Filter Offset */
#define ERXFCON     0x34U   /**< Ethernet RX Filter Control Register */
#define MACON1      0x40U   /**< MAC Control Register 1 */
#define MACON2      0x42U   /**< MAC Control Register 2 */
#define MABBIPG     0x44U   /**< MAC Back-To-Back Inter-Packet Gap Register */
#define MAIPG       0x46U   /**< MAC Inter-Packet Gap Register */
#define MACLCON     0x48U   /**< MAC Colision Control Register */
#define MAMXFL      0x4AU   /**< MAC Maximum Frame Length */
#define MICMD       0x52U   /**< MII Management Command Register */
#define MIREGADR    0x54U   /**< MII Management Address Register */
#define MAADR3      0x60U   /**< MAC Address 6-5th byte */
#define MAADR2      0x62U   /**< MAC Address 4-3rd byte */
#define MAADR1      0x64U   /**< MAC Address 2-1st byte */
#define MIWR        0x66U   /**< MII Management Write Data */
#define MIRD        0x68U   /**< MII Management Read Data */
#define MISTAT      0x6AU   /**< MII Management Status Register */
#define EPAUS       0x6CU   /**< Pause Timer Value */
#define ECON2       0x6EU   /**< Ethernet Control Register 2 */
#define ERXWM       0x70U   /**< Receive Watermark */
#define EIE         0x72U   /**< Ethernet Interrupt Enable Register */
#define EIDLED      0x74U   /**< Ethernet ID Status/Led Control Register */

/** @} */


/**
 *  @defgroup PHY_SFR_Address Physical SFR addresses
 *  @brief PHY special function register (SFR) addresses for the ENC624J600.
 * 
 *  @{
 */

#define PHCON1      0x00U   /**< PHY Control Register 1 */
#define PHSTAT1     0x01U   /**< PHY Status Register 1 */
#define PHANA       0x04U   /**< PHY Auto-Negotiation Advertisement Register */
#define PHANLPA     0x05U   /**< PHY Auto-Negotiation Link Partner Ability Register */
#define PHANE       0x06U   /**< PHY Auto-Negotiation Expansion Register */
#define PHCON2      0x11U   /**< PHY Control Register 2 */
#define PHSTAT2     0x1BU   /**< PHY Status Register 2 */
#define PHSTAT3     0x1FU   /**< PHY Status Register 3 */

/** @} */


/**
 *  @defgroup SPI_Instruction_Set SPI Instruction Set
 *  @brief SPI instruction opcodes for the ENC624J600.
 * 
 *  @{
 */

/** @name Single-byte instructions */
/** @{ */
#define B0SEL       0xC0U   /**< Selects SFR Bank 0 */
#define B1SEL       0xC2U   /**< Selects SFR Bank 1 */
#define B2SEL       0xC4U   /**< Selects SFR Bank 2 */
#define B3SEL       0xC6U   /**< Selects SFR Bank 3 */
#define SETETHRST   0xCAU   /**< Issues System Reset by setting ETHRST */
#define FCDISABLE   0xE0U   /**< Disables flow control */
#define FCSINGLE    0xE2U   /**< Transmits a single pause frame */
#define FCMULTIPLE  0xE4U   /**< Enables flow control with periodic pause frames */
#define FCCLEAR     0xE6U   /**< Terminates flow control with a final pause frame */
#define SETPKTDEC   0xCCU   /**< Decrements PKTCNT by setting PKTDEC */
#define DMASTOP     0xD2U   /**< Stops current DMA operation by clearing DMAST */
#define DMACKSUM    0xD8U   /**< Starts DMA and checksum operation */
#define DMACKSUMS   0xDAU   /**< Starts DMA checksum operation with seed */
#define DMACOPY     0xDCU   /**< Starts DMA copy and checksum operation */
#define DMACOPYS    0xDEU   /**< Starts DMA copy and checksum operation with seed */
#define SETTXRTS    0xD4U   /**< Sets TXRTS, sends an Ethernet packet */
#define ENABLERX    0xE8U   /**< Enables packet reception by setting RXEN */
#define DISABLERX   0xEAU   /**< Disables packet reception by clearing RXEN */
#define SETEIE      0xECU   /**< Enable Ethernet Interrupts by setting INT */
#define CLREIE      0xEEU   /**< Disable Ethernet Interrupts by clearing INT */
/** @} */

/** @name Two-byte instructions */
/** @{ */
#define RBSEL       0xC8U   /**< Read Bank Select */
/** @} */

/** @name Three-byte instructions */
/** @{ */
#define WGPRDPT     0x60U   /**< Write General Purpose Buffer Read Pointer (EGPRDPT) */
#define RGPRDPT     0x62U   /**< Read General Purpose Buffer Read Pointer (EGPRDPT) */
#define WRXRDPT     0x64U   /**< Write Receive Buffer Read Pointer (ERXRDPT) */
#define RRXRDPT     0x66U   /**< Read Receive Buffer Read Pointer (ERXRDPT) */
#define WUDARDPT    0x68U   /**< Write User-Defined Area Read Pointer (EUDARDPT) */
#define RUDARDPT    0x6AU   /**< Read User-Defined Area Read Pointer (EUDARDPT) */
#define WGPWRPT     0x6CU   /**< Write General Purpose Buffer Write Pointer (EGPWRPT) */
#define RGPWRPT     0x6EU   /**< Read General Purpose Buffer Write Pointer (EGPWRPT) */
#define WRXWRPT     0x70U   /**< Write Receive Buffer Write Pointer (ERXWRPT) */
#define RRXWRPT     0x72U   /**< Read Receive Buffer Write Pointer (ERXWRPT) */
#define WUDAWRPT    0x74U   /**< Write User-Defined Area Write Pointer (EUDAWRPT) */
#define RUDAWRPT    0x76U   /**< Read User-Defined Area Write Pointer (EUDAWRPT) */
/** @} */

/** @name Unbanked SFR operations */
/** @{ */
#define RCRU        0x20U   /**< Read Control Register(s), Unbanked */
#define WCRU        0x22U   /**< Write Control Register(s), Unbanked */
#define BFSU        0x24U   /**< Bit Field(s) Set, Unbanked */
#define BFCU        0x26U   /**< Bit Field(s) Clear, Unbanked */
/** @} */

/** @name SRAM operations */
/** @{ */
#define RGPDATA     0x28U   /**< Read Data from EGPDATA */
#define WGPDATA     0x2AU   /**< Write Data from EGPDATA */
#define RRXDATA     0x2CU   /**< Read Data from ERXDATA */
#define WRXDATA     0x2EU   /**< Write Data from ERXDATA */
#define RUDADATA    0x30U   /**< Read Data from EUDADATA */
#define WUDADATA    0x32U   /**< Write Data from EUDADATA */
/** @} */

/** @} */


/**
 *  @defgroup SFR_BitMasks SFR Bit Masks
 *  @brief Bit masks for special function registers (SFRs).
 * 
 *  @{
 */

/* ETXSTAT */
#define COLCNT0     0x0001U     /**< Transmit Collision Count Status bit */
#define COLCNT1     0x0002U     /**< Transmit Collision Count Status bit */
#define COLCNT2     0x0004U     /**< Transmit Collision Count Status bit */
#define COLCNT3     0x0008U     /**< Transmit Collision Count Status bit */
#define CRCBAD      0x0010U     /**< Transmit CRC Incorrect Status bit */
#define DEFER       0x0080U     /**< Transmit Defer Status bit */
#define EXDEFER     0x0100U     /**< Transmit Excessive Defer Status bit */
#define MAXCOL      0x0200U     /**< Transmit Maximum Collisions Status bit */
#define LATECOL     0x0400U     /**< Transmit Late Collision Status bit */

/* ESTAT */
#define PKTCNT0     0x0001U     /**< Receive Packet Count bits */
#define PKTCNT1     0x0002U     /**< Receive Packet Count bits */
#define PKTCNT2     0x0004U     /**< Receive Packet Count bits */
#define PKTCNT3     0x0008U     /**< Receive Packet Count bits */
#define PKTCNT4     0x0010U     /**< Receive Packet Count bits */
#define PKTCNT5     0x0020U     /**< Receive Packet Count bits */
#define PKTCNT6     0x0040U     /**< Receive Packet Count bits */
#define PKTCNT7     0x0080U     /**< Receive Packet Count bits */
#define PHYLNK      0x0100U     /**< PHY Linked Status bit */
#define PHYDPX      0x0400U     /**< PHY Full Duplex Status bit */
#define CLKRDY      0x1000U     /**< Clock Ready Status bit */
#define RXBUSY      0x2000U     /**< Receive Logic Active Status bit */
#define FCIDLE      0x4000U     /**< Flow Control Idle Status bit */
#define INT         0x8000U     /**< Interrupt Pending Status bit */

/* EIR */
#define PCFULIF     0x0001U     /**< Packet Counter Full Interrupt Flag bit */
#define RXABTIF     0x0002U     /**< Receive Abort Interrupt Flag bit */
#define TXABTIF     0x0004U     /**< Transmit Abort Interrupt Flag bit */
#define TXIF        0x0008U     /**< Transmit Done Interrupt Flag bit */
#define DMAIF       0x0020U     /**< DMA Interrupt Flag bit */
#define PKTIF       0x0040U     /**< RX Packet Pending Interrupt Flag bit */
#define LINKIF      0x0800U     /**< PHY Link Status Change Interrupt Flag bit */
#define AESIF       0x1000U     /**< AES Encrypt/Decrypt Interrupt Flag bit */
#define HASHIF      0x2000U     /**< MD5/SHA-1 Hash Interrupt Flag bit */
#define MODEXIF     0x4000U     /**< Modular Exponentiation Interrupt Flag bit */
#define CRYPTEN     0x8000U     /**< Modular Exponentiation and AES Cryptographic Modules Enable bit */

/* ECON1 */
#define RXEN        0x0001U     /**< Receive Enable bit */
#define TXRTS       0x0002U     /**< Transmit Request to Send Status/Control bit */
#define DMANOCS     0x0004U     /**< DMA No Checksum Control bit */
#define DMACSSD     0x0008U     /**< DMA Checksum Seed Control bit */
#define DMACPY      0x0010U     /**< DMA Copy Control bit */
#define DMAST       0x0020U     /**< DMA Start bit */
#define FCOP0       0x0040U     /**< Flow Control Operation Control/Status bit */
#define FCOP1       0x0080U     /**< Flow Control Operation Control/Status bit */
#define PKTDEC      0x0100U     /**< RX Packet Counter Decrement Control bit */
#define AESOP0      0x0200U     /**< AES Operation Control bit */
#define AESOP1      0x0400U     /**< AES Operation Control bit */
#define AESST       0x0800U     /**< AES Encrypt/Decrypt Start bit */
#define HASHLST     0x1000U     /**< MD5/SHA-1 Hash Last Block Control bit */
#define HASHOP      0x2000U     /**< MD5/SHA-1 Hash Operation Control bit */
#define HASHEN      0x4000U     /**< MD5/SHA-1 Hash Enable bit */
#define MODEXST     0x8000U     /**< Modular Exponentiation Start bit */

/* ERXFCON */
#define BCEN        0x0001U     /**< Broadcast Collection Filter */
#define MCEN        0x0002U     /**< Multicast Collection Filter */
#define NOTMEEN     0x0004U     /**< Not-Me Unicast Collection Filter */
#define UCEN        0x0008U     /**< Unicast Collection Filter */
#define RUNTEN      0x0010U     /**< Runt Error Rejection Filter */
#define RUNTEEN     0x0020U     /**< Runt Error Collection Filter */
#define CRCEN       0x0040U     /**< CRC Error Rejection Filter */
#define CRCEEN      0x0080U     /**< CRC Error Collection Filter */
#define PMEN0       0x0100U     /**< Pattern Match Collection Filter */
#define PMEN1       0x0200U     /**< Pattern Match Collection Filter */
#define PMEN2       0x0400U     /**< Pattern Match Collection Filter */
#define PMEN3       0x0800U     /**< Pattern Match Collection Filter */
#define NOTPM       0x1000U     /**< Pattern Match Inversion Control */
#define MPEN        0x4000U     /**< Magic Packet Collection Filter */
#define HTEN        0x8000U     /**< Hast-Table Collection Filter */

/* MACON1 */
#define PASSALL     0x0002U     /**< Pass All Received Frames Enable bit */
#define RXPAUS      0x0004U     /**< Pause Control Frame Reception Enable bit */
#define LOOPBK      0x0010U     /**< MAC Loopback Enable bit */

/* MACON2 */
#define FULDPX      0x0001U     /**< MAC Full-Duplex Enable bit */
#define HFRMEN      0x0004U     /**< Huge Frame Enable bit */
#define PHDREN      0x0008U     /**< Proprietary Header Enable bit */
#define TXCRCEN     0x0010U     /**< Transmit CRC Enable bit */
#define PADCFG0     0x0020U     /**< Automatic Pad and CRC Configuration bit */
#define PADCFG1     0x0040U     /**< Automatic Pad and CRC Configuration bit */
#define PADCFG2     0x0080U     /**< Automatic Pad and CRC Configuration bit */
#define NOBKOFF     0x1000U     /**< No Backoff Enable bit (applies to half duplex only) */
#define BPEN        0x2000U     /**< No Backoff During Back Pressure Enable bit (applies to half duplex only) */
#define MACDEFER    0x4000U     /**< Defer Transmission Enable bit (applies to half duplex only) */

/* MACLCON */
#define MAXRET0     0x0001U     /**< Maximum Retransmissions Control bit (half duplex only) */
#define MAXRET1     0x0002U     /**< Maximum Retransmissions Control bit (half duplex only) */
#define MAXRET2     0x0004U     /**< Maximum Retransmissions Control bit (half duplex only) */
#define MAXRET3     0x0008U     /**< Maximum Retransmissions Control bit (half duplex only) */

/* MICMD */
#define MIIRD       0x0001U     /**< MII Read Enable bit */
#define MIISCAN     0x0002U     /**< MII Scan Enable bit */

/* MISTAT */
#define BUSY        0x0001U     /**< MII Management Busy Status bit */
#define SCAN        0x0002U     /**< MII Management Scan Status bit */
#define NVALID      0x0004U     /**< MII Management Read Data Not Valid Status bit */

/* ECON2 */
#define AESLEN0     0x0001U     /**< AES Key Length Control bit */
#define AESLEN1     0x0002U     /**< AES Key Length Control bit */
#define MODLEN0     0x0004U     /**< Modular Exponentiation Length Control bit */
#define MODLEN1     0x0008U     /**< Modular Exponentiation Length Control bit */
#define ETHRST      0x0010U     /**< Master Ethernet Reset bit */
#define RXRST       0x0020U     /**< Receive Logic Reset bit */
#define TXRST       0x0040U     /**< Transmit Logic Reset bit */
#define AUTOFC      0x0080U     /**< Automatic Flow Control Enable bit */
#define COCON0      0x0100U     /**< CLKOUT Frequency Control bit */
#define COCON1      0x0200U     /**< CLKOUT Frequency Control bit */
#define COCON2      0x0400U     /**< CLKOUT Frequency Control bit */
#define COCON3      0x0800U     /**< CLKOUT Frequency Control bit */
#define SHA1MD5     0x1000U     /**< SHA-1/MD5 Hash Control bit */
#define TXMAC       0x2000U     /**< Automatically Transmit MAC Address Enable bit */
#define STRCH       0x4000U     /**< LED Stretching Enable bit */
#define ETHEN       0x8000U     /**< Ethernet Enable bit */

/* EIE */
#define PCFULIE     0x0001U     /**< Packet Counter Full Interrupt Enable bit */
#define RXABTIE     0x0002U     /**< Receive Abort Interrupt Enable bit */
#define TXABTIE     0x0004U     /**< Transmit Abort Interrupt Enable bit */
#define TXIE        0x0008U     /**< Transmit Done Interrupt Enable bit */
#define DMAIE       0x0020U     /**< DMA Interrupt Enable bit */
#define PKTIE       0x0040U     /**< RX Packet Pending Interrupt Enable bit */
#define LINKIE      0x0800U     /**< PHY Link Status Change Interrupt Enable bit */
#define AESIE       0x1000U     /**< AES Encrypt/Decrypt Interrupt Enable bit */
#define HASHIE      0x2000U     /**< MD5/SHA-1 Hash Interrupt Enable bit */
#define MODEXIE     0x4000U     /**< Modular Exponentiation Interrupt Enable bit */
#define INTIE       0x8000U     /**< INT Global Interrupt Enable bit */

/* EIDLED */
#define REVID0      0x0001U     /**< Silicon Revision ID bit */
#define REVID1      0x0002U     /**< Silicon Revision ID bit */
#define REVID2      0x0004U     /**< Silicon Revision ID bit */
#define REVID3      0x0008U     /**< Silicon Revision ID bit */
#define REVID4      0x0010U     /**< Silicon Revision ID bit */
#define DEVIDO      0x0020U     /**< Device ID bit */
#define DEVID1      0x0040U     /**< Device ID bit */
#define DEVID2      0x0080U     /**< Device ID bit */
#define LBCFG0      0x0100U     /**< LEDB Configuration bit */
#define LBCFG1      0x0200U     /**< LEDB Configuration bit */
#define LBCFG2      0x0400U     /**< LEDB Configuration bit */
#define LBCFG3      0x0800U     /**< LEDB Configuration bit */
#define LACFG0      0x1000U     /**< LEDA Configuration bit */
#define LACFG1      0x2000U     /**< LEDA Configuration bit */
#define LACFG2      0x4000U     /**< LEDA Configuration bit */
#define LACFG3      0x8000U     /**< LEDA Configuration bit */

/* PHCON1 */
#define PFULDPX     0x0100U     /**< PHY Duplex (applicable only when auto-negotiation is disable) */
#define RENEG       0x0200U     /**< Restart Auto-Negotiation Control bit */
#define PSLEEP      0x0800U     /**< PHY Sleep Enable bit */
#define ANEN        0x1000U     /**< PHY Auto-Negotiation Eanble bit */
#define SPD100      0x2000U     /**< PHY Speed Select Control bit (applicable only when auto-negotiation is disable) */
#define PLOOPBK     0x4000U     /**< PHY Loopback Enable bit */
#define PRST        0x8000U     /**< PHY Reset bit */

/* PHSTAT1 */
#define EXTREGS     0x0001U     /**< Extended Capabilities Registers Present Status bit */
#define LLSTAT      0x0004U     /**< Latching Link Status bit */
#define ANABLE      0x0008U     /**< Auto-Negotiation Ability Status bit */
#define LRFAULT     0x0010U     /**< Latching Remote Fault Condition Status bit */
#define ANDONE      0x0020U     /**< Auto-Negotiation Done Status bit */
#define HALF10      0x0800U     /**< 10Base-T Half-Duplex Ability Status bit */
#define FULL10      0x1000U     /**< 10Base-T Full-Duplex Ability Status bit */
#define HALF100     0x2000U     /**< 100Base-TX Half-Duplex Ability Status bit */
#define FULL100     0x4000U     /**< 100Base-TX Full-Duplex Ability Status bit */

/* PHANA */
#define ADIEEE0     0x0001U     /**< Advertise IEEE Standard Selector Field bit */
#define ADIEEE1     0x0002U     /**< Advertise IEEE Standard Selector Field bit */
#define ADIEEE2     0x0004U     /**< Advertise IEEE Standard Selector Field bit */
#define ADIEEE3     0x0008U     /**< Advertise IEEE Standard Selector Field bit */
#define ADIEEE4     0x0010U     /**< Advertise IEEE Standard Selector Field bit */
#define AD10        0x0020U     /**< Advertise 10Base-T Half-Duplex Ability bit */
#define AD10FD      0x0040U     /**< Advertise 10Base-T Full-Duplex Ability bit */
#define AD100       0x0080U     /**< Advertise 100Base-TX Half-Duplex Ability bit */
#define AD100FD     0x0100U     /**< Advertise 100Base-TX Full-Duplex Ability bit */
#define ADPAUS0     0x0400U     /**< Advertise PAUSE Flow Control Ability bits */
#define ADPAUS1     0x0800U     /**< Advertise PAUSE Flow Control Ability bits */
#define ADFAULT     0x2000U     /**< Advertise Remote Fault Condition bit */
#define ADNP        0x8000U     /**< Advertise Next Page Ability bit */

/* PHANLPA */
#define LPIEEE0     0x0001U     /**< Link Partner IEEE Standard Selector Field bit */
#define LPIEEE1     0x0002U     /**< Link Partner IEEE Standard Selector Field bit */
#define LPIEEE2     0x0004U     /**< Link Partner IEEE Standard Selector Field bit */
#define LPIEEE3     0x0008U     /**< Link Partner IEEE Standard Selector Field bit */
#define LPIEEE4     0x0010U     /**< Link Partner IEEE Standard Selector Field bit */
#define LP10        0x0020U     /**< Link Partner 10Base-T Half-Duplex Ability bit */
#define LP10FD      0x0040U     /**< Link Partner 10Base-T Full-Duplex Ability bit */
#define LP100       0x0080U     /**< Link Partner 100Base-TX Half-Duplex Ability bit */
#define LP100FD     0x0100U     /**< Link Partner 100Base-TX Full-Duplex Ability bit */
#define LP100T4     0x0200U     /**< Link Partner 100Base-T4 Ability bit */
#define LPPAUS0     0x0400U     /**< Link Partner PAUSE Flow Control Ability bit */
#define LPPAUS1     0x0800U     /**< Link Partner PAUSE Flow Control Ability bit */
#define LPFAULT     0x2000U     /**< Link Partner Remote Fault Condition bit */
#define LPACK       0x4000U     /**< Link Partner Acknowledge Local PHY Code Word Status bit */
#define LPNP        0x8000U     /**< Link Partner Next Page Ability bit */

/* PHANE */
#define LPANABL     0x0001U     /**< Link Partner Auto-Negotiation Able Status bit */
#define LPARCD      0x0002U     /**< Link Partner Abilities Received Status bit */
#define PDFLT       0x0010U     /**< Parallel Detection Fault Status bit */

/* PHCON2 */
#define EDSTAT      0x0002U     /**< Energy Detect Status bit */
#define FRCLINK     0x0004U     /**< Force Link Control bit */
#define EDTHRES     0x0800U     /**< Energy Detect Threshold Control bit */
#define EDPWRDN     0x2000U     /**< Energy Detect Power-Down Enable bit */

/* PHSTAT2 */
#define PLRITY      0x0010U     /**< TPIN+/- Polarity Status bit (applies to 10Base-T only) */

/* PHSTAT3 */
#define SPDDPX0     0x0004U     /**< Current Operating Speed and Duplex Status bit */
#define SPDDPX1     0x0008U     /**< Current Operating Speed and Duplex Status bit */
#define SPDDPX2     0x0010U     /**< Current Operating Speed and Duplex Status bit */

/** @} */


typedef enum {
    HALF_DUPLEX,
    FULL_DUPLEX
} enc624j600_duplex_mode;

typedef enum {
    CRC_ERROR_COLLECTION_FILTER,
    RUNT_ERROR_COLLECTION_FILTER,
    CRC_ERROR_REJECTION_FILTER,
    RUNT_ERROR_REJECTION_FILTER,
    UNICAST_COLLECTION_FILTER,
    NOT_ME_UNICAST_COLLECTION_FILTER,
    MULTICAST_COLLECTION_FILTER,
    BROADCAST_COLLECTION_FILTER,
    HASH_TABLE_COLLECTION_FILTER,
    MAGIC_PACKET_COLLECTION_FILTER,
    PATTERN_MATCH_COLLECTION_FILTER
} enc624j600_receive_filter;

typedef enum {
    ENABLE,
    DISABLE
} enc624j600_receive_filter_state;

typedef enum {
    EGPRDPT,
    EGPWRPT,
    ERXRDPT,
    ERXWRPT,
    EUDARDPT,
    EUDAWRPT
} enc624j600_buffer_pointer;

typedef enum {
    EGPDATA,
    ERXDATA,
    EUDADATA
} enc624j600_window_reg;


static enc624j600_duplex_mode duplex_mode;
static uint16_t next_receive_frame_pointer = 0U;


static void execute_single_byte_instruction(uint8_t opcode) {
    enc624j600_hal_cs_assert();
    
    enc624j600_hal_spi_transfer(opcode);
    
    enc624j600_hal_cs_deassert();
}

static uint16_t read_buffer_pointer(enc624j600_buffer_pointer pointer) {
    
    uint16_t result = 0;
    
    enc624j600_hal_cs_assert();
    
    switch (pointer) {
        
        case EGPRDPT:
            enc624j600_hal_spi_transfer(RGPRDPT);
            break;
            
        case EGPWRPT:
            enc624j600_hal_spi_transfer(RGPWRPT);
            break;
            
        case ERXRDPT:
            enc624j600_hal_spi_transfer(RRXRDPT);
            break;
            
        case ERXWRPT:
            enc624j600_hal_spi_transfer(RRXWRPT);
            break;
            
        case EUDARDPT:
            enc624j600_hal_spi_transfer(RUDARDPT);
            break;
            
        case EUDAWRPT:
            enc624j600_hal_spi_transfer(RUDAWRPT);
            break;
    }
    
    result = result | enc624j600_hal_spi_transfer(0xAA); // LSB
    result |= (uint16_t)enc624j600_hal_spi_transfer(0xAA) << 8; // MSB
    
    enc624j600_hal_cs_deassert();
    
    return result;
}

static void write_buffer_pointer(enc624j600_buffer_pointer pointer, uint16_t new_value) {
        
    enc624j600_hal_cs_assert();
    
    switch (pointer) {
        
        case EGPRDPT:
            enc624j600_hal_spi_transfer(WGPRDPT);
            break;
            
        case EGPWRPT:
            enc624j600_hal_spi_transfer(WGPWRPT);
            break;
            
        case ERXRDPT:
            enc624j600_hal_spi_transfer(WRXRDPT);
            break;
            
        case ERXWRPT:
            enc624j600_hal_spi_transfer(WRXWRPT);
            break;
            
        case EUDARDPT:
            enc624j600_hal_spi_transfer(WUDARDPT);
            break;
            
        case EUDAWRPT:
            enc624j600_hal_spi_transfer(WUDAWRPT);
            break;
    }
    
    enc624j600_hal_spi_transfer((uint8_t) (new_value & 0xFF)); // LSB
    enc624j600_hal_spi_transfer((uint8_t) ((new_value >> 8) & 0xFF)); // MSB
    
    enc624j600_hal_cs_deassert();
}

static uint16_t read_sfr_unbanked(uint8_t sfr_addr) {
    
    uint16_t result = 0;
    
    enc624j600_hal_cs_assert();
    
    enc624j600_hal_spi_transfer(RCRU);
    enc624j600_hal_spi_transfer(sfr_addr);
    
    result = enc624j600_hal_spi_transfer(0xAA); // LSB
    result |= (uint16_t)enc624j600_hal_spi_transfer(0xAA) << 8; // MSB
    
    enc624j600_hal_cs_deassert();
    
    return result;
}

static void write_sfr_unbanked(uint8_t sfr_addr, uint16_t new_value) {
    
    enc624j600_hal_cs_assert();
    
    enc624j600_hal_spi_transfer(WCRU);
    enc624j600_hal_spi_transfer(sfr_addr);
    
    enc624j600_hal_spi_transfer((uint8_t) (new_value & 0xFF)); // LSB
    enc624j600_hal_spi_transfer((uint8_t) ((new_value >> 8) & 0xFF)); // MSB
    
    enc624j600_hal_cs_deassert();
}

// has no effect on any SFR in the unbanked region (addresses 0x80 through 0x9F)
// has no effect on MAC or MII registers
static void bit_field_set_sfr_unbanked(uint8_t sfr_addr, uint16_t mask) {
    
    enc624j600_hal_cs_assert();
    
    enc624j600_hal_spi_transfer(BFSU);
    enc624j600_hal_spi_transfer(sfr_addr);
    
    enc624j600_hal_spi_transfer((uint8_t) (mask & 0xFF)); // LSB
    enc624j600_hal_spi_transfer((uint8_t) ((mask >> 8) & 0xFF)); // MSB
    
    enc624j600_hal_cs_deassert();
}

// has no effect on any SFR in the unbanked region (addresses 0x80 through 0x9F)
// has no effect on MAC or MII registers
static void bit_field_clear_sfr_unbanked(uint8_t sfr_addr, uint16_t mask) {
    
    enc624j600_hal_cs_assert();
    
    enc624j600_hal_spi_transfer(BFCU);
    enc624j600_hal_spi_transfer(sfr_addr);
    
    enc624j600_hal_spi_transfer((uint8_t) (mask & 0xFF)); // LSB
    enc624j600_hal_spi_transfer((uint8_t) ((mask >> 8) & 0xFF)); // MSB
    
    enc624j600_hal_cs_deassert();
}

/* Functions for bit set/clear only for MAC SFR */

static void bit_field_set_mac_sfr(uint8_t mac_sfr_addr, uint16_t mask) {
	
	uint16_t mac_sfr_value = read_sfr_unbanked(mac_sfr_addr);
	
	mac_sfr_value = mac_sfr_value | mask;
	
	write_sfr_unbanked(mac_sfr_addr, mac_sfr_value);
}

static void bit_field_clear_mac_sfr(uint8_t mac_sfr_addr, uint16_t mask) {
	
	uint16_t mac_sfr_value = read_sfr_unbanked(mac_sfr_addr);
	
	mac_sfr_value = mac_sfr_value & (~mask);
	
	write_sfr_unbanked(mac_sfr_addr, mac_sfr_value);
}

/*  Functions to read from/write to, location in SRAM pointed by buffer read/write pointer 
 *  (EGPRDPT,EGPWRPT; ERXRDPT,ERXWRPT; EUDARDPT,EUDAWRPT) automatic increment with 1
 * 
 *  Windows registers EGPDATA, ERXDATA, EUDADATA are all 8 bits wide
 */

static void read_from_window_reg(enc624j600_window_reg window_reg, uint8_t *buffer, uint16_t n) {
    
    enc624j600_hal_cs_assert();
    
    switch (window_reg) {
        
        case EGPDATA:
            enc624j600_hal_spi_transfer(RGPDATA);
            break;
            
        case ERXDATA:
            enc624j600_hal_spi_transfer(RRXDATA);
            break;
            
        case EUDADATA:
            enc624j600_hal_spi_transfer(RUDADATA);
            break;
    }
    
    int i;
    
    for (i = 0; i < n; i++) {
        buffer[i] = enc624j600_hal_spi_transfer(0xAA);
    }
    
    enc624j600_hal_cs_deassert();
}

static void write_to_window_reg(enc624j600_window_reg window_reg, uint8_t *buffer, uint16_t n) {
    
    enc624j600_hal_cs_assert();
    
    switch (window_reg) {
        
        case EGPDATA:
            enc624j600_hal_spi_transfer(WGPDATA);
            break;
            
        case ERXDATA:
            enc624j600_hal_spi_transfer(WRXDATA);
            break;
            
        case EUDADATA:
            enc624j600_hal_spi_transfer(WUDADATA);
            break;
    }
    
    int i;
    
    for (i = 0; i < n; i++) {
        enc624j600_hal_spi_transfer(buffer[i]);
    }
    
    enc624j600_hal_cs_deassert();
}

/* Functions to access PHY SFRs */

static uint16_t read_phy_sfr(uint8_t phy_sfr_addr) {
    
    write_sfr_unbanked(MIREGADR, 0x0100 | phy_sfr_addr);
    
	write_sfr_unbanked(MICMD, 0x0001);
    
    enc624j600_hal_delay(30U);
    
    while ((read_sfr_unbanked(MISTAT) & BUSY) > 0) {
        
    }
    
	write_sfr_unbanked(MICMD, 0x0000);
    
    return read_sfr_unbanked(MIRD);
}

static void write_phy_sfr(uint8_t phy_sfr_addr, uint16_t new_value) {
    
    write_sfr_unbanked(MIREGADR, 0x0100 | phy_sfr_addr);
    
    write_sfr_unbanked(MIWR, new_value);
    
    enc624j600_hal_delay(30U);
    
    while ((read_sfr_unbanked(MISTAT) & BUSY) > 0) {
        
    }
}

static void bit_field_set_phy_sfr(uint8_t phy_sfr_addr, uint16_t mask) {
    
    uint16_t phy_sfr_value = read_phy_sfr(phy_sfr_addr);
    
    phy_sfr_value = phy_sfr_value | mask;
    
    write_phy_sfr(phy_sfr_addr, phy_sfr_value);
}

static void bit_field_clear_phy_sfr(uint8_t phy_sfr_addr, uint16_t mask) {
    
    uint16_t phy_sfr_value = read_phy_sfr(phy_sfr_addr);
    
    phy_sfr_value = phy_sfr_value & (~mask);
    
    write_phy_sfr(phy_sfr_addr, phy_sfr_value);
}

/* Function for enable/disable receive filters */

static void configure_receive_filter(enc624j600_receive_filter filter, enc624j600_receive_filter_state new_state) {
    
    switch (filter) {
        
        case CRC_ERROR_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, CRCEEN);
                    break;
            
                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, CRCEEN);
                    break;
            }
            
            break;
        }
        
        case RUNT_ERROR_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, RUNTEEN);
                    break;
            
                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, RUNTEEN);
                    break;
            }
            
            break;
        }
        
        case CRC_ERROR_REJECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, CRCEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, CRCEN);
                    break;
            }
            
            break;
        }
        
        case RUNT_ERROR_REJECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, RUNTEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, RUNTEN);
                    break;
            }
            
            break;
        }
        
        case UNICAST_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, UCEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, UCEN);
                    break;
            }
            
            break;
        }
        
        case NOT_ME_UNICAST_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, NOTMEEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, NOTMEEN);
                    break;
            }
            
            break;
        }
        
        case MULTICAST_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, MCEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, MCEN);
                    break;
            }
            
            break;
        }
        
        case BROADCAST_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, BCEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, BCEN);
                    break;
            }
            
            break;
        }
        
        case HASH_TABLE_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, HTEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, HTEN);
                    break;
            }
            
            break;
        }
        
        
        case MAGIC_PACKET_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    bit_field_set_sfr_unbanked(ERXFCON, MPEN);
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, MPEN);
                    break;
            }
            
            break;
        }
        
        case PATTERN_MATCH_COLLECTION_FILTER: {
            
            switch (new_state) {
        
                case ENABLE:
                    // TODO:
                    break;

                case DISABLE:
                    bit_field_clear_sfr_unbanked(ERXFCON, PMEN0 | PMEN1 | PMEN2 | PMEN3);
                    break;
            }
            
            break;
        }
        
    }
    
}

static void reset(void) {
    
    uint8_t flag = 1;
    
    // insure that the chip exit POR (Power-on Reset)
    while (flag) {
		
        write_sfr_unbanked(EUDAST, 0x1234);
    
        if (read_sfr_unbanked(EUDAST) == 0x1234) {
            // SPI interface is working
            flag = 0;
        }
    }

    // insure that the internal clock is ready
    flag = 1;
    
    while (flag) {
        
        if ((read_sfr_unbanked(ESTAT) & CLKRDY) > 0) {
            // internal clock is ready
            flag = 0;
        }
    }
    
    // perform system reset
    execute_single_byte_instruction(SETETHRST);
    
    enc624j600_hal_delay(30U);
    
    flag = 1;
    while (flag) {
        
        if (read_sfr_unbanked(EUDAST) == 0) {
            // system reset took place
            flag = 0;
        }
    }
        
    enc624j600_hal_delay(200U); // 270us
}

static void mac_init(void) {
    // flow control - possible manual or automatic (in half/full-duplex);
    // current configuration is for automatic
    
    // RXPAUS - pause control frame reception enable bit (should be default (enable))
    // PASSALL - pass all received frames enable bit (should be default (disable))
    // EPAUS - number of pause quanta (should be default)
    
    // ERXWM (MSB) - upper threshold when flow control to enable - (16320 bytes)
    // ERXWM (LSB) - lower threshold when flow control to disable - (8160 bytes)
    write_sfr_unbanked(ERXWM, 0xAA55);
    
    // enable automatic flow control
    bit_field_set_sfr_unbanked(ECON2, AUTOFC);
    
    // enable transmit CRC
	bit_field_set_mac_sfr(MACON2, TXCRCEN);
    
    // enable zero-padding to 60 bytes before appending CRC
	bit_field_clear_mac_sfr(MACON2, PADCFG0 | PADCFG1 | PADCFG2);
	bit_field_set_mac_sfr(MACON2, PADCFG0);
    
    // enable automatic transmit MAC source address
    bit_field_set_sfr_unbanked(ECON2, TXMAC);
    
    // set maximum frame length to be accepted (received or transmitted) 1518 bytes
    write_sfr_unbanked(MAMXFL, 0x05EE);
    
    // HFRMEN - huge frame enable bit (should be default(disable))
}

static void phy_init(void) {
    
    // disable Sleep
    // disable loopback
    bit_field_clear_phy_sfr(PHCON1, PSLEEP | PLOOPBK);
    
    // enable auto-negotiation
    bit_field_set_phy_sfr(PHCON1, ANEN);
    
    // enable auto-negotiation to advertises
    // 10Base-T Half/Full-Duplex; 100Base-TX Half/Full-Duplex
    bit_field_set_phy_sfr(PHANA, AD10 | AD10FD | AD100 | AD100FD);
    
    // advertise PAUSE Flow Control
    bit_field_set_phy_sfr(PHANA, ADPAUS0);
}

void enc624j600_init(void) {
    	
	/*
	 *	Configuration
	 *		- Clock out enable/disable and frequency
	 *		- Transmit/Receive buffer size
	 *		- Receive filters
	 *		- Custom MAC address
	 *		- Enable/disable VLAN support
	 *		- Loop back enable/disable
	 *		- Sleep
	 * 
	 *	Status
	 *		- Speed
	 *		- Duplex mode
	 *		- MAC Address
	 */
	
    reset();
    
    // ### Disable clock out ###
    bit_field_clear_sfr_unbanked(ECON2, COCON0 | COCON1 | COCON2 | COCON3);
    
    // ### Receive buffer - 16 Kbytes (16384) ###
    // ERXST = 0x2000
    write_sfr_unbanked(ERXST, 0x2000U);
    
    // ERXHEAD will automatically be set to ERXST
    next_receive_frame_pointer = 0x2000U;
    
    // ERXTAIL = 0x5FFE last even address
    write_sfr_unbanked(ERXTAIL, 0x5FFEU);
    
    // init Receive buffer read/write pointers
    write_buffer_pointer(ERXRDPT, 0x2000U);
    write_buffer_pointer(ERXWRPT, 0x2000U);
    
    // ### Transmit buffer - 8 Kbytes (8192) ###
    
    // init General purpose buffer read/write pointers
    write_buffer_pointer(EGPRDPT, 0x0000U);
    write_buffer_pointer(EGPWRPT, 0x0000U);
    
    // disable user-defined buffer read/write pointers wrapping
    write_sfr_unbanked(EUDAST, 0x6000U);
    write_sfr_unbanked(EUDAND, 0x6001U);
    
    // init user-defined buffer read/write pointers
    write_buffer_pointer(EUDARDPT, 0x0000U);
    write_buffer_pointer(EUDAWRPT, 0x0000U);
    
    // ### Receive filters ###
    
    configure_receive_filter(CRC_ERROR_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(RUNT_ERROR_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(CRC_ERROR_REJECTION_FILTER, ENABLE);
    configure_receive_filter(RUNT_ERROR_REJECTION_FILTER, ENABLE);
    configure_receive_filter(UNICAST_COLLECTION_FILTER, ENABLE);
    configure_receive_filter(NOT_ME_UNICAST_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(MULTICAST_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(BROADCAST_COLLECTION_FILTER, ENABLE);
    configure_receive_filter(HASH_TABLE_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(MAGIC_PACKET_COLLECTION_FILTER, DISABLE);
    configure_receive_filter(PATTERN_MATCH_COLLECTION_FILTER, DISABLE);
    
    // ### MAC initialization ###
    mac_init();
    
    // ### PHY initialization ###
    phy_init();
    
    // enable frame reception
    execute_single_byte_instruction(ENABLERX);
    
    // ### After link establishment ###
    
    while ((read_phy_sfr(PHSTAT1) & LLSTAT) == 0U) {
        
    }
    
    // FULDPX - MAC Full-Duplex Enable bit
    // PHYDPX - PHY Full Duplex Status bit
    
    if ((read_sfr_unbanked(ESTAT) & PHYDPX) > 0U) {
        // full-duplex
		bit_field_set_mac_sfr(MACON2, FULDPX);
        write_sfr_unbanked(MABBIPG, 0x0015U);
        duplex_mode = FULL_DUPLEX;
    } else {
        // half-duplex
		bit_field_clear_mac_sfr(MACON2, FULDPX);
        write_sfr_unbanked(MABBIPG, 0x0012U);
        duplex_mode = HALF_DUPLEX;
    }
}

enc624j600_transmit_result enc624j600_transmit(uint8_t *destination_mac, uint8_t *length_type, uint8_t *data, uint16_t length) {
    
    // TODO: OPERATION MUST BE THREAD SAFE
    
    // Frame Length - 1518 bytes
    // Data Length - 1500 bytes max, 46 bytes min
    
    // Consider different steps for half/full-duplex
    
    // 1.Write the frame into SRAM (destination, protocol, data)
    // 2.Set ETXST - Transmit Data Start Pointer
    // 3.Set ETXLEN - Transmit Buffer Length Pointer (number of bytes)
    // 4.Set TXRTS bit to initiate transmission
    // 5.Wait the hardware to clear TXRTS then transmission is done
    // 6.Read the ETXSTAT, ETXWIRE for errors
	
	if (destination_mac == NULL || 
		length_type == NULL || 
		data == NULL) {
		return ENC_TRANSMIT_FAILED;
	}
    
    if (length <= 7) {
        return ENC_TRANSMIT_DATA_IS_TOO_SMALL;
    }
    
    if (length > 1500) {
        return ENC_TRANSMIT_DATA_EXCEED_MTU;
    }
    
    uint16_t gpwrpt_value = read_buffer_pointer(EGPWRPT);
    
    // write the destination MAC in SRAM
    write_to_window_reg(EGPDATA, destination_mac, 6U);
    
    // write the length/protocol in SRAM
    write_to_window_reg(EGPDATA, length_type, 2U);
    
    // write the data in SRAM
    write_to_window_reg(EGPDATA, data, length);
    
    // set the ETXST
    write_sfr_unbanked(ETXST, gpwrpt_value);
    
    // set the ETXLEN
    write_sfr_unbanked(ETXLEN, 8U + length);
    
    // set TXRTS bit
    execute_single_byte_instruction(SETTXRTS);
    
    // wait hardware to clear the TXRTS bit
    while ((read_sfr_unbanked(ECON1) & TXRTS) != 0) {
        
    }
    
    // Check for errors
    // for full-duplex check only ETXWIRE (total length of the packet, including padding and CRC)
    // for half-duplex check bit in ETXSTAT
    
    if (duplex_mode == FULL_DUPLEX) {
        
        if (length < 46U) {
            length = 46;
        }
        
        if (read_sfr_unbanked(ETXWIRE) != (18U + length)) {
            return ENC_TRANSMIT_FAILED;
        }
        
    } else {
        // also check for DEFER
        if ((read_sfr_unbanked(ETXSTAT) & (LATECOL | MAXCOL | EXDEFER)) > 0) {
            return ENC_TRANSMIT_FAILED; // TODO: return more specific error
        }
    }
    
    return ENC_TRANSMIT_SUCCEEDED;
}

enc624j600_receive_result enc624j600_receive(uint8_t *destination_mac, uint8_t *source_mac, uint8_t *length_type, uint8_t *buffer, uint16_t *received_bytes) {
    // Each frame starts on an even address
    
    // Receive Head Pointer - ERXHEAD, indicating the next location to be written
    // Receive Tail Pointer - ERXTAIL - must be two bytes behind the next frame
    // or two bytes behind head when there isn't frames, because when Tail = Head 
    // means the buffer is full
	
	if (destination_mac == NULL || 
		source_mac == NULL ||
		length_type == NULL || 
		buffer == NULL || 
		received_bytes == NULL) {
		return ENC_RECEIVE_FAILED;
	}
    
    // PKTCNT - Receive Packet Count bits
    if((read_sfr_unbanked(ESTAT) & 
       (PKTCNT0 | PKTCNT1 | PKTCNT2 | PKTCNT3 | PKTCNT4 | PKTCNT5 | PKTCNT6 | PKTCNT7)) == 0) {
        // no pending frames
        return ENC_RECEIVE_NO_PENDING_FRAME;
    }
    
    write_buffer_pointer(ERXRDPT, next_receive_frame_pointer);
    
    // read pointer to next frame
    uint8_t next_frame_address[2];
    read_from_window_reg(ERXDATA, next_frame_address, 2);
    
    // update next_receive_frame_pointer
    next_receive_frame_pointer = 0;
    next_receive_frame_pointer = next_receive_frame_pointer | next_frame_address[0];
    next_receive_frame_pointer = next_receive_frame_pointer | ((uint16_t)next_frame_address[1] << 8);
    
    // read the RSV (Receive Status Vector)
    uint8_t rsv[6]; // LSB --> MSB
    read_from_window_reg(ERXDATA, rsv, 6);
    
    // frame bigger than MAMXFL are discard
    uint16_t frame_length = 0;
    frame_length = frame_length | rsv[0];
    frame_length = frame_length | ((uint16_t)rsv[1] << 8);
    
    uint16_t data_length = frame_length - 18U;
    *received_bytes = data_length;
    
    // read destination address
    read_from_window_reg(ERXDATA, destination_mac, 6);
    
    // read source address
    read_from_window_reg(ERXDATA, source_mac, 6);
    
    // read type/length
    read_from_window_reg(ERXDATA, length_type, 2);
    
    // read data (!!! can have padding !!!)
    read_from_window_reg(ERXDATA, buffer, data_length);
    
    // set the ERXTAIL 2 bytes before the new frame
    write_sfr_unbanked(ERXTAIL, next_receive_frame_pointer - 2U);
    
    // decrement PKTCNT
    execute_single_byte_instruction(SETPKTDEC);
    
    return ENC_RECEIVE_SUCCEEDED;
}