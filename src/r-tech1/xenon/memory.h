#ifndef _rtech1_3ac0ba2587944345f2241085269c24f8
#define _rtech1_3ac0ba2587944345f2241085269c24f8

#ifdef USE_ALLEGRO

#include <allegro.h>

namespace Memory{
    struct memory{
        memory(unsigned char * stream, int length):
            stream(stream),
            position(stream),
            length(length){
        }

        int getSize() const {
            return position - stream;
        }

        /* points to the head */
        unsigned char * stream;
        /* points to the current position */
        unsigned char * position;
        int length;
    };

    static int pf_fclose(void *userdata){
        return 0;
        /* nothing */
    }

    static int pf_getc(void *userdata){
        memory * m = (memory*) userdata;
        if (m->position < m->stream + m->length){
            unsigned char x = *m->position;
            m->position += 1;
            return x;
        }
        return EOF;
    }

    static int pf_ungetc(int c, void *userdata){
        memory * m = (memory*) userdata;
        if (m->position > m->stream){
            m->position -= 1;
            return c;
        } else {
            return EOF;
        }
    }

    static long pf_fread(void *p, long n, void *userdata){
        memory *m = (memory*) userdata;
        unsigned char *cp = (unsigned char *)p;
        long i;
        int c;

        for (i=0; i<n; i++) {
            if ((c = pf_getc(m)) == EOF)
                break;

            *(cp++) = c;
        }

        return i;
    }

    static int pf_putc(int c, void *userdata){
        memory * m = (memory*) userdata;
        if (m->position < m->stream + m->length){
            *m->position = c;
            m->position += 1;
            return c;
        }
        return EOF;

    }

    static long pf_fwrite(const void *p, long n, void *userdata){
        memory *m = (memory*) userdata;
        const unsigned char * cp = (const unsigned char *) p;
        long i;
        int c;

        /* probably should replace this with memcpy */

        for (i=0; i<n; i++) {
            if ((c = pf_putc(cp[i], userdata)) == EOF)
                break;
        }

        return i;
    }

    static int pf_fseek(void *userdata, int offset){
        memory * m = (memory*) userdata;
        if (offset >= 0 && m->position + offset < m->stream + m->length){
            m->position += offset;
            return 0;
        } else {
            return -1;
        }
    }

    static int pf_feof(void *userdata){
        memory * m = (memory*) userdata;
        return m->position >= m->stream + m->length;
    }

    static int pf_ferror(void *userdata){
        memory * m = (memory*) userdata;
        return m->position < m->stream || m->position >= m->stream + m->length;
    }

    static PACKFILE_VTABLE makeTable(){
        PACKFILE_VTABLE table;
        table.pf_fclose = Memory::pf_fclose;
        table.pf_getc = Memory::pf_getc;
        table.pf_ungetc = Memory::pf_ungetc;
        table.pf_fread = Memory::pf_fread;
        table.pf_putc = Memory::pf_putc;
        table.pf_fwrite = Memory::pf_fwrite;
        table.pf_fseek = Memory::pf_fseek;
        table.pf_feof = Memory::pf_feof;
        table.pf_ferror = Memory::pf_ferror;
        return table;
    }
}

#endif

#endif
