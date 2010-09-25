
/**************************************************************************
 * This file is part of Celera Assembler, a software program that
 * assembles whole-genome shotgun reads into contigs and scaffolds.
 * Copyright (C) 1999-2004, The Venter Institute. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received (LICENSE.txt) a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *************************************************************************/

static const char *rcsid = "$Id: AS_BOG_PlaceContains.cc,v 1.1 2010-09-23 09:34:50 brianwalenz Exp $";

#include "AS_BOG_Datatypes.hh"
#include "AS_BOG_UnitigGraph.hh"
#include "AS_BOG_BestOverlapGraph.hh"

#include "MultiAlignStore.h"

#undef max




void
UnitigGraph::placeContains(void) {
  uint32   fragsPlaced  = 1;
  uint32   fragsPending = 0;

  while (fragsPlaced > 0) {
    fragsPlaced  = 0;
    fragsPending = 0;

    fprintf(stderr, "==> PLACING CONTAINED FRAGMENTS\n");

    for (uint32 fid=0; fid<_fi->numFragments()+1; fid++) {
      BestContainment *bestcont = bog_ptr->getBestContainer(fid);
      Unitig          *utg;

      if (bestcont == NULL)
        //  Not a contained fragment.
        continue;

      if (bestcont->isPlaced == true)
        //  Containee already placed.
        continue;

      if (Unitig::fragIn(bestcont->container) == 0) {
        //  Container not placed (yet).
        fragsPending++;
        continue;
      }

      utg = (*unitigs)[Unitig::fragIn(bestcont->container)];
      utg->addContainedFrag(fid, bestcont, verboseContains);
      assert(utg->id() == Unitig::fragIn(fid));

      bestcont->isPlaced = true;

      fragsPlaced++;
    }

    fprintf(stderr, "==> PLACING CONTAINED FRAGMENTS - placed %d fragments; still need to place %d\n",
            fragsPlaced, fragsPending);

    if ((fragsPlaced == 0) && (fragsPending > 0)) {
      fprintf(stderr, "Stopping contained fragment placement due to zombies.\n");
      fragsPlaced  = 0;
      fragsPending = 0;
    }
  }

  for (int ti=0; ti<unitigs->size(); ti++) {
    Unitig *utg = (*unitigs)[ti];

    if (utg)
      utg->sort();
  }
}
