/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALLIFTOVER_H
#define _HALLIFTOVER_H

#include "halBedScanner.h"
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

namespace hal {

    class Liftover : public BedScanner {
      public:
        Liftover();
        virtual ~Liftover();

        void convert(const Alignment *alignment, const Genome *srcGenome, std::istream *inputFile, const Genome *tgtGenome,
                     std::ostream *outputFile, bool addExtraColumns = false,
                     bool traverseDupes = true, bool outPSL = false, bool outPSLWithName = false,
                     const Genome *coalescenceLimit = NULL);

      protected:
        typedef std::list<BedLine> BedList;

        virtual void visitBegin();
        virtual void visitLine();
        virtual void visitEOF();
        virtual void writeLineResults();
        virtual void assignBlocksToIntervals();
        virtual bool compatible(const BedLine &tgtBed, const BedLine &newBlock);
        virtual void flipBlocks(BedList &bedList);
        virtual void computePSLInserts(BedList &bedList);
        virtual void writeBlocksAsIntervals();
        virtual void cleanResults();
        virtual void liftBlockIntervals();
        virtual void liftInterval(BedList &mappedBedLines) = 0;

      protected:
        AlignmentConstPtr _alignment;
        std::ostream *_outBedStream;
        bool _addExtraColumns;
        bool _traverseDupes;
        BedList _outBedLines;
        bool _outPSL;
        bool _outPSLWithName;

        BedList _mappedBlocks;

        const Genome *_srcGenome;
        const Genome *_tgtGenome;
        const Genome *_coalescenceLimit;
        const Sequence *_srcSequence;
        std::set<const Genome *> _tgtSet;

        ColumnIteratorPtr _colIt;
        std::set<std::string> _missedSet;
    };
}
#endif
// Local Variables:
// mode: c++
// End:
