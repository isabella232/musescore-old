//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id: midi.cpp,v 1.38 2006/03/22 12:04:14 wschweer Exp $
//
//  Copyright (C) 2002-2007 Werner Schweer (ws@seh.de)
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

#include "mscore.h"
#include "midifile.h"
#include "midi.h"
#include "canvas.h"
#include "file.h"
#include "score.h"
#include "key.h"
#include "clef.h"
#include "sig.h"
#include "tempo.h"
#include "note.h"
#include "chord.h"
#include "rest.h"
#include "segment.h"
#include "utils.h"
#include "text.h"
#include "slur.h"
#include "staff.h"
#include "measure.h"
#include "style.h"
#include "part.h"
#include "layout.h"
#include "timesig.h"
#include "barline.h"
#include "pedal.h"
#include "ottava.h"
#include "lyrics.h"
#include "bracket.h"
#include "keyfinder.h"

static unsigned const char gmOnMsg[] = { 0x7e, 0x7f, 0x09, 0x01 };
static unsigned const char gsOnMsg[] = { 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7f, 0x00, 0x41 };
static unsigned const char xgOnMsg[] = { 0x43, 0x10, 0x4c, 0x00, 0x00, 0x7e, 0x00 };
static unsigned const int  gmOnMsgLen = sizeof(gmOnMsg);
static unsigned const int  gsOnMsgLen = sizeof(gsOnMsg);
static unsigned const int  xgOnMsgLen = sizeof(xgOnMsg);

//---------------------------------------------------------
//   MidiInstrument
//---------------------------------------------------------

struct MidiInstrument {
      int type;
      int hbank, lbank, patch;
      int split;
      char* name;
      };

static MidiInstrument minstr[] = {
      // Piano
      { 7, 0, 0,  0, 60, "Grand Piano" },
      { 4, 0, 1,  0, 60, "GrndPnoK" },
      { 4, 0, 18, 0, 60, "MelloGrP" },
      { 4, 0, 40, 0, 60, "PianoStr" },
      { 4, 0, 41, 0, 60, "Dream" },
      { 7, 0, 0,  1, 60, "Bright Piano" },
      { 4, 0, 1,  1, 60, "BritPnoK" },
      { 7, 0, 0,  2, 60, "E.Grand" },
      { 4, 0, 1,  2, 60, "ElGrPnoK" },
      { 4, 0, 32, 2, 60, "Det.CP80" },
      { 4, 0, 40, 2, 60, "ElGrPno1" },
      { 4, 0, 41, 2, 60, "ElGrPno2" },
      { 7, 0, 0,  3, 60, "Honky-tonk" },
      { 4, 0, 1,  3, 60, "HonkyTonkK" },
      { 7, 0, 0,  4, 60, "E.Piano" },
      { 4, 0, 1,  4, 60, "El.Pno1K" },
      { 4, 0, 18, 4, 60, "MelloEP1" },
      { 4, 0, 32, 4, 60, "Chor.EP1" },
      { 4, 0, 40, 4, 60, "HardEl.P" },
      { 4, 0, 45, 4, 60, "VXElP1" },
      { 4, 0, 64, 4, 60, "60sEl.P" },
      { 7, 0, 0,  5, 60, "E.Piano 2" },
      { 4, 0, 1,  5, 60, "El.Pno2K" },
      { 4, 0, 32, 5, 60, "Chor.EP2" },
      { 4, 0, 33, 5, 60, "DX.Hard" },
      { 4, 0, 34, 5, 60, "DXLegend" },
      { 4, 0, 40, 5, 60, "DXPhase" },
      { 4, 0, 41, 5, 60, "DX+Analg" },
      { 4, 0, 42, 5, 60, "DXKotoEP" },
      { 4, 0, 45, 5, 60, "VXEl.P2" },
      { 7, 0, 0,  6, 60, "Harpsichord" },
      { 4, 0, 1,  6, 60, "Harpsi.K" },
      { 4, 0, 25, 6, 60, "Harpsi.2" },
      { 4, 0, 35, 6, 60, "Harpsi.3" },
      { 7, 0, 0,  7, 60, "Clav." },
      { 4, 0, 1,  7, 60, "Clavi.K" },
      { 4, 0, 27, 7, 60, "ClaviWah" },
      { 4, 0, 64, 7, 60, "PulseClv" },
      { 4, 0, 65, 7, 60, "PierceCl" },

// Chromatic Perc
      { 7, 0, 0, 8, 0, "Celesta" },
      { 7, 0, 0, 9, 0, "Glockenspiel" },
      { 7, 0, 0, 10, 0, "Music Box" },
      { 4, 0, 64, 10, 0, "Orgel" },
      { 7, 0, 0, 11, 0, "Vibraphone" },
      { 4, 0, 1, 11, 0, "VibesK" },
      { 4, 0, 45, 11, 0, "HardVibe" },
      { 7, 0, 0, 12, 0, "Marimba" },
      { 4, 0, 1, 12, 0, "MarimbaK" },
      { 4, 0, 64, 12, 0, "SineMrmb" },
      { 4, 0, 96, 12, 0, "Balafon" },
      { 4, 0, 97, 12, 0, "Balafon2" },
      { 4, 0, 98, 12, 0, "LogDrum" },
      { 7, 0, 0, 13, 0, "Xylophone" },
      { 7, 0, 0, 14, 0, "Tubular Bells" },
      { 4, 0, 96, 14, 0, "ChrchBel" },
      { 4, 0, 97, 14, 0, "Carillon" },
      { 7, 0, 0, 15, 0, "Dulcimer" },
      { 4, 0, 35, 15, 0, "Dulcimr2" },
      { 4, 0, 96, 15, 0, "Cimbalom" },
      { 4, 0, 97, 15, 0, "Santur" },
// Organ
      { 7, 0, 0, 16, 0, "Drawbar Organ" },
      { 4, 0, 32, 16, 0, "DelDrwOr" },
      { 4, 0, 33, 16, 0, "60sDrOr1" },
      { 4, 0, 34, 16, 0, "60sDrOr2" },
      { 4, 0, 35, 16, 0, "70sDrOr1" },
      { 4, 0, 36, 16, 0, "DrawOrg2" },
      { 4, 0, 37, 16, 0, "60sDrOr3" },
      { 4, 0, 38, 16, 0, "EvenBar" },
      { 4, 0, 40, 16, 0, "16+2\"2/3" },
      { 4, 0, 64, 16, 0, "OrganBa" },
      { 4, 0, 65, 16, 0, "70sDrOr2" },
      { 4, 0, 66, 16, 0, "CheezOrg" },
      { 4, 0, 67, 16, 0, "DrawOrg3" },
      { 7, 0, 0, 17, 0, "Perc. Organ" },
      { 4, 0, 24, 17, 0, "70sPcOr1" },
      { 4, 0, 32, 17, 0, "DetPrcOr" },
      { 4, 0, 33, 17, 0, "LiteOrg" },
      { 4, 0, 37, 17, 0, "PercOrg2" },
      { 7, 0, 0, 18, 0, "Rock Organ" },
      { 4, 0, 64, 18, 0, "RotaryOr" },
      { 4, 0, 65, 18, 0, "SloRotar" },
      { 4, 0, 66, 18, 0, "FstRotar" },
      { 7, 0, 0, 19, 0, "Church Organ" },
      { 4, 0, 32, 19, 0, "ChurOrg3" },
      { 4, 0, 35, 19, 0, "ChurOrg2" },
      { 4, 0, 40, 19, 0, "NotreDam" },
      { 4, 0, 64, 19, 0, "OrgFlute" },
      { 4, 0, 65, 19, 0, "TrmOrgFl" },
      { 7, 0, 0, 20, 0, "Reed Organ" },
      { 4, 0, 40, 20, 0, "PuffOrg" },
      { 7, 0, 0, 21, 0, "Akkordion" },
      { 4, 0, 32, 21, 0, "Accordlt" },
      { 7, 0, 0, 22, 0, "Harmonica" },
      { 4, 0, 32, 22, 0, "Harmo2" },
      { 7, 0, 0, 23, 0, "Bandoneon" },
      { 4, 0, 64, 23, 0, "TngoAcd2" },
// Guitar
      { 7, 0, 0, 24, 0, "Nylon Gtr." },
      { 4, 0, 16, 24, 0, "NylonGt2" },
      { 4, 0, 25, 24, 0, "NylonGt3" },
      { 4, 0, 43, 24, 0, "VelGtHrm" },
      { 4, 0, 96, 24, 0, "Ukelele" },
      { 7, 0, 0, 25, 0, "Steel Gtr." },
      { 4, 0, 16, 25, 0, "SteelGt2" },
      { 4, 0, 35, 25, 0, "12StrGtr" },
      { 4, 0, 40, 25, 0, "Nylon-Stl" },
      { 4, 0, 41, 25, 0, "Stl-Body" },
      { 4, 0, 96, 25, 0, "Mandolin" },
      { 7, 0, 0, 26, 0, "Jazz Guitar" },
      { 4, 0, 18, 26, 0, "MelloGtr" },
      { 4, 0, 32, 26, 0, "JazzAmp" },
      { 4, 0, 96, 26, 0, "PdlSteel" },
      { 7, 0, 0, 27, 0, "Clean Guitar" },
      { 4, 0, 32, 27, 0, "ChorusGt" },
      { 4, 0, 64, 27, 0, "CleanGt2" },
      { 7, 0, 0, 28, 0, "Muted Guitar" },
      { 4, 0, 40, 28, 0, "FunkGtr1" },
      { 4, 0, 41, 28, 0, "MuteStlG" },
      { 4, 0, 43, 28, 0, "FunkGtr2" },
      { 4, 0, 45, 28, 0, "JazzMan" },
      { 4, 0, 96, 28, 0, "Mu.DstGt" },
      { 7, 0, 0, 29, 0, "Overdrive Gtr" },
      { 4, 0, 43, 29, 0, "Gt.Pinch" },
      { 7, 0, 0, 30, 0, "DistortionGtr" },
      { 4, 0, 12, 30, 0, "DstRthmG" },
      { 4, 0, 24, 30, 0, "DistGtr2" },
      { 4, 0, 35, 30, 0, "DistGtr3" },
      { 4, 0, 36, 30, 0, "PowerGt2" },
      { 4, 0, 37, 30, 0, "PowerGt1" },
      { 4, 0, 38, 30, 0, "Dst.5ths" },
      { 4, 0, 40, 30, 0, "FeedbkGt" },
      { 4, 0, 41, 30, 0, "FeedbGt2" },
      { 4, 0, 43, 30, 0, "RkRythm2" },
      { 4, 0, 45, 30, 0, "RockRthm" },
      { 7, 0, 0, 31, 0, "Gtr.Harmonics" },
      { 4, 0, 65, 31, 0, "GtFeedbk" },
      { 4, 0, 66, 31, 0, "GtrHrmo2" },
      { 4, 0, 64, 31, 0, "AcoHarmo" },
// Bass
      { 7, 0, 0, 32, 0, "Acoustic Bass" },
      { 4, 0, 40, 32, 0, "JazzRthm" },
      { 4, 0, 45, 32, 0, "VXUprght" },
      { 7, 0, 0, 33, 0, "Fingered Bass" },
      { 4, 0, 18, 33, 0, "FingrDrk" },
      { 4, 0, 27, 33, 0, "FlangeBa" },
      { 4, 0, 40, 33, 0, "Ba-DstEG" },
      { 4, 0, 43, 33, 0, "FngrSlap" },
      { 4, 0, 45, 33, 0, "FngBass2" },
      { 4, 0, 64, 33, 0, "JazzBass" },
      { 4, 0, 65, 33, 0, "ModAlem" },
      { 7, 0, 0, 34, 0, "Picked Bass" },
      { 4, 0, 28, 34, 0, "MutePkBa" },
      { 7, 0, 0, 35, 0, "Fretless Bass" },
      { 4, 0, 32, 35, 0, "Fretles2" },
      { 4, 0, 33, 35, 0, "Fretles3" },
      { 4, 0, 34, 35, 0, "Fretles4" },
      { 4, 0, 96, 35, 0, "SynFretl" },
      { 4, 0, 97, 35, 0, "Smooth" },
      { 7, 0, 0, 36, 0, "Slap Bass 1" },
      { 4, 0, 27, 36, 0, "ResoSlap" },
      { 4, 0, 32, 36, 0, "PunchThm" },
      { 7, 0, 0, 37, 0, "Slap Bass 2" },
      { 4, 0, 43, 37, 0, "VeloSlap" },
      { 7, 0, 0, 38, 0, "Synth Bass 1" },
      { 4, 0, 18, 38, 0, "SynBa1Dk" },
      { 4, 0, 20, 38, 0, "FastResB" },
      { 4, 0, 24, 38, 0, "AcidBass" },
      { 4, 0, 35, 38, 0, "ClvBass" },
      { 4, 0, 40, 38, 0, "TeknoBa" },
      { 4, 0, 64, 38, 0, "Oscar" },
      { 4, 0, 65, 38, 0, "SqrBass" },
      { 4, 0, 66, 38, 0, "RubberBa" },
      { 4, 0, 96, 38, 0, "Hammer" },
      { 7, 0, 0, 39, 0, "Synth Bass 2" },
      { 4, 0, 6, 39, 0, "MelloSB1" },
      { 4, 0, 12, 39, 0, "SeqBass" },
      { 4, 0, 18, 39, 0, "ClkSynBa" },
      { 4, 0, 19, 39, 0, "SynBa2Dk" },
      { 4, 0, 32, 39, 0, "SmthBa2" },
      { 4, 0, 40, 39, 0, "ModulrBa" },
      { 4, 0, 41, 39, 0, "DXBass" },
      { 4, 0, 64, 39, 0, "XWireBa" },
// Strings/Orch
      { 7, 0, 0, 40, 0, "Violin" },
      { 4, 0, 8, 40, 0, "SlowVln" },
      { 7, 0, 0, 41, 0, "Viola" },
      { 7, 0, 0, 42, 0, "Cello" },
      { 7, 0, 0, 43, 0, "Contrabass" },
      { 7, 0, 0, 44, 0, "Tremolo Str." },
      { 4, 0, 8, 44, 0, "SlowTrSt" },
      { 4, 0, 40, 44, 0, "SuspStr" },
      { 7, 0, 0, 45, 0, "PizzicatoStr." },
      { 7, 0, 0, 46, 0, "Harp" },
      { 4, 0, 40, 46, 0, "YangChin" },
      { 7, 0, 0, 47, 0, "Timpani" },
// Ensemble
      { 7, 0, 0, 48, 0, "Strings 1" },
      { 4, 0, 3, 48, 0, "S.Strngs" },
      { 4, 0, 8, 48, 0, "SlowStr" },
      { 4, 0, 24, 48, 0, "ArcoStr" },
      { 4, 0, 35, 48, 0, "60sStrng" },
      { 4, 0, 40, 48, 0, "Orchestr" },
      { 4, 0, 41, 48, 0, "Orchstr2" },
      { 4, 0, 42, 48, 0, "TremOrch" },
      { 4, 0, 45, 48, 0, "VeloStr" },
      { 7, 0, 0, 49, 0, "Strings 2" },
      { 4, 0, 3, 49, 0, "S.SlwStr" },
      { 4, 0, 8, 49, 0, "LegatoSt" },
      { 4, 0, 40, 49, 0, "WarmStr" },
      { 4, 0, 41, 49, 0, "Kingdom" },
      { 4, 0, 64, 49, 0, "70sStr" },
      { 4, 0, 65, 49, 0, "StrEns3" },
      { 7, 0, 0, 50, 0, "Syn.Strings 1" },
      { 4, 0, 27, 50, 0, "ResoStr" },
      { 4, 0, 64, 50, 0, "SynStr4" },
      { 4, 0, 65, 50, 0, "SSStr" },
      { 4, 0, 35, 50, 0, "SynStr3" },
      { 7, 0, 0, 51, 0, "Syn.Strings 2" },
      { 7, 0, 0, 52, 0, "Choir Aahs" },
      { 4, 0, 3, 52, 0, "S.Choir" },
      { 4, 0, 16, 52, 0, "Ch.Aahs2" },
      { 4, 0, 32, 52, 0, "MelChoir" },
      { 4, 0, 40, 52, 0, "ChoirStr" },
      { 4, 0, 64, 52, 0, "StrngAah" },
      { 4, 0, 65, 52, 0, "MaleAah" },
      { 7, 0, 0, 53, 0, "Voice Oohs" },
      { 4, 0, 64, 53, 0, "VoiceDoo" },
      { 4, 0, 96, 53, 0, "VoiceHmn" },
      { 7, 0, 0, 54, 0, "Synth Voice" },
      { 4, 0, 40, 54, 0, "SynVox2" },
      { 4, 0, 41, 54, 0, "Choral" },
      { 4, 0, 64, 54, 0, "AnaVoice" },
      { 7, 0, 0, 55, 0, "Orchestra Hit" },
      { 4, 0, 35, 55, 0, "OrchHit2" },
      { 4, 0, 64, 55, 0, "Impact" },
      { 4, 0, 66, 55, 0, "DoublHit" },
      { 4, 0, 67, 55, 0, "BrStab80" },
// Brass
      { 7, 0, 0, 56, 0, "Trumpet" },
      { 4, 0, 16, 56, 0, "Trumpet2" },
      { 4, 0, 17, 56, 0, "BriteTrp" },
      { 4, 0, 32, 56, 0, "WarmTrp" },
      { 4, 0, 96, 56, 0, "FluglHrn" },
      { 7, 0, 0, 57, 0, "Trombone" },
      { 4, 0, 18, 57, 0, "Trmbone2" },
      { 7, 0, 0, 58, 0, "Tuba" },
      { 4, 0, 16, 58, 0, "Tuba2" },
      { 7, 0, 0, 59, 0, "Muted Trumpet" },
      { 4, 2, 64, 59, 0, "MuteTrp2" },
      { 7, 0, 0, 60, 0, "French Horn" },
      { 4, 0, 6, 60, 0, "FrHrSolo" },
      { 4, 0, 32, 60, 0, "FrHorn2" },
      { 4, 0, 37, 60, 0, "HornOrch" },
      { 7, 0, 0, 61, 0, "Brass Section" },
      { 4, 0, 35, 61, 0, "Tp-TbSec" },
      { 4, 0, 40, 61, 0, "BrssSec2" },
      { 4, 0, 41, 61, 0, "HiBrass" },
      { 4, 0, 42, 61, 0, "MelloBrs" },
      { 4, 0, 14, 61, 0, "SfrzndBr" },
      { 4, 0, 39, 61, 0, "BrssFall" },
      { 7, 0, 0, 62, 0, "Synth Brass 1" },
      { 4, 0, 12, 62, 0, "QuackBr" },
      { 4, 0, 20, 62, 0, "RezSynBr" },
      { 4, 0, 24, 62, 0, "PolyBrss" },
      { 4, 0, 27, 62, 0, "SynBras3" },
      { 4, 0, 32, 62, 0, "JumpBrss" },
      { 4, 0, 45, 62, 0, "AnaVelBr" },
      { 4, 0, 64, 62, 0, "AnaBrss1" },
      { 7, 0, 0, 63, 0, "Synth Brass 2" },
      { 4, 0, 18, 63, 0, "SoftBrs" },
      { 4, 0, 40, 63, 0, "SynBras4" },
      { 4, 0, 41, 63, 0, "ChoBrss" },
      { 4, 0, 45, 63, 0, "VelBras2" },
      { 4, 0, 64, 63, 0, "AnaBras2" },
// Reed
      { 7, 0, 0, 64, 0, "Soprano Sax" },
      { 7, 0, 0, 65, 0, "Alto Sax" },
      { 4, 0, 40, 65, 0, "SaxSect" },
      { 4, 0, 43, 65, 0, "HyprAlto" },
      { 7, 0, 0, 66, 0, "Tenor Sax" },
      { 4, 0, 40, 66, 0, "BrthTnSx" },
      { 4, 0, 41, 66, 0, "SoftTenr" },
      { 4, 0, 64, 66, 0, "TnrSax2" },
      { 7, 0, 0, 67, 0, "Baritone Sax" },
      { 7, 0, 0, 68, 0, "Oboe" },
      { 7, 0, 0, 69, 0, "English Horn" },
      { 7, 0, 0, 70, 0, "Bassoon" },
      { 7, 0, 0, 71, 0, "Clarinet" },
      { 4, 0, 96, 71, 0, "BassClar" },
// Pipe
      { 7, 0, 0, 72, 0, "Piccolo" },
      { 7, 0, 0, 73, 0, "Flute" },
      { 7, 0, 0, 74, 0, "Recorder" },
      { 7, 0, 0, 75, 0, "Pan Flute" },
      { 4, 0, 64, 75, 0, "PanFlut2" },
      { 4, 0, 96, 75, 0, "Kawala" },
      { 7, 0, 0, 76, 0, "Blown Bottle" },
      { 7, 0, 0, 77, 0, "Shakuhachi" },
      { 7, 0, 0, 78, 0, "Whistle" },
      { 7, 0, 0, 79, 0, "Ocarina" },
// SynthLead
      { 7, 0, 0, 80, 0, "Square Wave" },
      { 4, 0, 6, 80, 0, "Square2" },
      { 4, 0, 8, 80, 0, "LMSquare" },
      { 4, 0, 18, 80, 0, "Hollow" },
      { 4, 0, 19, 80, 0, "Shmoog" },
      { 4, 0, 64, 80, 0, "Mellow" },
      { 4, 0, 65, 80, 0, "SoloSine" },
      { 4, 0, 66, 80, 0, "SineLead" },
      { 7, 0, 0, 81, 0, "Saw Wave" },
      { 4, 0, 6, 81, 0, "Saw2" },
      { 4, 0, 8, 81, 0, "ThickSaw" },
      { 4, 0, 18, 81, 0, "DynaSaw" },
      { 4, 0, 19, 81, 0, "DigiSaw" },
      { 4, 0, 20, 81, 0, "BigLead" },
      { 4, 0, 24, 81, 0, "HeavySyn" },
      { 4, 0, 25, 81, 0, "WaspySyn" },
      { 4, 0, 40, 81, 0, "PulseSaw" },
      { 4, 0, 41, 81, 0, "Dr.Lead" },
      { 4, 0, 45, 81, 0, "VeloLead" },
      { 4, 0, 96, 81, 0, "SeqAna" },
      { 7, 0, 0, 82, 0, "Calliope" },
      { 4, 0, 65, 82, 0, "PurePad" },
      { 4, 0, 64, 82, 0, "VentSyn" },
      { 7, 0, 0, 83, 0, "Chiffer Lead" },
      { 4, 0, 64, 83, 0, "Rubby" },
      { 7, 0, 0, 84, 0, "Charang" },
      { 4, 0, 64, 84, 0, "DistLead" },
      { 4, 0, 65, 84, 0, "WireLead" },
      { 7, 0, 0, 85, 0, "Solo Vox" },
      { 4, 0, 24, 85, 0, "SynthAah" },
      { 4, 0, 64, 85, 0, "VoxLead" },
      { 7, 0, 0, 86, 0, "Fifth Saw" },
      { 4, 0, 35, 86, 0, "BigFive" },
      { 7, 0, 0, 87, 0, "Bass Lead" },
      { 4, 0, 16, 87, 0, "Big-Low" },
      { 4, 0, 64, 87, 0, "Fat-Prky" },
      { 4, 0, 65, 87, 0, "SoftWurl" },
// Synth Pad
      { 7, 0, 0, 88, 0, "New Age Pad" },
      { 4, 0, 64, 88, 0, "Fantasy2" },
      { 7, 0, 0, 89, 0, "Warm Pad" },
      { 4, 0, 16, 89, 0, "ThickPad" },
      { 4, 0, 17, 89, 0, "SoftPad" },
      { 4, 0, 18, 89, 0, "SinePad" },
      { 4, 0, 64, 89, 0, "HornPad" },
      { 4, 0, 65, 89, 0, "RotarStr" },
      { 7, 0, 0, 90, 0, "Polysynth Pad" },
      { 4, 0, 64, 90, 0, "PolyPd80" },
      { 4, 0, 65, 90, 0, "ClickPad" },
      { 4, 0, 66, 90, 0, "AnaPad" },
      { 4, 0, 67, 90, 0, "SquarPad" },
      { 7, 0, 0, 91, 0, "Choir Pad" },
      { 4, 0, 64, 91, 0, "Heaven2" },
      { 4, 0, 66, 91, 0, "Itopia" },
      { 4, 0, 67, 91, 0, "CCPad" },
      { 4, 0, 65, 91, 0, "LitePad" },
      { 7, 0, 0, 92, 0, "Bowed Pad" },
      { 4, 0, 64, 92, 0, "Glacier" },
      { 4, 0, 65, 92, 0, "GlassPad" },
      { 7, 0, 0, 93, 0, "Metallic Pad" },
      { 4, 0, 64, 93, 0, "TinePad" },
      { 4, 0, 65, 93, 0, "PanPad" },
      { 7, 0, 0, 94, 0, "Halo Pad" },
      { 7, 0, 0, 95, 0, "Sweep Pad" },
      { 4, 0, 20, 95, 0, "Shwimmer" },
      { 4, 0, 27, 95, 0, "Converge" },
      { 4, 0, 64, 95, 0, "PolarPad" },
      { 4, 0, 66, 95, 0, "Celstial" },
      { 4, 0, 65, 95, 0, "Sweepy" },
// Synth FX
      { 7, 0, 0, 96, 0, "Rain" },
      { 4, 0, 45, 96, 0, "ClaviPad" },
      { 4, 0, 64, 96, 0, "HrmoRain" },
      { 4, 0, 65, 96, 0, "AfrcnWnd" },
      { 4, 0, 66, 96, 0, "Caribean" },
      { 7, 0, 0, 97, 0, "Soundtrack" },
      { 4, 0, 27, 97, 0, "Prologue" },
      { 4, 0, 64, 97, 0, "Ancestrl" },
      { 4, 0, 65, 97, 0, "Rave" },
      { 7, 0, 0, 98, 0, "Crystal" },
      { 4, 0, 12, 98, 0, "SynDrCmp" },
      { 4, 0, 14, 98, 0, "Popcorn" },
      { 4, 0, 18, 98, 0, "TinyBell" },
      { 4, 0, 35, 98, 0, "RndGlock" },
      { 4, 0, 40, 98, 0, "GlockChi" },
      { 4, 0, 41, 98, 0, "ClearBel" },
      { 4, 0, 42, 98, 0, "ChorBell" },
      { 4, 0, 64, 98, 0, "SynMalet" },
      { 4, 0, 65, 98, 0, "SftCryst" },
      { 4, 0, 66, 98, 0, "LoudGlok" },
      { 4, 0, 67, 98, 0, "XmasBell" },
      { 4, 0, 68, 98, 0, "VibeBell" },
      { 4, 0, 69, 98, 0, "DigiBell" },
      { 4, 0, 70, 98, 0, "AirBells" },
      { 4, 0, 71, 98, 0, "BellHarp" },
      { 4, 0, 72, 98, 0, "Gamelmba" },
      { 7, 0, 0, 99, 0, "Athmosphere" },
      { 4, 0, 18, 99, 0, "WarmAtms" },
      { 4, 0, 19, 99, 0, "HollwRls" },
      { 4, 0, 40, 99, 0, "NylonEP" },
      { 4, 0, 64, 99, 0, "NylnHarp" },
      { 4, 0, 65, 99, 0, "HarpVox" },
      { 7, 0, 0, 100, 0, "Brightness" },
      { 7, 0, 0, 101, 0, "Goblins" },
      { 4, 0, 69, 101, 0, "MilkyWay" },
      { 4, 0, 72, 101, 0, "Puffy" },
      { 7, 0, 0, 102, 0, "Echoes" },
      { 7, 0, 0, 103, 0, "Sci-Fi" },
      { 4, 0, 65, 103, 0, "Odyssey" },
// Ethnic
      { 7, 0, 0, 104, 0, "Sitar" },
      { 4, 0, 32, 104, 0, "DetSitar" },
      { 4, 0, 35, 104, 0, "Sitar2" },
      { 4, 0, 96, 104, 0, "Tambra" },
      { 4, 0, 97, 104, 0, "Tamboura" },
      { 7, 0, 0, 105, 0, "Banjo" },
      { 4, 0, 28, 105, 0, "MuteBnjo" },
      { 4, 0, 96, 105, 0, "Rabab" },
      { 4, 0, 97, 105, 0, "Gopichnt" },
      { 4, 0, 98, 105, 0, "Oud" },
      { 7, 0, 0, 106, 0, "Shamisen" },
      { 4, 0, 96, 106, 0, "Tsugaru" },
      { 7, 0, 0, 107, 0, "Koto" },
      { 4, 0, 96, 107, 0, "T.Koto" },
      { 4, 0, 97, 107, 0, "Kanoon" },
      { 7, 0, 0, 108, 0, "Kalimba" },
      { 4, 0, 64, 108, 0, "BigKalim" },
      { 7, 0, 0, 109, 0, "Bagpipe" },
      { 7, 0, 0, 110, 0, "Fiddle" },
      { 7, 0, 0, 111, 0, "Shanai" },
      { 4, 0, 64, 111, 0, "Shanai2" },
      { 4, 0, 96, 111, 0, "Pungi" },
      { 4, 0, 97, 111, 0, "Hichriki" },
// Percussive
      { 7, 0, 0, 112, 0, "Tinkle Bell" },
      { 4, 0, 96, 112, 0, "Bonang" },
      { 4, 0, 97, 112, 0, "Gender" },
      { 4, 0, 98, 112, 0, "Gamelan" },
      { 4, 0, 99, 112, 0, "S.Gamlan" },
      { 4, 0, 100, 112, 0, "RamaCym" },
      { 4, 0, 101, 112, 0, "AsianBel" },
      { 7, 0, 0, 113, 0, "Agogo" },
      { 4, 0, 96, 113, 0, "Atrigane" },
      { 7, 0, 0, 114, 0, "Steel Drums" },
      { 4, 0, 97, 114, 0, "GlasPerc" },
      { 4, 0, 98, 114, 0, "ThaiBell" },
      { 4, 0, 96, 114, 0, "Tablas" },
      { 7, 0, 0, 115, 0, "Woodblock" },
      { 4, 0, 96, 115, 0, "Castanet" },
      { 7, 0, 0, 116, 0, "Taiko Drum" },
      { 4, 0, 96, 116, 0, "Gr.Cassa" },
      { 7, 0, 0, 117, 0, "Melodic Drum" },
      { 4, 0, 64, 117, 0, "MelTom2" },
      { 4, 0, 65, 117, 0, "RealTom" },
      { 4, 0, 66, 117, 0, "RockTom" },
      { 7, 0, 0, 118, 0, "Synth Drum" },
      { 4, 0, 64, 118, 0, "AnaTom" },
      { 4, 0, 65, 118, 0, "ElecPerc" },
      { 7, 0, 0, 119, 0, "Rev. Cymbal" },
      { 4, 0, 64, 119, 0, "RevCym2" },
      { 4, 0, 96, 119, 0, "RevSnar1" },
      { 4, 0, 97, 119, 0, "RevSnar2" },
      { 4, 0, 98, 119, 0, "RevKick1" },
      { 4, 0, 99, 119, 0, "RevConBD" },
      { 4, 0, 100, 119, 0, "RevTom1" },
      { 4, 0, 101, 119, 0, "RevTom2" },
// Special FX
      { 7,  0, 0, 120, 0, "GtrFret Noise" },
      { 7,  0, 0, 121, 0, "Breath Noise" },
      { 7,  0, 0, 122, 0, "Seashore" },
      { 7,  0, 0, 123, 0, "Bird Tweed" },
      { 7,  0, 0, 124, 0, "Telephone" },
      { 7,  0, 0, 125, 0, "Helicopter" },
      { 7,  0, 0, 126, 0, "Applaus" },
      { 7,  0, 0, 127, 0, "Gunshot" },
// Drums
      { 6, 17, 0,   0, 0, "Standard" },
      { 4, 17, 0,   1, 0, "Standrd2" },
      { 6, 17, 0,   8, 0, "Room" },
      { 4, 17, 0,  16, 0, "Rock" },
      { 6, 17, 0,  24, 0, "Electro" },
      { 6, 17, 0,  25, 0, "Analog" },
      { 6, 17, 0,  32, 0, "Jazz" },
      { 6, 17, 0,  40, 0, "Brush" },
      { 6, 17, 0,  48, 0, "Classic" },
      { 2, 17, 0,  16, 0, "Power" },
      { 2, 17, 0,  56, 0, "SFX1" },
      { 2, 17, 0, 127, 0, "GM" },
      { 4, 16, 0,   0, 0, "SFX1" },
      { 4, 16, 0,   1, 0, "SFX2" },
      { 4,  4, 0,   0, 0, "CuttngNz" },
      { 4,  4, 0,   1, 0, "CuttngNz2" },
      { 4,  4, 0,   3, 0, "StrSlap" },
      { 4,  4, 0,  16, 0, "Fl.KClik" },
      { 4,  4, 0,  32, 0, "Rain" },
      { 4,  4, 0,  33, 0, "Thunder" },
      { 4,  4, 0,  34, 0, "Wind" },
      { 4,  4, 0,  35, 0, "Stream" },
      { 4,  4, 0,  36, 0, "Bubble" },
      { 4,  4, 0,  37, 0, "Feed" },
      { 4,  4, 0,  48, 0, "Dog" },
      { 4,  4, 0,  49, 0, "Horse" },
      { 4,  4, 0,  50, 0, "Bird2" },
      { 4,  4, 0,  54, 0, "Ghost" },
      { 4,  4, 0,  55, 0, "Maou" },
      { 4,  4, 0,  64, 0, "Tel.Dial" },
      { 4,  4, 0,  65, 0, "DoorSqek" },
      { 4,  4, 0,  66, 0, "DoorSlam" },
      { 4,  4, 0,  67, 0, "Scratch" },
      { 4,  4, 0,  68, 0, "Scratch2" },
      { 4,  4, 0,  69, 0, "WindChm" },
      { 4,  4, 0,  70, 0, "Telphon2" },
      { 4,  4, 0,  80, 0, "CarEngin" },
      { 4,  4, 0,  81, 0, "CarStop" },
      { 4,  4, 0,  82, 0, "CarPass" },
      { 4,  4, 0,  83, 0, "CarCrash" },
      { 4,  4, 0,  84, 0, "Siren" },
      { 4,  4, 0,  85, 0, "Train" },
      { 4,  4, 0,  86, 0, "Jetplane" },
      { 4,  4, 0,  87, 0, "Starship" },
      { 4,  4, 0,  88, 0, "Burst" },
      { 4,  4, 0,  89, 0, "Coaster" },
      { 4,  4, 0,  90, 0, "SbMarine" },
      { 4,  4, 0,  96, 0, "Laughing" },
      { 4,  4, 0,  97, 0, "Scream" },
      { 4,  4, 0,  98, 0, "Punch" },
      { 4,  4, 0,  99, 0, "Heart" },
      { 4,  4, 0, 100, 0, "FootStep" },
      { 4,  4, 0, 112, 0, "MchinGun" },
      { 4,  4, 0, 113, 0, "LaserGun" },
      { 4,  4, 0, 114, 0, "Xplosion" },
      { 4,  4, 0, 115, 0, "FireWork" },
      { 4,  4, 0,   2, 0, "DstCutNz" },
      { 4,  4, 0,   4, 0, "B.Slide" },
      { 4,  4, 0,   5, 0, "P.Scrape" },
      { 4,  4, 0,  51, 0, "Kitty" },
      { 4,  4, 0,  52, 0, "Growl" },
      { 4,  4, 0,  53, 0, "Haunted" },
      { 4,  4, 0, 101, 0, "Applaus2" },
      };

//---------------------------------------------------------
//   instrName
//---------------------------------------------------------

static QString instrName(int type, int hbank, int lbank, int program)
      {
      if (program != -1) {
            for (unsigned int i = 0; i < sizeof(minstr)/sizeof(*minstr); ++i) {
                  MidiInstrument* mi = &minstr[i];
                  if ((mi->patch == program)
                     && (mi->type & type)
                     && (mi->hbank == hbank || hbank == -1)
                     && (mi->lbank == lbank || lbank == -1)) {
                        return QString(mi->name);
                        }
                  }
            }
      return QString();
      }

//---------------------------------------------------------
//   exportMidi
//---------------------------------------------------------

class ExportMidi : public SaveFile {
      Score* cs;

      void writeHeader();

   public:
      MidiFile mf;

      ExportMidi(Score* s) : mf() { cs = s; }
      virtual bool saver();
      };

//---------------------------------------------------------
//   exportMidi
//---------------------------------------------------------

void MuseScore::exportMidi()
      {
      ExportMidi em(cs);
      em.save(this, QString("."), QString(".mid"), tr("MuseScore: Export as Midi (SMF)"));
      }

//---------------------------------------------------------
//   writeHeader
//---------------------------------------------------------

void ExportMidi::writeHeader()
      {
      MidiTrack* track  = mf.tracks()->front();
      Measure* measure  = cs->mainLayout()->first();

      foreach(const Element* e, *measure->pel()) {
            if (e->type() == TEXT) {
                  Text* text = (Text*)(e);
                  QString str = text->getText();
                  int len     = str.length() + 1;
                  unsigned char* data = new unsigned char[len];
                  strcpy((char*)(data), str.toLatin1().data());
                  MidiMeta* ev = new MidiMeta();
                  ev->setData(data);
                  ev->setLen(len);

                  switch (text->subtype()) {
                        case TEXT_TITLE:
                              ev->setMetaType(META_TITLE);
                              track->insert(ev);
                              break;
                        case TEXT_SUBTITLE:
                              ev->setMetaType(META_SUBTITLE);
                              track->insert(ev);
                              break;
                        case TEXT_COMPOSER:
                              ev->setMetaType(META_COMPOSER);
                              track->insert(ev);
                              break;
                        case TEXT_TRANSLATOR:
                              ev->setMetaType(META_TRANSLATOR);
                              track->insert(ev);
                              break;
                        case TEXT_POET:
                              ev->setMetaType(META_POET);
                              track->insert(ev);
                              break;
                        default:
                              delete data;
                              break;
                        }
                  }
            }

      //--------------------------------------------
      //    write time signature
      //--------------------------------------------

      SigList* sigmap = cs->sigmap;
      for (iSigEvent is = sigmap->begin(); is != sigmap->end(); ++is) {
            SigEvent se   = is->second;
            unsigned char* data = new unsigned char[4];
            data[0] = se.nominator;
            int n;
            switch (se.denominator) {
                  case 1:  n = 0; break;
                  case 2:  n = 1; break;
                  case 4:  n = 2; break;
                  case 8:  n = 3; break;
                  case 16: n = 4; break;
                  case 32: n = 5; break;
                  default:
                        n = 2;
                        printf("ExportMidi: unknown time signature %d/%d\n", se.nominator, n);
                        break;
                  }
            data[1] = n;
            data[2] = 24;
            data[3] = 8;

            MidiMeta* ev = new MidiMeta;
            ev->setMetaType(META_TIME_SIGNATURE);
            ev->setData(data);
            ev->setLen(4);
            ev->setOntime(is->first);
            track->insert(ev);
            }

      //---------------------------------------------------
      //    write key signatures
      //    assume every staff corresponds to a midi track
      //---------------------------------------------------

      MidiTrackList* tl = mf.tracks();
      for (int i = 0; i < tl->size(); ++i) {
            MidiTrack* track  = tl->at(i);

            KeyList* keymap = cs->staff(i)->keymap();
            for (iKeyEvent ik = keymap->begin(); ik != keymap->end(); ++ik) {
                  MidiMeta* ev  = new MidiMeta;
                  ev->setOntime(ik->first);
                  int key       = ik->second;
                  ev->setMetaType(META_KEY_SIGNATURE);
                  ev->setLen(2);
                  unsigned char* data = new unsigned char[2];
                  data[0]   = key;
                  data[1]   = 0;  // major
                  ev->setData(data);
                  track->insert(ev);
                  }
            }

      //--------------------------------------------
      //    write tempo changes
      //--------------------------------------------

      TempoList* tempomap = cs->tempomap;
      for (iTEvent it = tempomap->begin(); it != tempomap->end(); ++it) {
            MidiMeta* ev = new MidiMeta;
            ev->setOntime(it->second->tick);
            int tempo     = it->second->tempo;
            ev->setMetaType(META_TEMPO);
            ev->setLen(3);
            unsigned char* data = new unsigned char[3];
            data[0]   = tempo >> 16;
            data[1]   = tempo >> 8;
            data[2]   = tempo;
            ev->setData(data);
            track->insert(ev);
            }
      }

//---------------------------------------------------------
//  saver
//    export midi file
//    return true on error
//---------------------------------------------------------

bool ExportMidi::saver()
      {
      mf.setDivision(::division);
      MidiTrackList* tracks = mf.tracks();
      int nstaves = cs->nstaves();

      for (int i = 0; i < nstaves; ++i)
            tracks->append(new MidiTrack(&mf));

      int gateTime = 80;  // 100 - legato (100%)

      writeHeader();

      int staffIdx = 0;
      int partIdx  = 0;

      foreach (Part* part, *cs->parts()) {
            int channel  = part->midiChannel();

            MidiTrack* track = tracks->at(staffIdx);
            track->setOutPort(0);

            MidiController* ev = new MidiController(0, 0, CTRL_PROGRAM,
               part->midiProgram());
            track->insert(ev);

            ev = new MidiController(1, 0, CTRL_VOLUME,
               part->volume());
            track->insert(ev);

            ev = new MidiController(2, 0, CTRL_PANPOT,
               part->pan());
            track->insert(ev);

            ev = new MidiController(3, 0, CTRL_REVERB_SEND,
               part->reverb());
            track->insert(ev);

            ev = new MidiController(4, 0, CTRL_CHORUS_SEND,
               part->chorus());
            track->insert(ev);

            for (int i = 0; i < part->staves()->size(); ++i) {
                  MidiTrack* track = tracks->at(staffIdx);
                  track->setOutChannel(channel);

                  QList<OttavaE> ol;
                  for (Measure* m = cs->mainLayout()->first(); m; m = m->next()) {
                        foreach(Element* e, *m->el()) {
                              if (e->type() == OTTAVA) {
                                    Ottava* ottava = (Ottava*)e;
                                    OttavaE oe;
                                    oe.offset = ottava->pitchShift();
                                    oe.start  = ottava->tick();
                                    oe.end    = ottava->tick2();
                                    ol.append(oe);
                                    }
                              }
                        }
                  for (Measure* m = cs->mainLayout()->first(); m; m = m->next()) {
                        for (int voice = 0; voice < VOICES; ++voice) {
                              for (Segment* seg = m->first(); seg; seg = seg->next()) {
                                    Element* el = seg->element(staffIdx * VOICES + voice);
                                    if (el) {
                                          if (el->type() != CHORD)
                                                continue;
                                          Chord* chord = (Chord*)el;
                                          NoteList* nl = chord->noteList();

                                          for (iNote in = nl->begin(); in != nl->end(); ++in) {
                                                Note* note = in->second;
                                                if (note->tieBack())
                                                      continue;
                                                unsigned len = 0;
                                                while (note->tieFor()) {
                                                      len += note->chord()->tickLen();
                                                      note = note->tieFor()->endNote();
                                                      }
                                                len += (note->chord()->tickLen() * gateTime / 100);

                                                unsigned tick   = chord->tick();
                                                len = len * gateTime / 100;
                                                int pitch  = note->pitch();

                                                foreach(OttavaE o, ol) {
                                                      if (tick >= o.start && tick <= o.end) {
                                                            pitch += o.offset;
                                                            break;
                                                            }
                                                      }

                                                MidiNoteOn* ev = new MidiNoteOn(tick, 0, pitch, 0x60);
                                                track->insert(ev);
                                                ev = new MidiNoteOn(tick+len, 0, pitch, 0);
                                                track->insert(ev);
                                                }
                                          }
                                    }
                              }

                        foreach(Element* e, *m->el())
                              switch(e->type()) {
                                    case PEDAL:
                                          {
                                          Pedal* pedal = (Pedal*)e;
                                          MidiController* ev = new MidiController(pedal->tick(), 0,
                                             CTRL_SUSTAIN, 0x7f);
                                          track->insert(ev);
                                          ev = new MidiController(pedal->tick2(), 0,
                                             CTRL_SUSTAIN, 0);
                                          track->insert(ev);
                                          }
                                          break;
                                    default:
                                          break;
                              }
                        }
                  ++staffIdx;
                  }
            ++partIdx;
            }
      return mf.write(&f);
      }

//---------------------------------------------------------
//   LoadMidi
//---------------------------------------------------------

class LoadMidi : public LoadFile {
   public:
      MidiFile mf;
      LoadMidi(Score*) : mf() {}
      virtual bool loader(QFile* f);
      };

//---------------------------------------------------------
//   importMidi
//---------------------------------------------------------

void MuseScore::importMidi()
      {
      QString fn = QFileDialog::getOpenFileName(
         this, tr("MuseScore: Import Midi File"),
         QString("."),
         QString("Midi SMF Files (*.mid *.mid.gz *.mid.bz2);; All files (*)")
         );
      if (fn.isEmpty())
            return;
      Score* score = new Score();
      score->read(fn);
      score->setCreated(true);
      appendScore(score);
      tab->setCurrentIndex(scoreList.size() - 1);
      }

//---------------------------------------------------------
//   importMidi
//---------------------------------------------------------

void Score::importMidi(const QString& name)
      {
      LoadMidi lm(this);
      lm.load(name);
      _saved = false;
      convertMidi(&(lm.mf));
      _created = true;
      }

//---------------------------------------------------------
//   loader
//    import midi file
//    return true on error
//---------------------------------------------------------

bool LoadMidi::loader(QFile* fp)
      {
      if (mf.read(fp)) {
            return true;
            }
      return false;
      }

// prepare:
//    * remove empty tracks
//    * Tracknamen feststellen; sollen Kommentare oder
//      Instrumentennamen verwendet werden?
//    - Instrumente feststellen
//          - Name (kommentar?)
//    - Schlagzeugtrack markieren
//    - Quantisierung festlegen:
//    for every measure:
//          * create measure
//          - insert notes
//          - Tupel ?

//---------------------------------------------------------
//   convertMidi
//---------------------------------------------------------

void Score::convertMidi(MidiFile* mf)
      {
      mf->separateChannel();
      mf->process1();
      mf->changeDivision(division);
      *sigmap = mf->siglist();

      MidiTrackList* tracks = mf->tracks();

      //---------------------------------------------------
      //  remove empty tracks
      //    determine maxPitch/minPitch/medPitch
      //    find out instrument
      //    build time signature list
      //---------------------------------------------------

      foreach (MidiTrack* track, *tracks) {
            track->maxPitch = 0;
            track->minPitch = 127;
            track->medPitch = 0;
            track->program  = 0;
		int events      = 0;
            const EventList el = track->events();
            foreach (MidiEvent* e, track->events()) {
                  if (e->type() == ME_NOTE) {
                        ++events;
                        int pitch = ((MidiNote*)e)->pitch();
                        if (pitch > track->maxPitch)
                              track->maxPitch = pitch;
                        if (pitch < track->minPitch)
                              track->minPitch = pitch;
                        track->medPitch += pitch;
                        }
                  else if (e->type() == ME_CONTROLLER && ((MidiController*)e)->controller() == CTRL_PROGRAM) {
                        track->program = ((MidiController*)e)->value();
                        }
                  }
            if (events == 0)
		      tracks->removeAt(tracks->indexOf(track));
            else
	            track->medPitch /= events;
            }
      mf->sortTracks();

      //---------------------------------------------------
      //  create instruments
      //---------------------------------------------------

      int ntracks = tracks->size();
      for (int i = 0; i < ntracks;) {
            MidiTrack* track = tracks->at(i);
            int staves = 1;
            Part* part = new Part(this);
            int ii;
            for (ii = i + 1; ii < ntracks; ++ii) {
                  MidiTrack* ntrack = tracks->at(ii);
                  if (part->midiProgram() != ntrack->program)
                        break;
                  ++staves;
                  }
            i = ii;

            if (staves == 1)
                  staves = (track->maxPitch - track->minPitch > 36) ? 2 : 1;

            for (int staff = 0; staff < staves; ++staff) {
                  Staff* s = new Staff(this, part, staff);
                  part->insertStaff(s);
                  _staves->push_back(s);
                  if (staves == 2 && part->midiProgram() == 0) {
                        s->clef()->setClef(0, staff == 0 ? CLEF_G : CLEF_F);
                        if (staff == 0) {
                              // assume this is a piano staff
                              s->setBracket(0, BRACKET_AKKOLADE);
                              s->setBracketSpan(0, 2);
                              }
                        }
                  else {
                        if (track->isDrumTrack())
                              s->clef()->setClef(0, CLEF_PERC);
                        else
                              s->clef()->setClef(0, track->medPitch < 58 ? CLEF_F : CLEF_G);
                        }
                  }
            if (track->name().isEmpty()) {
                  int hbank = -1, lbank = -1, program = 0;
                  foreach(const MidiEvent* e, track->events()) {
                        if (e->type() == ME_CONTROLLER) {
                              MidiController* mc = (MidiController*)e;
                              if (mc->controller() == CTRL_HBANK)
                                    hbank = mc->value();
                              else if (mc->controller() == CTRL_LBANK)
                                    lbank = mc->value();
                              else if (mc->controller() == ME_PROGRAM)
                                    program = mc->value();
                              }
                        }
                  QString t(instrName(mf->midiType(), hbank, lbank, program));
                  if (!t.isEmpty())
                        part->setLongName(t);
                  }
            else

                  part->setLongName(track->name());

            part->setTrackName(part->longName().toPlainText());
            part->setMidiChannel(track->outChannel());
            part->setMidiProgram(track->program);
            _parts.push_back(part);
            }

      int lastTick = 0;
      foreach (MidiTrack* midiTrack, *tracks) {
            const EventList el = midiTrack->events();
            if (!el.empty()) {
                  ciEvent i = el.end();
                  --i;
                  if (i.key() >lastTick)
                        lastTick = i.key();
                  }
            }

      //---------------------------------------------------
      //  remove empty measures at beginning
      //---------------------------------------------------

      int startBar, endBar, beat, tick;
      sigmap->tickValues(lastTick, &endBar, &beat, &tick);
      if (beat || tick)
            ++endBar;
      for (startBar = 0; startBar < endBar; ++startBar) {
            int tick1 = sigmap->bar2tick(startBar, 0, 0);
            int tick2 = sigmap->bar2tick(startBar + 1, 0, 0);
            int events = 0;
            foreach (MidiTrack* midiTrack, *tracks) {
                  const EventList el = midiTrack->events();
                  ciEvent i1 = el.lowerBound(tick1);
                  ciEvent i2 = el.lowerBound(tick2);
                  for (ciEvent ie = i1; ie != i2; ++ie) {
                        if (ie.value()->type() == ME_NOTE) {
                              ++events;
                              break;
                              }
                        }
                  }
            if (events)
                  break;
            }

      tick = sigmap->bar2tick(startBar, 0, 0);
      mf->move(-tick);

      //---------------------------------------------------
      //  count measures
      //---------------------------------------------------

      lastTick = 0;
      foreach (MidiTrack* midiTrack, *tracks) {
            const EventList el = midiTrack->events();
            for (ciEvent ie = el.begin(); ie != el.end(); ++ie) {
                  if (ie.value()->type() != ME_NOTE)
                        continue;
                  int tick = ie.key() + ((MidiNote*)ie.value())->duration();
                  if (tick > lastTick)
                        lastTick = tick;
                  }
            }
      int bars;
      sigmap->tickValues(lastTick, &bars, &beat, &tick);
      if (beat > 0 || tick > 0)
            ++bars;

      //---------------------------------------------------
      //  create measures
      //---------------------------------------------------

      for (int i = 0; i < bars; ++i) {
            Measure* measure  = new Measure(this);
            int tick = sigmap->bar2tick(i, 0, 0);
            measure->setTick(tick);

            for (iStaff i = _staves->begin(); i != _staves->end(); ++i) {
            	Staff* s = *i;
	            if (s->isTop()) {
      	            BarLine* barLine = new BarLine(this);
            	      barLine->setStaff(s);
	                  measure->setEndBarLine(barLine);
      	            }
                  }
      	_layout->push_back(measure);
            }

	foreach (MidiTrack* midiTrack, *tracks)
            midiTrack->cleanup();

	int staffIdx = 0;
	foreach (MidiTrack* midiTrack, *tracks)
            convertTrack(midiTrack, staffIdx++);

      for (iSigEvent is = sigmap->begin(); is != sigmap->end(); ++is) {
            SigEvent se = is->second;
            int tick    = is->first;
            Measure* m  = tick2measure(tick);
            if (!m)
                  continue;
            for (int staffIdx = 0; staffIdx < nstaves(); ++staffIdx) {
                  TimeSig* ts = new TimeSig(this);
                  ts->setTick(tick);
                  ts->setSig(se.denominator, se.nominator);
                  ts->setStaff(staff(staffIdx));
                  Segment* seg = m->getSegment(ts);
                  seg->add(ts);
                  }
            }
      }

//---------------------------------------------------------
//   MNote
//	Midi Note
//---------------------------------------------------------

struct MNote {
	int pitch, velo, ontime, duration, tpc;
      Tie* tie;

      MNote(int p, int v, int t, int l, int _tpc)
         : pitch(p), velo(v), ontime(t), duration(l), tpc(_tpc), tie(0) {}
      };

//---------------------------------------------------------
//   convertTrack
//---------------------------------------------------------

void Score::convertTrack(MidiTrack* midiTrack, int staffIdx)
	{
	const EventList el = midiTrack->events();

      // try to find out key
      int key = findKey(midiTrack, sigmap);

      QList<MNote*> notes;

	int ctick = 0;
      for (ciEvent i = el.begin(); i != el.end();) {
            MidiEvent* e = i.value();
            if (e->type() != ME_NOTE) {
                  ++i;
                  continue;
                  }
            //
            // process pending notes
            //
            while (!notes.isEmpty()) {
                  int tick = notes[0]->ontime;
                  int len  = i.key() - tick;
                  if (len <= 0)
                        break;
            	foreach (MNote* n, notes) {
                  	if (n->duration < len)
                              len = n->duration;
                        }
      		Measure* measure = tick2measure(tick);
                  // split notes on measure boundary
                  if ((tick + len) > measure->tick() + measure->tickLen()) {
                        len = measure->tick() + measure->tickLen() - tick;
                        }
                  Chord* chord = new Chord(this, tick);
                  chord->setStaff(staff(staffIdx));
                  chord->setTickLen(len);
                  Segment* s = measure->getSegment(chord);
                  s->add(chord);

            	foreach (MNote* n, notes) {
            		Note* note = new Note(this, n->pitch, false);
                        note->setTpc(n->tpc);
            		note->setStaff(staff(staffIdx));
            		chord->add(note);
                        note->setTick(tick);
                        if (n->tie) {
                              n->tie->setEndNote(note);
                              n->tie->setStaff(note->staff());
                              note->setTieBack(n->tie);
                              }
                        if (n->duration <= len) {
                              notes.removeAt(notes.indexOf(n));
                              continue;
                              }
				n->tie = new Tie(this);
                        n->tie->setStartNote(note);
				note->setTieFor(n->tie);
	                  n->ontime   += len;
                        n->duration -= len;
                        }
                  ctick += len;
                  }
            //
            // check for gap and fill with rest
            //
            int restLen = i.key() - ctick;
            while (restLen > 0) {
                  int len = restLen;
      		Measure* measure = tick2measure(ctick);
                  // split rest on measure boundary
                  if ((ctick + len) > measure->tick() + measure->tickLen()) {
                        len = measure->tick() + measure->tickLen() - ctick;
                        }
                  Rest* rest = new Rest(this, ctick, len);
                  rest->setStaff(staff(staffIdx));
                  Segment* s = measure->getSegment(rest);
                  s->add(rest);
                  ctick   += len;
                  restLen -= len;
                  }

            //
            // collect all notes on current
            // tick position
            //
            for (;i != el.end(); ++i) {
            	MidiEvent* e = i.value();
                  if (e->type() != ME_NOTE)
                        continue;
                  if (i.key() != ctick)
                        break;
                  MidiNote* mn = (MidiNote*)e;
            	MNote* n = new MNote(mn->pitch(), mn->velo(), mn->ontime(),
                     mn->duration(), mn->tpc());
      	      notes.append(n);
                  }
            }

      //
	// process pending notes
      //
      if (notes.isEmpty())
            return;
      int tick = notes[0]->ontime;
	Measure* measure = tick2measure(tick);
      Chord* chord = new Chord(this, tick);
      chord->setStaff(staff(staffIdx));
      Segment* s = measure->getSegment(chord);
      s->add(chord);
      int len = 0x7fffffff;      // MAXINT;
	foreach (MNote* n, notes) {
      	if (n->duration < len)
                  len = n->duration;
            }
      chord->setTickLen(len);
	foreach (MNote* n, notes) {
		Note* note = new Note(this, n->pitch, false);
		note->setStaff(staff(staffIdx));
            note->setTick(tick);
            note->setTpc(n->tpc);
		chord->add(note);
            n->duration -= len;
            if (n->duration <= 0) {
                  notes.removeAt(notes.indexOf(n));
                  delete n;
                  }
            else
                  n->ontime += len;
            }
      ctick += len;
      //
      // check for gap and fill with rest
      //
      int restLen = measure->tick() + measure->tickLen() - ctick;
      if (restLen > 0) {
            Rest* rest = new Rest(this, ctick, restLen);
		Measure* measure = tick2measure(ctick);
            rest->setStaff(staff(staffIdx));
            Segment* s = measure->getSegment(rest);
            s->add(rest);
            }

      bool keyFound = false;

      measure = tick2measure(0);
      for (ciEvent i = el.begin(); i != el.end(); ++i) {
            MidiEvent* e = i.value();
            if (e->type() == ME_META) {
                  MidiMeta* mm = (MidiMeta*)e;
                  switch(mm->metaType()) {
                        case META_LYRIC:
                              {
      		            Measure* measure = tick2measure(mm->ontime());
                              Segment* seg = measure->findSegment(Segment::SegChordRest, e->ontime());
                              if (seg == 0) {
                                    for (seg = measure->first(); seg;) {
                                          if (seg->subtype() != Segment::SegChordRest) {
                                                seg = seg->next();
                                                continue;
                                                }
                                          Segment* ns;
                                          for (ns = seg->next(); ns && ns->subtype() != Segment::SegChordRest; ns = ns->next())
                                                ;
                                          if (ns == 0 || ns->tick() > e->ontime())
                                                break;
                                          seg = ns;
                                          }
                                    }
                              if (seg == 0) {
                                    printf("no segment found for lyrics<%s> at tick %d\n",
                                       mm->data(), e->ontime());
                                    break;
                                    }
                              Lyrics* l = new Lyrics(this);
                              QString txt((char*)(mm->data()));
                              l->setText(txt);
                              seg->setLyrics(staffIdx, l);
                              }
                              break;
                        case META_TEMPO:
                              break;

                        case META_KEY_SIGNATURE:
                              {
                              unsigned char* data = mm->data();
                              int key = (char)data[0];
                              if (key < -7 || key > 7) {
                                    printf("ImportMidi: illegal key %d\n", key);
                                    break;
                                    }
            		      Staff* s = staff(staffIdx);
                              (*s->keymap())[e->ontime()] = key;
                              keyFound = false;
                              }
                              break;
                        case META_TITLE:  // mscore extension
                              {
                              Text* text = new Text(this);
                              text->setSubtype(TEXT_TITLE);
                              text->setText((char*)(mm->data()));
                              measure->add(text);
                              }
                              break;

                        case META_SUBTITLE:     // mscore extension
                              {
                              Text* text = new Text(this);
                              text->setSubtype(TEXT_SUBTITLE);
                              text->setText((char*)(mm->data()));
                              measure->add(text);
                              }
                              break;
                        case META_COMPOSER:     // mscore extension
                              {
                              Text* text = new Text(this);
                              text->setSubtype(TEXT_COMPOSER);
                              text->setText((char*)(mm->data()));
                              measure->add(text);
                              }
                              break;
                        case META_TRANSLATOR:   // mscore extension
                              {
                              Text* text = new Text(this);
                              text->setSubtype(TEXT_TRANSLATOR);
                              text->setText((char*)(mm->data()));
                              measure->add(text);
                              }
                              break;
                        case META_POET:         // mscore extension
                              {
                              Text* text = new Text(this);
                              text->setSubtype(TEXT_POET);
                              text->setText((char*)(mm->data()));
                              measure->add(text);
                              }
                              break;
                        }
                  }
            }

      if (!keyFound && !midiTrack->isDrumTrack()) {
            Staff* s = staff(staffIdx);
            (*s->keymap())[0] = key;
            }
      }

