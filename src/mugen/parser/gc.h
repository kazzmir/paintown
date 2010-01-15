#ifndef _paintown_parser_gc_h
#define _paintown_parser_gc_h

#include <list>
#include "mugen/ast/all.h"

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
static void save(const X x){
    saved_pointers.push_back(Ast::Collectable(x));
}

static void check(){
    if (!(saved_pointers.size() == 0)){
        *(char *) 0 = 0;
    }
}

/* garbage collection */
static void cleanup(SectionList * list){
    /* pointers that we should keep will be marked true in the map */
    std::map<const void*, bool> marks;

    if (list != 0){
        /* first mark roots */
        marks[list] = true;
        for (SectionList::iterator it = list->begin(); it != list->end(); it++){
            Ast::Section * section = *it;
            section->mark(marks);
        }
    }

    /* `memory' is a map of objects to the number of times the object was marked.
     * an object could be marked more than once because the following loop
     * will mark every object. if an object is the child of some other object
     * then it (the child) will be marked once itself and once due to the marking
     * of the parent. That is, if the child is A and some object B contains A then
     * the following marks will occur:
     *   mark(A) -> A is marked
     *   mark(B) -> B is marked, then A is marked
     * So A will have a mark count of 2 and B will have a mark count of 1.
     */
    std::map<const void *, int> memory;

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
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;

        /* anything already marked in the `marks' map is a live object so we should
         * not consider it here.
         */
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
        
        /* a mark count of 1 signifies objects with no parents */
        if (memory[collect.pointer()] == 1){
            collect.destroy();
        }
    }
    // std::cout << "Destroying everything" << std::endl;
    saved_pointers.clear();
}

} /* GC */

#endif
