//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id:$
//
//  Copyright (C) 2010 Werner Schweer and others
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

#include "chordline.h"
#include "xml.h"
#include "chord.h"
#include "measure.h"
#include "system.h"
#include "note.h"

//---------------------------------------------------------
//   ChordLine
//---------------------------------------------------------

ChordLine::ChordLine(Score* s)
   : Element(s)
      {
      setFlags(ELEMENT_MOVABLE | ELEMENT_SELECTABLE);
      modified = false;
      }

ChordLine::ChordLine(const ChordLine& cl)
   : Element(cl)
      {
      path = cl.path;
      }

//---------------------------------------------------------
//   setSubtype
//---------------------------------------------------------

void ChordLine::setSubtype(int st)
      {
      double x1, y1, x2, y2;
      x1 = 0.0;
      y1 = 0.0;
      switch(st) {
            default:
            case 0:                 // fall
                  x2 = x1 + 2;
                  y2 = y1 + 2;
                  break;
            case 1:                 // doit
                  x2 = x1 + 2;
                  y2 = y1 - 2;
                  break;
            }
      path = QPainterPath();
      path.moveTo(x1, y1);
      path.cubicTo(x1 + (x2-x1)/2, y1, x2, y1 - (y1-y2)/2, x2, y2);
      Element::setSubtype(st);
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void ChordLine::layout()
      {
      double _spatium = spatium();
      if (parent()) {
            QPointF p(chord()->upNote()->pos());
            setPos(p.x() + _spatium * .5, p.y());
            }
      else
            setPos(0.0, 0.0);
      QRectF r(path.boundingRect());
      setbbox(QRectF(r.x() * _spatium, r.y() * _spatium, r.width() * _spatium, r.height() * _spatium));
      }

//---------------------------------------------------------
//   read
//---------------------------------------------------------

void ChordLine::read(QDomElement e)
      {
      path = QPainterPath();
      for (e = e.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            QString tag(e.tagName());
            if (tag == "Path") {
                  path = QPainterPath();
                  QPointF curveTo;
                  QPointF p1;
                  int state;
                  for (QDomElement ee = e.firstChildElement(); !ee.isNull(); ee = ee.nextSiblingElement()) {
                        QString tag(ee.tagName());
                        if (tag == "Element") {
                              int type = ee.attribute("type").toInt();
                              double x = ee.attribute("x").toDouble();
                              double y = ee.attribute("y").toDouble();
                              switch(QPainterPath::ElementType(type)) {
                                    case QPainterPath::MoveToElement:
                                          path.moveTo(x, y);
                                          break;
                                    case QPainterPath::LineToElement:
                                          path.lineTo(x, y);
                                          break;
                                    case QPainterPath::CurveToElement:
                                          curveTo.rx() = x;
                                          curveTo.ry() = y;
                                          state = 1;
                                          break;
                                    case QPainterPath::CurveToDataElement:
                                          if (state == 1) {
                                                p1.rx() = x;
                                                p1.ry() = y;
                                                state = 2;
                                                }
                                          else if (state == 2) {
                                                path.cubicTo(curveTo, p1, QPointF(x, y));
                                                }
                                          break;
                                    }
                              }
                        else
                              domError(ee);
                        }
                  modified = true;
                  }
            else if (!Element::readProperties(e))
                  domError(e);
            }
      }

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void ChordLine::write(Xml& xml) const
      {
      xml.stag(name());
      Element::writeProperties(xml);
      if (modified) {
            int n = path.elementCount();
            xml.stag("Path");
            for (int i = 0; i < n; ++i) {
                  const QPainterPath::Element& e = path.elementAt(i);
                  xml.tagE(QString("Element type=\"%1\" x=\"%2\" y=\"%3\"")
                     .arg(int(e.type)).arg(e.x).arg(e.y));
                  }
            xml.etag();
            }
      xml.etag();
      }

//---------------------------------------------------------
//   Symbol::draw
//---------------------------------------------------------

void ChordLine::draw(QPainter& p, ScoreView*) const
      {
      double _spatium = spatium();
      p.scale(_spatium, _spatium);
      double lw = 0.15;
      QPen pen(p.pen());
      pen.setWidthF(lw);
      pen.setCapStyle(Qt::RoundCap);
      pen.setJoinStyle(Qt::RoundJoin);
      p.setPen(pen);
      p.setBrush(Qt::NoBrush);
      p.drawPath(path);
      }

//---------------------------------------------------------
//   editDrag
//---------------------------------------------------------

void ChordLine::editDrag(int grip, const QPointF& delta)
      {
      int n = path.elementCount();
      QPainterPath p;
      double sp = spatium();
      double dx = delta.x() / sp;
      double dy = delta.y() / sp;
      for (int i = 0; i < n; ++i) {
            const QPainterPath::Element& e = path.elementAt(i);
            double x = e.x;
            double y = e.y;
            if (grip == i) {
                  x += dx;
                  y += dy;
                  }
            switch(e.type) {
                  case QPainterPath::CurveToDataElement:
                        break;
                  case QPainterPath::MoveToElement:
                        p.moveTo(x, y);
                        break;
                  case QPainterPath::LineToElement:
                        p.lineTo(x, y);
                        break;
                  case QPainterPath::CurveToElement:
                        {
                        double x2 = path.elementAt(i+1).x;
                        double y2 = path.elementAt(i+1).y;
                        double x3 = path.elementAt(i+2).x;
                        double y3 = path.elementAt(i+2).y;
                        if (i + 1 == grip) {
                              x2 += dx;
                              y2 += dy;
                              }
                        else if (i + 2 == grip) {
                              x3 += dx;
                              y3 += dy;
                              }
                        p.cubicTo(x, y, x2, y2, x3, y3);
                        i += 2;
                        }
                        break;
                  }
            }
      path = p;
      modified = true;
      }

//---------------------------------------------------------
//   updateGrips
//---------------------------------------------------------

void ChordLine::updateGrips(int* grips, QRectF* grip) const
      {
      int n = path.elementCount();
      *grips = n;
      QPointF cp(canvasPos());
      double sp = spatium();
      for (int i = 0; i < n; ++i)
            grip[i].translate(cp + QPointF(path.elementAt(i).x * sp, path.elementAt(i).y * sp));
      }

