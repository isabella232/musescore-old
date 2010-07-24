//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id$
//
//  Copyright (C) 2002-2010 Werner Schweer and others
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

#include "globals.h"
#include "staff.h"
#include "part.h"
#include "clef.h"
#include "xml.h"
#include "score.h"
#include "bracket.h"
#include "keysig.h"
#include "segment.h"
#include "style.h"
#include "measure.h"
#include "tablature.h"
#include "stafftype.h"

//---------------------------------------------------------
//   idx
//---------------------------------------------------------

int Staff::idx() const
      {
      return _score->staffIdx(this);
      }

//---------------------------------------------------------
//   bracket
//---------------------------------------------------------

int Staff::bracket(int idx) const
      {
      if (idx < _brackets.size())
            return _brackets[idx]._bracket;
      return NO_BRACKET;
      }

//---------------------------------------------------------
//   bracketSpan
//---------------------------------------------------------

int Staff::bracketSpan(int idx) const
      {
      if (idx < _brackets.size())
            return _brackets[idx]._bracketSpan;
      return 0;
      }

//---------------------------------------------------------
//   setBracket
//---------------------------------------------------------

void Staff::setBracket(int idx, int val)
      {
      if (idx >= _brackets.size()) {
            for (int i = _brackets.size(); i <= idx; ++i)
                  _brackets.append(BracketItem());
            }
      _brackets[idx]._bracket = val;
      while (!_brackets.isEmpty() && (_brackets.last()._bracket == NO_BRACKET))
            _brackets.removeLast();
      }

//---------------------------------------------------------
//   setBracketSpan
//---------------------------------------------------------

void Staff::setBracketSpan(int idx, int val)
      {
      if (idx >= _brackets.size()) {
            for (int i = _brackets.size(); i <= idx; ++i)
                  _brackets.append(BracketItem());
            }
      _brackets[idx]._bracketSpan = val;
      }

//---------------------------------------------------------
//   addBracket
//---------------------------------------------------------

void Staff::addBracket(BracketItem b)
      {
      if (!_brackets.isEmpty() && _brackets[0]._bracket == NO_BRACKET) {
            _brackets[0] = b;
            }
      else {
            //
            // create new bracket level
            //
            foreach(Staff* s, _score->staves()) {
                  if (s == this)
                        s->_brackets.append(b);
                  else
                        s->_brackets.append(BracketItem());
                  }
            }
      }

//---------------------------------------------------------
//   cleanupBrackets
//---------------------------------------------------------

void Staff::cleanupBrackets()
      {
      int index = idx();
      int n = _score->nstaves();
      for (int i = 0; i < _brackets.size(); ++i) {
            if (_brackets[i]._bracket != NO_BRACKET) {
                  int span = _brackets[i]._bracketSpan;
                  if (span > (n - index)) {
                        span = n - index;
                        _brackets[i]._bracketSpan = span;
                        }
                  }
            }
      for (int i = 0; i < _brackets.size(); ++i) {
            if (_brackets[i]._bracket != NO_BRACKET) {
                  int span = _brackets[i]._bracketSpan;
                  if (span <= 1)
                        _brackets[i] = BracketItem();
                  else {
                        // delete all other brackets with same span
                        for (int k = i + 1; k < _brackets.size(); ++k) {
                              if (span == _brackets[k]._bracketSpan)
                                    _brackets[k] = BracketItem();
                              }
                        }
                  }
            }
      }

//---------------------------------------------------------
//   trackName
//---------------------------------------------------------

QString Staff::trackName() const
      {
      return _part->instr()->trackName();
      }

//---------------------------------------------------------
//   Staff
//---------------------------------------------------------

Staff::Staff(Score* s, Part* p, int rs)
      {
      _score          = s;
      _rstaff         = rs;
      _part           = p;
      _clefList       = new ClefList;
      _keymap         = new KeyList;
      (*_keymap)[0]   = 0;                  // default to C major
      _staffType      = _score->staffTypes()[PITCHED_STAFF_TYPE];
      _linkTo         = 0;
      _show           = true;
      _small          = false;
      _invisible      = false;
      _barLineSpan    = 1;
      _updateClefList = true;
      _updateKeymap   = true;
      }

//---------------------------------------------------------
//   ~Staff
//---------------------------------------------------------

Staff::~Staff()
      {
      delete _clefList;
      delete _keymap;
      _keymap   = 0;      // DEBUG
      _clefList = 0;
      }

//---------------------------------------------------------
//   Staff::clef
//---------------------------------------------------------

int Staff::clef(int tick) const
      {
      return _clefList->clef(tick);
      }

//---------------------------------------------------------
//   Staff::key
//---------------------------------------------------------

KeySigEvent Staff::key(int tick) const
      {
      return _keymap->key(tick);
      }

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void Staff::write(Xml& xml) const
      {
      xml.stag("Staff");
      xml.tag("type", score()->staffTypes().indexOf(_staffType));
      if (small() && !xml.excerptmode)    // switch small staves to normal ones when extracting part
            xml.tag("small", small());
      if (invisible())
            xml.tag("invisible", invisible());
//      _clefList->write(xml, "cleflist");
//      _keymap->write(xml, "keylist");
      foreach(const BracketItem& i, _brackets)
            xml.tagE("bracket type=\"%d\" span=\"%d\"", i._bracket, i._bracketSpan);
      if (_barLineSpan != 1)
            xml.tag("barLineSpan", _barLineSpan);
      xml.etag();
      }

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void Staff::read(QDomElement e)
      {
      setSmall(false);
      for (e = e.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            QString tag(e.tagName());
            int v = e.text().toInt();
            if (tag == "type") {
                  StaffType* st = score()->staffTypes().value(v);
                  if (st)
                        _staffType = st;
                  }
            else if (tag == "lines")
                  ;                       // obsolete: setLines(v);
            else if (tag == "small")
                  setSmall(v);
            else if (tag == "invisible")
                  setInvisible(v);
            else if (tag == "slashStyle")
                  ;                       // obsolete: setSlashStyle(v);
            else if (tag == "cleflist")
                  _clefList->read(e, _score);
            else if (tag == "keylist")
                  _keymap->read(e, _score);
            else if (tag == "bracket") {
                  BracketItem b;
                  b._bracket = e.attribute("type", "-1").toInt();
                  b._bracketSpan = e.attribute("span", "0").toInt();
                  _brackets.append(b);
                  }
            else if (tag == "barLineSpan")
                  _barLineSpan = v;
            else
                  domError(e);
            }
      }

//---------------------------------------------------------
//   changeKeySig
///   Change key signature.
/// change key signature at tick into subtype st for all staves
/// in response to gui command (drop keysig on measure or keysig)
//---------------------------------------------------------

void Staff::changeKeySig(int tick, KeySigEvent st)
      {
// printf("Staff::changeKeySig "); st.print(); printf("\n");

      Measure* measure = _score->tick2measure(tick);
      if (!measure) {
            printf("measure for tick %d not found!\n", tick);
            return;
            }
      Segment* s = measure->findSegment(SegKeySig, tick);
      if (!s) {
            s = new Segment(measure, SegKeySig, tick);
            _score->undoAddElement(s);
            }
      int track = idx() * VOICES;
      KeySig* ks = static_cast<KeySig*>(s->element(track));

      KeySig* nks = new KeySig(score());
      nks->setTrack(track);
      nks->changeType(st);
      nks->setParent(s);

      if (ks)
            _score->undoChangeElement(ks, nks);
      else
            _score->undoAddElement(nks);
      }

//---------------------------------------------------------
//   changeClef
//---------------------------------------------------------

void Staff::changeClef(int tick, int st)
      {
      if (tick == 0 && st == CLEF_PERC)
            part()->instr()->setUseDrumset(true);

      Measure* measure = _score->tick2measure(tick);
      if (!measure) {
            printf("measure for tick %d not found!\n", tick);
            return;
            }
      if (measure->prevMeasure())
            measure = measure->prevMeasure();
      Segment* s = measure->findSegment(SegClef, tick);
      if (!s) {
            s = new Segment(measure, SegClef, tick);
            _score->undoAddElement(s);
            }
      int track = idx() * VOICES;
      Clef* clef = static_cast<Clef*>(s->element(track));

      Clef* nclef = new Clef(score());
      nclef->setTrack(track);
      nclef->setSubtype(st);
      nclef->setParent(s);

      if (clef)
            _score->undoChangeElement(clef, nclef);
      else
            _score->undoAddElement(nclef);
      }

//---------------------------------------------------------
//   height
//---------------------------------------------------------

double Staff::height() const
      {
      return (lines()-1) * spatium() * _staffType->lineDistance().val();
      }

//---------------------------------------------------------
//   spatium
//---------------------------------------------------------

double Staff::spatium() const
      {
      return _score->spatium() * mag();
      }

//---------------------------------------------------------
//   mag
//---------------------------------------------------------

double Staff::mag() const
      {
      return _small ? score()->styleD(ST_smallStaffMag) : 1.0;
      }

//---------------------------------------------------------
//   setKey
//---------------------------------------------------------

void Staff::setKey(int tick, int st)
      {
      KeySigEvent ke;
      ke.setAccidentalType(st);

      (*_keymap)[tick] = ke;
      }

void Staff::setKey(int tick, const KeySigEvent& st)
      {
      (*_keymap)[tick] = st;
      }

//---------------------------------------------------------
//   setClef
//---------------------------------------------------------

void Staff::setClef(int tick, int clef)
      {
      (*_clefList)[tick] = clef;
      }

//---------------------------------------------------------
//   channel
//---------------------------------------------------------

int Staff::channel(int tick,  int voice) const
      {
      if (_channelList[voice].isEmpty())
            return 0;
      QMap<int, int>::const_iterator i = _channelList[voice].lowerBound(tick);
      if (i == _channelList[voice].begin())
            return _channelList[voice].begin().value();
      --i;
      return i.value();
      }

//---------------------------------------------------------
//   lines
//---------------------------------------------------------

int Staff::lines() const
      {
      if (useTablature())
            return part()->instr()->tablature()->strings();
      return _staffType->lines();
      }

//---------------------------------------------------------
//   slashStyle
//---------------------------------------------------------

bool Staff::slashStyle() const
      {
      return _staffType->slashStyle();
      }

//---------------------------------------------------------
//   setSlashStyle
//---------------------------------------------------------

void Staff::setSlashStyle(bool val)
      {
      _staffType->setSlashStyle(val);
      }

//---------------------------------------------------------
//   setLines
//---------------------------------------------------------

void Staff::setLines(int val)
      {
      _staffType->setLines(val);
      }

//---------------------------------------------------------
//   useTablature
//---------------------------------------------------------

bool Staff::useTablature() const
      {
      return _staffType->group() == TAB_STAFF;
      }

//---------------------------------------------------------
//   setUseTablature
//---------------------------------------------------------

void Staff::setUseTablature(bool val)
      {
      _staffType = score()->staffTypes()[val ? TAB_STAFF_TYPE : PITCHED_STAFF_TYPE];
      }

