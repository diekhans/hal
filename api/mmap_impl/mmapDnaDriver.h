/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 * Copyright (C) 2012-2019 by UCSC Computational Genomics Lab
 *
 * Released under the MIT license, see LICENSE.txt
 */
#ifndef _MMAPDNADRIVER_H
#define _MMAPDNADRIVER_H
#include "halDnaDriver.h"

namespace hal {
    class MMapGenome;
    class MMapAlignment;

    /**
     * Mmap implementation of DnaAccess.
     */
    class MMapDnaAccess : public DnaAccess {
      public:
        MMapDnaAccess(MMapGenome *genome, hal_index_t index);

        virtual ~MMapDnaAccess() {
        }

        void flush();

      protected:
        virtual void fetch(hal_index_t index) const;

      private:
        MMapGenome *_genome;
        bool _isUdcProtocol;
    };
}

#endif
// Local Variables:
// mode: c++
// End:
