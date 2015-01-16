#include <string.h>
#include "spin1_api.h"

#define TIMER_TICK_PERIOD  10000 // 10ms
#define XCHIPS 2
#define YCHIPS 2
#define SECS   300

uint coreID, chipID, chipNum;
sdp_msg_t my_msg;
uint time_step = 0, j=0;

// Spinnaker function prototypes
void send_msg(char *s);
void get_temps(uint ticks, uint null);

//int frac(float num, uint precision);
//char *itoa(uint n);
//char *ftoa(float num, int precision);

void ftoa(float n, char *res, int precision);
uint itoa(uint n, char s[], uint len);
void reverse(char *s, int len);

void sdp_init(void);

// Main
int c_main(void)
{
  // Get core and chip IDs
  coreID = spin1_get_core_id();
  chipID = spin1_get_chip_id();

  // get this chip's number
  chipNum = ((chipID&255) * YCHIPS) + (chipID>>8);

  // initialise SDP message buffer
  sdp_init();

  // set timer tick value (in microseconds)
  spin1_set_timer_tick(TIMER_TICK_PERIOD);

  // Timer callback which reports status to the host
  spin1_callback_on(TIMER_TICK, get_temps, 1);

  // Go
  spin1_start(SYNC_WAIT);

  return 0;
}

// Get temperatures and send them to host
void get_temps(uint ticks, uint null)
{
  char s[20], sout[50];
  float t;
  uint t1, k;
  uint temp1, temp2, temp3;

  // This reads out values every 0.5s
	t1 = time_step%50;

  if (chipID==0 && coreID==1 && time_step==0)
  {
    // Print out header
    io_printf(sout, "Time,ChipID,T1,T2,T3");
    send_msg(sout);
  }

  if (time_step==SECS*100+4)
  {
    spin1_exit(0);

    io_printf(s, "EOF");
    send_msg(s);
  }

  if (coreID==1 && t1<4 && chipNum==t1)		
	//if (coreID==1)
	{
    t = (float)spin1_get_simulation_time()*TIMER_TICK_PERIOD/1e6;
		//io_printf(s, "T: %ss. Trial: %d Progress: %d%%", ftoa(t,1));
		//send_msg(s);

		// Start tempearture measurement
		sc[SC_TS0] = 0x80000000;
		// Wait for measurement TS0 to finish
		k = 0;
		while(!(sc[SC_TS0] & (1<<24))) k++;
		// Get value
		temp1 = sc[SC_TS0] & 0x00ffffff;
		// Stop measurement
		sc[SC_TS0] = 0<<31;
		//io_printf(IO_BUF, "k(T1):%d\n", k);

		// Start tempearture measurement
		sc[SC_TS1] = 0x80000000;
		// Wait for measurement TS1 to finish
		k=0;
		while(!(sc[SC_TS1] & (1<<24))) k++;
		// Get value
		temp2 = sc[SC_TS1] & 0x00ffffff;
		// Stop measurement
		sc[SC_TS1] = 0<<31;
		//io_printf(IO_BUF, "k(T2):%d\n", k);

		// Start tempearture measurement
		sc[SC_TS2] = 0x80000000;
		// Wait for measurement TS2 to finish
		k=0;
		while(!(sc[SC_TS2] & (1<<24))) k++;
		// Get value
		temp3 = sc[SC_TS2] & 0x00ffffff;
		// Stop measurement
		sc[SC_TS2] = 0<<31;
		//io_printf(IO_BUF, "k(T3):%d\n\n", k);

    ftoa(t, s, 2);
		io_printf(sout, "%s,%d,%d,%d,%d,%d", s, ++j, chipNum, temp1, temp2, temp3);
		// Send SDP message to host
    send_msg(sout);
	}

	time_step++;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int precision)
{
  // Extract integer part
  int ipart = (int)n;

  // Extract floating part
  float fpart = n - (int)n;

  // convert integer part to string
  uint i = itoa(ipart, res, 0);

  // check for display option after point
  if (precision != 0)
  {
    res[i] = '.';  // add dot

    // This computes pow(10,precision)
    uint m=1;
    if (precision>0)
    for (int i=0; i<precision; i++)
      m*=10;

    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter is needed
    // to handle cases like 233.007
    fpart = fpart * m;

    itoa((int)fpart, res+i+1, precision);
  }
}

 // Converts a given integer num to string str[].  len is the number
 // of digits required in output. If len is more than the number
 // of digits in num, then 0s are added at the beginning.
uint itoa(uint num, char s[], uint len)
{
    uint i = 0;
/*    while (num)
    {
      s[i++] = (num%10) + '0';
      num = num/10;
    }
*/

    do {
        s[i++] = '0' + num%10;
        num /= 10;
    } while (num>0);

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < len)
      s[i++] = '0';
 
    reverse(s, i);
    s[i] = '\0';
    return i;
}

// reverses a string 's' of length 'len'
void reverse(char *s, int len)
{
    int i=0, j=len-1;
    char temp;
    while (i<j)
    {
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
        i++;
        j--;
    }
}

// Send SDP packet to host (for reporting purposes)
void send_msg(char *s)
{
  int s_len;

  s_len = strlen(s);
  spin1_memcpy(my_msg.data, (void *)s, s_len);
  
  my_msg.length = sizeof(sdp_hdr_t) + sizeof(cmd_hdr_t) + s_len;

  // Send SDP message
  while(!spin1_send_sdp_msg(&my_msg, 1)); // 10ms timeout
}

void sdp_init(void)
{
  my_msg.tag       = 1;             // IPTag 1
  my_msg.dest_port = PORT_ETH;      // Ethernet
  my_msg.dest_addr = sv->eth_addr;  // Eth connected chip on this board

  my_msg.flags     = 0x07;          // Flags = 7
  my_msg.srce_port = spin1_get_core_id ();  // Source port
  my_msg.srce_addr = spin1_get_chip_id ();  // Source addr
}

