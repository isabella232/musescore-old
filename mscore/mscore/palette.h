//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id$
//
//  Copyright (C) 2002-2009 Werner Schweer and others
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

#ifndef __PALETTE_H__
#define __PALETTE_H__

class Element;
class Sym;
class Xml;
class Palette;

#include "ui_palette.h"

//---------------------------------------------------------
//   PaletteProperties
//---------------------------------------------------------

class PaletteProperties : public QDialog, private Ui::PaletteProperties {
      Q_OBJECT

      Palette* palette;
      virtual void accept();

   public:
      PaletteProperties(Palette* p, QWidget* parent = 0);
      };

//---------------------------------------------------------
//   PaletteBoxButton
//---------------------------------------------------------

enum PaletteCommand {
      PALETTE_DELETE,
      PALETTE_EDIT,
      PALETTE_UP,
      PALETTE_DOWN,
      PALETTE_NEW
      };

class PaletteBoxButton : public QToolButton {
      Q_OBJECT

      Palette* palette;
      QAction* editAction;

      int id;

      virtual void paintEvent(QPaintEvent*);
      virtual void changeEvent(QEvent*);

   private slots:
      void deleteTriggered()     { emit paletteCmd(PALETTE_DELETE, id);  }
      void propertiesTriggered() { emit paletteCmd(PALETTE_EDIT, id);    }
      void upTriggered()         { emit paletteCmd(PALETTE_UP, id);      }
      void downTriggered()       { emit paletteCmd(PALETTE_DOWN, id);    }
      void newTriggered()        { emit paletteCmd(PALETTE_NEW, id);     }
      void beforePulldown();
      void enableEditing(bool);

   signals:
      void paletteCmd(int, int);

   public:
      PaletteBoxButton(QWidget*, Palette*, QWidget* parent = 0);
      void setId(int v) { id = v; }
      };

//---------------------------------------------------------
//   PaletteBox
//---------------------------------------------------------

class PaletteBox : public QDockWidget {
      Q_OBJECT

      QVBoxLayout* vbox;
      bool _dirty;

      virtual void closeEvent(QCloseEvent*);

   private slots:
      void paletteCmd(int, int);
      void setDirty() { _dirty = true; }

   signals:
      void paletteVisible(bool);

   public:
      PaletteBox(QWidget* parent = 0);
      void addPalette(Palette*);
      bool dirty() const      { return _dirty; }
      void write(const QString& path);
      bool read(QFile*);
      };

//---------------------------------------------------------
//   PaletteCell
//---------------------------------------------------------

struct PaletteCell {
      Element* element;
      QString name;
      bool drawStaff;
      double x, y;
      };

//---------------------------------------------------------
//   PaletteScrollArea
//---------------------------------------------------------

class PaletteScrollArea : public QScrollArea {
      Q_OBJECT
      bool _restrictHeight;

      virtual void resizeEvent(QResizeEvent*);

   public:
      PaletteScrollArea(QWidget* w, QWidget* parent = 0);
      bool restrictHeight() const      { return _restrictHeight; }
      void setRestrictHeight(bool val) { _restrictHeight = val;  }
      };

//---------------------------------------------------------
//   Palette
//---------------------------------------------------------

class Palette : public QWidget {
      Q_OBJECT

      QString _name;
      QList<PaletteCell*> cells;

      int hgrid, vgrid;
      int currentIdx;
      int selectedIdx;
      QPoint dragStartPosition;
      int dragSrcIdx;

      qreal extraMag;
      bool _drawGrid;
      bool _selectable;
      bool _readOnly;
      qreal _yOffset;
      bool _drumPalette;

      void redraw(const QRect&);
      virtual void paintEvent(QPaintEvent*);
      virtual void mousePressEvent(QMouseEvent*);
      virtual void mouseDoubleClickEvent(QMouseEvent*);
      virtual void mouseMoveEvent(QMouseEvent*);
      virtual void leaveEvent(QEvent*);
      virtual bool event(QEvent*);

      virtual void dragEnterEvent(QDragEnterEvent*);
      virtual void dragMoveEvent(QDragMoveEvent*);
      virtual void dropEvent(QDropEvent*);

      int idx(const QPoint&) const;
      QRect idxRect(int);
      void layoutCell(PaletteCell*);

   private slots:
      void actionToggled(bool val);

   signals:
      void startDragElement(Element*);
      void boxClicked(int);
      void changed();

   public:
      Palette(QWidget* parent = 0);
      ~Palette();

      void append(Element*, const QString& name);
      void add(int idx, Element*, const QString& name);
      void append(int sym);

      void setGrid(int, int);
      Element* element(int idx)      { return cells[idx]->element; }
      void setDrawGrid(bool val)     { _drawGrid = val; }
      bool drawGrid() const          { return _drawGrid; }
      void write(Xml&, const QString& name) const;
      void read(QDomElement);
      void clear();
      void setSelectable(bool val)   { _selectable = val;  }
      bool selectable() const        { return _selectable; }
      int getSelectedIdx() const     { return selectedIdx; }
      void setSelected(int idx)      { selectedIdx = idx;  }
      bool readOnly() const          { return _readOnly;   }
      void setReadOnly(bool val);
      void setMag(qreal val)         { extraMag = val;     }
      qreal mag() const              { return extraMag;    }
      void setYOffset(qreal val)     { _yOffset = val;     }
      qreal yOffset() const          { return _yOffset;        }
      int columns() const            { return width() / hgrid; }
      int rows() const;
      int resizeWidth(int);
      bool drumPalette() const       { return _drumPalette; }
      void setDrumPalette(bool val)  { _drumPalette = val;  }
      QString name() const           { return _name;        }
      void setName(const QString& s) { _name = s;           }
      int gridWidth() const          { return hgrid;        }
      int gridHeight() const         { return vgrid;        }
      };

#endif
