/*
 * Copyright (C) 2013 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */

#ifndef _HALWIGGLESCANNER_H
#define _HALWIGGLESCANNER_H

#include "hal.h"
#include <cstdlib>
#include <deque>
#include <fstream>
#include <string>
#include <vector>

namespace hal {

    /** Parse a WIGGLE file line by line */
    class WiggleScanner {
      public:
        WiggleScanner();
        virtual ~WiggleScanner();
        virtual void scan(const std::string &wigglePath);
        virtual void scan(std::istream *wiggleStream);

      protected:
        virtual void visitBegin();
        virtual void visitLine();
        virtual void visitHeader();
        virtual void visitEOF();

        virtual bool scanHeader(const std::string &lineBuffer);
        virtual void scanLine(const std::string &lineBuffer);
        static void skipWhiteSpaces(std::istream *wiggleStream);

      protected:
        std::istream *_wiggleStream;
        double _value;
        hal_index_t _first;
        hal_index_t _last;

        // header information
        std::string _sequenceName;
        hal_index_t _start;
        hal_index_t _step;
        hal_index_t _span;
        bool _fixedStep;

        hal_index_t _lineNumber;
        std::string _buffer;
        hal_index_t _offset;
    };
}

#endif
// Local Variables:
// mode: c++
// End:
