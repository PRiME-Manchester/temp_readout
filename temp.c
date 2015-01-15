#include <string.h>
#include "spin1_api.h"

#define TIMER_TICK_PERIOD  500000 // 500ms
#define XCHIPS 2
#define YCHIPS 2

uint coreID, chipID, chipNum;
sdp_msg_t my_msg;

// Spinnaker function prototypes
void send_msg(char *s);
void get_temps(uint ticks, uint null);
int frac(float num, uint precision);
char *itoa(uint n);
char *ftoa(float num, int precision);
void sdp_init(void);

// Main
int c_main(void)
{
  // Get core and chip IDs
  coreID = spin1_get_core_id();
  chipID = spin1_get_chip_id();

  // get this chip's number
  chipNum = (chipID&255 * YCHIPS) + chipID>>8;

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
  //char s[100];
  float t;
  int k;
  int temp1, temp2, temp3;

  if (coreID==1)
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

		io_printf(IO_BUF, "T:%5ss. ChipID:%d. T1=%8d. T2=%8d. T3=%8d\n", ftoa(t,1), chipNum, temp1, temp2, temp3);

		//io_printf(s, "Chip ID: %d Temp1: %d Temp2 %d Temp3 %d", chipNum, temp1, temp2, temp3);
		//send_msg(s);
	}
}

// Return fractional part
int frac(float num, uint precision)
{ 
  int m=1;

  if (precision>0)
    for (int i=0; i<precision; i++)
      m*=10;
      
  return (int)((num-(int)num)*m);
}

char *itoa(uint n)
{
    char s[32];
    static char rv[32];
    int i = 0, j;
// pop one decimal value at a time starting
// with the least significant
    do {
        s[i++] = '0' + n%10;
        n /= 10;
    } while (n>0);

// digits will be in reverse order
    for (j = 0; j < i; j++)
      rv[j] = s[i-j-1];

    rv[j] = '\0';
    return rv;
}

char *ftoa(float num, int precision)
{
  static char s[20];

  strcpy(s, itoa((int)num));
  strcat(s, ".");
  strcat(s, itoa(frac(num, precision)));

  return s;
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

