/*********************************************************/
/*      Configuration for assigning LCD bits to Pins     */
/*********************************************************/
/* Arduino pins used for Commands - these must be within the range of 23-29 */

#define SPI_DATA PC1
#define SPI_CLK PC2
#define SPI_CS PC3
#define SPI_RESET PC5
#define SPI_CD PC7
/*******************************************************/
/*      end of pin configuration                       */
/*******************************************************/
// macros to fast write data to pins

#define fastWriteHigh(_pin) (PORTC |= (1<< _pin))
#define fastWriteLow(_pin)  (PORTC &= ~(1 << _pin))
#define setInput(_pin) (DDRC &= ~(1 <<_pin))
#define setOutput(_pin) (DDRC |= 1 <<_pin)

#define LOADING_Y 55

