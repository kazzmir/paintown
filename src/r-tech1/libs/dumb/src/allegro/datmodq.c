/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /       '   '  '
 *  |  | \  \       |  |    ||         |   \/   |         .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |         '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |         .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/ynamic    \____/niversal  /__\  /____\usic   /|  .  . ibliotheque
 *                                                      /  \
 *                                                     / .  \
 * datmodq.c - Integration of MOD files with          / / \  \
 *             Allegro's datafiles.                  | <  /   \_
 *                                                   |  \/ /\   /
 * By entheh.                                         \_  /  > /
 *                                                      | \ / /
 *                                                      |  ' /
 *                                                       \__/
 */

#include <allegro.h>

#include "aldumb.h"
#include "internal/aldumb.h"

static void *dat_read_mod_quick(PACKFILE *f, long size) {
    DUMBFILE *df;
    DUH *duh;

    (void)size;

    df = dumbfile_open_packfile(f);

    if (!df)
        return NULL;

    duh = dumb_read_mod_quick(df, 2);

    dumbfile_close(df);

    return duh;
}

/* dumb_register_dat_mod_quick(): tells Allegro about the MOD datafile object.
 * If you intend to load a datafile containing a MOD object, you must call this
 * function first. It is recommended you pass DUMB_DAT_MOD, but you may have
 * a reason to use a different type (perhaps you already have a datafile with
 * MOD files in and they use a different type).
 *
 * This installs the quick loader: the song length and fast seek points are
 * not calculated.
 */
void dumb_register_dat_mod_quick(long type) {
    register_datafile_object(type, &dat_read_mod_quick, &_dat_unload_duh);
}
