//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id$
//
//  Copyright (C) 2008-2011 Werner Schweer and others
//
//  Some Code inspired by "The JAZZ++ Midi Sequencer"
//  Copyright (C) 1994-2000 Andreas Voss and Per Sigmond, all rights reserved.
//
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

#include "harmony.h"
#include "pitchspelling.h"
#include "score.h"
#include "system.h"
#include "measure.h"
#include "scoreview.h"
#include "segment.h"
#include "painter.h"
#include "chordlist.h"
#include "mscore.h"

//---------------------------------------------------------
//   harmonyName
//---------------------------------------------------------

QString Harmony::harmonyName() const
      {
      bool germanNames = score()->styleB(ST_useGermanNoteNames);

      HChord hc = descr() ? descr()->chord : HChord();
      QString s;

      if (!_degreeList.isEmpty()) {
            hc.add(_degreeList);
            // try to find the chord in chordList
            const ChordDescription* newExtension = 0;
            ChordList* cl = score()->style()->chordList();
            foreach(const ChordDescription* cd, *cl) {
                  if (cd->chord == hc && !cd->names.isEmpty()) {
                        newExtension = cd;
                        break;
                        }
                  }
            // now determine the chord name
            if (newExtension)
                  s = tpc2name(_rootTpc, germanNames) + newExtension->names.front();
            else
                  // not in table, fallback to using HChord.name()
                  s = hc.name(_rootTpc);
            //s += " ";
            } // end if (degreeList ...
      else {
            s = tpc2name(_rootTpc, germanNames);
            if (descr()) {
                  //s += " ";
                  s += descr()->names.front();
                  }
            }
      if (_baseTpc != INVALID_TPC) {
            s += "/";
            s += tpc2name(_baseTpc, germanNames);
            }
      return s;
      }

//---------------------------------------------------------
//   resolveDegreeList
//    try to detect chord number and to eliminate degree
//    list
//---------------------------------------------------------

void Harmony::resolveDegreeList()
      {
      if (_degreeList.isEmpty())
            return;

      HChord hc = descr() ? descr()->chord : HChord();

      hc.add(_degreeList);

// printf("resolveDegreeList: <%s> <%s-%s>: ", _descr->name, _descr->xmlKind, _descr->xmlDegrees);
// hc.print();
// _descr->chord.print();

      // try to find the chord in chordList
      ChordList* cl = score()->style()->chordList();
      foreach(const ChordDescription* cd, *cl) {
            if ((cd->chord == hc) && !cd->names.isEmpty()) {
printf("ResolveDegreeList: found in table as %s\n", qPrintable(cd->names.front()));
                  _id = cd->id;
                  _degreeList.clear();
                  return;
                  }
            }
printf("ResolveDegreeList: not found in table\n");
      }

//---------------------------------------------------------
//   Harmony
//---------------------------------------------------------

Harmony::Harmony(Score* score)
   : Text(score)
      {
      setSubtype(HARMONY);
      setTextStyle(TEXT_STYLE_HARMONY);

      _rootTpc   = INVALID_TPC;
      _baseTpc   = INVALID_TPC;
      _id        = -1;
      }

Harmony::Harmony(const Harmony& h)
   : Text(h)
      {
      _rootTpc    = h._rootTpc;
      _baseTpc    = h._baseTpc;
      _id         = h._id;
      _degreeList = h._degreeList;
      }

//---------------------------------------------------------
//   ~Harmony
//---------------------------------------------------------

Harmony::~Harmony()
      {
      foreach(const TextSegment* ts, textList)
            delete ts;
      }

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void Harmony::write(Xml& xml) const
      {
      xml.stag("Harmony");
      if (_rootTpc != INVALID_TPC) {
            xml.tag("root", _rootTpc);
            if (_id != -1)
                  xml.tag("extension", _id);
            if (_baseTpc != INVALID_TPC)
                  xml.tag("base", _baseTpc);
            foreach(const HDegree& hd, _degreeList) {
                  int tp = hd.type();
                  if (tp == ADD || tp == ALTER || tp == SUBTRACT) {
                        xml.stag("degree");
                        xml.tag("degree-value", hd.value());
                        xml.tag("degree-alter", hd.alter());
                        switch (tp) {
                              case ADD:
                                    xml.tag("degree-type", "add");
                                    break;
                              case ALTER:
                                    xml.tag("degree-type", "alter");
                                    break;
                              case SUBTRACT:
                                    xml.tag("degree-type", "subtract");
                                    break;
                              default:
                                    break;
                              }
                        xml.etag();
                        }
                  }
            Element::writeProperties(xml);
            }
      else
            Text::writeProperties(xml);
      xml.etag();
      }

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void Harmony::read(QDomElement e)
      {
      // convert table to tpc values
      static const int table[] = {
            14, 9, 16, 11, 18, 13, 8, 15, 10, 17, 12, 19
            };
      for (e = e.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            QString tag(e.tagName());
            int i = e.text().toInt();
            if (tag == "base") {
                  if (score()->mscVersion() >= 106)
                        setBaseTpc(i);
                  else
                        setBaseTpc(table[i-1]);    // obsolete
                  }
            else if (tag == "extension")
                  setId(i);
            else if (tag == "root") {
                  if (score()->mscVersion() >= 106)
                        setRootTpc(i);
                  else
                        setRootTpc(table[i-1]);    // obsolete
                  }
            else if (tag == "degree") {
                  int degreeValue = 0;
                  int degreeAlter = 0;
                  QString degreeType = "";
                  for (QDomElement ee = e.firstChildElement(); !ee.isNull(); ee = ee.nextSiblingElement()) {
                        QString tag(ee.tagName());
                        if (tag == "degree-value") {
                              degreeValue = ee.text().toInt();
                              }
                        else if (tag == "degree-alter") {
                              degreeAlter = ee.text().toInt();
                              }
                        else if (tag == "degree-type") {
                              degreeType = ee.text();
                              }
                        else
                              domError(ee);
                        }
                  if (degreeValue <= 0 || degreeValue > 13
                      || degreeAlter < -2 || degreeAlter > 2
                      || (degreeType != "add" && degreeType != "alter" && degreeType != "subtract")) {
                        printf("incorrect degree: degreeValue=%d degreeAlter=%d degreeType=%s\n",
                               degreeValue, degreeAlter, qPrintable(degreeType));
                        }
                  else {
                        if (degreeType == "add")
                              addDegree(HDegree(degreeValue, degreeAlter, ADD));
                        else if (degreeType == "alter")
                              addDegree(HDegree(degreeValue, degreeAlter, ALTER));
                        else if (degreeType == "subtract")
                              addDegree(HDegree(degreeValue, degreeAlter, SUBTRACT));
                        }
                  }
            else if (!Text::readProperties(e))
                  domError(e);
            }
      render();
      }

//---------------------------------------------------------
//   convertRoot
//    convert something like "C#" into tpc 21
//---------------------------------------------------------

static int convertRoot(const QString& s, bool germanNames)
      {
      int n = s.size();
      if (n < 1)
            return INVALID_TPC;
      int alter = 0;
      if (n > 1) {
            if (s[1].toLower().toAscii() == 'b')
                  alter = -1;
            else if (s[1] == '#')
                  alter = 1;
            }
      int r;
      if (germanNames) {
            switch(s[0].toLower().toAscii()) {
                  case 'c':   r = 0; break;
                  case 'd':   r = 1; break;
                  case 'e':   r = 2; break;
                  case 'f':   r = 3; break;
                  case 'g':   r = 4; break;
                  case 'a':   r = 5; break;
                  case 'h':   r = 6; break;
                  case 'b':
                        if (alter)
                              return INVALID_TPC;
                        r = 6;
                        alter = -1;
                        break;
                  default:
                        return INVALID_TPC;
                  }
            static const int spellings[] = {
               // bb  b   -   #  ##
                  0,  7, 14, 21, 28,  // C
                  2,  9, 16, 23, 30,  // D
                  4, 11, 18, 25, 32,  // E
                 -1,  6, 13, 20, 27,  // F
                  1,  8, 15, 22, 29,  // G
                  3, 10, 17, 24, 31,  // A
                  5, 12, 19, 26, 33,  // B
                  };
            r = spellings[r * 5 + alter + 2];
            }
      else {
            switch(s[0].toLower().toAscii()) {
                  case 'c':   r = 0; break;
                  case 'd':   r = 1; break;
                  case 'e':   r = 2; break;
                  case 'f':   r = 3; break;
                  case 'g':   r = 4; break;
                  case 'a':   r = 5; break;
                  case 'b':   r = 6; break;
                  default:    return INVALID_TPC;
                  }
            static const int spellings[] = {
               // bb  b   -   #  ##
                  0,  7, 14, 21, 28,  // C
                  2,  9, 16, 23, 30,  // D
                  4, 11, 18, 25, 32,  // E
                 -1,  6, 13, 20, 27,  // F
                  1,  8, 15, 22, 29,  // G
                  3, 10, 17, 24, 31,  // A
                  5, 12, 19, 26, 33,  // B
                  };
            r = spellings[r * 5 + alter + 2];
            }
      return r;
      }

//---------------------------------------------------------
//   parseHarmony
//    return ChordDescription
//---------------------------------------------------------

bool Harmony::parseHarmony(const QString& ss, int* root, int* base)
      {
      _id = -1;
      QString s = ss.simplified();
      _userName = s;
      int n = s.size();
      if (n < 1)
            return false;
      bool germanNames = score()->styleB(ST_useGermanNoteNames);
      int r = convertRoot(s, germanNames);
      if (r == INVALID_TPC) {
            printf("1:parseHarmony failed <%s>\n", qPrintable(ss));
            return false;
            }
      *root = r;
      int idx = ((n > 1) && ((s[1] == 'b') || (s[1] == '#'))) ? 2 : 1;
      *base = INVALID_TPC;
      int slash = s.indexOf('/');
      if (slash != -1) {
            QString bs = s.mid(slash+1);
            s     = s.mid(idx, slash - idx);
            *base = convertRoot(bs, germanNames);
            }
      else
            s = s.mid(idx).simplified();
      s = s.toLower();
      foreach(const ChordDescription* cd, *score()->style()->chordList()) {
            foreach(QString ss, cd->names) {
                  if (s == ss) {
                        _id = cd->id;
                        return true;
                        }
                  }
            }
      _userName = s;
      printf("2:parseHarmony failed <%s><%s>\n", qPrintable(ss), qPrintable(s));
      return false;
      }

//---------------------------------------------------------
//   startEdit
//---------------------------------------------------------

void Harmony::startEdit(ScoreView* view, const QPointF& p)
      {
      if (!textList.isEmpty()) {
            QString s(harmonyName());
            setText(s);
            }
      Text::startEdit(view, p);
      }

//---------------------------------------------------------
//   edit
//---------------------------------------------------------

bool Harmony::edit(ScoreView* view, int grip, int key, Qt::KeyboardModifiers mod, const QString& s)
      {
      bool rv = Text::edit(view, grip, key, mod, s);
      QString str = getText();
      int root, base;
      if (!str.isEmpty() && !parseHarmony(str, &root, &base)) {
            QTextCharFormat tf;
            tf.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            tf.setUnderlineColor(Qt::red);
            QTextCursor c(doc());
            c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            c.setCharFormat(tf);
            }
      else {
            QTextCharFormat tf;
            QTextCursor c(doc());
            c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            c.setCharFormat(tf);
            }
      return rv;
      }

//---------------------------------------------------------
//   endEdit
//---------------------------------------------------------

void Harmony::endEdit()
      {
      Text::endEdit();
      setHarmony(getText());
      }

//---------------------------------------------------------
//   setHarmony
//---------------------------------------------------------

void Harmony::setHarmony(const QString& s)
      {
      int r, b;
      parseHarmony(s, &r, &b);
      if (_id != -1) {
            setRootTpc(r);
            setBaseTpc(b);
            render();
            }
      else {
            // syntax error, leave text as is
            foreach(const TextSegment* s, textList)
                  delete s;
            textList.clear();

            setRootTpc(INVALID_TPC);
            setBaseTpc(INVALID_TPC);
            _id = -1;
            }
      }

//---------------------------------------------------------
//   baseLine
//---------------------------------------------------------

qreal Harmony::baseLine() const
      {
      return (_editMode || textList.isEmpty()) ? Text::baseLine() : 0.0;
      }

//---------------------------------------------------------
//   text
//---------------------------------------------------------

QString HDegree::text() const
      {
      if (_type == UNDEF)
            return QString();
      QString degree;
      switch(_type) {
            case UNDEF: break;
            case ADD:         degree += "add"; break;
            case ALTER:       degree += "alt"; break;
            case SUBTRACT:    degree += "sub"; break;
            }
      switch(_alter) {
            case -1:          degree += "b"; break;
            case 1:           degree += "#"; break;
            default:          break;
            }
      return degree + QString("%1").arg(_value);
      }

//---------------------------------------------------------
//   fromXml
//    lookup harmony in harmony data base
//    using musicXml "kind" string and degree list
//---------------------------------------------------------

const ChordDescription* Harmony::fromXml(const QString& kind,  const QList<HDegree>& dl)
      {
      QStringList degrees;

      foreach(const HDegree& d, dl)
            degrees.append(d.text());

      QString lowerCaseKind = kind.toLower();
      ChordList* cl = score()->style()->chordList();
      foreach(const ChordDescription* cd, *cl) {
            QString k     = cd->xmlKind;
            QStringList d = cd->xmlDegrees;
            if ((lowerCaseKind == k) && (d == degrees)) {
//                  printf("harmony found in db: %s %s -> %d\n", qPrintable(kind), qPrintable(degrees), cd->id);
                  return cd;
                  }
            }
      return 0;
      }

//---------------------------------------------------------
//   fromXml
//    lookup harmony in harmony data base
//    using musicXml "kind" string and degree list
//---------------------------------------------------------

const ChordDescription* Harmony::fromXml(const QString& kind)
      {
      QString lowerCaseKind = kind.toLower();
      ChordList* cl = score()->style()->chordList();
      foreach(const ChordDescription* cd, *cl) {
            if (lowerCaseKind == cd->xmlKind)
                  return cd;
            }
      return 0;
      }

//---------------------------------------------------------
//   descr
//---------------------------------------------------------

const ChordDescription* Harmony::descr() const
      {
      return score()->style()->chordDescription(_id);
      }

//---------------------------------------------------------
//   isEmpty
//---------------------------------------------------------

bool Harmony::isEmpty() const
      {
      return textList.isEmpty() && doc()->isEmpty();
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void Harmony::layout()
      {
      setSubtype(TEXT_CHORD);    // apply style changes

      if (_editMode || textList.isEmpty()) {
            Text::layout();
            return;
            }
      style().layout(this);
      Measure* m = measure();
      double yy = track() < 0 ? 0.0 : m->system()->staff(track() / VOICES)->y();
      double xx = 0.0;  // (segment()->tick() < 0) ? 0.0 : m->tick2pos(segment()->tick());

      setPos(ipos() + QPointF(xx, yy));

      QRectF bb;
      foreach(const TextSegment* ts, textList)
            bb |= ts->boundingRect().translated(ts->x, ts->y);
      setbbox(bb);
      }

//---------------------------------------------------------
//   shape
//---------------------------------------------------------

QPainterPath Harmony::shape() const
      {
      QPainterPath pp;
      pp.addRect(bbox());
      return pp;
      }

//---------------------------------------------------------
//   draw
//---------------------------------------------------------

void Harmony::draw(Painter* painter) const
      {
      if (_editMode || textList.isEmpty()) {
            Text::draw(painter);
            return;
            }
      foreach(const TextSegment* ts, textList) {
            painter->setFont(ts->font);
            painter->drawText(ts->x, ts->y, ts->text);
            }
      }

//---------------------------------------------------------
//   TextSegment
//---------------------------------------------------------

TextSegment::TextSegment(const QString& s, const QFont& f, double x, double y)
      {
      set(s, f, x, y);
      select = false;
      }

//---------------------------------------------------------
//   width
//---------------------------------------------------------

double TextSegment::width() const
      {
      QFontMetricsF fm(font);
      double w = 0.0;
      foreach(QChar c, text) {
            w += fm.width(c);
            }
      return w;
      }

//---------------------------------------------------------
//   boundingRect
//---------------------------------------------------------

QRectF TextSegment::boundingRect() const
      {
      QFontMetricsF fm(font);
      return fm.boundingRect(text);
      }

//---------------------------------------------------------
//   set
//---------------------------------------------------------

void TextSegment::set(const QString& s, const QFont& f, double _x, double _y)
      {
      font = f;
      x    = _x;
      y    = _y;
      setText(s);
      }

//---------------------------------------------------------
//   render
//---------------------------------------------------------

void Harmony::render(const QList<RenderAction>& renderList, double& x, double& y, int tpc)
      {
      ChordList* chordList = score()->style()->chordList();
      QStack<QPointF> stack;
      int fontIdx = 0;
      double _spatium = spatium();
      double mag = (DPI / PPI) * (_spatium / (SPATIUM20 * DPI));

      foreach(const RenderAction& a, renderList) {
            if (a.type == RenderAction::RENDER_SET) {
                  TextSegment* ts = new TextSegment(fontList[fontIdx], x, y);
                  ChordSymbol cs = chordList->symbol(a.text);
                  if (cs.isValid()) {
                        ts->font = fontList[cs.fontIdx];
                        ts->setText(QString(cs.code));
                        }
                  else
                        ts->setText(a.text);
                  textList.append(ts);
                  x += ts->width();
                  }
            else if (a.type == RenderAction::RENDER_MOVE) {
                  x += a.movex * mag;
                  y += a.movey * mag;
                  }
            else if (a.type == RenderAction::RENDER_PUSH)
                  stack.push(QPointF(x,y));
            else if (a.type == RenderAction::RENDER_POP) {
                  if (!stack.isEmpty()) {
                        QPointF pt = stack.pop();
                        x = pt.x();
                        y = pt.y();
                        }
                  else
                        printf("RenderAction::RENDER_POP: stack empty\n");
                  }
            else if (a.type == RenderAction::RENDER_NOTE) {
                  bool germanNames = score()->styleB(ST_useGermanNoteNames);
                  QChar c;
                  int acc;
                  tpc2name(tpc, germanNames, &c, &acc);
                  TextSegment* ts = new TextSegment(fontList[fontIdx], x, y);
                  ChordSymbol cs = chordList->symbol(QString(c));
                  if (cs.isValid()) {
                        ts->font = fontList[cs.fontIdx];
                        ts->setText(QString(cs.code));
                        }
                  else
                        ts->setText(QString(c));
                  textList.append(ts);
                  x += ts->width();
                  }
            else if (a.type == RenderAction::RENDER_ACCIDENTAL) {
                  QChar c;
                  int acc;
                  tpc2name(tpc, false, &c, &acc);
                  if (acc) {
                        TextSegment* ts = new TextSegment(fontList[fontIdx], x, y);
                        QString s;
                        if (acc == -1)
                              s = "b";
                        else if (acc == 1)
                              s = "#";
                        ChordSymbol cs = chordList->symbol(s);
                        if (cs.isValid()) {
                              ts->font = fontList[cs.fontIdx];
                              ts->setText(QString(cs.code));
                              }
                        else
                              ts->setText(s);
                        textList.append(ts);
                        x += ts->width();
                        }
                  }
            else
                  printf("========unknown render action %d\n", a.type);
            }
      }

//---------------------------------------------------------
//   render
//    construct Chord Symbol
//---------------------------------------------------------

void Harmony::render(const TextStyle* st)
      {
      if (_rootTpc == INVALID_TPC)
            return;

      if (st == 0)
            st = &score()->textStyle(_textStyle);
      ChordList* chordList = score()->style()->chordList();

      fontList.clear();
      foreach(ChordFont cf, chordList->fonts) {
            if (cf.family.isEmpty() || cf.family == "default")
                  fontList.append(st->fontPx(spatium() * cf.mag));
            else {
                  QFont ff(st->fontPx(spatium() * cf.mag));
                  ff.setFamily(cf.family);
                  fontList.append(ff);
                  }
            }
      if (fontList.isEmpty())
            fontList.append(st->fontPx(spatium()));

      foreach(const TextSegment* s, textList)
            delete s;
      textList.clear();

      double x = 0.0, y = 0.0;
      render(chordList->renderListRoot, x, y, _rootTpc);
      ChordDescription* cd = chordList->value(_id);
      if (cd) {
            render(cd->renderList, x, y, 0);
            }
      if (_baseTpc != INVALID_TPC)
            render(chordList->renderListBase, x, y, _baseTpc);
      }

//---------------------------------------------------------
//   spatiumChanged
//---------------------------------------------------------

void Harmony::spatiumChanged(double oldValue, double newValue)
      {
      Text::spatiumChanged(oldValue, newValue);
      render();
      }

//---------------------------------------------------------
//   dragAnchor
//---------------------------------------------------------

QLineF Harmony::dragAnchor() const
      {
      double xp = 0.0;
      for (Element* e = parent(); e; e = e->parent())
            xp += e->x();
      double yp = measure()->system()->staffY(staffIdx());
      QPointF p(xp, yp);
      return QLineF(p, canvasPos());
      }

//---------------------------------------------------------
//   extensionName
//---------------------------------------------------------

QString Harmony::extensionName() const
      {
      return _id != -1 ? descr()->names.front() : _userName;
      }

//---------------------------------------------------------
//   xmlKind
//---------------------------------------------------------

QString Harmony::xmlKind() const
      {
      return _id != -1 ? descr()->xmlKind : QString();
      }

//---------------------------------------------------------
//   xmlDegrees
//---------------------------------------------------------

QStringList Harmony::xmlDegrees() const
      {
      return _id != -1 ? descr()->xmlDegrees : QStringList();
      }

//---------------------------------------------------------
//   degree
//---------------------------------------------------------

HDegree Harmony::degree(int i) const
      {
      return _degreeList.value(i);
      }

//---------------------------------------------------------
//   addDegree
//---------------------------------------------------------

void Harmony::addDegree(const HDegree& d)
      {
      _degreeList << d;
      }

//---------------------------------------------------------
//   numberOfDegrees
//---------------------------------------------------------

int Harmony::numberOfDegrees() const
      {
      return _degreeList.size();
      }

//---------------------------------------------------------
//   clearDegrees
//---------------------------------------------------------

void Harmony::clearDegrees()
      {
      _degreeList.clear();
      }

//---------------------------------------------------------
//   degreeList
//---------------------------------------------------------

const QList<HDegree>& Harmony::degreeList() const
      {
      return _degreeList;
      }

