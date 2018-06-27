#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include "plugin_defs.h"

static int audio_fd;        /* audio file descpritor */
static int freq;            /* given sampling frequency */

/* return plugin information */
int
get_plugin_info(int opcode, char *buf, int buflen)
{
  switch(opcode) {
  case 0:
    strncpy(buf, "OSS adin plugin", buflen);
    break;
  }
  return 0;
}

/* return argument string for "-input" */
void
adin_get_optname(char *buf, int buflen)
{
  strncpy(buf, "myadin", buflen);
}

/* return property of this adin input */
int
adin_get_configuration(int opcode)
{
  switch(opcode) {
  case 0:   /* enable real-time processing of 1st pass by default? */
    return 1;   /* yes */
  case 1:       /* enable frontend voice detection by level/zc by default? */
    return 1;   /* yes */
  case 2:       /* input module threading is needed or not, if supported? */
    return 1;   /* yes, needed*/
  }
}

/* standby, will be called once at startup */
boolean
adin_standby(int sfreq, void *dummy)
{
  freq = sfreq;  /* just store required sampling frequency to local */
  return TRUE;
}

/* open device */
boolean
adin_open(char *pathname)
{
  int fmt;
  int stereo;
  int ret;
  int s;

  if ((audio_fd = open(pathname ? pathname : "/dev/dsp", O_RDONLY)) == -1) {
    printf("Error: cannot open %s\n", pathname ? pathname : "/dev/dsp");
    return FALSE;
  }
  fmt = AFMT_S16_LE;               /* 16bit signed (little endian) */
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &fmt) == -1) {
    printf("Error: failed set format to 16bit signed\n");
    return FALSE;
  }
  stereo = 0;           /* mono */
  ret = ioctl(audio_fd, SNDCTL_DSP_STEREO, &stereo);
  if (ret == -1 || stereo != 0) {
    stereo = 1;
    ret = ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &stereo);
    if (ret == -1 || stereo != 1) {
      printf("Error: failed to set monoral channel\n");
      return FALSE;
    }
  }
  s = freq;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &s) == -1) {
    printf("Erorr: failed to set sample rate to %dHz\n", freq);
    return FALSE;
  }

  return(TRUE);
}

/* read samples */
int
adin_read(SP32 *buf, int sampnum)
{
  audio_buf_info info;
  int size, cnt;

  /* get sample num that can be read without blocking */
  if (ioctl(audio_fd, SNDCTL_DSP_GETISPACE, &info) == -1) {
    printf("Error: adin_oss: failed to get number of samples in the buffer\n");
    return(ADIN_ERROR);
  }

  /* Convert S32_LE to S16_LE */
  buf >>= 16

  /* get them as much as possible */
  size = sampnum * sizeof(SP16);
  if (size > info.bytes) size = info.bytes;
  cnt = read(audio_fd, buf, size);
  if ( cnt < 0 ) {
    printf("Error: adin_oss: failed to read samples\n");
    return (ADIN_ERROR);
  }
  cnt /= sizeof(short);
  return(cnt);
}

/* close device */
boolean
adin_close()
{
  close(audio_fd);
  return TRUE;
}
/* end of program */
