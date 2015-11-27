// ===================================================================
//Comparable.h - Class for comparing AVL tree nodes
// Written by Brad Appleton (1997)
// http://www.bradapp.com/ftp/src/libs/C++/AvlTrees.html

#ifndef COMPARABLE_H
#define COMPARABLE_H

#include <ostream>

using namespace std;

// cmp_t is an enumeration type indicating the result of a
// comparison.
//     NOTE: I would place this inside the Comparable class but
//           when I do, g++ complains when I use cmp_t. Even
//           when I prefix it with Comparable:: or Comparable<KeyType>::
//           (If you can get this working please let me know)
//
enum  cmp_t {
   MIN_CMP = -1,   // less than
   EQ_CMP  = 0,    // equal to
   MAX_CMP = 1     // greater than
};

// Class "Comparable" corresponds to an arbitrary comparable element
// with a keyfield that has an ordering relation. The template parameter
// KeyType is the "type" of the keyfield
//
template <class KeyType>
class Comparable {
private:
   KeyType myKey;

public:
   Comparable(KeyType key) : myKey(key) {};
   virtual ~Comparable() {};

   // Use default copy-ctor, assignment, & destructor

      // Compare this item against the given key & return the result
   virtual cmp_t Compare(KeyType key) const {
      return (key == myKey) ? EQ_CMP
                            : ((key < myKey) ? MIN_CMP : MAX_CMP);
   }

     // Get the key-field of an item
   KeyType Key() const { return  myKey; }

     // Print on output stream
   ostream &
   Print(ostream & os) const {
      os << myKey;
      return  os;
   }
};

template <class KeyType>
inline ostream & operator<<(ostream & os, Comparable<KeyType> item) {
   return  item.Print(os);
}

#endif  /* COMPARABLE_H */


// ===================================================================
// Avl.h - Implementation of an Avl balanced tree
// Written by Brad Appleton (1997)
// http://www.bradapp.com/ftp/src/libs/C++/AvlTrees.html

#ifndef AVL_H
#define AVL_H

#include <stddef.h>
#include <iostream>
#include "Comparable.h"

using namespace std;

// Indices into a subtree array

// AvlNode -- Class to implement an AVL Tree
//
template <class KeyType>
class AvlNode {
public:
    // Max number of subtrees per node
    enum  { MAX_SUBTREES = 2 };
    enum  dir_t { LEFT = 0, RIGHT = 1 };

    // Return the opposite direction of the given index
    static  dir_t
        Opposite(dir_t dir) {
        return dir_t(1 - int(dir));
    }

    // ----- Constructors and destructors:

    AvlNode(Comparable<KeyType>  * item=NULL);
    virtual ~AvlNode(void);

    // ----- Query attributes:

    // Get this node's data
    Comparable<KeyType> *
        Data() const { return  myData; }

    // Get this node's key field
    KeyType
        Key() const { return  myData->Key(); }

    // Query the balance factor, it will be a value between -1 .. 1
    // where:
    //     -1 => left subtree is taller than right subtree
    //      0 => left and right subtree are equal in height
    //      1 => right subtree is taller than left subtree
    short
        Bal(void) const { return  myBal; }

    // Get the item at the top of the left/right subtree of this
    // item (the result may be NULL if there is no such item).
    //
    AvlNode *
        Subtree(dir_t dir) const { return  mySubtree[dir]; }

    AvlNode<KeyType>*
        Parent(AvlNode<KeyType> *);

    // ----- Search/Insert/Delete
    //
    //   NOTE: These are all static functions instead of member functions
    //         because most of them need to modify the given tree root
    //         pointer. If these were instance member functions than
    //         that would correspond to having to modify the 'this'
    //         pointer, which is not allowed in C++. Most of the
    //         functions that are static and which take an AVL tree
    //         pointer as a parameter are static for this reason.

    // Look for the given key, return NULL if not found,
    // otherwise return the item's address.
    static AvlNode<KeyType> *
        Search(KeyType key, AvlNode<KeyType> * root, cmp_t cmp=EQ_CMP);

    // Insert the given key, return a pointer to the node if it was inserted,
    // otherwise return NULL
    static AvlNode<KeyType> *
        Insert(Comparable<KeyType> * item, AvlNode<KeyType> * & root);

    // Delete the given key from the tree. Return the corresponding
    // node, or return NULL if it was not found.
    static Comparable<KeyType> *
        Delete(KeyType key, AvlNode<KeyType> * & root, cmp_t cmp=EQ_CMP);

    // Verification

    // Return the height of this tree
    int
        Height() const;

    // Verify this tree is a valid AVL tree, return TRUE if it is,
    // return FALSE otherwise
    int
        Check() const;

    // If you want to provide your own allocation scheme than simply
    // #define the preprocessor manifest constant named CUSTOM_ALLOCATE
    // and make sure you provide and link with your own overloaded
    // versions of operators "new" and "delete" for this class.
#ifdef CUSTOM_ALLOCATE
    void *
        operator  new(size_t);

    void
        operator  delete(void *);
#endif  /* CUSTOM_ALLOCATE */


private:
    // Use mnemonic constants for valid balance-factor values
    enum balance_t { LEFT_HEAVY = -1, BALANCED = 0, RIGHT_HEAVY = 1 };

    // Use mnemonic constants for indicating a change in height
    enum height_effect_t { HEIGHT_NOCHANGE = 0, HEIGHT_CHANGE = 1 };

    // Return true if the tree is too heavy on the left side
    inline static int
        LEFT_IMBALANCE(short bal) {
        return (bal < LEFT_HEAVY);
    }

    // Return true if the tree is too heavy on the right side
    inline static int
        RIGHT_IMBALANCE(short bal) {
        return (bal > RIGHT_HEAVY);
    }

    // ----- Private data

    Comparable<KeyType> * myData;  // Data field
    AvlNode<KeyType>    * mySubtree[MAX_SUBTREES];   // Pointers to subtrees
    short      myBal;   // Balance factor

    // Reset all subtrees to null and clear the balance factor
    void
        Reset(void) {
        myBal = 0 ;
        mySubtree[LEFT] = mySubtree[RIGHT] = NULL ;
    }

    // ----- Routines that do the *real* insertion/deletion

    // Insert the given key into the given tree. Return the node if
    // it already exists. Otherwise return NULL to indicate that
    // the key was successfully inserted.  Upon return, the "change"
    // parameter will be '1' if the tree height changed as a result
    // of the insertion (otherwise "change" will be 0).
    static AvlNode<KeyType> *
        Insert(Comparable<KeyType> * item,
               AvlNode<KeyType> * & root,
               int & change);

    // Delete the given key from the given tree. Return NULL if the
    // key is not found in the tree. Otherwise return a pointer to the
    // node that was removed from the tree.  Upon return, the "change"
    // parameter will be '1' if the tree height changed as a result
    // of the deletion (otherwise "change" will be 0).
    static Comparable<KeyType> *
        Delete(KeyType key,
               AvlNode<KeyType> * & root,
               int & change,
               cmp_t cmp=EQ_CMP);

    // Routines for rebalancing and rotating subtrees

    // Perform an XX rotation for the given direction 'X'.
    // Return 1 if the tree height changes due to rotation,
    // otherwise return 0.
    static int
        RotateOnce(AvlNode<KeyType> * & root, dir_t dir);

    // Perform an XY rotation for the given direction 'X'
    // Return 1 if the tree height changes due to rotation,
    // otherwise return 0.
    static int
        RotateTwice(AvlNode<KeyType> * & root, dir_t dir);

    // Rebalance a (sub)tree if it has become imbalanced
    static int
        ReBalance(AvlNode<KeyType> * & root);

    // Perform a comparison of the given key against the given
    // item using the given criteria (min, max, or equivalence
    // comparison). Returns:
    //   EQ_CMP if the keys are equivalent
    //   MIN_CMP if this key is less than the item's key
    //   MAX_CMP if this key is greater than item's key
    cmp_t
        Compare(KeyType key, cmp_t cmp=EQ_CMP) const;

private:
    // Disallow copying and assignment
    AvlNode(const AvlNode<KeyType> &);
    AvlNode & operator=(const AvlNode<KeyType> &);

};


// Class AvlTree is a simple container object to "house" an AvlNode
// that represents the root-node of and AvlTree. Most of the member
// functions simply delegate to the root AvlNode.
template <class KeyType>
class AvlTree {
private:
    // Disallow copying and assignment
    AvlTree(const AvlTree<KeyType> &);
    AvlTree & operator=(const AvlTree<KeyType> &);

public:
    // Member data
    AvlNode<KeyType> * myRoot;   // The root of the tree

    // Constructor and destructor
AvlTree() : myRoot(NULL) {};
    ~AvlTree() { if (myRoot)  delete myRoot; }

    // Dump the tree to the given output stream
    void DumpTree() const;

    // See if the tree is empty
    int IsEmpty() const {
        return  (myRoot == NULL);
    }

    // Search, Insert, Delete, and Check
    AvlNode<KeyType> *
        Search(KeyType key, cmp_t cmp=EQ_CMP) {
        return  AvlNode<KeyType>::Search(key, myRoot, cmp);
    }

    AvlNode<KeyType> *
        Insert(Comparable<KeyType> * item) {
        return  AvlNode<KeyType>::Insert(item, myRoot);
    }

    Comparable<KeyType> *
        Delete(KeyType key, cmp_t cmp=EQ_CMP) {
        return  AvlNode<KeyType>::Delete(key, myRoot, cmp);
    }

    // As with all binary trees, a node's in-order successor is the
    // left-most child of its right subtree, and a node's in-order predecessor
    // is the right-most child of its left subtree.
    AvlNode<KeyType>*Next(AvlNode<KeyType>*node) {
        AvlNode<KeyType> *q, *p = node->Subtree(AvlNode<KeyType>::RIGHT);
        if (p) {
            while (p->Subtree(AvlNode<KeyType>::LEFT)) p = p->Subtree(AvlNode<KeyType>::LEFT);
            return p;
        } else {
            // find parent, check if node is on left subtree
            q = node;
            p = node->Parent(myRoot);
            while (p && (q == p->Subtree(AvlNode<KeyType>::RIGHT))) {
                q = p;
                p = p->Parent(myRoot);
            }

            return p;
        }
    }

    AvlNode<KeyType>*Prev(AvlNode<KeyType>*node) {
        AvlNode<KeyType> *q, *p = node->Subtree(AvlNode<KeyType>::LEFT);
        if (p) {
            while (p->Subtree(AvlNode<KeyType>::RIGHT)) p = p->Subtree(AvlNode<KeyType>::RIGHT);
            return p;
        } else {
            // find parent, check if node is on left subtree
            q = node;
            p = node->Parent(myRoot);
            while (p && (q == p->Subtree(AvlNode<KeyType>::LEFT))) {
                q = p;
                p = p->Parent(myRoot);
            }

            return p;
        }
    }

    int
        Check() const {
        return  (myRoot) ? myRoot->Check() : 1;
    }
};

// ---------------------------------------------------------------- Definitions

// Return the minimum of two numbers
inline static int
MIN(int a, int b) {
    return  ((a) < (b)) ? (a) : (b);
}

// Return the maximum of two numbers
inline static int
MAX(int a, int b) {
    return  ((a) > (b)) ? (a) : (b);
}


// ----------------------------------------------- Constructors and Destructors

template <class KeyType>
AvlNode<KeyType>::AvlNode(Comparable<KeyType> * item)
: myData(item), myBal(0)
{
    Reset();
}

template <class KeyType>
AvlNode<KeyType>::~AvlNode(void) {
    if (mySubtree[LEFT])  delete  mySubtree[LEFT];
    if (mySubtree[RIGHT]) delete  mySubtree[RIGHT];
}

// ------------------------------------------------- Rotating and Re-Balancing

template <class KeyType>
int
AvlNode<KeyType>::RotateOnce(AvlNode<KeyType> * & root, dir_t dir)
{
    dir_t  otherDir = Opposite(dir);
    AvlNode<KeyType> * oldRoot = root;

    // See if otherDir subtree is balanced. If it is, then this
    // rotation will *not* change the overall tree height.
    // Otherwise, this rotation will shorten the tree height.
    int  heightChange = (root->mySubtree[otherDir]->myBal == 0)
        ? HEIGHT_NOCHANGE
        : HEIGHT_CHANGE;

    // assign new root
    root = oldRoot->mySubtree[otherDir];

    // new-root exchanges it's "dir" mySubtree for it's parent
    oldRoot->mySubtree[otherDir] = root->mySubtree[dir];
    root->mySubtree[dir] = oldRoot;

    // update balances
    oldRoot->myBal = -((dir == LEFT) ? --(root->myBal) : ++(root->myBal));

    return  heightChange;
}

template <class KeyType>
int
AvlNode<KeyType>::RotateTwice(AvlNode<KeyType> * & root, dir_t dir)
{
    dir_t  otherDir = Opposite(dir);
    AvlNode<KeyType> * oldRoot = root;
    AvlNode<KeyType> * oldOtherDirSubtree = root->mySubtree[otherDir];

    // assign new root
    root = oldRoot->mySubtree[otherDir]->mySubtree[dir];

    // new-root exchanges it's "dir" mySubtree for it's grandparent
    oldRoot->mySubtree[otherDir] = root->mySubtree[dir];
    root->mySubtree[dir] = oldRoot;

    // new-root exchanges it's "other-dir" mySubtree for it's parent
    oldOtherDirSubtree->mySubtree[dir] = root->mySubtree[otherDir];
    root->mySubtree[otherDir] = oldOtherDirSubtree;

    // update balances
    root->mySubtree[LEFT]->myBal  = -MAX(root->myBal, 0);
    root->mySubtree[RIGHT]->myBal = -MIN(root->myBal, 0);
    root->myBal = 0;

    // A double rotation always shortens the overall height of the tree
    return  HEIGHT_CHANGE;
}

template <class KeyType>
int
AvlNode<KeyType>::ReBalance(AvlNode<KeyType> * & root) {
    int  heightChange = HEIGHT_NOCHANGE;

    if (LEFT_IMBALANCE(root->myBal)) {
        // Need a right rotation
        if (root->mySubtree[LEFT]->myBal  ==  RIGHT_HEAVY) {
            // RL rotation needed
            heightChange = RotateTwice(root, RIGHT);
        } else {
            // RR rotation needed
            heightChange = RotateOnce(root, RIGHT);
        }
    } else if (RIGHT_IMBALANCE(root->myBal)) {
        // Need a left rotation
        if (root->mySubtree[RIGHT]->myBal  ==  LEFT_HEAVY) {
            // LR rotation needed
            heightChange = RotateTwice(root, LEFT);
        } else {
            // LL rotation needed
            heightChange = RotateOnce(root, LEFT);
        }
    }

    return  heightChange;
}

// ------------------------------------------------------- Comparisons

template <class KeyType>
cmp_t
AvlNode<KeyType>::Compare(KeyType key, cmp_t cmp) const
{
    switch (cmp) {
      default:
      case EQ_CMP :  // Standard comparison
        return  myData->Compare(key);

      case MIN_CMP :  // Find the minimal element in this tree
        return  (mySubtree[LEFT] == NULL) ? EQ_CMP : MIN_CMP;

      case MAX_CMP :  // Find the maximal element in this tree
        return  (mySubtree[RIGHT] == NULL) ? EQ_CMP : MAX_CMP;
    }
}

template <class KeyType>
AvlNode<KeyType>*
AvlNode<KeyType>::Parent(AvlNode<KeyType>* myRoot) {
    if (this == myRoot) return 0;

    AvlNode<KeyType> *p = myRoot;
    while (p) {
        if (this == p->Subtree(AvlNode<KeyType>::LEFT)) return p;
        if (this == p->Subtree(AvlNode<KeyType>::RIGHT)) return p;

        cmp_t  result = p->Compare(this->Key());
        if (result == MIN_CMP)
            p = p->Subtree(AvlNode<KeyType>::LEFT);
        else
            p = p->Subtree(AvlNode<KeyType>::RIGHT);
    }

    return 0;
}

// ------------------------------------------------------- Search/Insert/Delete

template <class KeyType>
AvlNode<KeyType> *
AvlNode<KeyType>::Search(KeyType key, AvlNode<KeyType> * root, cmp_t cmp)
{
    cmp_t result;
    while (root  &&  (result = root->Compare(key, cmp))) {
        root = root->mySubtree[(result < 0) ? LEFT : RIGHT];
    }
    return  (root) ? root : NULL;
}

template <class KeyType>
AvlNode<KeyType> *
AvlNode<KeyType>::Insert(Comparable<KeyType> *   item,
                         AvlNode<KeyType>    * & root)
{
    int  change;
    return  Insert(item, root, change);
}

template <class KeyType>
Comparable<KeyType> *
AvlNode<KeyType>::Delete(KeyType key, AvlNode<KeyType> * & root, cmp_t cmp)
{
    int  change;
    return  Delete(key, root, change, cmp);
}


template <class KeyType>
AvlNode<KeyType> *
AvlNode<KeyType>::Insert(Comparable<KeyType> *   item,
                         AvlNode<KeyType>    * & root,
                         int                   & change)
{
    // See if the tree is empty
    if (root == NULL) {
        // Insert new node here
        root = new AvlNode<KeyType>(item);
        change = HEIGHT_CHANGE;
        return root;
    }

    // Initialize
    AvlNode<KeyType> * found = NULL;
    int  increase = 0;

    // Compare items and determine which direction to search
    cmp_t  result = root->Compare(item->Key());
    dir_t  dir = (result == MIN_CMP) ? LEFT : RIGHT;

    if (result != EQ_CMP) {
        // Insert into "dir" subtree
        found = Insert(item, root->mySubtree[dir], change);
        if (!found) return NULL;     // already here - don't insert
        increase = result * change;  // set balance factor increment
    } else  {   // key already in tree at this node
        increase = HEIGHT_NOCHANGE;
        return NULL;
    }

    root->myBal += increase;    // update balance factor

    // ----------------------------------------------------------------------
    // re-balance if needed -- height of current tree increases only if its
    // subtree height increases and the current tree needs no rotation.
    // ----------------------------------------------------------------------

    change =  (increase && root->myBal)
        ? (1 - ReBalance(root))
        : HEIGHT_NOCHANGE;
    return  found;
}


template <class KeyType>
Comparable<KeyType> *
AvlNode<KeyType>::Delete(KeyType              key,
                         AvlNode<KeyType> * & root,
                         int                & change,
                         cmp_t                cmp)
{
    // See if the tree is empty
    if (root == NULL) {
        // Key not found
        change = HEIGHT_NOCHANGE;
        return  NULL;
    }

    // Initialize
    Comparable<KeyType> * found = NULL;
    int  decrease = 0;

    // Compare items and determine which direction to search
    cmp_t  result = root->Compare(key, cmp);
    dir_t  dir = (result == MIN_CMP) ? LEFT : RIGHT;

    if (result != EQ_CMP) {
        // Delete from "dir" subtree
        found = Delete(key, root->mySubtree[dir], change, cmp);
        if (! found)  return  found;   // not found - can't delete
        decrease = result * change;    // set balance factor decrement
    } else  {   // Found key at this node
        found = root->myData;  // set return value

        // ---------------------------------------------------------------------
        // At this point we know "result" is zero and "root" points to
        // the node that we need to delete.  There are three cases:
        //
        //    1) The node is a leaf.  Remove it and return.
        //
        //    2) The node is a branch (has only 1 child). Make "root"
        //       (the pointer to this node) point to the child.
        //
        //    3) The node has two children. We swap items with the successor
        //       of "root" (the smallest item in its right subtree) and delete
        //       the successor from the right subtree of "root".  The
        //       identifier "decrease" should be reset if the subtree height
        //       decreased due to the deletion of the successor of "root".
        // ---------------------------------------------------------------------

        if ((root->mySubtree[LEFT] == NULL) &&
            (root->mySubtree[RIGHT] == NULL)) {
            // We have a leaf -- remove it
            delete  root;
            root = NULL;
            change = HEIGHT_CHANGE;    // height changed from 1 to 0
            return  found;
        } else if ((root->mySubtree[LEFT] == NULL) ||
                   (root->mySubtree[RIGHT] == NULL)) {
            // We have one child -- only child becomes new root
            AvlNode<KeyType> * toDelete = root;
            root = root->mySubtree[(root->mySubtree[RIGHT]) ? RIGHT : LEFT];
            change = HEIGHT_CHANGE;    // We just shortened the subtree
            // Null-out the subtree pointers so we dont recursively delete
            toDelete->mySubtree[LEFT] = toDelete->mySubtree[RIGHT] = NULL;
            delete  toDelete;
            return  found;
        } else {
            // We have two children -- find successor and replace our current
            // data item with that of the successor
            root->myData = Delete(key, root->mySubtree[RIGHT],
                                  decrease, MIN_CMP);
        }
    }

    root->myBal -= decrease;       // update balance factor

    // ------------------------------------------------------------------------
    // Rebalance if necessary -- the height of current tree changes if one
    // of two things happens: (1) a rotation was performed which changed
    // the height of the subtree (2) the subtree height decreased and now
    // matches the height of its other subtree (so the current tree now
    // has a zero balance when it previously did not).
    // ------------------------------------------------------------------------
    //change = (decrease) ? ((root->myBal) ? balance(root) : HEIGHT_CHANGE)
    //                    : HEIGHT_NOCHANGE ;
    if (decrease) {
        if (root->myBal) {
            change = ReBalance(root);  // rebalance and see if height changed
        } else {
            change = HEIGHT_CHANGE;   // balanced because subtree decreased
        }
    } else {
        change = HEIGHT_NOCHANGE;
    }

    return  found;
}

// --------------------------------------------------------------- Verification

template <class KeyType>
int
AvlNode<KeyType>::Height() const {
    int  leftHeight  = (mySubtree[LEFT])  ? mySubtree[LEFT]->Height()  : 0;
    int  rightHeight = (mySubtree[RIGHT]) ? mySubtree[RIGHT]->Height() : 0;
    return  (1 + MAX(leftHeight, rightHeight));
}

template <class KeyType>
int
AvlNode<KeyType>::Check() const {
    int  valid = 1;

    // First verify that subtrees are correct
    if (mySubtree[LEFT])   valid *= mySubtree[LEFT]->Check();
    if (mySubtree[RIGHT])  valid *= mySubtree[RIGHT]->Check();

    // Now get the height of each subtree
    int  leftHeight  = (mySubtree[LEFT])  ? mySubtree[LEFT]->Height()  : 0;
    int  rightHeight = (mySubtree[RIGHT]) ? mySubtree[RIGHT]->Height() : 0;

    // Verify that AVL tree property is satisfied
    int  diffHeight = rightHeight - leftHeight;
    if (LEFT_IMBALANCE(diffHeight) || RIGHT_IMBALANCE(diffHeight)) {
        valid = 0;
        cerr << "Height difference is " << diffHeight
             << " at node " << Key() << endl;
    }

    // Verify that balance-factor is correct
    if (diffHeight != myBal) {
        valid = 0;
        cerr << "Height difference " << diffHeight
             << " doesn't match balance-factor of " << myBal
             << " at node " << Key() << endl;
    }

    // Verify that search-tree property is satisfied
    if ((mySubtree[LEFT])
        &&
        (mySubtree[LEFT]->Compare(Key()) == MIN_CMP)) {
        valid = 0;
        cerr << "Node " << Key() << " is *smaller* than left subtree"
             << mySubtree[LEFT]->Key() << endl;
    }
    if ((mySubtree[RIGHT])
        &&
        (mySubtree[RIGHT]->Compare(Key()) == MAX_CMP)) {
        valid = 0;
        cerr << "Node " << Key() << " is *greater* than right subtree"
             << mySubtree[RIGHT]->Key() << endl;
    }

    return  valid;
}

//----------------------------------------------- Routines for dumping the tree

static inline ostream &
Indent(ostream & os, int len) {
    for (int i = 0; i < len; i++) {
        os << ' ';
    }
    return  os;
}

enum TraversalOrder { LTREE, KEY, RTREE };

template <class KeyType>
static void
Dump(ostream & os,
     TraversalOrder order,
     const AvlNode<KeyType> * node,
     int level=0)
{
    unsigned  len = (level * 5) + 1;
    if ((order == LTREE) && (node->Subtree(AvlNode<KeyType>::LEFT) == NULL)) {
        Indent(os, len) << "     **NULL**" << endl;
    }
    if (order == KEY) {
        Indent(os, len) << node->Key() << ":" << node->Bal() << endl;
    }
    if ((order == RTREE) && (node->Subtree(AvlNode<KeyType>::RIGHT) == NULL)) {
        Indent(os, len) << "     **NULL**" << endl;
    }
}

template <class KeyType>
static void
Dump(ostream & os, const AvlNode<KeyType> * node, int level=0)
{
    if (node == NULL) {
        os << "***EMPTY TREE***" << endl;
    } else {
        Dump(os, RTREE, node, level);
        if (node->Subtree(AvlNode<KeyType>::RIGHT)  !=  NULL) {
            Dump(os, node->Subtree(AvlNode<KeyType>::RIGHT), level+1);
        }
        Dump(os, KEY, node, level);
        if (node->Subtree(AvlNode<KeyType>::LEFT)  !=  NULL) {
            Dump(os, node->Subtree(AvlNode<KeyType>::LEFT), level+1);
        }
        Dump(os, LTREE, node, level);
    }// if non-empty tree
}

template <class KeyType>
void AvlTree<KeyType>::DumpTree() const {
    Dump(cout, myRoot);
}

#endif  /* AVL_H */


// ===================================================================
// simple_polygon.h - Class for a polygon
// Written by Glenn Burkhardt (2014)
/*
 * simple_polygon.h
 *
 */

#ifndef SIMPLE_POLYGON_H_
#define SIMPLE_POLYGON_H_

#include <stdlib.h>

typedef struct {
    double x, y;
} Point;

struct Polygon {
    Polygon(int npts) {
        n = npts;
        V = (Point*)malloc(npts * sizeof(Point));
    }

    ~Polygon() {
        free(V);
    }

public:
    int n;
    Point *V;  // should have n elements, V[n-1] != V[0]
};

typedef struct Polygon Polygon;

// simple_Polygon(): test if a Polygon P is simple or not
//     Input:  Pn = a polygon with n vertices V[]
//     Return: FALSE(0) = is NOT simple
//             TRUE(1)  = IS simple
bool simple_Polygon( Polygon &Pn );

#endif /* SIMPLE_POLYGON_H_ */


// ===================================================================
//simple_Polygon.cpp - Check if a polygon is simple
// Written by Dan Sunday (2001) (http://geomalgorithms.com/a09-_intersect-3.html)
// Modified by Glenn Burkhardt (2014) to integrate it with the AVL balanced tree


// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

// http://geomalgorithms.com/a09-_intersect-3.html#Simple-Polygons

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Avl.h"
#include "simple_polygon.h"


// Assume that classes are already given for the objects:
//    Point with 2D coordinates {float x, y;}
//    Polygon with n vertices {int n; Point *V;} with V[n]=V[0]
//    Tnode is a node element structure for a BBT
//    BBT is a class for a Balanced Binary Tree
//        such as an AVL, a 2-3, or a red-black tree
//===================================================================

enum SEG_SIDE { LEFT, RIGHT };

// xyorder(): determines the xy lexicographical order of two points
//      returns: (+1) if p1 > p2; (-1) if p1 < p2; and 0 if equal
int xyorder( Point* p1, Point* p2 )
{
    // test the x-coord first
    if (p1->x > p2->x) return 1;
    if (p1->x < p2->x) return (-1);
    // and test the y-coord second
    if (p1->y > p2->y) return 1;
    if (p1->y < p2->y) return (-1);
    // when you exclude all other possibilities, what remains is...
    return 0;  // they are the same point
}

// isLeft(): tests if point P2 is Left|On|Right of the line P0 to P1.
//      returns: >0 for left, 0 for on, and <0 for right of the line.
//      (see the January 2001 Algorithm on Area of Triangles)
inline double
isLeft( Point P0, Point P1, Point P2 )
{
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}
//===================================================================

class SLseg;
// EventQueue Class

// Event element data struct
typedef struct _event Event;
struct _event {
    int      edge;         // polygon edge i is V[i] to V[i+1]
    enum SEG_SIDE type;    // event type: LEFT or RIGHT vertex
    Point*   vertex;           // event vertex
    SLseg* seg;            // segment in tree
    Event* otherEnd;       // segment is [this.vertex, otherEnd.vertex]
};

int E_compare( const void* v1, const void* v2 ) // qsort compare two events
{
    Event**    pe1 = (Event**)v1;
    Event**    pe2 = (Event**)v2;

    int r = xyorder( (*pe1)->vertex, (*pe2)->vertex );
    if (r == 0) {
        if ((*pe1)->type == (*pe2)->type) return 0;
        if ((*pe1)->type == LEFT) return -1;
        else return 1;
    } else
        return r;
}

// the EventQueue is a presorted array (no insertions needed)
class EventQueue {
    int      ne;               // total number of events in array
    int      ix;               // index of next event on queue
    Event*   Edata;            // array of all events
    Event**  Eq;               // sorted list of event pointers
public:
    EventQueue(Polygon &P);    // constructor
    ~EventQueue(void)          // destructor
    {
        delete[] Eq;
        delete[] Edata;
    }

    Event*   next();                    // next event on queue
};

// EventQueue Routines
EventQueue::EventQueue( Polygon &P )
{
    ix = 0;
    ne = 2 * P.n;          // 2 vertex events for each edge
    Edata = (Event*)new Event[ne];
    Eq = (Event**)new Event*[ne];
    for (int i=0; i < ne; i++)          // init Eq array pointers
        Eq[i] = &Edata[i];

    // Initialize event queue with edge segment endpoints
    for (int i=0; i < P.n; i++) {       // init data for edge i
        Eq[2*i]->edge = i;
        Eq[2*i+1]->edge = i;
        Eq[2*i]->vertex   = &(P.V[i]);
        Eq[2*i]->otherEnd = Eq[2*i+1];
        Eq[2*i+1]->otherEnd = Eq[2*i];
        Eq[2*i]->seg = Eq[2*i+1]->seg = 0;

        Point *pi1 = (i+1 < P.n) ? &(P.V[i+1]):&(P.V[0]);
        Eq[2*i+1]->vertex = pi1;
        if (xyorder( &P.V[i], pi1) < 0) { // determine type
            Eq[2*i]->type   = LEFT;
            Eq[2*i+1]->type = RIGHT;
        }
        else {
            Eq[2*i]->type   = RIGHT;
            Eq[2*i+1]->type = LEFT;
        }
    }

    // Sort Eq[] by increasing x and y
    ::qsort( Eq, ne, sizeof(Event*), E_compare );
}

Event* EventQueue::next()
{
    if (ix >= ne)
        return (Event*)0;
    else
        return Eq[ix++];
}
//===================================================================


// SweepLine Class

// SweepLine segment data struct
class SLseg : public Comparable<SLseg*> {
public:
    int      edge;         // polygon edge i is V[i] to V[i+1]
    Point    lP;           // leftmost vertex point
    Point    rP;           // rightmost vertex point
    Point*   lPp;          // pointer to leftmost vertex in poly V
    SLseg*   above;        // segment above this one
    SLseg*   below;        // segment below this one

    SLseg() : Comparable<SLseg*>(this) {}
    ~SLseg() {}

    // return true if 'this' is below 'a'
    bool operator< (const SLseg& a)
    {
        // First check if these two segments share a left vertex
        if (this->lPp == a.lPp) {
            // Same point - the two segments share a vertex.
            // use y coord of right end points
            if (this->rP.y < a.rP.y)
                return true;
            else
                return false;
        }

        return isLeft(this->lP, this->rP, a.lP) > 0;
    }

    bool operator== (const SLseg& a)
    {
        return this->edge == a.edge;
    }

    cmp_t Compare(SLseg* key) const
    {
        return (*key == *this) ? EQ_CMP
            : ((*key < *this) ? MIN_CMP : MAX_CMP);
    }
};

typedef AvlNode<SLseg*> Tnode;

// the Sweep Line itself
class SweepLine {
    int      nv;           // number of vertices in polygon
    Polygon* Pn;           // initial Polygon
    AvlTree<SLseg*> Tree;  // balanced binary tree
public:
    SweepLine(Polygon &P)          // constructor
    { nv = P.n; Pn = &P; }

    ~SweepLine(void)               // destructor
    {
        cleanTree(Tree.myRoot);
    }

    void cleanTree(Tnode *p)
    {
        if (!p) return;
        delete p->Data();
        cleanTree(p->Subtree(AvlNode<SLseg*>::LEFT));
        cleanTree(p->Subtree(AvlNode<SLseg*>::RIGHT));
    }

    SLseg*   add( Event* );
    SLseg*   find( Event* );
    bool     intersect( SLseg*, SLseg* );
    void     remove( SLseg* );
};

SLseg* SweepLine::add( Event* E )
{
    // fill in SLseg element data
    SLseg* s = new SLseg;
    s->edge  = E->edge;
    E->seg = s;

    // if it is being added, then it must be a LEFT edge event
    // but need to determine which endpoint is the left one
    Point* v1 = &(Pn->V[s->edge]);
    Point* eN = (s->edge+1 < Pn->n ? &(Pn->V[s->edge+1]):&(Pn->V[0]));
    Point* v2 = eN;
    if (xyorder( v1, v2) < 0) { // determine which is leftmost
        s->lPp = v1;
        s->lP = *v1;
        s->rP = *v2;
    }
    else {
        s->rP = *v1;
        s->lP = *v2;
        s->lPp = v2;
    }
    s->above = (SLseg*)0;
    s->below = (SLseg*)0;

    // add a node to the balanced binary tree
    Tnode* nd = Tree.Insert(s);
    Tnode* nx = Tree.Next(nd);
    Tnode* np = Tree.Prev(nd);

    if (nx != (Tnode*)0) {
        s->above = (SLseg*)nx->Data();
        s->above->below = s;
    }
    if (np != (Tnode*)0) {
        s->below = (SLseg*)np->Data();
        s->below->above = s;
    }
    return s;
}

void SweepLine::remove( SLseg* s )
{
    // remove the node from the balanced binary tree
    Tnode* nd = Tree.Search(s);
    if (nd == (Tnode*)0)
        return;      // not there !

    // get the above and below segments pointing to each other
    Tnode* nx = Tree.Next(nd);
    if (nx != (Tnode*)0) {
        SLseg* sx = (SLseg*)(nx->Data());
        sx->below = s->below;
    }
    Tnode* np = Tree.Prev(nd);
    if (np != (Tnode*)0) {
        SLseg* sp = (SLseg*)(np->Data());
        sp->above = s->above;
    }
    Tree.Delete(nd->Key());       // now can safely remove it
    delete s;                     // note:  s == nd->Data()
}

// test intersect of 2 segments and return: 0=none, 1=intersect
bool SweepLine::intersect( SLseg* s1, SLseg* s2)
{
    if (s1 == (SLseg*)0 || s2 == (SLseg*)0)
        return false;      // no intersect if either segment doesn't exist

    // check for consecutive edges in polygon
    int e1 = s1->edge;
    int e2 = s2->edge;
    if (((e1+1)%nv == e2) || (e1 == (e2+1)%nv))
        return false;      // no non-simple intersect since consecutive

    // test for existence of an intersect point
    double lsign, rsign;
    lsign = isLeft(s1->lP, s1->rP, s2->lP);    // s2 left point sign
    rsign = isLeft(s1->lP, s1->rP, s2->rP);    // s2 right point sign
    if (lsign * rsign > 0) // s2 endpoints have same sign relative to s1
        return false;      // => on same side => no intersect is possible
    lsign = isLeft(s2->lP, s2->rP, s1->lP);    // s1 left point sign
    rsign = isLeft(s2->lP, s2->rP, s1->rP);    // s1 right point sign
    if (lsign * rsign > 0) // s1 endpoints have same sign relative to s2
        return false;      // => on same side => no intersect is possible
    // the segments s1 and s2 straddle each other
    return true;           // => an intersect exists
}
//===================================================================


// simple_Polygon(): test if a Polygon P is simple or not
//     Input:  Pn = a polygon with n vertices V[]
//     Return: FALSE(0) = is NOT simple
//             TRUE(1)  = IS simple

bool simple_Polygon( Polygon &Pn )
{
    EventQueue  Eq(Pn);
    SweepLine   SL(Pn);
    Event*      e;                 // the current event
    SLseg*      s;                 // the current SL segment

    // This loop processes all events in the sorted queue
    // Events are only left or right vertices since
    // No new events will be added (an intersect => Done)
    while ((e = Eq.next())) {      // while there are events
        if (e->type == LEFT) {     // process a left vertex
            s = SL.add(e);         // add it to the sweep line
            if (SL.intersect( s, s->above))
                return false;      // Pn is NOT simple
            if (SL.intersect( s, s->below))
                return false;      // Pn is NOT simple
        }
        else {                     // process a right vertex
            s = e->otherEnd->seg;
            if (SL.intersect( s->above, s->below))
                return false;      // Pn is NOT simple
            SL.remove(s);          // remove it from the sweep line
        }
    }
    return true;      // Pn is simple
}
//===================================================================
