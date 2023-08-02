# CopyAs

[Hiew](https://hiew.ru) External Module (HEM) to copy block data in different formats.

## Installation

Download the `.hem` file and put it in your HIEW `hem` folder.

## Usage

After opening a file in HIEW, mark a block, press `F11` to load a Hiew module and choose it from the menu.
The marked block data will be shown in different formats. Press `F5` to copy a string to clipboard.

### Example

```
 00000000:  4D 5A 90 00-03 00 00 00-04 00 00 00-FF FF 00 00  MZ         ÿÿ        
 00000010:  B8 00 00 00-00 00 00 00-40 00 00 00-00 00 00 00  ¸       @             
 00000020:  00 00 00 00-00 00 00 00-00 00 00 00-00 00 00 00                        
 00000030:  00 00 00 00-00 00 00 00-00 00 00 00-F0 00 00 00              ð         
 00000040:  0E 1F BA 0E-00 B4 09 CD-21 B8 01 4C-CD 21 54 68    º  ´ Í!¸ LÍ!Th      
 00000050:  69 73 20 70-72 6F 67 72-61 6D 20 63-61 6E 6E 6F  is program canno      
 00000060:  74 20 62 65-20 72 75 6E-20 69 6E 20-44 4F 53 20  t be run in DOS       
 00000070:  6D 6F 64 65-2E 0D 0D 0A-24 00 00 00-00 00 00 00  mode.   $             
 00000080:  9B 55 17 40-DF 34 79 13-DF 34 79 13-DF 34 79 13  ›U @ß4y ß4y ß4y       
 00000090:  BD 2B 6A 13-D7 34 79 13-5C 28 77 13-DE 34 79 13  ½+j ×4y \(w Þ4y       
 000000A0:  B0 2B 72 13-DE 34 79 13                                             [x]
 000000B0:  B0 2B 7D 13-DD 34 79 13  +------------------CopyAs------------------+
 000000C0:  A6 15 7D 13-DC 34 79 13  | 52696368                                 |
 000000D0:  18 32 7F 13-DE 34 79 13  | 52 69 63 68                              |
 000000E0:  52 69 63 68-DF 34 79 13  | \x52\x69\x63\x68                         |
 000000F0:  50 45 00 00-4C 01 06 00  | 0x52, 0x69, 0x63, 0x68                   |
 00000100:  00 00 00 00-E0 00 0F 01  | Rich                                     |
 00000110:  00 C0 00 00-00 00 00 00  +------------------------------------------+
 00000120:  00 50 00 00-00 00 40 00                                              
 00000130:  04 00 00 00-00 00 00 00-04 00 00 00-00 00 00 00                      
 00000140:  00 40 01 00-00 10 00 00-00 00 00 00-02 00 00 00   @                  
 00000150:  00 00 10 00-00 10 00 00-00 00 10 00-00 10 00 00                      
 00000160:  00 00 00 00-10 00 00 00-00 00 00 00-00 00 00 00                      
 00000170:  1C 22 01 00-F0 00 00 00-00 00 01 00-48 0A 00 00   "  ð       H       
 00000180:  00 00 00 00-00 00 00 00-00 00 00 00-00 00 00 00                      
 00000190:  C4 20 01 00-08 00 00 00-00 00 00 00-00 00 00 00  Ä                   
 000001A0:  00 00 00 00-00 00 00 00-00 00 00 00-00 00 00 00                      
 000001B0:  00 00 00 00-00 00 00 00-00 00 00 00-00 00 00 00                                                                          
```

## Author

Fernando Mercês - https://infosec.exchange/@mer0x36
