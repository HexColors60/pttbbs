#include "bbs.h"
#include <termios.h>

/* ----------------------------------------------------- */
/* basic tty control                                     */
/* ----------------------------------------------------- */
void
init_tty(void)
{
    struct termios tty_state, tty_new;

    if (tcgetattr(1, &tty_state) < 0) {
	syslog(LOG_ERR, "tcgetattr(): %m");
	return;
    }
    memcpy(&tty_new, &tty_state, sizeof(tty_new));
    tty_new.c_lflag &= ~(ICANON | ECHO | ISIG);
    /*
     * tty_new.c_cc[VTIME] = 0; tty_new.c_cc[VMIN] = 1;
     */
#if 1
    cfmakeraw(&tty_new);
    tty_new.c_cflag &= ~(CSIZE|PARENB);
    tty_new.c_cflag |= CS8;
    tcsetattr(1, TCSANOW, &tty_new);
#else
    tcsetattr(1, TCSANOW, &tty_new);
    system("stty raw -echo");
#endif
}

/* ----------------------------------------------------- */
/* init tty control code                                 */
/* ----------------------------------------------------- */


#define TERMCOMSIZE (40)

// SLMT: Read
// 關於這個 function 內使用的東西可以上網搜尋 IOCTL_TTY 的文件
static void
sig_term_resize(int sig GCC_UNUSED)
{
  // 關於這個 struct
  struct winsize newsize;

  // 暫時忽略 SIGWINCH 信號，等到調整完後會重新註冊這個 function
  Signal(SIGWINCH, SIG_IGN);	/* Don't bother me! */

  // 取得現在視窗的大小
  // TODO: 為什麼是抓 standard input (fd = 0) ?
  ioctl(0, TIOCGWINSZ, &newsize);

  // 根據抓到的長寬來調整內建的 terminal
  term_resize(newsize.ws_col, newsize.ws_row);
}

// SLMT: Read
void term_resize(int w, int h)
{
  // 是否需要重新更新 terminal
  int dorefresh = 0;

  // 暫時忽略 SIGWINCH 信號，等到調整完後會重新註冊這個 function
  Signal(SIGWINCH, SIG_IGN);	/* Don't bother me! */

  /* make sure reasonable size */
  h = MAX(24, MIN(100, h));
  w = MAX(80, MIN(200, w));

  // t_columns 與 t_lines 宣告在 var.c
  if (w != t_columns || h != t_lines) {
    // invoke terminal system resize
    resizeterm(h, w);

    t_lines = h;
    t_columns = w;
    dorefresh = 1;
  }

  // 這些變數都宣告再 var.c
  b_lines = t_lines - 1;
  p_lines = t_lines - 4;

  // 重新接收 SIGWINCH 信號
  Signal(SIGWINCH, sig_term_resize);

  if (dorefresh) {
    redrawwin();
    refresh();
  }
}

// SLMT: Read
int
term_init(void)
{
  // Make Term auto resize
  Signal(SIGWINCH, sig_term_resize);
  return YEA;
}

void
bell(void)
{
    const char c = Ctrl('G');
    write(1, &c, 1);
}
