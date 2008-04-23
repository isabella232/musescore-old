//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id: editinstrument.cpp,v 1.4 2006/03/02 17:08:33 wschweer Exp $
//
//  Copyright (C) 2002-2007 Werner Schweer and others
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

#include "editinstrument.h"
#include "instrtemplate.h"

//---------------------------------------------------------
//   EditInstrument
//---------------------------------------------------------

EditInstrument::EditInstrument(QWidget* parent)
   : QDialog(parent)
      {
      lt = new InstrumentTemplate;
      setupUi(this);
      connect(minPitch, SIGNAL(valueChanged(int)), SLOT(valueChanged()));
      connect(maxPitch, SIGNAL(valueChanged(int)), SLOT(valueChanged()));
      }

//---------------------------------------------------------
//   setInstrument
//---------------------------------------------------------

void EditInstrument::setInstrument(InstrumentTemplate* t)
      {
      instr = t;
//TODO      *lt   = *t;
//      nameEdit->setText(t->name);
//      shortNameEdit->setText(t->shortName);
      minPitch->setValue(t->minPitch);
      maxPitch->setValue(t->maxPitch);
      transpose->setValue(t->transpose);
      midiProgram->setValue(t->midiProgram);
      staves->setValue(t->staves);
      }

//---------------------------------------------------------
//   ~EditInstrument
//---------------------------------------------------------

EditInstrument::~EditInstrument()
      {
      delete lt;
      }

//---------------------------------------------------------
//   on_buttonCancel_pressed
//---------------------------------------------------------

void EditInstrument::on_buttonCancel_pressed()
      {
//      printf("cancel\n");
      }

//---------------------------------------------------------
//   on_buttonOk_pressed
//---------------------------------------------------------

void EditInstrument::on_buttonOk_pressed()
      {
      valueChanged();
//TODO      *instr = *lt;
      }

//---------------------------------------------------------
//   valueChanged
//---------------------------------------------------------

void EditInstrument::valueChanged()
      {
//      lt->name        = nameEdit->text();
//      lt->shortName   = shortNameEdit->text();
      lt->minPitch    = minPitch->value();
      lt->maxPitch    = maxPitch->value();
      lt->transpose   = transpose->value();
      lt->midiProgram = midiProgram->value();
      lt->staves      = staves->value();
      }

