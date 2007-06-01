//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id:$
//
//  Copyright (C) 2007 Werner Schweer (ws@seh.de)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef __MUSEDATA_H__
#define __MUSEDATA_H__

class Staff;
class Part;
class Score;
class ChordRest;
class Measure;

//---------------------------------------------------------
//   MuseData
//    used importing Musedata files
//---------------------------------------------------------

class MuseData {
      int _division;
      int curTick;
      QList<QStringList> parts;
      Score* score;
      ChordRest* chordRest;
      int ntuplet;
      Measure* measure;
      int voice;

      void musicalAttribute(QString s, Part*);
      void readPart(QStringList sl, Part*);
      void readNote(Part*, const QString& s);
      void readChord(Part*, const QString& s);
      void readRest(Part*, const QString& s);
      void readBackup(const QString& s);
      Measure* createMeasure();
      int countStaves(const QStringList& sl);

   public:
      MuseData(Score* s) { score = s; }
      bool read(const QString&);
      void convert();
      };

#endif

