/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _DEFAULTCOLUMNITERATOR_H
#define _DEFAULTCOLUMNITERATOR_H

#include <set>
#include <stack>
#include <vector>
#include <map>
#include "halColumnIterator.h"
#include "halRearrangement.h"
#include "halCommon.h"
#include "columnIteratorStack.h"

namespace hal {

class DefaultColumnIterator : public ColumnIterator
{
public:

   DefaultColumnIterator(const hal::Genome* reference, 
                         const hal::Genome* root,
                         hal_index_t columnIndex,
                         hal_index_t lastIndex,
                         hal_size_t maxInsertionLength,
                         bool noDupes,
                         bool noAncestors);
   
   ~DefaultColumnIterator();

   /** Move column iterator one column to the right along reference
    * genoem sequence */
    void toRight() const;

   bool lastColumn() const;

   const hal::Genome* getReferenceGenome() const;
   const hal::Sequence* getReferenceSequence() const;
   hal_index_t getReferenceSequencePosition() const;

   /** Get a pointer to the column map */
   const ColumnMap* getColumnMap() const;

   hal_index_t getArrayIndex() const;

   void defragment() const;

private:

   typedef ColumnIteratorStack::LinkedBottomIterator LinkedBottomIterator;
   typedef ColumnIteratorStack::LinkedTopIterator LinkedTopIterator;
   typedef ColumnIteratorStack::Entry StackEntry;
   
   typedef std::map<const Genome*, PositionCache*> VisitCache;

private:

   void recursiveUpdate(bool init) const;
   bool handleDeletion(TopSegmentIteratorConstPtr inputTopIterator) const;
   bool handleInsertion(TopSegmentIteratorConstPtr inputTopIterator) const;

   void updateParent(LinkedTopIterator* topIt) const;
   void updateChild(LinkedBottomIterator* bottomIt, hal_size_t index) const;
   void updateNextTopDup(LinkedTopIterator* topIt) const;
   void updateParseUp(LinkedBottomIterator* bottomIt) const;
   void updateParseDown(LinkedTopIterator* topIt) const;

   bool inBounds() const;
   void nextFreeIndex() const;
   bool colMapInsert(DNAIteratorConstPtr dnaIt) const;

   void resetColMap() const;
   void eraseColMap() const;
   
private:

   // everything's mutable to keep const behaviour consistent with
   // other iterators (which provide both const and non-const access)
   // the fact that this iterator has no writable interface makes it
   // seem like a dumb excercise though. 
   mutable const Genome* _root;
   mutable ColumnIteratorStack _stack;
   mutable ColumnIteratorStack _indelStack;
   mutable const Sequence* _ref;
   mutable size_t _curInsertionLength;

   mutable RearrangementPtr _rearrangement;
   mutable hal_size_t _maxInsertionLength;
   mutable bool _noDupes;
   mutable bool _noAncestors;

   mutable ColumnMap _colMap;
   mutable TopSegmentIteratorConstPtr _top;
   mutable TopSegmentIteratorConstPtr _next;
   mutable VisitCache _visitCache;
   mutable bool _break;
};




}
#endif

