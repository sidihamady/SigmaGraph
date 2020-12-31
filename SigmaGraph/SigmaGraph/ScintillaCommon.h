#pragma once

// Should a wxPopupWindow be used for the call tips and autocomplete windows?
#ifndef SCINTILLA_USE_POPUP
#define SCINTILLA_USE_POPUP 1
#endif

#define SCINTILLA_INVALID_POSITION -1

// Define start of Scintilla messages to be greater than all Windows edit (EM_*) messages
// as many EM_ messages can be used although that use is deprecated.
#define SCINTILLA_START 2000
#define SCINTILLA_OPTIONAL_START 3000
#define SCINTILLA_LEXER_START 4000
#define SCINTILLA_WS_INVISIBLE 0
#define SCINTILLA_WS_VISIBLEALWAYS 1
#define SCINTILLA_WS_VISIBLEAFTERINDENT 2
#define SCINTILLA_EOL_CRLF 0
#define SCINTILLA_EOL_CR 1
#define SCINTILLA_EOL_LF 2

// The SC_CP_UTF8 value can be used to enter Unicode mode.
// This is the same value as CP_UTF8 in Windows
#define SCINTILLA_CP_UTF8 65001

// The SC_CP_DBCS value can be used to indicate a DBCS mode for GTK+.
#define SCINTILLA_CP_DBCS 1
#define SCINTILLA_MARKER_MAX 31
#define SCINTILLA_MARK_CIRCLE 0
#define SCINTILLA_MARK_ROUNDRECT 1
#define SCINTILLA_MARK_ARROW 2
#define SCINTILLA_MARK_SMALLRECT 3
#define SCINTILLA_MARK_SHORTARROW 4
#define SCINTILLA_MARK_EMPTY 5
#define SCINTILLA_MARK_ARROWDOWN 6
#define SCINTILLA_MARK_MINUS 7
#define SCINTILLA_MARK_PLUS 8

// Shapes used for outlining column.
#define SCINTILLA_MARK_VLINE 9
#define SCINTILLA_MARK_LCORNER 10
#define SCINTILLA_MARK_TCORNER 11
#define SCINTILLA_MARK_BOXPLUS 12
#define SCINTILLA_MARK_BOXPLUSCONNECTED 13
#define SCINTILLA_MARK_BOXMINUS 14
#define SCINTILLA_MARK_BOXMINUSCONNECTED 15
#define SCINTILLA_MARK_LCORNERCURVE 16
#define SCINTILLA_MARK_TCORNERCURVE 17
#define SCINTILLA_MARK_CIRCLEPLUS 18
#define SCINTILLA_MARK_CIRCLEPLUSCONNECTED 19
#define SCINTILLA_MARK_CIRCLEMINUS 20
#define SCINTILLA_MARK_CIRCLEMINUSCONNECTED 21

// Invisible mark that only sets the line background color.
#define SCINTILLA_MARK_BACKGROUND 22
#define SCINTILLA_MARK_DOTDOTDOT 23
#define SCINTILLA_MARK_ARROWS 24
#define SCINTILLA_MARK_PIXMAP 25
#define SCINTILLA_MARK_FULLRECT 26
#define SCINTILLA_MARK_CHARACTER 10000

// Markers used for outlining column.
#define SCINTILLA_MARKNUM_FOLDEREND 25
#define SCINTILLA_MARKNUM_FOLDEROPENMID 26
#define SCINTILLA_MARKNUM_FOLDERMIDTAIL 27
#define SCINTILLA_MARKNUM_FOLDERTAIL 28
#define SCINTILLA_MARKNUM_FOLDERSUB 29
#define SCINTILLA_MARKNUM_FOLDER 30
#define SCINTILLA_MARKNUM_FOLDEROPEN 31
#define SCINTILLA_MASK_FOLDERS 0xFE000000
#define SCINTILLA_MARGIN_SYMBOL 0
#define SCINTILLA_MARGIN_NUMBER 1
#define SCINTILLA_MARGIN_BACK 2
#define SCINTILLA_MARGIN_FORE 3

// Styles in range 32..38 are predefined for parts of the UI and are not used as normal styles.
// Style 39 is for future use.
#define SCINTILLA_STYLE_DEFAULT 32
#define SCINTILLA_STYLE_LINENUMBER 33
#define SCINTILLA_STYLE_BRACELIGHT 34
#define SCINTILLA_STYLE_BRACEBAD 35
#define SCINTILLA_STYLE_CONTROLCHAR 36
#define SCINTILLA_STYLE_INDENTGUIDE 37
#define SCINTILLA_STYLE_CALLTIP 38
#define SCINTILLA_STYLE_FOLDINGLINE 39
#define SCINTILLA_STYLE_LASTPREDEFINED 40
#define SCINTILLA_STYLE_MAX 127

// Character set identifiers are used in StyleSetCharacterSet.
// The values are the same as the Windows *_CHARSET values.
#define SCINTILLA_CHARSET_ANSI 0
#define SCINTILLA_CHARSET_DEFAULT 1
#define SCINTILLA_CHARSET_BALTIC 186
#define SCINTILLA_CHARSET_CHINESEBIG5 136
#define SCINTILLA_CHARSET_EASTEUROPE 238
#define SCINTILLA_CHARSET_GB2312 134
#define SCINTILLA_CHARSET_GREEK 161
#define SCINTILLA_CHARSET_HANGUL 129
#define SCINTILLA_CHARSET_MAC 77
#define SCINTILLA_CHARSET_OEM 255
#define SCINTILLA_CHARSET_RUSSIAN 204
#define SCINTILLA_CHARSET_CYRILLIC 1251
#define SCINTILLA_CHARSET_SHIFTJIS 128
#define SCINTILLA_CHARSET_SYMBOL 2
#define SCINTILLA_CHARSET_TURKISH 162
#define SCINTILLA_CHARSET_JOHAB 130
#define SCINTILLA_CHARSET_HEBREW 177
#define SCINTILLA_CHARSET_ARABIC 178
#define SCINTILLA_CHARSET_VIETNAMESE 163
#define SCINTILLA_CHARSET_THAI 222
#define SCINTILLA_CHARSET_8859_15 1000
#define SCINTILLA_CASE_MIXED 0
#define SCINTILLA_CASE_UPPER 1
#define SCINTILLA_CASE_LOWER 2
#define SCINTILLA_INDIC_MAX 7
#define SCINTILLA_INDIC_PLAIN 0
#define SCINTILLA_INDIC_SQUIGGLE 1
#define SCINTILLA_INDIC_TT 2
#define SCINTILLA_INDIC_DIAGONAL 3
#define SCINTILLA_INDIC_STRIKE 4
#define SCINTILLA_INDIC_HIDDEN 5
#define SCINTILLA_INDIC_BOX 6
#define SCINTILLA_INDIC_ROUNDBOX 7
#define SCINTILLA_INDIC0_MASK 0x20
#define SCINTILLA_INDIC1_MASK 0x40
#define SCINTILLA_INDIC2_MASK 0x80
#define SCINTILLA_INDICS_MASK 0xE0

// PrintColourMode - use same colours as screen.
#define SCINTILLA_PRINT_NORMAL 0

// PrintColourMode - invert the light value of each style for printing.
#define SCINTILLA_PRINT_INVERTLIGHT 1

// PrintColourMode - force black text on white background for printing.
#define SCINTILLA_PRINT_BLACKONWHITE 2

// PrintColourMode - text stays coloured, but all background is forced to be white for printing.
#define SCINTILLA_PRINT_COLOURONWHITE 3

// PrintColourMode - only the default-background is forced to be white for printing.
#define SCINTILLA_PRINT_COLOURONWHITEDEFAULTBG 4
#define SCINTILLA_FIND_WHOLEWORD 2
#define SCINTILLA_FIND_MATCHCASE 4
#define SCINTILLA_FIND_WORDSTART 0x00100000
#define SCINTILLA_FIND_REGEXP 0x00200000
#define SCINTILLA_FIND_POSIX 0x00400000
#define SCINTILLA_FOLDLEVELBASE 0x400
#define SCINTILLA_FOLDLEVELWHITEFLAG 0x1000
#define SCINTILLA_FOLDLEVELHEADERFLAG 0x2000
#define SCINTILLA_FOLDLEVELBOXHEADERFLAG 0x4000
#define SCINTILLA_FOLDLEVELBOXFOOTERFLAG 0x8000
#define SCINTILLA_FOLDLEVELCONTRACTED 0x10000
#define SCINTILLA_FOLDLEVELUNINDENT 0x20000
#define SCINTILLA_FOLDLEVELNUMBERMASK 0x0FFF
#define SCINTILLA_FOLDFLAG_LINEBEFORE_EXPANDED 0x0002
#define SCINTILLA_FOLDFLAG_LINEBEFORE_CONTRACTED 0x0004
#define SCINTILLA_FOLDFLAG_LINEAFTER_EXPANDED 0x0008
#define SCINTILLA_FOLDFLAG_LINEAFTER_CONTRACTED 0x0010
#define SCINTILLA_FOLDFLAG_LEVELNUMBERS 0x0040
#define SCINTILLA_FOLDFLAG_BOX 0x0001
#define SCINTILLA_TIME_FOREVER 10000000
#define SCINTILLA_WRAP_NONE 0
#define SCINTILLA_WRAP_WORD 1
#define SCINTILLA_WRAP_CHAR 2
#define SCINTILLA_WRAPVISUALFLAG_NONE 0x0000
#define SCINTILLA_WRAPVISUALFLAG_END 0x0001
#define SCINTILLA_WRAPVISUALFLAG_START 0x0002
#define SCINTILLA_WRAPVISUALFLAGLOC_DEFAULT 0x0000
#define SCINTILLA_WRAPVISUALFLAGLOC_END_BY_TEXT 0x0001
#define SCINTILLA_WRAPVISUALFLAGLOC_START_BY_TEXT 0x0002
#define SCINTILLA_CACHE_NONE 0
#define SCINTILLA_CACHE_CARET 1
#define SCINTILLA_CACHE_PAGE 2
#define SCINTILLA_CACHE_DOCUMENT 3
#define SCINTILLA_EDGE_NONE 0
#define SCINTILLA_EDGE_LINE 1
#define SCINTILLA_EDGE_BACKGROUND 2
#define SCINTILLA_CURSORNORMAL -1
#define SCINTILLA_CURSORWAIT 4

// Constants for use with SetVisiblePolicy, similar to SetCaretPolicy.
#define SCINTILLA_VISIBLE_SLOP 0x01
#define SCINTILLA_VISIBLE_STRICT 0x04

// Caret policy, used by SetXCaretPolicy and SetYCaretPolicy.
// If CARET_SLOP is set, we can define a slop value: caretSlop.
// This value defines an unwanted zone (UZ) where the caret is... unwanted.
// This zone is defined as a number of pixels near the vertical margins,
// and as a number of lines near the horizontal margins.
// By keeping the caret away from the edges, it is seen within its context,
// so it is likely that the identifier that the caret is on can be completely seen,
// and that the current line is seen with some of the lines following it which are
// often dependent on that line.
#define SCINTILLA_CARET_SLOP 0x01

// If CARET_STRICT is set, the policy is enforced... strictly.
// The caret is centred on the display if slop is not set,
// and cannot go in the UZ if slop is set.
#define SCINTILLA_CARET_STRICT 0x04

// If CARET_JUMPS is set, the display is moved more energetically
// so the caret can move in the same direction longer before the policy is applied again.
#define SCINTILLA_CARET_JUMPS 0x10

// If CARET_EVEN is not set, instead of having symmetrical UZs,
// the left and bottom UZs are extended up to right and top UZs respectively.
// This way, we favour the displaying of useful information: the begining of lines,
// where most code reside, and the lines after the caret, eg. the body of a function.
#define SCINTILLA_CARET_EVEN 0x08

// Selection modes
#define SCINTILLA_SEL_STREAM 0
#define SCINTILLA_SEL_RECTANGLE 1
#define SCINTILLA_SEL_LINES 2
#define SCINTILLA_ALPHA_TRANSPARENT 0
#define SCINTILLA_ALPHA_OPAQUE 255
#define SCINTILLA_ALPHA_NOALPHA 256

// Maximum value of keywordSet parameter of SetKeyWords.
#define SCINTILLA_KEYWORDSET_MAX 8

// Notifications
// Type of modification and the action which caused the modification.
// These are defined as a bit mask to make it easy to specify which notifications are wanted.
// One bit is set from each of SC_MOD_* and SC_PERFORMED_*.
#define SCINTILLA_MOD_INSERTTEXT 0x1
#define SCINTILLA_MOD_DELETETEXT 0x2
#define SCINTILLA_MOD_CHANGESTYLE 0x4
#define SCINTILLA_MOD_CHANGEFOLD 0x8
#define SCINTILLA_PERFORMED_USER 0x10
#define SCINTILLA_PERFORMED_UNDO 0x20
#define SCINTILLA_PERFORMED_REDO 0x40
#define SCINTILLA_MULTISTEPUNDOREDO 0x80
#define SCINTILLA_LASTSTEPINUNDOREDO 0x100
#define SCINTILLA_MOD_CHANGEMARKER 0x200
#define SCINTILLA_MOD_BEFOREINSERT 0x400
#define SCINTILLA_MOD_BEFOREDELETE 0x800
#define SCINTILLA_MULTILINEUNDOREDO 0x1000
#define SCINTILLA_MODEVENTMASKALL 0x1FFF

// Symbolic key codes and modifier flags.
// ASCII and other printable characters below 256.
// Extended keys above 300.
#define SCINTILLA_KEY_DOWN 300
#define SCINTILLA_KEY_UP 301
#define SCINTILLA_KEY_LEFT 302
#define SCINTILLA_KEY_RIGHT 303
#define SCINTILLA_KEY_HOME 304
#define SCINTILLA_KEY_END 305
#define SCINTILLA_KEY_PRIOR 306
#define SCINTILLA_KEY_NEXT 307
#define SCINTILLA_KEY_DELETE 308
#define SCINTILLA_KEY_INSERT 309
#define SCINTILLA_KEY_ESCAPE 7
#define SCINTILLA_KEY_BACK 8
#define SCINTILLA_KEY_TAB 9
#define SCINTILLA_KEY_RETURN 13
#define SCINTILLA_KEY_ADD 310
#define SCINTILLA_KEY_SUBTRACT 311
#define SCINTILLA_KEY_DIVIDE 312
#define SCINTILLA_SCMOD_NORM 0
#define SCINTILLA_SCMOD_SHIFT 1
#define SCINTILLA_SCMOD_CTRL 2
#define SCINTILLA_SCMOD_ALT 4

// For SciLexer.h
#define SCINTILLA_LEX_CONTAINER 0
#define SCINTILLA_LEX_NULL 1
#define SCINTILLA_LEX_CPP 3
#define SCINTILLA_LEX_LUA 15

// When a lexer specifies its language as SCLEX_AUTOMATIC it receives a
// value assigned in sequence from SCLEX_AUTOMATIC+1.
#define SCINTILLA_LEX_AUTOMATIC 1000

// Lexical states for SCLEX_CPP
#define SCINTILLA_C_DEFAULT 0
#define SCINTILLA_C_COMMENT 1
#define SCINTILLA_C_COMMENTLINE 2
#define SCINTILLA_C_COMMENTDOC 3
#define SCINTILLA_C_NUMBER 4
#define SCINTILLA_C_WORD 5
#define SCINTILLA_C_STRING 6
#define SCINTILLA_C_CHARACTER 7
#define SCINTILLA_C_UUID 8
#define SCINTILLA_C_PREPROCESSOR 9
#define SCINTILLA_C_OPERATOR 10
#define SCINTILLA_C_IDENTIFIER 11
#define SCINTILLA_C_STRINGEOL 12
#define SCINTILLA_C_VERBATIM 13
#define SCINTILLA_C_REGEX 14
#define SCINTILLA_C_COMMENTLINEDOC 15
#define SCINTILLA_C_WORD2 16
#define SCINTILLA_C_COMMENTDOCKEYWORD 17
#define SCINTILLA_C_COMMENTDOCKEYWORDERROR 18
#define SCINTILLA_C_GLOBALCLASS 19

// Lexical states for SCLEX_LUA
#define SCINTILLA_LUA_DEFAULT 0
#define SCINTILLA_LUA_COMMENT 1
#define SCINTILLA_LUA_COMMENTLINE 2
#define SCINTILLA_LUA_COMMENTDOC 3
#define SCINTILLA_LUA_NUMBER 4
#define SCINTILLA_LUA_WORD 5
#define SCINTILLA_LUA_STRING 6
#define SCINTILLA_LUA_CHARACTER 7
#define SCINTILLA_LUA_LITERALSTRING 8
#define SCINTILLA_LUA_PREPROCESSOR 9
#define SCINTILLA_LUA_OPERATOR 10
#define SCINTILLA_LUA_IDENTIFIER 11
#define SCINTILLA_LUA_STRINGEOL 12
#define SCINTILLA_LUA_WORD2 13
#define SCINTILLA_LUA_WORD3 14
#define SCINTILLA_LUA_WORD4 15
#define SCINTILLA_LUA_WORD5 16
#define SCINTILLA_LUA_WORD6 17
#define SCINTILLA_LUA_WORD7 18
#define SCINTILLA_LUA_WORD8 19

// Lexical states for SCLEX_ERRORLIST
#define SCINTILLA_ERR_DEFAULT 0
#define SCINTILLA_ERR_LUA 8
#define SCINTILLA_ERR_CTAG 9

//-----------------------------------------
// Commands that can be bound to keystrokes


// Redoes the next action on the undo history.
#define SCINTILLA_CMD_REDO 2011

// Select all the text in the document.
#define SCINTILLA_CMD_SELECTALL 2013

// Undo one action in the undo history.
#define SCINTILLA_CMD_UNDO 2176

// Cut the selection to the clipboard.
#define SCINTILLA_CMD_CUT 2177

// Copy the selection to the clipboard.
#define SCINTILLA_CMD_COPY 2178

// Paste the contents of the clipboard into the document replacing the selection.
#define SCINTILLA_CMD_PASTE 2179

// Clear the selection.
#define SCINTILLA_CMD_CLEAR 2180

// Move caret down one line.
#define SCINTILLA_CMD_LINEDOWN 2300

// Move caret down one line extending selection to new caret position.
#define SCINTILLA_CMD_LINEDOWNEXTEND 2301

// Move caret up one line.
#define SCINTILLA_CMD_LINEUP 2302

// Move caret up one line extending selection to new caret position.
#define SCINTILLA_CMD_LINEUPEXTEND 2303

// Move caret left one character.
#define SCINTILLA_CMD_CHARLEFT 2304

// Move caret left one character extending selection to new caret position.
#define SCINTILLA_CMD_CHARLEFTEXTEND 2305

// Move caret right one character.
#define SCINTILLA_CMD_CHARRIGHT 2306

// Move caret right one character extending selection to new caret position.
#define SCINTILLA_CMD_CHARRIGHTEXTEND 2307

// Move caret left one word.
#define SCINTILLA_CMD_WORDLEFT 2308

// Move caret left one word extending selection to new caret position.
#define SCINTILLA_CMD_WORDLEFTEXTEND 2309

// Move caret right one word.
#define SCINTILLA_CMD_WORDRIGHT 2310

// Move caret right one word extending selection to new caret position.
#define SCINTILLA_CMD_WORDRIGHTEXTEND 2311

// Move caret to first position on line.
#define SCINTILLA_CMD_HOME 2312

// Move caret to first position on line extending selection to new caret position.
#define SCINTILLA_CMD_HOMEEXTEND 2313

// Move caret to last position on line.
#define SCINTILLA_CMD_LINEEND 2314

// Move caret to last position on line extending selection to new caret position.
#define SCINTILLA_CMD_LINEENDEXTEND 2315

// Move caret to first position in document.
#define SCINTILLA_CMD_DOCUMENTSTART 2316

// Move caret to first position in document extending selection to new caret position.
#define SCINTILLA_CMD_DOCUMENTSTARTEXTEND 2317

// Move caret to last position in document.
#define SCINTILLA_CMD_DOCUMENTEND 2318

// Move caret to last position in document extending selection to new caret position.
#define SCINTILLA_CMD_DOCUMENTENDEXTEND 2319

// Move caret one page up.
#define SCINTILLA_CMD_PAGEUP 2320

// Move caret one page up extending selection to new caret position.
#define SCINTILLA_CMD_PAGEUPEXTEND 2321

// Move caret one page down.
#define SCINTILLA_CMD_PAGEDOWN 2322

// Move caret one page down extending selection to new caret position.
#define SCINTILLA_CMD_PAGEDOWNEXTEND 2323

// Switch from insert to overtype mode or the reverse.
#define SCINTILLA_CMD_EDITTOGGLEOVERTYPE 2324

// Cancel any modes such as call tip or auto-completion list display.
#define SCINTILLA_CMD_CANCEL 2325

// Delete the selection or if no selection, the character before the caret.
#define SCINTILLA_CMD_DELETEBACK 2326

// If selection is empty or all on one line replace the selection with a tab character.
// If more than one line selected, indent the lines.
#define SCINTILLA_CMD_TAB 2327

// Dedent the selected lines.
#define SCINTILLA_CMD_BACKTAB 2328

// Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
#define SCINTILLA_CMD_NEWLINE 2329

// Insert a Form Feed character.
#define SCINTILLA_CMD_FORMFEED 2330

// Move caret to before first visible character on line.
// If already there move to first character on line.
#define SCINTILLA_CMD_VCHOME 2331

// Like VCHome but extending selection to new caret position.
#define SCINTILLA_CMD_VCHOMEEXTEND 2332

// Magnify the displayed text by increasing the sizes by 1 point.
#define SCINTILLA_CMD_ZOOMIN 2333

// Make the displayed text smaller by decreasing the sizes by 1 point.
#define SCINTILLA_CMD_ZOOMOUT 2334

// Delete the word to the left of the caret.
#define SCINTILLA_CMD_DELWORDLEFT 2335

// Delete the word to the right of the caret.
#define SCINTILLA_CMD_DELWORDRIGHT 2336

// Cut the line containing the caret.
#define SCINTILLA_CMD_LINECUT 2337

// Delete the line containing the caret.
#define SCINTILLA_CMD_LINEDELETE 2338

// Switch the current line with the previous.
#define SCINTILLA_CMD_LINETRANSPOSE 2339

// Duplicate the current line.
#define SCINTILLA_CMD_LINEDUPLICATE 2404

// Transform the selection to lower case.
#define SCINTILLA_CMD_LOWERCASE 2340

// Transform the selection to upper case.
#define SCINTILLA_CMD_UPPERCASE 2341

// Scroll the document down, keeping the caret visible.
#define SCINTILLA_CMD_LINESCROLLDOWN 2342

// Scroll the document up, keeping the caret visible.
#define SCINTILLA_CMD_LINESCROLLUP 2343

// Delete the selection or if no selection, the character before the caret.
// Will not delete the character before at the start of a line.
#define SCINTILLA_CMD_DELETEBACKNOTLINE 2344

// Move caret to first position on display line.
#define SCINTILLA_CMD_HOMEDISPLAY 2345

// Move caret to first position on display line extending selection to
// new caret position.
#define SCINTILLA_CMD_HOMEDISPLAYEXTEND 2346

// Move caret to last position on display line.
#define SCINTILLA_CMD_LINEENDDISPLAY 2347

// Move caret to last position on display line extending selection to new
// caret position.
#define SCINTILLA_CMD_LINEENDDISPLAYEXTEND 2348

// These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
// except they behave differently when word-wrap is enabled:
// They go first to the start / end of the display line, like (Home|LineEnd)Display
// The difference is that, the cursor is already at the point, it goes on to the start
// or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
#define SCINTILLA_CMD_HOMEWRAP 2349
#define SCINTILLA_CMD_HOMEWRAPEXTEND 2450
#define SCINTILLA_CMD_LINEENDWRAP 2451
#define SCINTILLA_CMD_LINEENDWRAPEXTEND 2452
#define SCINTILLA_CMD_VCHOMEWRAP 2453
#define SCINTILLA_CMD_VCHOMEWRAPEXTEND 2454

// Copy the line containing the caret.
#define SCINTILLA_CMD_LINECOPY 2455

// Move to the previous change in capitalisation.
#define SCINTILLA_CMD_WORDPARTLEFT 2390

// Move to the previous change in capitalisation extending selection
// to new caret position.
#define SCINTILLA_CMD_WORDPARTLEFTEXTEND 2391

// Move to the change next in capitalisation.
#define SCINTILLA_CMD_WORDPARTRIGHT 2392

// Move to the next change in capitalisation extending selection
// to new caret position.
#define SCINTILLA_CMD_WORDPARTRIGHTEXTEND 2393

// Delete back from the current position to the start of the line.
#define SCINTILLA_CMD_DELLINELEFT 2395

// Delete forwards from the current position to the end of the line.
#define SCINTILLA_CMD_DELLINERIGHT 2396

// Move caret between paragraphs (delimited by empty lines).
#define SCINTILLA_CMD_PARADOWN 2413
#define SCINTILLA_CMD_PARADOWNEXTEND 2414
#define SCINTILLA_CMD_PARAUP 2415
#define SCINTILLA_CMD_PARAUPEXTEND 2416

// Move caret down one line, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_LINEDOWNRECTEXTEND 2426

// Move caret up one line, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_LINEUPRECTEXTEND 2427

// Move caret left one character, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_CHARLEFTRECTEXTEND 2428

// Move caret right one character, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_CHARRIGHTRECTEXTEND 2429

// Move caret to first position on line, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_HOMERECTEXTEND 2430

// Move caret to before first visible character on line.
// If already there move to first character on line.
// In either case, extend rectangular selection to new caret position.
#define SCINTILLA_CMD_VCHOMERECTEXTEND 2431

// Move caret to last position on line, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_LINEENDRECTEXTEND 2432

// Move caret one page up, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_PAGEUPRECTEXTEND 2433

// Move caret one page down, extending rectangular selection to new caret position.
#define SCINTILLA_CMD_PAGEDOWNRECTEXTEND 2434

// Move caret to top of page, or one page up if already at top of page.
#define SCINTILLA_CMD_STUTTEREDPAGEUP 2435

// Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
#define SCINTILLA_CMD_STUTTEREDPAGEUPEXTEND 2436

// Move caret to bottom of page, or one page down if already at bottom of page.
#define SCINTILLA_CMD_STUTTEREDPAGEDOWN 2437

// Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
#define SCINTILLA_CMD_STUTTEREDPAGEDOWNEXTEND 2438

// Move caret left one word, position cursor at end of word.
#define SCINTILLA_CMD_WORDLEFTEND 2439

// Move caret left one word, position cursor at end of word, extending selection to new caret position.
#define SCINTILLA_CMD_WORDLEFTENDEXTEND 2440

// Move caret right one word, position cursor at end of word.
#define SCINTILLA_CMD_WORDRIGHTEND 2441

// Move caret right one word, position cursor at end of word, extending selection to new caret position.
#define SCINTILLA_CMD_WORDRIGHTENDEXTEND 2442
