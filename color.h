/**
 * @file    color.h
 * @brief   ANSI escape-code macros for terminal color theming.
 *
 * Provides two layers of macros:
 *
 *   Layer 1 — Raw ANSI codes:
 *       Direct escape sequences for standard terminal colors.
 *       Format: "\033[<style>;<color_code>m"
 *         - \033   is the ESC character (octal 33).
 *         - [0;3Xm selects a regular foreground color.
 *         - [1;3Xm selects a bold foreground color.
 *         - [0m    resets all attributes to terminal default.
 *
 *   Layer 2 — Semantic aliases:
 *       Named by purpose rather than color (e.g. CLR_SUCCESS, CLR_ERROR).
 *       main.c and display.c use ONLY the semantic aliases. This means the
 *       entire color scheme can be changed by editing only this file.
 *
 * Usage pattern:
 *   printf(CLR_SUCCESS "[INFO] : Inserted.\n" COLOR_RESET);
 *
 * The COLOR_RESET macro MUST close every colored printf. Without it, the
 * terminal color "bleeds" into subsequent output, including the shell prompt.
 *
 * Compatibility:
 *   ANSI escape codes are supported by virtually all modern POSIX terminals
 *   (Linux, macOS, WSL). They are NOT supported by the Windows Command Prompt
 *   (cmd.exe) without enabling Virtual Terminal Processing. If building for
 *   bare Windows, replace the macro bodies with empty strings "".
 */

#ifndef COLOR_H
#define COLOR_H


/* ═══════════════════════════════════════════════════════════════════════════
 * LAYER 1 — RAW ANSI ESCAPE CODES
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Resets all terminal attributes (color, bold, underline) to default. */
#define COLOR_RESET     "\033[0m"

/* Regular (non-bold) foreground colors */
#define COLOR_RED       "\033[0;31m"  /**< Standard red text.              */
#define COLOR_GREEN     "\033[0;32m"  /**< Standard green text.            */
#define COLOR_YELLOW    "\033[0;33m"  /**< Standard yellow text.           */
#define COLOR_BLUE      "\033[0;34m"  /**< Standard blue text.             */
#define COLOR_MAGENTA   "\033[0;35m"  /**< Standard magenta text.          */
#define COLOR_CYAN      "\033[0;36m"  /**< Standard cyan text.             */
#define COLOR_WHITE     "\033[0;37m"  /**< Standard white text.            */
#define COLOR_GRAY      "\033[0;90m"  /**< Bright-black (dark gray) text.  */

/* Bold foreground colors (brighter and heavier weight) */
#define COLOR_BOLD_RED      "\033[1;31m"  /**< Bold red — errors, RED nodes.   */
#define COLOR_BOLD_GREEN    "\033[1;32m"  /**< Bold green — success messages.  */
#define COLOR_BOLD_YELLOW   "\033[1;33m"  /**< Bold yellow — menu options.     */
#define COLOR_BOLD_CYAN     "\033[1;36m"  /**< Bold cyan — title banner.       */
#define COLOR_BOLD_WHITE    "\033[1;37m"  /**< Bold white — input prompts.     */
#define COLOR_BOLD_MAGENTA  "\033[1;35m"  /**< Bold magenta — reserved.        */


/* ═══════════════════════════════════════════════════════════════════════════
 * LAYER 2 — SEMANTIC ALIASES
 * These are the macros used in main.c and display.c.
 * Change the color scheme here without touching any other file.
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Menu title banner (e.g., "RED-BLACK TREE - MAIN MENU"). */
#define CLR_TITLE       COLOR_BOLD_CYAN

/** Horizontal divider lines flanking the menu. */
#define CLR_DIVIDER     COLOR_CYAN

/** Individual menu option lines (e.g., "1. Insert"). */
#define CLR_OPTION      COLOR_BOLD_YELLOW

/** Input prompt text (e.g., "Enter your choice:"). */
#define CLR_PROMPT      COLOR_BOLD_WHITE

/**
 * Success/confirmation messages.
 * Applied to: "Element inserted", "Element deleted", MIN/MAX found, etc.
 */
#define CLR_SUCCESS     COLOR_BOLD_GREEN

/**
 * Error and empty-state messages.
 * Applied to: "Tree is empty", "Element not found", "Invalid option".
 */
#define CLR_ERROR       COLOR_BOLD_RED

/** General informational labels (e.g., "[Tree - In-Order Traversal]"). */
#define CLR_INFO        COLOR_CYAN

/**
 * Color for RED tree nodes in display output.
 * Bold red ensures RED nodes are visually distinct from terminal default text,
 * even on terminals with a dark background.
 */
#define CLR_NODE_RED    COLOR_BOLD_RED

/**
 * Color for BLACK tree nodes in display output.
 * Gray (bright-black) provides contrast without being as loud as white,
 * letting RED nodes stand out naturally when both appear side-by-side.
 */
#define CLR_NODE_BLACK  COLOR_GRAY


#endif /* COLOR_H */