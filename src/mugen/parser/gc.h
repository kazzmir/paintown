#ifndef _paintown_parser_gc_h
#define _paintown_parser_gc_h

#include <list>
#include "../ast/all.h"

namespace GC{

/* poor man's garbage collection.
 * after parsing every pointer stored in this list is checked to see if it
 * is stored in the resulting AST. If the AST does not know about the pointer
 * then no one else does and the pointer should be deleted.
 *
 * void* doesn't work because it doesn't call the destructor. instead use
 * a new class, Collectable, with constructors and fields for every class
 * that is allocated. Collectable will call the appropriate destructor.
 */
static std::list<Ast::Collectable> saved_pointers;
typedef std::list<Ast::Section*> SectionList;

template<class X>
void save(const X x){
    saved_pointers.push_back(Ast::Collectable(x));
}

/* garbage collection */
void cleanup(SectionList * list){
    /* pointers that we should keep will be marked true in the map */
    std::map<const void*, bool> marks;

    /* first mark roots */
    marks[list] = true;
    for (SectionList::iterator it = list->begin(); it != list->end(); it++){
        Ast::Section * section = *it;
        section->mark(marks);
    }

    /* all unmarked pointers should be deleted but since the destructors
     * of AST nodes will delete child objects we only need to delete
     * AST nodes that are not the child object of another node.
     * If we mark every single node and add up all the times a node was marked
     * then nodes marked only once will be top-level nodes. Those are the
     * nodes we can destroy safely and ensure the rest of the nodes will
     * die with them.
     *
     * this has worst-case performance of O(N^2)
     */
    std::map<const void *, int> memory;
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;

        /* only look at unmarked objects */
        if (! marks[collect.pointer()]){
            std::map<const void *, bool> temp;
            collect.mark(temp);

            /* merge marks into the total count */
            for (std::map<const void *, bool>::iterator mit = temp.begin(); mit != temp.end(); mit++){
                memory[mit->first] += 1;
            }
        }
    }

    /* finally destroy the nodes with no parents */
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;
        if (memory[collect.pointer()] == 1){
            collect.destroy();
        }
    }
    // std::cout << "Destroying everything" << std::endl;
    saved_pointers.clear();
}

} /* GC */

#endif
