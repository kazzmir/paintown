#ifndef _paintown_parser_gc_h
#define _paintown_parser_gc_h

#include <list>
#include "mugen/ast/all.h"
#include <exception>

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
        throw std::exception();
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

    /* All live objects are now known from the `marks' map. Garbage collection thus
     * means deleting any object that is not in the `marks' map. That is, the set
     * of all objects is AllObjects and all live objects are LiveObjects so
     * the set DeadObjects is just:
     *   DeadObjects = AllObjects - LiveObjects
     * This is a straight-forward computation but actually destroying the DeadObjects
     * is not due to ownership of pointers.
     */

    /* `memory' is a map of objects to the number of times the object was marked.
     * an object could be marked more than once because the following loop
     * will mark every object. if an object is the child of some other object
     * then it (the child) will be marked once itself and once due to the marking
     * of the parent. That is, if the child is A and some object B contains A then
     * the following marks will occur:
     *   mark(A) -> A is marked
     *   mark(B) -> B is marked, then A is marked
     * So A will have a mark count of 2 and B will have a mark count of 1.
     *
     * Each AST node only has one parent so there is no way for A to be live
     * but B to be dead.
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
     * this has worst-case performance of O(N^2). The performance depends on the
     * layout of the tree of AST nodes. For a flat tree like
     *   n - n - n - ...
     * Where all nodes are siblings the performance should be O(n), but if all nodes
     * form a vertical tree then it will be O(n^2)
     *   n
     *   |
     *   n
     *   |
     *   n
     *  ...
     *
     *  One might wonder why nodes destroy their children nodes at all, why not just
     *  let the garbage collector destroy all nodes that haven't been marked as live?
     *  The answer is that the live AST tree that is returned to the game engine
     *  is returned as a single root node. The game engine will delete that node to
     *  destroy the entire tree. If destroying the root node didn't destroy the entire
     *  tree then the game engine would have to get a list of all AST nodes and iterate
     *  through it, destroying all nodes as they came up. I suppose that could be done
     *  but then the parser would have to return a root AST node and an extra data
     *  structure that contained a list of all live nodes (or somehow shove them
     *  into the root AST node). It seems the most elegant solution is to just
     *  make the garbage collector do more work.
     *
     *  Perhaps a more efficient method than this would be if each node knew its parent
     *  then the check for liveness can consist of checking if the node is in
     *  the `marks' map, if not then check if the node is already in some `dead' map,
     *  and if not then find the root of the node, add the root to a to-delete-later
     *  list, and mark all children and parent nodes as dead. In psuedo-code:
     *    check(node):
     *      # Do we know about this node?
     *      if live(node) then return
     *      if dead(node) then return
     *
     *      # Unknown node, so update bookkeeping
     *      make_dead(node)
     *      delete_later(find_root(node))
     *      for each parent_node in node:
     *        make_dead(parent_node)
     *      for each sub_node in node:
     *        make_dead(sub_node)
     *
     *  So lets say we have a vertical tree of depth N and the entire tree is dead.
     *   node1
     *   |
     *   node2
     *  ...
     *   |
     *   nodeN
     *
     *  And the first node we find in the saved_pointers list is node(N/2), i.e the
     *  middle of the tree. We mark all parents as dead (N/2) and children as dead (N/2).
     *  Also assume the root is found while marking parents, so that doesn't incur an
     *  extra cost. Total work for one marking the middle node is N. Looking at all other
     *  nodes takes N time, but for each node they are already marked as dead so work
     *  per node is 1.
     *
     *  Total work is 2N for a vertical tree.
     *
     *  Currently, AST nodes do not keep track of their parents. If garbage collection
     *  is a bottle-neck then this algorithm should be implemented.
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
