#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.H"
#include "config.h"
#include "keyboard.h"
#include "macro.h"
#include "ui.h"


int focus_count = 0;
Focus focus_focus[10];
void (*focus_handler[10])(SDL_keysym *);

Focus kbd_focus;
static SDL_bool seqfile_open = SDL_FALSE;
static FILE *seqfile;
static char keytable[SDLK_LAST][5];

//SDL KeyCode, Unshifted, Shifted, CBM, Control
static short DefaultKybdMapping[][5] = {
    0x8,0x14,0x94,0x94,0x0,
    0xD,0xD,0x8D,0x8D,0x0,
    0x20,0x20,0xA0,0xA0,0x0,
    0x27,0x27,0x22,0x0,0x0,
    0x2A,0x2A,0xC0,0xDF,0x0,
    0x2B,0x2B,0xDB,0xA6,0x0,
    0x2C,0x2C,0x3C,0x3C,0x0,
    0x2D,0x2D,0xDD,0xDC,0x0,
    0x2E,0x2E,0x3E,0x3E,0x0,
    0x2F,0x2F,0x3F,0x3F,0x0,
    0x30,0x30,0x29,0x30,0x92,
    0x31,0x31,0x21,0x81,0x90,
    0x32,0x32,0x40,0x95,0x5,
    0x33,0x33,0x23,0x96,0x1C,
    0x34,0x34,0x24,0x97,0x9F,
    0x35,0x35,0x25,0x98,0x9C,
    0x36,0x36,0x5F,0x99,0x1E,
    0x37,0x37,0x26,0x9A,0x1F,
    0x38,0x38,0x2A,0x9B,0x9E,
    0x39,0x39,0x28,0x29,0x12,
    0x3A,0x3A,0x5B,0x5B,0x1B,
    0x3B,0x3B,0x3A,0x5D,0x1D,
    0x3D,0x3D,0x2B,0x0,0x0,
    0x5B,0x5B,0xBA,0xA4,0x0,
    0x5C,0x5E,0xDE,0xDE,0x0,
    0x5D,0x5D,0xC0,0xDF,0x0,
    0x60,0x5F,0x5F,0x5F,0x6,
    0x61,0x41,0xC1,0xB0,0x1,
    0x62,0x42,0xC2,0xBF,0x2,
    0x63,0x43,0xC3,0xBC,0x3,
    0x64,0x44,0xC4,0xAC,0x4,
    0x65,0x45,0xC5,0xB1,0x5,
    0x66,0x46,0xC6,0xBB,0x6,
    0x67,0x47,0xC7,0xA5,0x7,
    0x68,0x48,0xC8,0xB4,0x8,
    0x69,0x49,0xC9,0xA2,0x9,
    0x6A,0x4A,0xCA,0xB5,0xA,
    0x6B,0x4B,0xCB,0xA1,0xB,
    0x6C,0x4C,0xCC,0xB6,0xC,
    0x6D,0x4D,0xCD,0xA7,0xD,
    0x6E,0x4E,0xCE,0xAA,0xE,
    0x6F,0x4F,0xCF,0xB9,0xF,
    0x70,0x50,0xD0,0xAF,0x10,
    0x71,0x51,0xD1,0xAB,0x11,
    0x72,0x52,0xD2,0xB2,0x12,
    0x73,0x53,0xD3,0xAE,0x13,
    0x74,0x54,0xD4,0xA3,0x14,
    0x75,0x55,0xD5,0xB8,0x15,
    0x76,0x56,0xD6,0xBE,0x16,
    0x77,0x57,0xD7,0xB3,0x17,
    0x78,0x58,0xD8,0xBD,0x18,
    0x79,0x59,0xD9,0xB7,0x19,
    0x7A,0x5A,0xDA,0xAD,0x1A,
    0x7F,0x5C,0xA9,0xA8,0x1C,
    0x100,0x30,0x29,0x30,0x92,
    0x101,0x31,0x21,0x81,0x90,
    0x102,0x32,0x40,0x95,0x5,
    0x103,0x33,0x23,0x96,0x1C,
    0x104,0x34,0x24,0x97,0x9F,
    0x105,0x35,0x25,0x98,0x9C,
    0x106,0x36,0x5F,0x99,0x1E,
    0x107,0x37,0x26,0x9A,0x1F,
    0x108,0x38,0x2A,0x9B,0x9E,
    0x109,0x39,0x28,0x29,0x12,
    0x10A,0x2E,0x3E,0x3E,0x0,
    0x10B,0x2F,0x3F,0x3F,0x0,
    0x10C,0x2A,0xC0,0xDF,0x0,
    0x10D,0x2D,0xDD,0xDC,0x0,
    0x10E,0x2B,0xDB,0xA6,0x0,
    0x10F,0xD,0x8D,0x8D,0x0,
    0x110,0x3D,0x3D,0x3D,0x1F,
    0x111,0x91,0x91,0x0,0x0,
    0x112,0x11,0x11,0x0,0x0,
    0x113,0x1D,0x1D,0x0,0x0,
    0x114,0x9D,0x9D,0x0,0x0,
    0x116,0x13,0x93,0x93,0x0,
    0x117,0x3D,0x3D,0x3D,0x1F,
    0x11A,0x85,0x85,0x85,0x0,
    0x11B,0x89,0x89,0x89,0x0,
    0x11C,0x86,0x86,0x86,0x0,
    0x11D,0x8A,0x8A,0x8A,0x0,
    0x11E,0x87,0x87,0x87,0x0,
    0x11F,0x8B,0x8B,0x8B,0x0,
    0x120,0x88,0x88,0x88,0x0,
    0x121,0x8C,0x8C,0x8C,0x0};

int kbd_init(char *keyboardcfg) {
    char linebuf[SDLK_LAST];
    int line = 0, _Value, _Unshifted, _Shifted, _CBM, _Control;
    FILE *in;

    SDL_EnableUNICODE(1);

    kbd_focus = FOCUS_TERM;

    //Inits all locations of keytable to int(64)
    memset(keytable, 0,sizeof(keytable[0][0]) * SDLK_LAST * 5);
    
    //Initialize default keyboard map
    for(int i = 0; i < 85 ; i++)
    {
        keytable[DefaultKybdMapping[i][0]][0] = (unsigned char) DefaultKybdMapping[i][1];
        keytable[DefaultKybdMapping[i][0]][1] = (unsigned char) DefaultKybdMapping[i][2];
        keytable[DefaultKybdMapping[i][0]][2] = (unsigned char) DefaultKybdMapping[i][3];
        keytable[DefaultKybdMapping[i][0]][3] = (unsigned char) DefaultKybdMapping[i][4];
    }

  if ((in = fopen(keyboardcfg, "r")) == NULL) {
    printf("Couldn't open %s\n", keyboardcfg);
  }else{

  while (fgets(linebuf, 256, in) != NULL) {

    if (strlen(linebuf) >= sizeof(linebuf) - 1) {
      printf("line %d too long in file: %s\n", line, keyboardcfg);
      fclose(in);
      return(1);
    }

    if (linebuf[0] == '#') {
    } else if (strlen(linebuf) >= 3) {

    if (sscanf(linebuf, "%d %d  %d  %d  %d\n", &_Value, &_Unshifted, &_Shifted, &_CBM, &_Control) == 5) {
        keytable[_Value][0] = _Unshifted;
        keytable[_Value][1] = _Shifted;
        keytable[_Value][2] = _CBM;
        keytable[_Value][3] = _Control;
      } else {
          printf("Syntax error in %s on line %d\n",keyboardcfg, line);
      }

    } else {

      if (!sscanf(linebuf, "%s", linebuf)) {
          printf("Syntax error in %s on line %d\n",keyboardcfg, line);
      }

    }

    ++line;
  }

  if (ferror(in)) {
    printf("read error\n");
    fclose(in);
    return(1);
  }

  fclose(in);
  }
    ui_init();
    return(0);
}


void kbd_add_focus(Focus focus, void (*handler)(SDL_keysym *)) {
  focus_focus[focus_count] = focus;
  focus_handler[focus_count] = handler;
  ++focus_count;
}


void kbd_loadseq(char *filename) {// Opens SEQ but doesn't seem to do anything more then open a handle to it.
  if (!seqfile_open) {
    if ((seqfile = fopen(filename, "rb"))) {
      seqfile_open = SDL_TRUE;
    } else {
      printf("opening %s failed\n", filename);
    }
  } else {
    puts("file is already open");
  }
}


void kbd_loadseq_abort(void) {
  if (seqfile_open) {
    fclose(seqfile);
    seqfile_open = SDL_FALSE;
  }
}


unsigned char translatekey(SDL_keysym *keysym, unsigned char *shift, unsigned char *ctrl, unsigned char *cbm) {
  *shift = keysym->mod & (KMOD_SHIFT | KMOD_CAPS) ? 1 : 0;
  *ctrl = keysym->mod & KMOD_CTRL ? 1 : 0;
  *cbm = keysym->mod & KMOD_ALT ? 1 : 0;
  switch (keysym->sym) {

  case SDLK_ESCAPE:
    ui_menu();
    return(0);
    break;

  case SDLK_PAGEUP:
    ui_pageup();
    return(0);
    break;

  case SDLK_PAGEDOWN:
    ui_pagedown();
    return(0);
    break;

  default:
    break;
  }
    if (*shift) {
        return(keytable[keysym->sym][1]);
    } else if (*cbm) {
        return(keytable[keysym->sym])[2];
    } else if (*ctrl) {
        return(keytable[keysym->sym][3]);
    } else {
        return(keytable[keysym->sym][0]);
    }
}

int kbd_getkey() {
    SDL_Event event;
    int c, f;
    char *_DebugMsg;
    unsigned char key = 0;
    
    unsigned char shift, ctrl, cbm;
 
    
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
                
            case SDL_QUIT:
                exit(1);
                break;
                
            case SDL_KEYDOWN:
                
                if (kbd_focus == FOCUS_TERM) {
                    if (event.key.keysym.mod & KMOD_META) {
                        ui_metakey(&event.key.keysym);
                    } else {
                        key = translatekey(&event.key.keysym, &shift, &ctrl, &cbm);
                        asprintf(&_DebugMsg,"Key: %d Mod: %d Shift: %d Ctrl: %d CBM: %d", event.key.keysym.sym,event.key.keysym.mod,shift,ctrl,cbm);
                        cfg_debug(_DebugMsg);
                        if (macro_rec && key != 0) {
                            macrobuf_key[macro_len] = key;
                            macrobuf_shift[macro_len] = shift;
                            macrobuf_ctrl[macro_len] = ctrl;
                            macrobuf_cbm[macro_len] = cbm;
                            if (++macro_len == macro_maxlen) {
                                macro_rec = 0;
                            }
                        }
                        return(key);
                    }
                } else {
                    for (f = 0; f < focus_count; ++f) {
                        if (f >= focus_count) {
                            printf("focus_fixme\n");
                            exit(1);
                        }
                        if (kbd_focus == focus_focus[f]) {
                            focus_handler[f](&event.key.keysym);
                            f = focus_count;
                        }
                    }
                }
                break;
                
        }
    }
    
    if (seqfile_open) {
        if ((c = fgetc(seqfile)) == EOF) {
            fclose(seqfile);
            seqfile_open = SDL_FALSE;
            return(0);
        } else {
            return(c + 256);
        }
    }
    
    if (macro_play) {
        key = macrobuf_key[macro_ctr];
        shift = macrobuf_shift[macro_ctr];
        ctrl = macrobuf_ctrl[macro_ctr];
        cbm = macrobuf_cbm[macro_ctr];
        if (++macro_ctr == macro_len) {
            macro_play = 0;
        }
        return(key);
    }
    return(key);
}

